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
        // predefined binary operators
        static ValuePtr GenEQ(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenNE(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenLT(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenGT(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenLE(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenGE(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenLAnd(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenLOr(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenLXor(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenAdd(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenSub(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenMul(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenDiv(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenRem(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenAnd(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenOr(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenXor(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenShl(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenLShr(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);
        static ValuePtr GenAShr(Builder&, const ValuePtr& lhs, const ValuePtr& rhs, TypePtr*);

        // predefined unary operators
        static ValuePtr GenInc(Builder&, const ValuePtr&, TypePtr*);
        static ValuePtr GenDec(Builder&, const ValuePtr&, TypePtr*);
        static ValuePtr GenNeg(Builder&, const ValuePtr&, TypePtr*);
        static ValuePtr GenLNot(Builder&, const ValuePtr&, TypePtr*);
        static ValuePtr GenNot(Builder&, const ValuePtr&, TypePtr*);

        Builder(Context&, const std::string& module_id, const std::string& filename);

        [[nodiscard]] llvm::Function* GetGlobalCtor() const;
        [[nodiscard]] llvm::Function* GetGlobalDtor() const;
        void CloseGlobals() const;

        [[nodiscard]] Context& GetContext() const;
        [[nodiscard]] llvm::LLVMContext& IRContext() const;
        [[nodiscard]] llvm::IRBuilder<>& IRBuilder() const;
        [[nodiscard]] llvm::Module& IRModule() const;

        BinaryFn& GenBinaryFn(const std::string& operator_);
        UnaryFn& GenUnaryFn(const std::string& operator_);

        void Dump() const;
        void EmitToFile(const std::string& filename) const;

        ValuePtr& GetFunction(const TypePtr&, const std::string&);
        ValuePtr GetCtor(const TypePtr&);

        ValuePtr& GetSymbol(const std::string& name);

        void Push();
        void Pop();

        TypePtr& CurrentResult();

        ValuePtr GenCast(const ValuePtr& src, const TypePtr& dst);

    private:
        Context& m_Context;

        std::unique_ptr<llvm::LLVMContext> m_IRContext;
        std::unique_ptr<llvm::IRBuilder<>> m_IRBuilder;
        std::unique_ptr<llvm::Module> m_IRModule;

        llvm::Function *m_GlobalCtor, *m_GlobalDtor;

        std::map<std::string, BinaryFn> m_BinaryFns;
        std::map<std::string, UnaryFn> m_UnaryFns;

        std::map<TypePtr, std::map<std::string, ValuePtr>> m_Functions;
        std::vector<std::map<std::string, ValuePtr>> m_Stack;
        std::map<std::string, ValuePtr> m_Symbols;

        TypePtr m_CurrentResult;
    };
}
