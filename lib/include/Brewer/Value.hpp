#pragma once

#include <Brewer/Brewer.hpp>
#include <llvm-c/Types.h>

namespace Brewer
{
    class Value
    {
    public:
        explicit Value(TypePtr type);
        TypePtr GetType();

        virtual ~Value();
        virtual LLVMValueRef Get() = 0;

    private:
        TypePtr m_Type;
    };

    class RValue : public Value
    {
    public:
        static RValuePtr Direct(const TypePtr& type, LLVMValueRef value);

        RValue(const TypePtr& type, LLVMValueRef value);
        LLVMValueRef Get() override;

    private:
        LLVMValueRef m_Value;
    };

    class LValue : public Value
    {
    public:
        LValue(const TypePtr& type, LLVMValueRef ptr);
        LLVMValueRef Get() override;
        void Set(LLVMValueRef val);

    private:
        LLVMValueRef m_Ptr;
    };
}
