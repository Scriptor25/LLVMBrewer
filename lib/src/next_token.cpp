#include <istream>
#include <Brewer/Parser.hpp>

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

void Brewer::Parser::NewLine()
{
    m_Location.Column = 0;
    ++m_Location.Row;
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
        break;
    }
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
