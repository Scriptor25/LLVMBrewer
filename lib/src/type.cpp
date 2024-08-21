#include <iostream>
#include <map>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>

Brewer::TypePtr& Brewer::Type::Get(const std::string& name)
{
    static std::map<std::string, TypePtr> types{
        {"void", std::make_shared<Type>("void", Type_Void, 0)},
        {"i1", std::make_shared<Type>("i1", Type_Integer, 1)},
        {"i8", std::make_shared<Type>("i8", Type_Integer, 8)},
        {"i16", std::make_shared<Type>("i16", Type_Integer, 16)},
        {"i32", std::make_shared<Type>("i32", Type_Integer, 32)},
        {"i64", std::make_shared<Type>("i64", Type_Integer, 64)},
        {"f16", std::make_shared<Type>("f16", Type_Float, 16)},
        {"f32", std::make_shared<Type>("f32", Type_Float, 32)},
        {"f64", std::make_shared<Type>("f64", Type_Float, 64)},
    };

    return types[name];
}

Brewer::TypePtr Brewer::Type::GetHigherOrder(const TypePtr& a, const TypePtr& b)
{
    if (a == b)
        return a;

    if (a->IsInt())
    {
        if (b->IsInt())
        {
            if (a->Size() >= b->Size())
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
            if (a->Size() >= b->Size())
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

Brewer::Type::Type(std::string name, const TypeID id, const size_t size)
    : m_Name(std::move(name)), m_ID(id), m_Size(size)
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

const std::string& Brewer::Type::Name() const
{
    return m_Name;
}

size_t Brewer::Type::Size() const
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
    const auto name = base->Name() + "*";
    auto& type = Type::Get(name);
    if (!type)
        type = std::make_shared<PointerType>(name, base);
    return std::dynamic_pointer_cast<PointerType>(type);
}

Brewer::PointerType::PointerType(const std::string& name, TypePtr base)
    : Type(name, Type_Pointer, 64), m_Base(std::move(base))
{
}

llvm::PointerType* Brewer::PointerType::GenIR(Builder& builder) const
{
    return llvm::PointerType::get(builder.Context(), 0);
}

Brewer::TypePtr Brewer::PointerType::Base() const
{
    return m_Base;
}

Brewer::ArrayTypePtr Brewer::ArrayType::Get(const TypePtr& base, const size_t length)
{
    auto name = base->Name() + '[' + std::to_string(length) + ']';
    auto& type = Type::Get(name);
    if (!type)
        type = std::make_shared<ArrayType>(name, base, length);
    return std::dynamic_pointer_cast<ArrayType>(type);
}

Brewer::ArrayType::ArrayType(const std::string& name, const TypePtr& base, const size_t length)
    : Type(name, Type_Array, base->Size() * length), m_Base(base), m_Length(length)
{
}

llvm::ArrayType* Brewer::ArrayType::GenIR(Builder& builder) const
{
    return llvm::ArrayType::get(m_Base->GenIR(builder), m_Length);
}


Brewer::StructTypePtr Brewer::StructType::Get(const std::vector<TypePtr>& elements)
{
    std::string name;
    size_t size = 0;
    name += '{';
    for (size_t i = 0; i < elements.size(); ++i)
    {
        if (i > 0) name += ',';
        name += elements[i]->Name();
        size += elements[i]->Size();
    }
    name += '}';
    auto& type = Type::Get(name);
    if (!type)
        type = std::make_shared<StructType>(name, size, elements);
    return std::dynamic_pointer_cast<StructType>(type);
}

Brewer::StructType::StructType(const std::string& name, const size_t size, const std::vector<TypePtr>& elements)
    : Type(name, Type_Struct, size), m_Elements(elements)
{
}

llvm::StructType* Brewer::StructType::GenIR(Builder& builder) const
{
    std::vector<llvm::Type*> elements(m_Elements.size());
    for (size_t i = 0; i < elements.size(); ++i)
        elements[i] = m_Elements[i]->GenIR(builder);
    return llvm::StructType::get(builder.Context(), elements, false);
}

Brewer::FunctionTypePtr Brewer::FunctionType::Get(const TypePtr& result,
                                                  const std::vector<TypePtr>& params,
                                                  const bool vararg)
{
    std::string name = result->Name() + '(';
    for (size_t i = 0; i < params.size(); ++i)
    {
        if (i > 0) name += ',';
        name += params[i]->Name();
    }
    if (vararg)
    {
        if (!params.empty())
            name += ',';
        name += '?';
    }
    name += ')';
    auto& type = Type::Get(name);
    if (!type)
        type = std::make_shared<FunctionType>(name, result, params, vararg);
    return std::dynamic_pointer_cast<FunctionType>(type);
}

Brewer::FunctionType::FunctionType(const std::string& name,
                                   TypePtr result,
                                   const std::vector<TypePtr>& params,
                                   const bool vararg)
    : Type(name, Type_Function, 0), m_Result(std::move(result)), m_Params(params), m_VarArg(vararg)
{
}

llvm::FunctionType* Brewer::FunctionType::GenIR(Builder& builder) const
{
    std::vector<llvm::Type*> params(m_Params.size());
    for (size_t i = 0; i < params.size(); ++i)
        params[i] = m_Params[i]->GenIR(builder);
    return llvm::FunctionType::get(m_Result->GenIR(builder), params, m_VarArg);
}

Brewer::TypePtr Brewer::FunctionType::Result()
{
    return m_Result;
}

Brewer::TypePtr Brewer::FunctionType::Param(const size_t i)
{
    return m_Params[i];
}
