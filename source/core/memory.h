#pragma once

#include <memory>
#include <type_traits>
#include <utility>

namespace Khan
{
    // Allocators
    template<typename T>
    using TAllocator = std::allocator<T>;

    // Deleters
    template<typename T>
    using TDeleter = std::default_delete<T>;


    // Smart Pointers
    template<typename T, typename TDeleterType = TDeleter<T>>
    using TUniquePtr = std::unique_ptr<T, TDeleterType>;

    template<typename T>
    using TSharedPtr = std::shared_ptr<T>;

    template<typename T>
    using TWeakPtr = std::weak_ptr<T>;


    // Smart Pointer Creation
    template<typename T, typename... TArgs>
    [[nodiscard]]
    TUniquePtr<T> MakeUnique(TArgs&&... args)
    {
        return std::make_unique<T>(std::forward<TArgs>(args)...);
    }

    template<typename T, typename TDeleterType, typename... TArgs>
    [[nodiscard]]
    TUniquePtr<T, TDeleterType> MakeUniqueWithDeleter(TDeleterType deleter, TArgs&&... args)
    {
        return TUniquePtr<T, TDeleterType>(new T(std::forward<TArgs>(args)...), std::move(deleter));
    }

    template<typename T, typename... TArgs>
    [[nodiscard]]
    TSharedPtr<T> MakeShared(TArgs&&... args)
    {
        return std::make_shared<T>(std::forward<TArgs>(args)...);
    }

    template<typename T, typename TDeleterType, typename... TArgs>
    [[nodiscard]]
    TSharedPtr<T> MakeSharedWithDeleter(TDeleterType deleter, TArgs&&... args)
    {
        return TSharedPtr<T>(new T(std::forward<TArgs>(args)...), std::move(deleter));
    }

    template<typename T, typename TAllocatorType, typename... TArgs>
    [[nodiscard]]
    TSharedPtr<T> AllocateShared(const TAllocatorType& allocator, TArgs&&... args)
    {
        return std::allocate_shared<T>(allocator, std::forward<TArgs>(args)...);
    }


    // Raw Memory Utilities
    template<typename T, typename... TArgs>
    [[nodiscard]]
    T* TConstructAt(T* location, TArgs&&... args)
    {
        return std::construct_at(location,std::forward<TArgs>(args)...);
    }

    template<typename T>
    void TDestroyAt(T* location)
    {
        std::destroy_at(location);
    }
}