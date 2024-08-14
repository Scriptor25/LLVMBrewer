#include <Brewer/AST.hpp>

Brewer::UnaryExpression::UnaryExpression(const SourceLocation& loc,
                                         std::string op,
                                         ExprPtr operand,
                                         const bool lh)
    : Expression(loc), Op(std::move(op)), Operand(std::move(operand)), LH(lh)
{
}

Brewer::ValuePtr Brewer::UnaryExpression::GenIR(Builder&) const
{
    throw std::runtime_error("TODO");
}

std::ostream& Brewer::UnaryExpression::Dump(std::ostream& stream) const
{
    if (LH)
        return stream << Op << Operand;
    return stream << Operand << Op;
}
