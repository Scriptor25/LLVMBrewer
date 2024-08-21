#include <ostream>
#include <string>
#include <Brewer/Token.hpp>

static const char* get_string(const Brewer::TokenType type)
{
    switch (type)
    {
    case Brewer::TokenType_EOF: return "EoF";
    case Brewer::TokenType_Name: return "Name";
    case Brewer::TokenType_Bin: return "Bin";
    case Brewer::TokenType_Oct: return "Oct";
    case Brewer::TokenType_Dec: return "Dec";
    case Brewer::TokenType_Hex: return "Hex";
    case Brewer::TokenType_Float: return "Float";
    case Brewer::TokenType_Char: return "Char";
    case Brewer::TokenType_String: return "String";
    case Brewer::TokenType_Operator: return "Operator";
    case Brewer::TokenType_Other: return "Other";
    default: return "?";
    }
}

std::ostream& Brewer::operator<<(std::ostream& stream, const Token& token)
{
    return stream << token.Location << ": '" << token.Value << "' (" << get_string(token.Type) << ")";
}
