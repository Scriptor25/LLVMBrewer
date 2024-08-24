#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Context.hpp>
#include <Brewer/Parser.hpp>
#include <Brewer/Util.hpp>

Brewer::Parser::Parser(Builder& builder, std::istream& stream, const std::string& filename)
    : m_Context(builder.GetContext()), m_Builder(builder), m_Stream(stream), m_Location{filename, 1, 0}
{
    Next();
}

Brewer::Context& Brewer::Parser::GetContext() const
{
    return m_Context;
}

Brewer::Builder& Brewer::Parser::GetBuilder() const
{
    return m_Builder;
}

Brewer::StmtFn& Brewer::Parser::ParseStmtFn(const std::string& beg)
{
    return m_StmtFnMap[beg];
}

Brewer::ExprFn& Brewer::Parser::ParseExprFn(const std::string& beg)
{
    return m_ExprFnMap[beg];
}

Brewer::Token& Brewer::Parser::Next()
{
    return m_Token = NextToken();
}

Brewer::Token& Brewer::Parser::Current()
{
    return m_Token;
}

bool Brewer::Parser::At(const TokenType type) const
{
    return m_Token.Type == type;
}

bool Brewer::Parser::At(const std::string& value) const
{
    return m_Token.Value == value;
}

bool Brewer::Parser::AtEOF() const
{
    return m_Token.Type == TokenType_EOF;
}

bool Brewer::Parser::NextIfAt(const TokenType type)
{
    if (At(type))
    {
        Next();
        return true;
    }
    return false;
}

bool Brewer::Parser::NextIfAt(const std::string& value)
{
    if (At(value))
    {
        Next();
        return true;
    }
    return false;
}

Brewer::Token Brewer::Parser::Skip()
{
    auto token = m_Token;
    Next();
    return token;
}

Brewer::Token Brewer::Parser::Expect(const TokenType type)
{
    if (At(type))
        return Skip();
    auto [Location, Type, Value] = Skip();
    return std::cerr
        << "at " << Location << ": "
        << "expected type " << type << ", got " << Type
        << std::endl
        << ErrMark<Token>();
}

Brewer::Token Brewer::Parser::Expect(const std::string& value)
{
    if (At(value))
        return Skip();
    auto [Location, Type, Value] = Skip();
    return std::cerr
        << "at " << Location << ": "
        << "expected value '" << value << "', got '" << Value << "'"
        << std::endl
        << ErrMark<Token>();
}

Brewer::StmtPtr Brewer::Parser::Parse()
{
    if (const auto& fn = m_StmtFnMap[Current().Value])
        return fn(*this);

    return ParseExpr();
}

Brewer::ExprPtr Brewer::Parser::ParseExpr()
{
    return ParseBinary();
}
