# Vulkan Test

[![Build Status](https://github.com/gandis0713/vulkan-test/workflows/Build%for%macos/badge.svg)](https://github.com/gandis0713/vulkan-test/workflows/Build%for%macos/badge.svg)
[![Build Status](https://github.com/gandis0713/vulkan-test/workflows/Build%for%Windows/badge.svg)](https://github.com/gandis0713/vulkan-test/workflows/Build%for%Windows/badge.svg)
[![Build Status](https://github.com/gandis0713/vulkan-test/workflows/Build%for%android/badge.svg)](https://github.com/gandis0713/vulkan-test/workflows/Build%for%android/badge.svg)

Vulkan test to abstract the graphics API.

## Requirement

- vcpkg
- ninja (if you want to build as ninja)

## How to build
```shell
$> cmake --preset $(preset)
$> cmake --build preset $(preset)
```
