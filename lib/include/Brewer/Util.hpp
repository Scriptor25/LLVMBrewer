#pragma once

#include <cstdarg>
#include <iostream>
#include <memory>
#include <vector>

namespace Brewer
{
    template <typename T, typename U>
    std::unique_ptr<T> dynamic_pointer_cast(std::unique_ptr<U>&& u_ptr)
    {
        if (auto t_ptr = dynamic_cast<T*>(u_ptr.get()))
        {
            u_ptr.release();
            return std::unique_ptr<T>(t_ptr);
        }
        return {};
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& stream, const std::vector<T>& v)
    {
        for (size_t i = 0; i < v.size(); ++i)
        {
            if (i > 0) stream << ", ";
            stream << v[i];
        }
        return stream;
    }

    template <typename T>
    T error(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        return {};
    }
}
