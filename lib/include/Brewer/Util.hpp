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
}
