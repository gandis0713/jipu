#!/bin/zsh

SPV_DIR=$(pwd)/$(dirname "$0")

rm -f ${SPV_DIR}/vert.spv
rm -f ${SPV_DIR}/frag.spv

$VULKAN_SDK/bin/glslc ${SPV_DIR}/shader.vert -o ${SPV_DIR}/vert.spv
$VULKAN_SDK/bin/glslc ${SPV_DIR}/shader.frag -o ${SPV_DIR}/frag.spv