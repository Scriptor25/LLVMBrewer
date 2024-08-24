#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>

Brewer::PointerTypePtr Brewer::PointerType::Get(const TypePtr& base)
{
    const auto name = base->GetName() + "*";
    auto& type = Type::Get(base->GetContext(), name);
    if (!type)
        type = std::make_shared<PointerType>(name, base);
    return std::dynamic_pointer_cast<PointerType>(type);
}

Brewer::PointerType::PointerType(const std::string& name, const TypePtr& base)
    : Type(base->GetContext(), name, Type_Pointer, 64), m_Base(base)
{
}

llvm::PointerType* Brewer::PointerType::GenIR(Builder& builder) const
{
    return llvm::PointerType::get(builder.IRContext(), 0);
}

Brewer::TypePtr Brewer::PointerType::GetBase() const
{
    return m_Base;
}
