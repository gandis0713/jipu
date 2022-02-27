export VK_LAYER_PATH=$VULKAN_ROOT/x86_64/etc/vulkan/explicit_layer.d
export VK_INSTANCE_LAYERS=VK_LAYER_LUNARG_api_dump:VK_LAYER_KHRONOS_validation
make && ./main.out