#include <ostream>
#include <string>
#include <Brewer/Token.hpp>

std::ostream& Brewer::operator<<(std::ostream& stream, const TokenType& type)
{
    switch (type)
    {
    case TokenType_EOF: return stream << "EoF";
    case TokenType_Name: return stream << "Name";
    case TokenType_Bin: return stream << "Bin";
    case TokenType_Oct: return stream << "Oct";
    case TokenType_Dec: return stream << "Dec";
    case TokenType_Hex: return stream << "Hex";
    case TokenType_Float: return stream << "Float";
    case TokenType_Char: return stream << "Char";
    case TokenType_String: return stream << "String";
    case TokenType_Operator: return stream << "Operator";
    case TokenType_Other: return stream << "Other";
    default: return stream << "?";
    }
}

std::ostream& Brewer::operator<<(std::ostream& stream, const Token& token)
{
    return stream << token.Location << ": '" << token.Value << "' (" << token.Type << ")";
}
