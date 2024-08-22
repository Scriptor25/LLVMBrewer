#include <memory>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>

Brewer::Value::Value(Builder& builder, TypePtr type)
    : m_Builder(builder), m_Type(std::move(type)), m_IRType(m_Type->GenIR(m_Builder))
{
}

Brewer::Value::~Value() = default;

Brewer::Builder& Brewer::Value::GetBuilder() const
{
    return m_Builder;
}

Brewer::TypePtr Brewer::Value::GetType() const
{
    return m_Type;
}

llvm::Type* Brewer::Value::GetIRType() const
{
    return m_IRType;
}

Brewer::RValuePtr Brewer::RValue::Direct(Builder& builder, const TypePtr& type, llvm::Value* value)
{
    return std::make_shared<RValue>(builder, type, value);
}

Brewer::RValue::RValue(Builder& builder, const TypePtr& type, llvm::Value* value)
    : Value(builder, type), m_Value(value)
{
}

llvm::Value* Brewer::RValue::Get() const
{
    return m_Value;
}

Brewer::LValuePtr Brewer::LValue::Alloca(Builder& builder, const TypePtr& type)
{
    const auto bkp = builder.IRBuilder().GetInsertBlock();
    builder.IRBuilder().SetInsertPointPastAllocas(bkp->getParent());
    const auto ty = type->GenIR(builder);
    const auto ptr = builder.IRBuilder().CreateAlloca(ty);
    builder.IRBuilder().SetInsertPoint(bkp);
    return Direct(builder, type, ptr);
}

Brewer::LValuePtr Brewer::LValue::Direct(Builder& builder, const TypePtr& type, llvm::Value* ptr)
{
    return std::make_shared<LValue>(builder, type, ptr);
}

Brewer::LValue::LValue(Builder& builder, const TypePtr& type, llvm::Value* ptr)
    : Value(builder, type), m_Ptr(ptr)
{
}

llvm::Value* Brewer::LValue::Get() const
{
    return GetBuilder().IRBuilder().CreateLoad(GetIRType(), m_Ptr);
}

void Brewer::LValue::Set(llvm::Value* value) const
{
    GetBuilder().IRBuilder().CreateStore(value, m_Ptr);
}
