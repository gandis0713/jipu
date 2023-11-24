# JIPU

[![Build Status](https://github.com/gandis0713/jipu/workflows/Android/badge.svg)](https://github.com/gandis0713/jipu/workflows/Android/badge.svg)
[![Build Status](https://github.com/gandis0713/jipu/workflows/macOS/badge.svg)](https://github.com/gandis0713/jipu/workflows/macOS/badge.svg)
[![Build Status](https://github.com/gandis0713/jipu/workflows/Windows/badge.svg)](https://github.com/gandis0713/jipu/workflows/Windows/badge.svg)

JIPU to abstract the modern graphics API.

## Status
|Graphics API| support |
|-|-|
|DirectX 12 | planed |
|Vulkan| in progress |
|Metal | Planed |

## Requirement

- vcpkg
- ninja (if you want to build as ninja)

## How to build
```shell
$> cmake --preset $(preset)
$> cmake --build preset $(preset)
```
