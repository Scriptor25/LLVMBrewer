#include <Brewer/AST.hpp>

Brewer::Statement::Statement(SourceLocation loc)
    : Location(std::move(loc))
{
}

Brewer::Statement::~Statement() = default;

std::ostream& Brewer::operator<<(std::ostream& stream, const StmtPtr& ptr)
{
    return ptr->Dump(stream);
}
