#include <memory>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>

Brewer::Value::Value(Builder& builder, TypePtr type)
    : m_Builder(builder), m_Type(std::move(type)), m_IRType(m_Type->GenIR(m_Builder))
{
}

Brewer::Builder& Brewer::Value::GetBuilder() const
{
    return m_Builder;
}

Brewer::TypePtr Brewer::Value::GetType()
{
    return m_Type;
}

llvm::Type* Brewer::Value::GetIRType() const
{
    return m_IRType;
}

Brewer::Value::~Value() = default;

Brewer::RValuePtr Brewer::RValue::Direct(Builder& builder, const TypePtr& type, llvm::Value* value)
{
    return std::make_unique<RValue>(builder, type, value);
}

Brewer::RValue::RValue(Builder& builder, const TypePtr& type, llvm::Value* value)
    : Value(builder, type), m_Value(value)
{
}

llvm::Value* Brewer::RValue::Get() const
{
    return m_Value;
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
