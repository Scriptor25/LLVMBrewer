#include <Brewer/Context.hpp>
#include <Brewer/Type.hpp>

Brewer::Context::Context()
{
    m_Types["void"] = std::make_shared<Type>(*this, "void", Type_Void, 0);
    m_Types["i1"] = std::make_shared<Type>(*this, "i1", Type_Integer, 1);
    m_Types["i8"] = std::make_shared<Type>(*this, "i8", Type_Integer, 8);
    m_Types["i16"] = std::make_shared<Type>(*this, "i16", Type_Integer, 16);
    m_Types["i32"] = std::make_shared<Type>(*this, "i32", Type_Integer, 32);
    m_Types["i64"] = std::make_shared<Type>(*this, "i64", Type_Integer, 64);
    m_Types["f16"] = std::make_shared<Type>(*this, "f16", Type_Float, 16);
    m_Types["f32"] = std::make_shared<Type>(*this, "f32", Type_Float, 32);
    m_Types["f64"] = std::make_shared<Type>(*this, "f64", Type_Float, 64);
}

Brewer::TypePtr& Brewer::Context::GetType(const std::string& name)
{
    return m_Types[name];
}

Brewer::TypePtr& Brewer::Context::GetSymbol(const std::string& name)
{
    return m_Symbols[name];
}

Brewer::TypePtr Brewer::Context::GetVoidTy()
{
    return m_Types["void"];
}

Brewer::TypePtr Brewer::Context::GetIntNTy(size_t n)
{
    switch (n)
    {
    case 1: return m_Types["i1"];
    case 8: return m_Types["i8"];
    case 16: return m_Types["i16"];
    case 32: return m_Types["i32"];
    case 64: return m_Types["i64"];
    default: return {};
    }
}

Brewer::TypePtr Brewer::Context::GetInt1Ty()
{
    return m_Types["i1"];
}

Brewer::TypePtr Brewer::Context::GetInt8Ty()
{
    return m_Types["i8"];
}

Brewer::TypePtr Brewer::Context::GetInt16Ty()
{
    return m_Types["i16"];
}

Brewer::TypePtr Brewer::Context::GetInt32Ty()
{
    return m_Types["i32"];
}

Brewer::TypePtr Brewer::Context::GetInt64Ty()
{
    return m_Types["i64"];
}

Brewer::TypePtr Brewer::Context::GetFloatNTy(size_t n)
{
    switch (n)
    {
    case 16: return m_Types["f16"];
    case 32: return m_Types["f32"];
    case 64: return m_Types["f64"];
    default: return {};
    }
}

Brewer::TypePtr Brewer::Context::GetFloat16Ty()
{
    return m_Types["f16"];
}

Brewer::TypePtr Brewer::Context::GetFloat32Ty()
{
    return m_Types["f32"];
}

Brewer::TypePtr Brewer::Context::GetFloat64Ty()
{
    return m_Types["f64"];
}

Brewer::TypePtr& Brewer::Context::CurrentResult()
{
    return m_CurrentResult;
}

void Brewer::Context::Push()
{
    m_SymbolStack.push_back(m_Symbols);
}

void Brewer::Context::Pop()
{
    m_Symbols = m_SymbolStack.back();
    m_SymbolStack.pop_back();
}
