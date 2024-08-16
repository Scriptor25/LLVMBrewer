#pragma once

#include <functional>
#include <map>
#include <Brewer/Brewer.hpp>
#include <Brewer/Token.hpp>

namespace Brewer
{
    typedef std::function<StmtPtr(Parser&)> StmtFn;
    typedef std::function<ExprPtr(Parser&)> ExprFn;

    class Parser
    {
    public:
        Parser(std::istream& stream, const std::string& filename);

        StmtFn& ParseStmtFn(const std::string&);
        ExprFn& ParseExprFn(const std::string&);

        Token& Next();
        Token& Current();

        [[nodiscard]] bool At(TokenType) const;
        [[nodiscard]] bool At(const std::string&) const;
        [[nodiscard]] bool AtEOF() const;

        bool NextIfAt(TokenType);
        bool NextIfAt(const std::string&);

        Token Skip();
        Token Expect(TokenType);
        Token Expect(const std::string&);

        StmtPtr Parse();
        ExprPtr ParseExpr();

    private:
        int Get();
        void Escape();
        void NewLine();
        Token NextToken();

        ExprPtr ParseBinary();
        ExprPtr ParseBinary(ExprPtr, int);
        ExprPtr ParseCall();
        ExprPtr ParseCall(ExprPtr);
        ExprPtr ParseUnary();
        ExprPtr ParseUnary(ExprPtr);
        ExprPtr ParseIndex();
        ExprPtr ParseIndex(ExprPtr);
        ExprPtr ParsePrimary();

        std::istream& m_Stream;
        SourceLocation m_SL;
        int chr = -1;

        Token m_Token;

        std::map<std::string, StmtFn> m_StmtFnMap;
        std::map<std::string, ExprFn> m_ExprFnMap;
    };
}
