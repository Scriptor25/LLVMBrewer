#include <memory>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>
#include <llvm-c/Core.h>

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

LLVMTypeRef Brewer::Value::GetIRType() const
{
    return m_IRType;
}

Brewer::Value::~Value() = default;

Brewer::RValuePtr Brewer::RValue::Direct(Builder& builder, const TypePtr& type, LLVMValueRef value)
{
    return std::make_unique<RValue>(builder, type, value);
}

Brewer::RValue::RValue(Builder& builder, const TypePtr& type, LLVMValueRef value)
    : Value(builder, type), m_Value(value)
{
}

LLVMValueRef Brewer::RValue::Get() const
{
    return m_Value;
}

void Brewer::RValue::Erase() const
{
    if (!LLVMIsConstant(m_Value))
        LLVMInstructionEraseFromParent(m_Value);
}

Brewer::LValue::LValue(Builder& builder, const TypePtr& type, LLVMValueRef ptr)
    : Value(builder, type), m_Ptr(ptr)
{
}

LLVMValueRef Brewer::LValue::Get() const
{
    return LLVMBuildLoad2(GetBuilder().IRBuilder(), GetIRType(), m_Ptr, "");
}

void Brewer::LValue::Erase() const
{
    if (!LLVMIsConstant(m_Ptr))
        LLVMInstructionEraseFromParent(m_Ptr);
}

void Brewer::LValue::Set(LLVMValueRef value) const
{
    LLVMBuildStore(GetBuilder().IRBuilder(), value, m_Ptr);
}
