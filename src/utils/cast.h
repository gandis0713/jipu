#pragma once

#define VULKAN_DOWNCAST(CLASS)                                       \
    inline Vulkan##CLASS& downcast(CLASS& that) noexcept             \
    {                                                                \
        return static_cast<Vulkan##CLASS&>(that);                    \
    }                                                                \
    inline const Vulkan##CLASS& downcast(const CLASS& that) noexcept \
    {                                                                \
        return static_cast<const Vulkan##CLASS&>(that);              \
    }                                                                \
    inline Vulkan##CLASS* downcast(CLASS* that) noexcept             \
    {                                                                \
        return static_cast<Vulkan##CLASS*>(that);                    \
    }                                                                \
    inline Vulkan##CLASS const* downcast(CLASS const* that) noexcept \
    {                                                                \
        return static_cast<Vulkan##CLASS const*>(that);              \
    }