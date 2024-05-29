#pragma once

#include <memory>

namespace Sapphire
{
    template<typename T>
    using ScopedPtr = std::unique_ptr<T>;
    template<typename T, typename ... Args>
    constexpr ScopedPtr<T> CreateScopedPtr(Args&& ... Arg)
    {
        return std::make_unique<T>(std::forward<Args>(Arg)...);
    }

    template<typename T>
    using SharedPtr = std::unique_ptr<T>;
    template<typename T, typename ... Args>
    constexpr SharedPtr<T> CreateSharedPtr(Args&& ... Arg)
    {
        return std::make_unique<T>(std::forward<Args>(Arg)...);
    }
}