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

Brewer::Expression::Expression(const SourceLocation& loc, TypePtr type)
    : Statement(loc), Type(std::move(type))
{
}

void Brewer::Expression::GenIRNoVal(Builder& builder) const
{
    GenIR(builder);
}

std::ostream& Brewer::operator<<(std::ostream& stream, const ExprPtr& ptr)
{
    return ptr->Dump(stream);
}
