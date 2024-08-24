#pragma once

#include <string>
#include <vector>
#include <Brewer/AST.hpp>
#include <Brewer/Brewer.hpp>
#include <llvm/IR/Value.h>

namespace Test
{
    struct Prototype
    {
        Prototype(std::string name, const std::vector<std::string>& params);

        std::ostream& Dump(std::ostream& stream) const;
        llvm::Function* GenIR(Brewer::Builder& builder) const;
        [[nodiscard]] Brewer::PointerTypePtr GetType(Brewer::Context&) const;

        std::string Name;
        std::vector<std::string> Params;
    };

    std::ostream& operator<<(std::ostream& stream, const Prototype& proto);

    struct DefStatement : Brewer::Statement
    {
        DefStatement(const Brewer::SourceLocation& loc,
                     Prototype proto,
                     Brewer::ExprPtr body);

        std::ostream& Dump(std::ostream& stream) const override;
        void GenIRNoVal(Brewer::Builder& builder) const override;

        Prototype Proto;
        Brewer::ExprPtr Body;
    };

    struct ExternStatement : Brewer::Statement
    {
        ExternStatement(const Brewer::SourceLocation& loc, Prototype proto);

        std::ostream& Dump(std::ostream& stream) const override;
        void GenIRNoVal(Brewer::Builder& builder) const override;

        Prototype Proto;
    };

    struct IfExpression : Brewer::Expression
    {
        IfExpression(const Brewer::SourceLocation&,
                     const Brewer::TypePtr&,
                     Brewer::ExprPtr condition,
                     Brewer::ExprPtr then,
                     Brewer::ExprPtr else_);

        std::ostream& Dump(std::ostream& stream) const override;
        Brewer::ValuePtr GenIR(Brewer::Builder& builder) const override;

        Brewer::ExprPtr Condition;
        Brewer::ExprPtr Then;
        Brewer::ExprPtr Else;
    };
}
