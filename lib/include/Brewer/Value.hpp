#pragma once

#include <Brewer/Brewer.hpp>
#include <llvm/IR/Value.h>

namespace Brewer
{
    class Value
    {
    public:
        explicit Value(Builder&, TypePtr type);
        virtual ~Value();

        [[nodiscard]] Builder& GetBuilder() const;

        [[nodiscard]] TypePtr GetType() const;

        [[nodiscard]] llvm::Type* GetIRType() const;

        [[nodiscard]] virtual llvm::Value* Get() const = 0;

    private:
        Builder& m_Builder;
        TypePtr m_Type;
        llvm::Type* m_IRType;
    };

    class RValue : public Value
    {
    public:
        static RValuePtr Direct(Builder&, const TypePtr& type, llvm::Value* value);

        RValue(Builder&, const TypePtr& type, llvm::Value* value);

        [[nodiscard]] llvm::Value* Get() const override;

    private:
        llvm::Value* m_Value;
    };

    class LValue : public Value
    {
    public:
        static LValuePtr Alloca(Builder&, const TypePtr& type);
        static LValuePtr Direct(Builder&, const TypePtr& type, llvm::Value* ptr);

        LValue(Builder&, const TypePtr& type, llvm::Value* ptr);

        [[nodiscard]] llvm::Value* Get() const override;
        [[nodiscard]] llvm::Value* GetPtr() const;

        void Set(llvm::Value* value) const;

    private:
        llvm::Value* m_Ptr;
    };
}
