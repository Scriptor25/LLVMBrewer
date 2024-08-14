#pragma once

#include <string>

namespace Brewer
{
    struct SourceLocation
    {
        std::string Filename;
        size_t Row = 1;
        size_t Column = 0;
    };

    std::ostream& operator<<(std::ostream&, const SourceLocation&);
}
