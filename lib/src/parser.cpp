#include <Brewer/AST.hpp>
#include <Brewer/Context.hpp>
#include <Brewer/Parser.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>

Brewer::Parser::Parser(Context& context, std::istream& stream, const std::string& filename)
    : m_Context(context), m_Stream(stream), m_Location{filename, 1, 0}
{
    Next();
}

Brewer::Context& Brewer::Parser::GetContext() const
{
    return m_Context;
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

Brewer::TypePtr Brewer::Parser::ParseType()
{
    TypePtr type;

    if (NextIfAt("struct"))
    {
        if (!NextIfAt("{"))
        {
            type = StructType::Get(m_Context);
        }
        else
        {
            std::vector<TypePtr> elements;
            while (!NextIfAt("}"))
            {
                elements.push_back(ParseType());
                if (!At("}")) Expect(",");
            }
            type = StructType::Get(elements);
        }
    }
    else
    {
        auto [Location, Type, Value] = Expect(TokenType_Name);
        type = m_Context.GetType(Value);
        if (!type)
            return std::cerr
                << "at " << Location << ": "
                << "undefined type " << Value
                << std::endl
                << ErrMark<TypePtr>();
    }

    while (true)
    {
        if (NextIfAt("*"))
        {
            type = PointerType::Get(type);
            continue;
        }

        if (NextIfAt("["))
        {
            const auto length = dynamic_pointer_cast<ConstIntExpression>(ParseExpr());
            auto [Location, Type, Value] = Expect("]");
            if (!length)
                return std::cerr
                    << "at " << Location << ": "
                    << "array length must be a constant int"
                    << std::endl
                    << ErrMark<TypePtr>();
            type = ArrayType::Get(type, length->Value);
            continue;
        }

        if (NextIfAt("("))
        {
            std::vector<TypePtr> params;
            bool vararg = false;
            while (!NextIfAt(")"))
            {
                if (NextIfAt("?"))
                {
                    vararg = true;
                    Expect(")");
                    break;
                }

                params.push_back(ParseType());
                if (!At(")")) Expect(",");
            }
            type = FunctionType::Get(type, params, vararg);
            continue;
        }

        return type;
    }
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
        || c == '^'
        || c == '='
        || c == '<'
        || c == '>';
}

int Brewer::Parser::Get()
{
    ++m_Location.Column;
    return m_Stream.get();
}

void Brewer::Parser::Escape()
{
    if (m_CC != '\\')
        return;

    m_CC = Get();
    switch (m_CC)
    {
    case 'a':
        m_CC = 0x07;
        break;
    case 'b':
        m_CC = 0x08;
        break;
    case 't':
        m_CC = 0x09;
        break;
    case 'n':
        m_CC = 0x0A;
        break;
    case 'v':
        m_CC = 0x0B;
        break;
    case 'f':
        m_CC = 0x0C;
        break;
    case 'r':
        m_CC = 0x0D;
        break;
    case 'x':
        {
            m_CC = Get();
            std::string value;
            value += static_cast<char>(m_CC);
            m_CC = Get();
            value += static_cast<char>(m_CC);
            m_CC = std::stoi(value, nullptr, 16);
        }
        break;
    default:
        {
            if (is_oct_digit(m_CC))
            {
                std::string value;
                value += static_cast<char>(m_CC);
                m_CC = Get();
                value += static_cast<char>(m_CC);
                m_CC = Get();
                value += static_cast<char>(m_CC);
                m_CC = std::stoi(value, nullptr, 8);
            }
        }
        break;
    }
}

void Brewer::Parser::NewLine()
{
    m_Location.Column = 0;
    ++m_Location.Row;
}

Brewer::Token Brewer::Parser::NextToken()
{
    enum State
    {
        State_Normal,
        State_Comment,
        State_Name,
        State_Radix,
        State_Bin,
        State_Oct,
        State_Dec,
        State_Hex,
        State_Char,
        State_String,
        State_Operator,
    };

    if (m_CC < 0)
        m_CC = Get();

    auto state = State_Normal;
    bool isfloat;
    std::string value;
    SourceLocation loc;

    while (m_CC >= 0 || state != State_Normal)
    {
        switch (state)
        {
        case State_Normal:
            switch (m_CC)
            {
            case '#':
                state = State_Comment;
                break;

            case '"':
                loc = m_Location;
                state = State_String;
                break;

            case '\'':
                loc = m_Location;
                state = State_Char;
                break;

            case '0':
                loc = m_Location;
                state = State_Radix;
                break;

            case '\r':
                m_Location.Column = 0;
                break;

            case '\n':
                NewLine();
                break;

            default:
                if (m_CC <= 0x20)
                    break;

                if (isdigit(m_CC))
                {
                    loc = m_Location;
                    state = State_Dec;
                    isfloat = false;
                    value += static_cast<char>(m_CC);
                    break;
                }

                if (isalnum(m_CC) || m_CC == '_')
                {
                    loc = m_Location;
                    state = State_Name;
                    value += static_cast<char>(m_CC);
                    break;
                }

                if (is_operator(m_CC))
                {
                    loc = m_Location;
                    state = State_Operator;
                    value += static_cast<char>(m_CC);
                    break;
                }

                loc = m_Location;
                value += static_cast<char>(m_CC);
                m_CC = Get();

                if (value[0] == '.' && isdigit(m_CC))
                {
                    state = State_Dec;
                    isfloat = true;
                    value += static_cast<char>(m_CC);
                    break;
                }

                return {loc, TokenType_Other, value};
            }
            break;

        case State_Comment:
            if (m_CC == '#')
                state = State_Normal;
            else if (m_CC == '\n')
                NewLine();
            break;

        case State_String:
            if (m_CC != '"')
            {
                if (m_CC == '\\')
                    Escape();
                value += static_cast<char>(m_CC);
                break;
            }
            m_CC = Get();
            return {loc, TokenType_String, value};

        case State_Char:
            if (m_CC != '\'')
            {
                if (m_CC == '\\')
                    Escape();
                value += static_cast<char>(m_CC);
                break;
            }
            m_CC = Get();
            return {loc, TokenType_Char, value};

        case State_Radix:
            if (m_CC == 'b' || m_CC == 'B')
            {
                state = State_Bin;
                break;
            }
            if (m_CC == 'x' || m_CC == 'X')
            {
                state = State_Hex;
                break;
            }
            if (m_CC == '.')
            {
                state = State_Dec;
                isfloat = true;
                value += "0.";
                break;
            }
            if (is_oct_digit(m_CC))
            {
                state = State_Oct;
                value += static_cast<char>(m_CC);
                break;
            }
            return {loc, TokenType_Dec, "0"};

        case State_Bin:
            if (m_CC == '0' || m_CC == '1' || m_CC == 'u')
            {
                value += static_cast<char>(m_CC);
                break;
            }
            return {loc, TokenType_Bin, value};

        case State_Oct:
            if (is_oct_digit(m_CC) || m_CC == 'u')
            {
                value += static_cast<char>(m_CC);
                break;
            }
            return {loc, TokenType_Oct, value};

        case State_Dec:
            if (m_CC == '.')
            {
                isfloat = true;
                value += static_cast<char>(m_CC);
                break;
            }
            if (isdigit(m_CC) || m_CC == 'u')
            {
                value += static_cast<char>(m_CC);
                break;
            }
            return {loc, isfloat ? TokenType_Float : TokenType_Dec, value};

        case State_Hex:
            if (isxdigit(m_CC) || m_CC == 'u')
            {
                value += static_cast<char>(m_CC);
                break;
            }
            return {loc, TokenType_Hex, value};

        case State_Name:
            if (isalnum(m_CC) || m_CC == '_')
            {
                value += static_cast<char>(m_CC);
                break;
            }
            return {loc, TokenType_Name, value};

        case State_Operator:
            if (is_compound_operator(m_CC))
            {
                value += static_cast<char>(m_CC);
                break;
            }
            return {loc, TokenType_Operator, value};
        }

        m_CC = Get();
    }

    return {m_Location, TokenType_EOF, ""};
}

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
        if (Value == "=="
            || Value == "!="
            || Value == "<="
            || Value == ">="
            || Value == "<"
            || Value == ">"
            || Value == "&&"
            || Value == "||"
            || Value == "^^")
            type = m_Context.GetType("i1");
        else if (Value.find('=') != std::string::npos)
            type = lhs->Type;
        else
            type = Type::GetHigherOrder(lhs->Type, rhs->Type);

        lhs = std::make_unique<BinaryExpression>(Location, type, Value, std::move(lhs), std::move(rhs));
    }

    return lhs;
}

Brewer::ExprPtr Brewer::Parser::ParseCall()
{
    auto base = ParseCall(ParseUnary());
    if (!base) return {};
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

        auto type = std::dynamic_pointer_cast<FunctionType>(
            std::dynamic_pointer_cast<PointerType>(callee->Type)->Base())->Result();

        callee = std::make_unique<CallExpression>(Location, type, std::move(callee), args);
    }

    return callee;
}

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
        operand = std::make_unique<UnaryExpression>(Location, operand->Type, Value, std::move(operand), false);
    }

    return operand;
}

Brewer::ExprPtr Brewer::Parser::ParseIndex()
{
    auto base = ParsePrimary();
    if (!base) return {};
    return ParseIndex(std::move(base));
}

Brewer::ExprPtr Brewer::Parser::ParseIndex(ExprPtr base)
{
    while (At("["))
    {
        auto [Location, Type, Value] = Skip();

        auto index = ParseExpr();
        Expect("]");

        TypePtr element;
        if (const auto type = std::dynamic_pointer_cast<PointerType>(base->Type)) element = type->Base();
        if (const auto type = std::dynamic_pointer_cast<ArrayType>(base->Type)) element = type->Base();
        base = std::make_unique<IndexExpression>(Location, element, std::move(base), std::move(index));
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
        TypePtr type;
        if (Value == "!") type = m_Context.GetType("i1");
        else type = operand->Type;
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
