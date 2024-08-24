#include <iostream>
#include <Brewer/Builder.hpp>
#include <Brewer/Context.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>
#include <utility>

Brewer::TypePtr& Brewer::Type::Get(Context& context, const std::string& name)
{
    return context.GetType(name);
}

Brewer::PointerTypePtr Brewer::Type::GetFunPtr(const FuncMode mode,
                                               const TypePtr& self,
                                               const TypePtr& result,
                                               const std::vector<TypePtr>& params,
                                               const bool vararg)
{
    return PointerType::Get(FunctionType::Get(mode, self, result, params, vararg));
}

Brewer::TypePtr Brewer::Type::GetHigherOrder(const TypePtr& a, const TypePtr& b)
{
    if (a == b)
        return a;

    if (a->IsInt())
    {
        if (b->IsInt())
        {
            if (a->GetSize() >= b->GetSize())
                return a;
            return b;
        }

        if (b->IsFloat())
            return b;

        if (b->IsPointer())
            return a;
    }

    if (a->IsFloat())
    {
        if (b->IsInt())
            return a;

        if (b->IsFloat())
        {
            if (a->GetSize() >= b->GetSize())
                return a;
            return b;
        }

        if (b->IsPointer())
            return a;
    }

    if (a->IsPointer())
    {
        if (b->IsInt())
            return b;

        if (b->IsFloat())
            return b;
    }

    return std::cerr
        << "cannot determine higher order type of "
        << a
        << " and "
        << b
        << std::endl
        << ErrMark<TypePtr>();
}

Brewer::Type::Type(Context& context, std::string name, const TypeID id, const size_t size)
    : m_Context(context), m_Name(std::move(name)), m_ID(id), m_Size(size)
{
}

Brewer::Type::~Type() = default;

llvm::Type* Brewer::Type::GenIR(Builder& builder) const
{
    switch (m_ID)
    {
    case Type_Void: return builder.IRBuilder().getVoidTy();
    case Type_Integer: return builder.IRBuilder().getIntNTy(m_Size);
    case Type_Float:
        switch (m_Size)
        {
        case 16: return builder.IRBuilder().getHalfTy();
        case 32: return builder.IRBuilder().getFloatTy();
        case 64: return builder.IRBuilder().getDoubleTy();
        default: return {};
        }
    default: return {};
    }
}

Brewer::Context& Brewer::Type::GetContext() const
{
    return m_Context;
}

const std::string& Brewer::Type::GetName() const
{
    return m_Name;
}

Brewer::TypeID Brewer::Type::GetID() const
{
    return m_ID;
}

size_t Brewer::Type::GetSize() const
{
    return m_Size;
}

bool Brewer::Type::IsVoid() const
{
    return m_ID == Type_Void;
}

bool Brewer::Type::IsInt(const size_t size) const
{
    return m_ID == Type_Integer && (!size || m_Size == size);
}

bool Brewer::Type::IsInt1() const
{
    return IsInt(1);
}

bool Brewer::Type::IsInt8() const
{
    return IsInt(8);
}

bool Brewer::Type::IsInt16() const
{
    return IsInt(16);
}

bool Brewer::Type::IsInt32() const
{
    return IsInt(32);
}

bool Brewer::Type::IsInt64() const
{
    return IsInt(64);
}

bool Brewer::Type::IsFloat(const size_t size) const
{
    return m_ID == Type_Float && (!size || m_Size == size);
}

bool Brewer::Type::IsFloat16() const
{
    return IsFloat(16);
}

bool Brewer::Type::IsFloat32() const
{
    return IsFloat(32);
}

bool Brewer::Type::IsFloat64() const
{
    return IsFloat(64);
}

bool Brewer::Type::IsPointer() const
{
    return m_ID == Type_Pointer;
}

bool Brewer::Type::IsArray() const
{
    return m_ID == Type_Array;
}

bool Brewer::Type::IsStruct() const
{
    return m_ID == Type_Struct;
}

bool Brewer::Type::IsFunction() const
{
    return m_ID == Type_Function;
}

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

Brewer::ArrayTypePtr Brewer::ArrayType::Get(const TypePtr& base, const size_t length)
{
    auto name = base->GetName() + '[' + std::to_string(length) + ']';
    auto& type = Type::Get(base->GetContext(), name);
    if (!type)
        type = std::make_shared<ArrayType>(name, base, length);
    return std::dynamic_pointer_cast<ArrayType>(type);
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

Brewer::StructElement::StructElement(TypePtr type, std::string name)
    : Type(std::move(type)), Name(std::move(name))
{
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
    return std::dynamic_pointer_cast<StructType>(type);
}

Brewer::StructTypePtr Brewer::StructType::Get(Context& context)
{
    std::string name = "{}";

    auto& type = Type::Get(context, name);
    if (!type)
        type = std::make_shared<StructType>(context, name, 0, std::vector<StructElement>());
    return std::dynamic_pointer_cast<StructType>(type);
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

Brewer::FunctionTypePtr Brewer::FunctionType::Get(const FuncMode mode,
                                                  const TypePtr& self,
                                                  const TypePtr& result,
                                                  const std::vector<TypePtr>& params,
                                                  const bool vararg)
{
    std::string name = result->GetName();
    if (self)
    {
        name += '(';
        switch (mode)
        {
        case FuncMode_Ctor:
            name += '+';
            break;
        case FuncMode_Dtor:
            name += '-';
            break;
        case FuncMode_Member:
            name += ':';
            break;
        default:
            name += '?';
            break;
        }
        name += self->GetName();
        name += ')';
    }
    name += '(';
    for (size_t i = 0; i < params.size(); ++i)
    {
        if (i > 0) name += ',';
        name += params[i]->GetName();
    }
    if (vararg)
    {
        if (!params.empty())
            name += ',';
        name += '?';
    }
    name += ')';
    auto& type = Type::Get(result->GetContext(), name);
    if (!type)
        type = std::make_shared<FunctionType>(name, mode, self, result, params, vararg);
    return std::dynamic_pointer_cast<FunctionType>(type);
}

Brewer::FunctionType::FunctionType(const std::string& name,
                                   const FuncMode mode,
                                   TypePtr self,
                                   TypePtr result,
                                   const std::vector<TypePtr>& params,
                                   const bool vararg)
    : Type(result->GetContext(), name, Type_Function, 0),
      m_Mode(mode),
      m_Self(std::move(self)),
      m_Result(std::move(result)),
      m_Params(params),
      m_VarArg(vararg)
{
}

llvm::FunctionType* Brewer::FunctionType::GenIR(Builder& builder) const
{
    std::vector<llvm::Type*> params((m_Self ? 1 : 0) + m_Params.size());
    if (m_Self) params[0] = llvm::PointerType::get(builder.IRContext(), 0);
    for (size_t i = m_Self ? 1 : 0; i < params.size(); ++i)
        params[i] = m_Params[i]->GenIR(builder);
    return llvm::FunctionType::get(m_Result->GenIR(builder), params, m_VarArg);
}

Brewer::FuncMode Brewer::FunctionType::GetMode() const
{
    return m_Mode;
}

Brewer::TypePtr Brewer::FunctionType::GetSelf() const
{
    return m_Self;
}

Brewer::TypePtr Brewer::FunctionType::GetResult() const
{
    return m_Result;
}

Brewer::TypePtr Brewer::FunctionType::GetParam(const size_t i) const
{
    if (i >= m_Params.size() && m_VarArg) return {};
    return m_Params[i];
}

bool Brewer::FunctionType::IsVarArg() const
{
    return m_VarArg;
}
