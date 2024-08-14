#include <ostream>
#include <Brewer/SourceLocation.hpp>

std::ostream& Brewer::operator<<(std::ostream& stream, const SourceLocation& location)
{
    return stream << location.Filename << "(" << location.Row << "," << location.Column << ")";
}
