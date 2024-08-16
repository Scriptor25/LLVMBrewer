#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <Brewer/Brewer.hpp>
#include <Brewer/SourceLocation.hpp>

namespace Brewer
{
    struct Statement
    {
        explicit Statement(SourceLocation loc);

        virtual ~Statement();
        virtual std::ostream& Dump(std::ostream&) const = 0;
        virtual ValuePtr GenIR(Builder&) const = 0;

        SourceLocation Location;
    };

    struct Expression : Statement
    {
        explicit Expression(const SourceLocation& loc);
    };

    struct BinaryExpression : Expression
    {
        BinaryExpression(const SourceLocation& loc, std::string operator_, ExprPtr lhs, ExprPtr rhs);

        std::ostream& Dump(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;

        std::string Operator;
        ExprPtr LHS;
        ExprPtr RHS;
    };

    struct CallExpression : Expression
    {
        CallExpression(const SourceLocation& loc, ExprPtr callee, std::vector<ExprPtr>& args);

        std::ostream& Dump(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;

        ExprPtr Callee;
        std::vector<ExprPtr> Args;
    };

    struct ConstCharExpression : Expression
    {
        ConstCharExpression(const SourceLocation& loc, char value);

        std::ostream& Dump(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;

        char Value;
    };

    struct ConstFloatExpression : Expression
    {
        ConstFloatExpression(const SourceLocation& loc, double value);

        std::ostream& Dump(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;

        double Value;
    };

    struct ConstIntExpression : Expression
    {
        ConstIntExpression(const SourceLocation& loc, unsigned long long value);

        std::ostream& Dump(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;

        unsigned long long Value;
    };

    struct ConstStringExpression : Expression
    {
        ConstStringExpression(const SourceLocation& loc, std::string value);

        std::ostream& Dump(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;

        std::string Value;
    };

    struct IndexExpression : Expression
    {
        IndexExpression(const SourceLocation& loc, ExprPtr base, ExprPtr index);

        std::ostream& Dump(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;

        ExprPtr Base;
        ExprPtr Index;
    };

    struct SymbolExpression : Expression
    {
        SymbolExpression(const SourceLocation& loc, std::string name);

        std::ostream& Dump(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;

        std::string Name;
    };

    struct UnaryExpression : Expression
    {
        UnaryExpression(const SourceLocation& loc, std::string operator_, ExprPtr operand, bool lh);

        std::ostream& Dump(std::ostream&) const override;
        ValuePtr GenIR(Builder&) const override;

        std::string Operator;
        ExprPtr Operand;
        bool LH;
    };

    std::ostream& operator<<(std::ostream&, const StmtPtr&);
    std::ostream& operator<<(std::ostream&, const ExprPtr&);
}
