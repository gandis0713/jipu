#include "vulkan_submit_context.h"

#include "vulkan_buffer.h"
#include "vulkan_command_buffer.h"
#include "vulkan_device.h"
#include "vulkan_texture.h"

#include <spdlog/spdlog.h>

namespace jipu
{

void VulkanSubmit::add(VkCommandBuffer commandBuffer)
{
    info.commandBuffers.push_back(commandBuffer);
    object.commandBuffers.insert(commandBuffer);
}

void VulkanSubmit::add(VkBuffer buffer)
{
    object.buffers.insert(buffer);
}

void VulkanSubmit::add(VkImage image)
{
    object.images.insert(image);
}

void VulkanSubmit::add(VkImageView imageView)
{
    object.imageViews.insert(imageView);
}

void VulkanSubmit::add(VkSampler sampler)
{
    object.samplers.insert(sampler);
}

void VulkanSubmit::add(VkPipeline pipeline)
{
    object.pipelines.insert(pipeline);
}

void VulkanSubmit::add(VkPipelineLayout pipelineLayout)
{
    object.pipelineLayouts.insert(pipelineLayout);
}

void VulkanSubmit::add(VkDescriptorSet descriptorSet)
{
    object.descriptorSet.insert(descriptorSet);
}

void VulkanSubmit::add(VkDescriptorSetLayout descriptorSetLayout)
{
    object.descriptorSetLayouts.insert(descriptorSetLayout);
}

void VulkanSubmit::add(VkFramebuffer framebuffer)
{
    object.framebuffers.insert(framebuffer);
}

void VulkanSubmit::add(VkRenderPass renderPass)
{
    object.renderPasses.insert(renderPass);
}

void VulkanSubmit::addSignalSemaphore(const std::vector<VkSemaphore>& semaphores)
{
    info.signalSemaphores.insert(info.signalSemaphores.end(), semaphores.begin(), semaphores.end());
    object.signalSemaphores.insert(semaphores.begin(), semaphores.end());
}
void VulkanSubmit::addWaitSemaphore(const std::vector<VkSemaphore>& semaphores, const std::vector<VkPipelineStageFlags>& stage)
{
    info.waitSemaphores.insert(info.waitSemaphores.end(), semaphores.begin(), semaphores.end());
    info.waitStages.insert(info.waitStages.end(), stage.begin(), stage.end());

    // do not insert wait semaphores to resource. because it is not used.
}

VulkanSubmitContext VulkanSubmitContext::create(VulkanDevice* device, const std::vector<VulkanCommandRecordResult>& results)
{
    VulkanSubmitContext context{};

    {
        auto findSrcBuffer = [](const std::vector<VulkanCommandRecordResult>& results, Buffer* buffer) -> bool {
            auto it = std::find_if(results.begin(), results.end(), [buffer](const auto& result) {
                return result.resourceInfo.src.buffers.contains(buffer);
            });

            return it != results.end();
        };

        auto findSrcTexture = [&](const std::vector<VulkanCommandRecordResult>& results, Texture* texture) -> bool {
            auto it = std::find_if(results.begin(), results.end(), [texture](const auto& result) {
                return result.resourceInfo.src.textures.contains(texture);
            });

            return it != results.end();
        };

        auto findSrcResource = [&](const std::vector<VulkanCommandRecordResult>& results, const CommandResourceInfo::Destination& dst) -> bool {
            for (const auto& [buffer, _] : dst.buffers)
            {
                if (findSrcBuffer(results, buffer))
                {
                    return true;
                }
            }

            for (const auto& [texture, _] : dst.textures)
            {
                if (findSrcTexture(results, texture))
                {
                    return true;
                }
            }

            return false;
        };

        auto getSrcBufferUsageInfo = [&](const std::vector<VulkanCommandRecordResult>& results, Buffer* buffer) -> BufferUsageInfo {
            auto it = std::find_if(results.rbegin(), results.rend(), [buffer](const auto& result) {
                return result.resourceInfo.src.buffers.contains(buffer);
            });

            if (it != results.rend())
            {
                return it->resourceInfo.src.buffers.at(buffer);
            }

            spdlog::error("Failed to find source buffer usage info.");
            return {};
        };

        auto getSrcTextureUsageInfo = [&](const std::vector<VulkanCommandRecordResult>& results, Texture* texture) -> TextureUsageInfo {
            auto it = std::find_if(results.rbegin(), results.rend(), [texture](const auto& result) {
                return result.resourceInfo.src.textures.contains(texture);
            });

            if (it != results.rend())
            {
                return it->resourceInfo.src.textures.at(texture);
            }

            spdlog::error("Failed to find source texture usage info.");
            return {};
        };

        auto getSrcResourceUsageInfo = [&](const std::vector<VulkanCommandRecordResult>& results, const CommandResourceInfo::Destination& dst) -> CommandResourceInfo::Source {
            CommandResourceInfo::Source src{};
            for (const auto& [buffer, _] : dst.buffers)
            {
                if (findSrcBuffer(results, buffer))
                {
                    src.buffers[buffer] = getSrcBufferUsageInfo(results, buffer);
                }
            }

            for (const auto& [texture, _] : dst.textures)
            {
                if (findSrcTexture(results, texture))
                {
                    src.textures[texture] = getSrcTextureUsageInfo(results, texture);
                }
            }

            return src;
        };

        auto getSrcBufferSemaphores = [](std::vector<VulkanSubmit>& submits, Buffer* buffer) -> std::vector<VkSemaphore> {
            std::vector<VkSemaphore> semaphores{};
            for (const auto& submit : submits)
            {
                if (submit.object.buffers.contains(downcast(buffer)->getVkBuffer()))
                {
                    semaphores.insert(semaphores.end(), submit.info.signalSemaphores.begin(), submit.info.signalSemaphores.end());
                }
            }

            return semaphores;
        };

        auto getSrcTextureSemaphores = [](std::vector<VulkanSubmit>& submits, Texture* texture) -> std::vector<VkSemaphore> {
            std::vector<VkSemaphore> semaphores{};
            for (const auto& submit : submits)
            {
                if (submit.object.images.contains(downcast(texture)->getVkImage()))
                {
                    semaphores.insert(semaphores.end(), submit.info.signalSemaphores.begin(), submit.info.signalSemaphores.end());
                }
            }

            return semaphores;
        };

        auto getSubmitType = [](const CommandResourceInfo::Source& src) -> SubmitType {
            if (!src.buffers.empty() || !src.textures.empty())
            {
                spdlog::error("There is no output resource.");
                return SubmitType::kNone;
            }

            for (const auto& [texture, textureUsageInfo] : src.textures)
            {
                if (textureUsageInfo.stageFlags & VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
                {
                    auto owner = downcast(texture)->getOwner();

                    switch (owner)
                    {
                    case VulkanTexture::Owner::Swapchain:
                        return SubmitType::kPresent;
                    case VulkanTexture::Owner::User:
                    default:
                        return SubmitType::kRender;
                    }
                }
                if (textureUsageInfo.stageFlags & VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT)
                {
                    return SubmitType::kCompute;
                }

                if (textureUsageInfo.stageFlags & VK_PIPELINE_STAGE_TRANSFER_BIT)
                {
                    return SubmitType::kTransfer;
                }
            }

            for (const auto& [_, bufferUsageInfo] : src.buffers)
            {
                if (bufferUsageInfo.stageFlags & VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT)
                {
                    return SubmitType::kCompute;
                }
                if (bufferUsageInfo.stageFlags & VK_PIPELINE_STAGE_TRANSFER_BIT)
                {
                    return SubmitType::kTransfer;
                }
            }

            return SubmitType::kNone;
        };

        VulkanSubmit currentSubmit{};
        std::vector<VulkanCommandRecordResult> sumittedRecordResults{};
        for (const auto& result : results)
        {
            auto hasSrcDependency = findSrcResource(sumittedRecordResults, result.resourceInfo.dst);

            if (hasSrcDependency) // prepare next submit info
            {
                context.m_submits.push_back(currentSubmit);
                currentSubmit = {};
            }

            // set command buffer
            {
                auto commandBuffer = downcast(result.commandBuffer)->getVkCommandBuffer();
                currentSubmit.add(commandBuffer);
            }

            // set wait semaphore
            if (hasSrcDependency) // set wait semaphore and resource info
            {
                std::vector<VkPipelineStageFlags> waitStages{};
                std::vector<VkSemaphore> waitSemaphores{};

                auto srcResourceUsageInfo = getSrcResourceUsageInfo(sumittedRecordResults, result.resourceInfo.dst);
                for (auto& [buffer, srcBufferUsageInfo] : srcResourceUsageInfo.buffers)
                {
                    auto bufferWaitSemaphores = getSrcBufferSemaphores(context.m_submits, buffer);
                    waitSemaphores.insert(waitSemaphores.end(), bufferWaitSemaphores.begin(), bufferWaitSemaphores.end());

                    auto dstBufferUsageInfo = result.resourceInfo.dst.buffers.at(buffer);
                    waitStages.push_back(dstBufferUsageInfo.stageFlags);
                }

                for (auto& [texture, srcTextureUsageInfo] : srcResourceUsageInfo.textures)
                {
                    auto textureWaitSemaphores = getSrcTextureSemaphores(context.m_submits, texture);
                    waitSemaphores.insert(waitSemaphores.end(), textureWaitSemaphores.begin(), textureWaitSemaphores.end());

                    auto dstTextureUsageInfo = result.resourceInfo.dst.textures.at(texture);
                    waitStages.push_back(dstTextureUsageInfo.stageFlags);
                }

                currentSubmit.addWaitSemaphore(waitSemaphores, waitStages);
            }

            // set signal semaphore
            if (!result.resourceInfo.src.buffers.empty() ||
                !result.resourceInfo.src.textures.empty())
            {
                // TODO: multiple signal semaphore for external resource
                std::vector<VkSemaphore> signalSemaphores{};
                auto semaphore = device->getSemaphorePool()->create();
                signalSemaphores.push_back(semaphore);
                currentSubmit.addSignalSemaphore(signalSemaphores);
            }

            // set submit type
            {
                currentSubmit.info.type = getSubmitType(result.resourceInfo.src);
            }

            sumittedRecordResults.push_back(result);
        }
    }

    return context;
}

std::vector<VulkanSubmit> VulkanSubmitContext::getSubmits() const
{
    return m_submits;
}

std::vector<VulkanSubmit::Info> VulkanSubmitContext::getSubmitInfos() const
{
    std::vector<VulkanSubmit::Info> m_infos{};
    for (const auto& submit : m_submits)
    {
        m_infos.push_back(submit.info);
    }
    return m_infos;
}

std::vector<VulkanSubmit::Object> VulkanSubmitContext::getSubmitObjects() const
{
    std::vector<VulkanSubmit::Object> m_objects{};
    for (const auto& submit : m_submits)
    {
        m_objects.push_back(submit.object);
    }

    return m_objects;
}

} // namespace jipu