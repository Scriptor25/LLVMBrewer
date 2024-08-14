#pragma once

#include <memory>

namespace std
{
    template <typename T, typename U>
    std::unique_ptr<T> dynamic_pointer_cast(std::unique_ptr<U>&& uptr) // NOLINT(*-dcl58-cpp)
    {
        if (auto tptr = dynamic_cast<T*>(uptr.get()))
        {
            uptr.release();
            return std::unique_ptr<T>(tptr);
        }
        return {};
    }
}
