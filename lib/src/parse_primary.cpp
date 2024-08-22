#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Context.hpp>
#include <Brewer/Parser.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>
#include <Brewer/Value.hpp>

Brewer::ExprPtr Brewer::Parser::ParsePrimary()
{
    if (At(TokenType_EOF))
        return std::cerr << "reached eof" << std::endl << ErrMark<ExprPtr>();

    if (const auto& fn = m_ExprFnMap[Current().Value])
        return fn(*this);

    auto loc = Current().Location;

    if (NextIfAt("("))
    {
        auto ptr = ParseExpr();
        Expect(")");
        return ptr;
    }

    if (At(TokenType_Operator))
    {
        auto [Location, Type, Value] = Skip();
        auto operand = ParseCall();
        TypePtr type;
        m_Builder.GenUnaryFn(Value)(m_Builder, RValue::Empty(m_Builder, operand->Type), &type);
        return std::make_unique<UnaryExpression>(Location, type, Value, std::move(operand), true);
    }

    if (At(TokenType_Name))
    {
        auto [Location, Type, Value] = Skip();
        auto type = m_Context.GetSymbol(Value);
        return std::make_unique<SymbolExpression>(Location, type, Value);
    }
    if (At(TokenType_Bin))
        return std::make_unique<ConstIntExpression>(loc,
                                                    Type::Get(m_Context, "i64"),
                                                    std::stoull(Skip().Value, nullptr, 2));
    if (At(TokenType_Oct))
        return std::make_unique<ConstIntExpression>(loc,
                                                    Type::Get(m_Context, "i64"),
                                                    std::stoull(Skip().Value, nullptr, 8));
    if (At(TokenType_Dec))
        return std::make_unique<ConstIntExpression>(loc,
                                                    Type::Get(m_Context, "i64"),
                                                    std::stoull(Skip().Value, nullptr, 10));
    if (At(TokenType_Hex))
        return std::make_unique<ConstIntExpression>(loc,
                                                    Type::Get(m_Context, "i64"),
                                                    std::stoull(Skip().Value, nullptr, 16));
    if (At(TokenType_Float))
        return std::make_unique<ConstFloatExpression>(loc,
                                                      Type::Get(m_Context, "f64"),
                                                      std::stold(Skip().Value));
    if (At(TokenType_Char))
        return std::make_unique<ConstCharExpression>(loc,
                                                     Type::Get(m_Context, "i8"),
                                                     Skip().Value[0]);
    if (At(TokenType_String))
        return std::make_unique<ConstStringExpression>(loc,
                                                       PointerType::Get(Type::Get(m_Context, "i8")),
                                                       Skip().Value);

    const auto [Location, Type, Value] = Skip();
    return std::cerr
        << "at " << Location << ": "
        << "unhandled token "
        << "'" << Value << "' "
        << "(" << Type << ")"
        << std::endl
        << ErrMark<ExprPtr>();
}
