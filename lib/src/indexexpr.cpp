#include <Brewer/AST.hpp>

Brewer::IndexExpression::IndexExpression(const SourceLocation& loc, const TypePtr& type, ExprPtr base, ExprPtr index)
    : Expression(loc, type), Base(std::move(base)), Index(std::move(index))
{
}

std::ostream& Brewer::IndexExpression::Dump(std::ostream& stream) const
{
    return stream << Base << '[' << Index << ']';
}

Brewer::ValuePtr Brewer::IndexExpression::GenIR(Builder&) const
{
    throw std::runtime_error("TODO");
}
