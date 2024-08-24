#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>

Brewer::StructElement::StructElement(TypePtr type, std::string name)
    : Type(std::move(type)), Name(std::move(name))
{
}

Brewer::StructTypePtr Brewer::StructType::From(const TypePtr& type)
{
    return std::dynamic_pointer_cast<StructType>(type);
}

Brewer::StructTypePtr Brewer::StructType::Get(const std::vector<StructElement>& elements)
{
    std::string name;
    size_t size = 0;
    name += "{ ";
    for (size_t i = 0; i < elements.size(); ++i)
    {
        if (i > 0) name += ", ";
        name += elements[i].Type->GetName();
        size += elements[i].Type->GetSize();
    }
    name += " }";
    auto& type = Type::Get(elements[0].Type->GetContext(), name);
    if (!type)
        type = std::make_shared<StructType>(elements[0].Type->GetContext(), name, size, elements);
    return From(type);
}

Brewer::StructTypePtr Brewer::StructType::Get(Context& context)
{
    std::string name = "{}";

    auto& type = Type::Get(context, name);
    if (!type)
        type = std::make_shared<StructType>(context, name, 0, std::vector<StructElement>());
    return From(type);
}

Brewer::StructType::StructType(Context& context,
                               const std::string& name,
                               const size_t size,
                               const std::vector<StructElement>& elements)
    : Type(context, name, Type_Struct, size), m_Elements(elements)
{
}

llvm::StructType* Brewer::StructType::GenIR(Builder& builder) const
{
    std::vector<llvm::Type*> elements(m_Elements.size());
    for (size_t i = 0; i < elements.size(); ++i)
        elements[i] = m_Elements[i].Type->GenIR(builder);
    return llvm::StructType::get(builder.IRContext(), elements, false);
}

Brewer::StructElement& Brewer::StructType::GetElement(const size_t i)
{
    return m_Elements[i];
}

Brewer::TypePtr Brewer::StructType::GetElement(const std::string& name, size_t& index)
{
    for (size_t i = 0; i < m_Elements.size(); ++i)
        if (m_Elements[i].Name == name)
        {
            index = i;
            return m_Elements[i].Type;
        }
    return {};
}
