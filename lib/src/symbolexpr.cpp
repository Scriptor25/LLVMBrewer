#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>

Brewer::SymbolExpression::SymbolExpression(const SourceLocation& loc, const std::string& name)
    : Expression(loc), Name(name)
{
}

Brewer::ValuePtr Brewer::SymbolExpression::GenIR(Builder& builder) const
{
    return builder[Name];
}

std::ostream& Brewer::SymbolExpression::Dump(std::ostream& stream) const
{
    return stream << Name;
}
