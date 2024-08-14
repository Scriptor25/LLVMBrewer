#pragma once

#include <Brewer/SourceLocation.hpp>

namespace Brewer
{
    enum TokenType
    {
        TokenType_EOF,
        TokenType_Name,
        TokenType_Bin,
        TokenType_Oct,
        TokenType_Dec,
        TokenType_Hex,
        TokenType_Float,
        TokenType_Char,
        TokenType_String,
        TokenType_Operator,
        TokenType_Other,
    };

    struct Token
    {
        SourceLocation Location{"", 0, 0};
        TokenType Type{TokenType_EOF};
        std::string Value;
    };

    std::ostream& operator<<(std::ostream&, const Token&);
}
