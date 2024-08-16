#pragma once

#include <Brewer/Brewer.hpp>
#include <llvm-c/Types.h>

namespace Brewer
{
    class Value
    {
    public:
        explicit Value(Builder&, TypePtr type);
        [[nodiscard]] Builder& GetBuilder() const;
        TypePtr GetType();
        [[nodiscard]] LLVMTypeRef GetIRType() const;

        virtual ~Value();
        [[nodiscard]] virtual LLVMValueRef Get() const = 0;
        virtual void Erase() const = 0;

    private:
        Builder& m_Builder;
        TypePtr m_Type;
        LLVMTypeRef m_IRType;
    };

    class RValue : public Value
    {
    public:
        static RValuePtr Direct(Builder&, const TypePtr& type, LLVMValueRef value);

        RValue(Builder&, const TypePtr& type, LLVMValueRef value);
        [[nodiscard]] LLVMValueRef Get() const override;
        void Erase() const override;

    private:
        LLVMValueRef m_Value;
    };

    class LValue : public Value
    {
    public:
        LValue(Builder&, const TypePtr& type, LLVMValueRef ptr);
        [[nodiscard]] LLVMValueRef Get() const override;
        void Erase() const override;
        void Set(LLVMValueRef value) const;

    private:
        LLVMValueRef m_Ptr;
    };
}
