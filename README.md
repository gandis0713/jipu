# JIPU

[![Build Status](https://github.com/gandis0713/jipu/workflows/Android/badge.svg)](https://github.com/gandis0713/jipu/workflows/Android/badge.svg)
[![Build Status](https://github.com/gandis0713/jipu/workflows/macOS/badge.svg)](https://github.com/gandis0713/jipu/workflows/macOS/badge.svg)
[![Build Status](https://github.com/gandis0713/jipu/workflows/Windows/badge.svg)](https://github.com/gandis0713/jipu/workflows/Windows/badge.svg)

JIPU to abstract the modern graphics API.

## Status
|Graphics API| Support | Platform |
|-|-|-|
|DirectX 12 | planed | Windows |
|Vulkan| in progress | Android, Linux, Windows |
|Metal | Planed | macOS, iOS |

## Requirement

- vcpkg ( 2023.06.20 )
- cmake ( 3.25 or higher )
- vulkan ( if you want to use vulkan )
- ninja ( if you want to build as ninja )

## How to build
```shell
$> cmake --preset $(preset)
$> cmake --build preset $(preset)
```
