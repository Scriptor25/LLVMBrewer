#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Parser.hpp>
#include <Brewer/Value.hpp>

Brewer::ExprPtr Brewer::Parser::ParseBinary()
{
    auto lhs = ParseCall();
    if (!lhs) return {};
    return ParseBinary(std::move(lhs), 0);
}

static int get_precedence(const std::string& op)
{
    static std::map<std::string, int> precedences{
        {"=", 0},
        {"<<=", 0},
        {">>=", 0},
        {">>>=", 0},
        {"+=", 0},
        {"-=", 0},
        {"*=", 0},
        {"/=", 0},
        {"%=", 0},
        {"&=", 0},
        {"|=", 0},
        {"^=", 0},
        {"&&", 1},
        {"||", 1},
        {"<", 2},
        {">", 2},
        {"<=", 2},
        {">=", 2},
        {"==", 2},
        {"!=", 2},
        {"&", 3},
        {"|", 3},
        {"^", 3},
        {"<<", 4},
        {">>", 4},
        {">>>", 4},
        {"+", 5},
        {"-", 5},
        {"*", 6},
        {"/", 6},
        {"%", 6},
    };

    if (precedences.count(op)) return precedences[op];
    return -1;
}

Brewer::ExprPtr Brewer::Parser::ParseBinary(ExprPtr lhs, const int min_precedence)
{
    while (At(TokenType_Operator) && get_precedence(Current().Value) >= min_precedence)
    {
        auto [Location, Type, Value] = Skip();
        const auto precedence = get_precedence(Value);

        ExprPtr rhs = ParseCall();
        if (!rhs) return {};
        while (At(TokenType_Operator) && get_precedence(Current().Value) > precedence)
        {
            const auto next_precedence = get_precedence(Current().Value);
            rhs = ParseBinary(std::move(rhs), precedence + (next_precedence > precedence ? 1 : 0));
            if (!rhs) return {};
        }

        TypePtr type;
        if (Value == "=") type = lhs->Type;
        else m_Builder.GenBinaryFn(Value)(m_Builder, Value::Empty(lhs->Type), Value::Empty(rhs->Type), &type);

        lhs = std::make_unique<BinaryExpression>(Location, type, Value, std::move(lhs), std::move(rhs));
    }

    return lhs;
}
