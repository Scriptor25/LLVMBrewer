#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <Brewer/Brewer.hpp>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace Brewer
{
    class Builder
    {
    public:
        static ValuePtr GenEQ(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenNE(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenLT(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenGT(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenLE(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenGE(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenLAnd(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenLOr(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenLXor(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenAdd(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenSub(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenMul(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenDiv(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenRem(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenAnd(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenOr(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenXor(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenShl(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenLShr(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);
        static ValuePtr GenAShr(Builder&, const ValuePtr& lhs, const ValuePtr& rhs);

        static ValuePtr GenInc(Builder&, const ValuePtr& val);
        static ValuePtr GenDec(Builder&, const ValuePtr& val);
        static ValuePtr GenNeg(Builder&, const ValuePtr& val);
        static ValuePtr GenLNot(Builder&, const ValuePtr& val);
        static ValuePtr GenNot(Builder&, const ValuePtr& val);

        Builder(Context&, const std::string& module_id, const std::string& filename);

        [[nodiscard]] Context& GetContext() const;
        [[nodiscard]] llvm::LLVMContext& IRContext() const;
        [[nodiscard]] llvm::IRBuilder<>& IRBuilder() const;
        [[nodiscard]] llvm::Module& IRModule() const;

        BinaryFn& GenBinaryFn(const std::string& operator_);
        UnaryFn& GenUnaryFn(const std::string& operator_);

        void Dump() const;
        void EmitToFile(const std::string& filename) const;

        void Push();
        void Pop();
        ValuePtr& operator[](const std::string& name);

        ValuePtr GenCast(const ValuePtr& src, const TypePtr& dst);

    private:
        Context& m_Context;

        std::unique_ptr<llvm::LLVMContext> m_IRContext;
        std::unique_ptr<llvm::IRBuilder<>> m_IRBuilder;
        std::unique_ptr<llvm::Module> m_IRModule;

        std::map<std::string, BinaryFn> m_BinaryFns;
        std::map<std::string, UnaryFn> m_UnaryFns;

        std::vector<std::map<std::string, ValuePtr>> m_Stack;
        std::map<std::string, ValuePtr> m_Values;
    };
}
