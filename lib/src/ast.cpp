#include <Brewer/AST.hpp>

Brewer::Statement::Statement(const SourceLocation& loc)
    : Location(loc)
{
}

Brewer::Statement::~Statement() = default;

std::ostream& Brewer::operator<<(std::ostream& stream, const StmtPtr& ptr)
{
    return ptr->Dump(stream);
}

Brewer::Expression::Expression(const SourceLocation& loc)
    : Statement(loc)
{
}

std::ostream& Brewer::operator<<(std::ostream& stream, const ExprPtr& ptr)
{
    return ptr->Dump(stream);
}
