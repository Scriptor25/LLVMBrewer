#pragma once

#include <iostream>
#include <vector>

namespace Brewer
{
    template <typename T>
    struct ErrMark
    {
    };

    template <typename T>
    T operator<<(std::ostream&, const ErrMark<T>&)
    {
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

    template <typename T, typename U>
    std::unique_ptr<T> dynamic_pointer_cast(std::unique_ptr<U>&& u)
    {
        if (auto t = dynamic_cast<T*>(u.get()))
        {
            u.release();
            return std::unique_ptr<T>(t);
        }
        return {};
    }
}
