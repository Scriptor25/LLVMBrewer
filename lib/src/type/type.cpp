#include <iostream>
#include <Brewer/Builder.hpp>
#include <Brewer/Context.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>

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

bool Brewer::Type::IsFuncPtr() const
{
    return m_ID == Type_Pointer && dynamic_cast<const PointerType*>(this)->GetBase()->IsFunction();
}
