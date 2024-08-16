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
        Builder(const std::string& module_id, const std::string& filename);
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

        ValuePtr GenEQ(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenNE(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenLT(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenGT(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenLE(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenGE(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenLAnd(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenLOr(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenLXor(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenAdd(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenSub(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenMul(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenDiv(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenRem(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenAnd(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenOr(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenXor(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenShl(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenShr(const ValuePtr& lhs, const ValuePtr& rhs);
        ValuePtr GenAShr(const ValuePtr& lhs, const ValuePtr& rhs);

        ValuePtr GenNeg(const ValuePtr& val);
        ValuePtr GenLNot(const ValuePtr& val);
        ValuePtr GenNot(const ValuePtr& val);

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
