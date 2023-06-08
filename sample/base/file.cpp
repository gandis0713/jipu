#include "file.h"

#include <fmt/format.h>
#include <fstream>

#if defined(__ANDROID__) || defined(ANDROID)
    #include <game-activity/native_app_glue/android_native_app_glue.h>
#endif

namespace
{

#if defined(__ANDROID__) || defined(ANDROID)

std::vector<char> _readFile(android_app* app, const std::filesystem::path& filePath)
{
    // Read the file
    assert(app);
    AAsset* file = AAssetManager_open(app->activity->assetManager,
                                      filePath.c_str(), AASSET_MODE_BUFFER);
    size_t fileLength = AAsset_getLength(file);

    std::vector<char> fileContent{};
    fileContent.resize(fileLength);

    AAsset_read(file, fileContent.data(), fileLength);
    AAsset_close(file);

    return fileContent;
}

#else

std::vector<char> _readFile(const std::filesystem::path& file_path)
{
    std::ifstream file(file_path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error(fmt::format("Failed to open file: {}", file_path.generic_string()));
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

#endif
} // namespace

namespace vkt::utils
{

std::vector<char> readFile(void* platformContext, const std::filesystem::path& filePath)
{
#if defined(__ANDROID__) || defined(ANDROID)
    android_app* app = static_cast<android_app*>(platformContext);
    return _readFile(app, filePath);
#else
    return _readFile(filePath);
#endif
}

} // namespace vkt::utils
