#include <iostream>
#include <memory>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>
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

Brewer::LValuePtr Brewer::Value::Dereference() const
{
    if (const auto type = std::dynamic_pointer_cast<PointerType>(m_Type))
        return LValue::Direct(m_Builder, type->GetBase(), Get());
    return std::cerr
        << "cannot dereference non pointer type " << m_Type->GetName()
        << std::endl
        << ErrMark<LValuePtr>();
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

Brewer::LValuePtr Brewer::LValue::Alloca(Builder& builder, const TypePtr& type, const std::string& name)
{
    const auto bkp = builder.IRBuilder().GetInsertBlock();
    builder.IRBuilder().SetInsertPointPastAllocas(bkp->getParent());
    const auto ty = type->GenIR(builder);
    const auto ptr = builder.IRBuilder().CreateAlloca(ty, nullptr, name);
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

llvm::Value* Brewer::LValue::GetPtr() const
{
    return m_Ptr;
}

void Brewer::LValue::Set(llvm::Value* value) const
{
    GetBuilder().IRBuilder().CreateStore(value, m_Ptr);
}
