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
        explicit Statement(const SourceLocation& loc);

        virtual ~Statement();
        virtual ValuePtr GenIR(Builder&) const = 0;
        virtual std::ostream& Dump(std::ostream&) const = 0;

        SourceLocation Location;
    };

    struct Expression : Statement
    {
        explicit Expression(const SourceLocation& loc);
    };

    struct BinaryExpression : Expression
    {
        BinaryExpression(const SourceLocation& loc, std::string op, ExprPtr lhs, ExprPtr rhs);

        ValuePtr GenIR(Builder&) const override;
        std::ostream& Dump(std::ostream&) const override;

        std::string Op;
        ExprPtr LHS;
        ExprPtr RHS;
    };

    struct CallExpression : Expression
    {
        CallExpression(const SourceLocation& loc, ExprPtr callee, std::vector<ExprPtr>& args);

        ValuePtr GenIR(Builder&) const override;
        std::ostream& Dump(std::ostream&) const override;

        ExprPtr Callee;
        std::vector<ExprPtr> Args;
    };

    struct ConstCharExpression : Expression
    {
        ConstCharExpression(const SourceLocation& loc, char value);

        ValuePtr GenIR(Builder&) const override;
        std::ostream& Dump(std::ostream&) const override;

        char Value;
    };

    struct ConstFloatExpression : Expression
    {
        ConstFloatExpression(const SourceLocation& loc, double value);

        ValuePtr GenIR(Builder&) const override;
        std::ostream& Dump(std::ostream&) const override;

        double Value;
    };

    struct ConstIntExpression : Expression
    {
        ConstIntExpression(const SourceLocation& loc, unsigned long long value);

        ValuePtr GenIR(Builder&) const override;
        std::ostream& Dump(std::ostream&) const override;

        unsigned long long Value;
    };

    struct ConstStringExpression : Expression
    {
        ConstStringExpression(const SourceLocation& loc, std::string value);

        ValuePtr GenIR(Builder&) const override;
        std::ostream& Dump(std::ostream&) const override;

        std::string Value;
    };

    struct IndexExpression : Expression
    {
        IndexExpression(const SourceLocation& loc, ExprPtr base, ExprPtr index);

        ValuePtr GenIR(Builder&) const override;
        std::ostream& Dump(std::ostream&) const override;

        ExprPtr Base;
        ExprPtr Index;
    };

    struct SymbolExpression : Expression
    {
        SymbolExpression(const SourceLocation& loc, const std::string& name);

        ValuePtr GenIR(Builder&) const override;
        std::ostream& Dump(std::ostream&) const override;

        std::string Name;
    };

    struct UnaryExpression : Expression
    {
        UnaryExpression(const SourceLocation& loc, std::string op, ExprPtr operand, bool lh);

        ValuePtr GenIR(Builder&) const override;
        std::ostream& Dump(std::ostream&) const override;

        std::string Op;
        ExprPtr Operand;
        bool LH;
    };

    std::ostream& operator<<(std::ostream&, const StmtPtr&);
    std::ostream& operator<<(std::ostream&, const ExprPtr&);

    template <typename T>
    std::ostream& operator<<(std::ostream& stream, const std::vector<T>& v)
    {
        for (size_t i = 0; i < v.size(); ++i)
        {
            if (i > 0) stream << ", ";
            stream << v[i];
        }
        return stream;
    }
}
