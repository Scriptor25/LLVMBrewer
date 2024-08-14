#include <memory>
#include <stdexcept>
#include <Brewer/Value.hpp>

Brewer::Value::Value(TypePtr type)
    : m_Type(std::move(type))
{
}

Brewer::TypePtr Brewer::Value::GetType()
{
    return m_Type;
}

Brewer::Value::~Value() = default;

Brewer::RValuePtr Brewer::RValue::Direct(const TypePtr& type, LLVMValueRef value)
{
    return std::make_unique<RValue>(type, value);
}

Brewer::RValue::RValue(const TypePtr& type, LLVMValueRef value)
    : Value(type), m_Value(value)
{
}

LLVMValueRef Brewer::RValue::Get()
{
    return m_Value;
}

Brewer::LValue::LValue(const TypePtr& type, LLVMValueRef ptr)
    : Value(type), m_Ptr(ptr)
{
}

LLVMValueRef Brewer::LValue::Get()
{
    throw std::runtime_error("TODO");
}

void Brewer::LValue::Set(LLVMValueRef val)
{
    throw std::runtime_error("TODO");
}
