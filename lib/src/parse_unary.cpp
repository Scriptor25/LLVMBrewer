#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Parser.hpp>
#include <Brewer/Value.hpp>

Brewer::ExprPtr Brewer::Parser::ParseUnary()
{
    auto operand = ParseIndex();
    if (!operand) return {};
    return ParseUnary(std::move(operand));
}

Brewer::ExprPtr Brewer::Parser::ParseUnary(ExprPtr operand)
{
    if (At("++") || At("--"))
    {
        auto [Location, Type, Value] = Skip();
        TypePtr type;
        m_Builder.GenUnaryFn(Value)(m_Builder, RValue::Empty(m_Builder, operand->Type), &type);
        operand = std::make_unique<UnaryExpression>(Location, type, Value, std::move(operand), false);
    }

    return operand;
}
