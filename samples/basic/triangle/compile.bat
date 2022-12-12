del vert.spv
del frag.spv

call %VULKAN_SDK%/bin/glslc %~dp0/shader.vert -o %~dp0/vert.spv
call %VULKAN_SDK%/bin/glslc %~dp0/shader.frag -o %~dp0/frag.spv