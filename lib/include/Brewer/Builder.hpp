#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <Brewer/Brewer.hpp>
#include <llvm-c/Core.h>

namespace Brewer
{
    class Builder
    {
    public:
        explicit Builder(const std::string& name);
        ~Builder();

        [[nodiscard]] const LLVMContextRef& Context() const;
        [[nodiscard]] const LLVMBuilderRef& IRBuilder() const;
        [[nodiscard]] const LLVMModuleRef& Module() const;

        void Close() const;

        void Dump() const;
        void EmitToFile(const std::string& filename) const;

        void Push();
        void Pop();
        ValuePtr& operator[](const std::string& name);

        [[nodiscard]] ValuePtr GenEQ(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenNE(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenLT(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenGT(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenLE(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenGE(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenLAnd(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenLOr(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenLXor(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenAdd(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenSub(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenMul(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenDiv(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenRem(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenAnd(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenOr(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenXor(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenShl(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenShr(const ValuePtr& lhs, const ValuePtr& rhs) const;
        [[nodiscard]] ValuePtr GenAShr(const ValuePtr& lhs, const ValuePtr& rhs) const;

        ValuePtr GenCast(const ValuePtr& value, const TypePtr& type);

    private:
        LLVMContextRef m_Context;
        LLVMBuilderRef m_IRBuilder;
        LLVMModuleRef m_Module;

        LLVMValueRef m_Global;

        std::vector<std::map<std::string, ValuePtr>> m_Stack;
        std::map<std::string, ValuePtr> m_Values;
    };
}
