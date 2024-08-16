#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <utility>

Brewer::SymbolExpression::SymbolExpression(const SourceLocation& loc, std::string name)
    : Expression(loc), Name(std::move(name))
{
}

std::ostream& Brewer::SymbolExpression::Dump(std::ostream& stream) const
{
    return stream << Name;
}

Brewer::ValuePtr Brewer::SymbolExpression::GenIR(Builder& builder) const
{
    return builder[Name];
}
