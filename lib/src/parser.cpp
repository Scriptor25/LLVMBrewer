#include <map>
#include <stdexcept>
#include <Brewer/AST.hpp>
#include <Brewer/Parser.hpp>
#include <Brewer/Util.hpp>

Brewer::Parser::Parser(std::istream& stream, const std::string& filename)
    : m_Stream(stream), m_SL{filename, 1, 0}
{
    Next();
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
    throw std::runtime_error("unexpected token");
}

Brewer::Token Brewer::Parser::Expect(const std::string& value)
{
    if (At(value))
        return Skip();
    throw std::runtime_error("unexpected token");
}

Brewer::StmtPtr Brewer::Parser::Parse()
{
    if (const auto& fn = m_StmtFnMap[Current().Value])
        return fn(*this);

    return ParseBinary();
}

Brewer::ExprPtr Brewer::Parser::ParseExpr()
{
    return ParseBinary();
}

static int is_oct_digit(const int c)
{
    return 0x30 <= c && c <= 0x37;
}

static int is_operator(const int c)
{
    return c == '+'
        || c == '-'
        || c == '*'
        || c == '/'
        || c == '%'
        || c == '&'
        || c == '|'
        || c == '^'
        || c == '='
        || c == '<'
        || c == '>'
        || c == '!'
        || c == '~';
}

static int is_compound_operator(const int c)
{
    return c == '+'
        || c == '-'
        || c == '&'
        || c == '|'
        || c == '='
        || c == '<'
        || c == '>';
}

int Brewer::Parser::Get()
{
    ++m_SL.Column;
    return m_Stream.get();
}

void Brewer::Parser::Escape()
{
    if (chr != '\\')
        return;

    chr = Get();
    switch (chr)
    {
    case 'a': chr = 0x07;
    case 'b': chr = 0x08;
    case 't': chr = 0x09;
    case 'n': chr = 0x0A;
    case 'v': chr = 0x0B;
    case 'f': chr = 0x0C;
    case 'r': chr = 0x0D;
    case 'x':
        {
            chr = Get();
            std::string value;
            value += static_cast<char>(chr);
            chr = Get();
            value += static_cast<char>(chr);
            chr = std::stoi(value, nullptr, 16);
        }
    default:
        {
            if (is_oct_digit(chr))
            {
                std::string value;
                value += static_cast<char>(chr);
                chr = Get();
                value += static_cast<char>(chr);
                chr = Get();
                value += static_cast<char>(chr);
                chr = std::stoi(value, nullptr, 8);
            }
        }
    }
}

void Brewer::Parser::NewLine()
{
    m_SL.Column = 0;
    ++m_SL.Row;
}

Brewer::Token Brewer::Parser::NextToken()
{
    enum TokenizerState
    {
        TokenizerState_Normal,
        TokenizerState_Comment,
        TokenizerState_Name,
        TokenizerState_Radix,
        TokenizerState_Bin,
        TokenizerState_Oct,
        TokenizerState_Dec,
        TokenizerState_Hex,
        TokenizerState_Char,
        TokenizerState_String,
        TokenizerState_Operator,
    };

    if (chr < 0)
        chr = Get();

    while (0x00 <= chr && chr <= 0x20)
    {
        if (chr == '\n')
            NewLine();
        chr = Get();
    }

    TokenizerState state = TokenizerState_Normal;
    bool isfloat;
    std::string value;
    SourceLocation sl;

    while (chr >= 0 || state != TokenizerState_Normal)
    {
        switch (state)
        {
        case TokenizerState_Normal:
            switch (chr)
            {
            case '#':
                state = TokenizerState_Comment;
                break;

            case '"':
                sl = m_SL;
                state = TokenizerState_String;
                break;

            case '\'':
                sl = m_SL;
                state = TokenizerState_Char;
                break;

            case '0':
                sl = m_SL;
                state = TokenizerState_Radix;
                break;

            default:
                if (chr <= 0x20)
                {
                    if (chr == '\n')
                        NewLine();
                    break;
                }

                if (isdigit(chr))
                {
                    sl = m_SL;
                    state = TokenizerState_Dec;
                    isfloat = false;
                    value += static_cast<char>(chr);
                    break;
                }

                if (isalnum(chr) || chr == '_')
                {
                    sl = m_SL;
                    state = TokenizerState_Name;
                    value += static_cast<char>(chr);
                    break;
                }

                if (is_operator(chr))
                {
                    sl = m_SL;
                    state = TokenizerState_Operator;
                    value += static_cast<char>(chr);
                    break;
                }

                sl = m_SL;
                value += static_cast<char>(chr);
                chr = Get();

                if (value[0] == '.' && isdigit(chr))
                {
                    state = TokenizerState_Dec;
                    isfloat = true;
                    value += static_cast<char>(chr);
                    break;
                }

                return {sl, TokenType_Other, value};
            }
            break;

        case TokenizerState_Comment:
            if (chr == '#')
                state = TokenizerState_Normal;
            else if (chr == '\n')
                NewLine();
            break;

        case TokenizerState_String:
            if (chr == '"')
            {
                chr = Get();
                return {sl, TokenType_String, value};
            }
            if (chr == '\\')
                Escape();
            value += static_cast<char>(chr);
            break;

        case TokenizerState_Char:
            if (chr == '\'')
            {
                chr = Get();
                return {sl, TokenType_Char, value};
            }
            if (chr == '\\')
                Escape();
            value += static_cast<char>(chr);
            break;

        case TokenizerState_Radix:
            if (chr == 'b' || chr == 'B')
            {
                state = TokenizerState_Bin;
                break;
            }
            if (chr == 'x' || chr == 'X')
            {
                state = TokenizerState_Hex;
                break;
            }
            if (chr == '.')
            {
                state = TokenizerState_Dec;
                isfloat = true;
                value += "0.";
                break;
            }
            if (is_oct_digit(chr))
            {
                state = TokenizerState_Oct;
                value += static_cast<char>(chr);
                break;
            }
            return {sl, TokenType_Dec, "0"};

        case TokenizerState_Bin:
            if (chr == '0' || chr == '1' || chr == 'u')
            {
                value += static_cast<char>(chr);
                break;
            }
            return {sl, TokenType_Bin, value};

        case TokenizerState_Oct:
            if (is_oct_digit(chr) || chr == 'u')
            {
                value += static_cast<char>(chr);
                break;
            }
            return {sl, TokenType_Oct, value};

        case TokenizerState_Dec:
            if (chr == '.')
            {
                isfloat = true;
                value += static_cast<char>(chr);
                break;
            }
            if (isdigit(chr) || chr == 'u')
            {
                value += static_cast<char>(chr);
                break;
            }
            return {sl, isfloat ? TokenType_Float : TokenType_Dec, value};

        case TokenizerState_Hex:
            if (isxdigit(chr) || chr == 'u')
            {
                value += static_cast<char>(chr);
                break;
            }
            return {sl, TokenType_Hex, value};

        case TokenizerState_Name:
            if (!isalnum(chr) && chr != '_')
                return {sl, TokenType_Name, value};
            value += static_cast<char>(chr);
            break;

        case TokenizerState_Operator:
            if (!is_compound_operator(chr))
                return {sl, TokenType_Operator, value};
            value += static_cast<char>(chr);
            break;
        }

        chr = Get();
    }

    return {m_SL, TokenType_EOF, ""};
}

Brewer::ExprPtr Brewer::Parser::ParseBinary()
{
    return ParseBinary(ParseCall(), 0);
}

Brewer::ExprPtr Brewer::Parser::ParseBinary(ExprPtr lhs, const int min_precedence)
{
    static std::map<std::string, int> precedences{
        {"++", -1},
        {"--", -1},
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

    while (At(TokenType_Operator) && precedences[Current().Value] >= min_precedence)
    {
        auto [Location, Type, Value] = Skip();
        const auto precedence = precedences[Value];

        auto rhs = dynamic_pointer_cast<Expression>(ParseCall());
        while (At(TokenType_Operator) && precedences[Current().Value] > precedence)
        {
            const auto next_precedence = precedences[Current().Value];
            rhs = ParseBinary(std::move(rhs), precedence + (next_precedence > precedence ? 1 : 0));
        }

        lhs = std::make_unique<BinaryExpression>(Location, Value, std::move(lhs), std::move(rhs));
    }

    return lhs;
}

Brewer::ExprPtr Brewer::Parser::ParseCall()
{
    auto base = ParseCall(ParseUnary());
    if (At("["))
        base = ParseIndex(std::move(base));
    return base;
}

Brewer::ExprPtr Brewer::Parser::ParseCall(ExprPtr callee)
{
    while (At("("))
    {
        auto [Location, Type, Value] = Skip();

        std::vector<ExprPtr> args;
        while (!NextIfAt(")"))
        {
            auto arg = ParseExpr();
            args.push_back(std::move(arg));

            if (!At(")"))
                Expect(",");
        }

        callee = std::make_unique<CallExpression>(Location, std::move(callee), args);
    }

    return callee;
}

Brewer::ExprPtr Brewer::Parser::ParseUnary()
{
    return ParseUnary(ParseIndex());
}

Brewer::ExprPtr Brewer::Parser::ParseUnary(ExprPtr operand)
{
    if (At("++") || At("--"))
    {
        auto [Location, Type, Value] = Skip();
        operand = std::make_unique<UnaryExpression>(Location, Value, std::move(operand), false);
    }

    return operand;
}

Brewer::ExprPtr Brewer::Parser::ParseIndex()
{
    return ParseIndex(ParsePrimary());
}

Brewer::ExprPtr Brewer::Parser::ParseIndex(ExprPtr base)
{
    while (At("["))
    {
        auto [Location, Type, Value] = Skip();

        auto index = ParseExpr();
        Expect("]");

        base = std::make_unique<IndexExpression>(Location, std::move(base), std::move(index));
    }

    return base;
}

Brewer::ExprPtr Brewer::Parser::ParsePrimary()
{
    if (At(TokenType_EOF))
        throw std::runtime_error("reached eof");

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
        return std::make_unique<UnaryExpression>(Location, Value, std::move(operand), true);
    }

    if (At(TokenType_Name)) return std::make_unique<SymbolExpression>(loc, Skip().Value);
    if (At(TokenType_Bin)) return std::make_unique<ConstIntExpression>(loc, std::stoull(Skip().Value, nullptr, 2));
    if (At(TokenType_Oct)) return std::make_unique<ConstIntExpression>(loc, std::stoull(Skip().Value, nullptr, 8));
    if (At(TokenType_Dec)) return std::make_unique<ConstIntExpression>(loc, std::stoull(Skip().Value, nullptr, 10));
    if (At(TokenType_Hex)) return std::make_unique<ConstIntExpression>(loc, std::stoull(Skip().Value, nullptr, 16));
    if (At(TokenType_Float)) return std::make_unique<ConstFloatExpression>(loc, std::stold(Skip().Value));
    if (At(TokenType_Char)) return std::make_unique<ConstCharExpression>(loc, Skip().Value[0]);
    if (At(TokenType_String)) return std::make_unique<ConstStringExpression>(loc, Skip().Value);

    throw std::runtime_error("unhandled token");
}
