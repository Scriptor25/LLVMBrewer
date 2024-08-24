#include <Brewer/Type.hpp>

Brewer::ArrayTypePtr Brewer::ArrayType::From(const TypePtr& type)
{
    return std::dynamic_pointer_cast<ArrayType>(type);
}

Brewer::ArrayTypePtr Brewer::ArrayType::Get(const TypePtr& base, const size_t length)
{
    auto name = base->GetName() + '[' + std::to_string(length) + ']';
    auto& type = Type::Get(base->GetContext(), name);
    if (!type)
        type = std::make_shared<ArrayType>(name, base, length);
    return From(type);
}

Brewer::ArrayType::ArrayType(const std::string& name, const TypePtr& base, const size_t length)
    : Type(base->GetContext(), name, Type_Array, base->GetSize() * length), m_Base(base), m_Length(length)
{
}

llvm::ArrayType* Brewer::ArrayType::GenIR(Builder& builder) const
{
    return llvm::ArrayType::get(m_Base->GenIR(builder), m_Length);
}

Brewer::TypePtr Brewer::ArrayType::GetBase() const
{
    return m_Base;
}

size_t Brewer::ArrayType::GetLength() const
{
    return m_Length;
}
