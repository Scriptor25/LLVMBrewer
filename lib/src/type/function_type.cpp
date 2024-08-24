#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>

Brewer::FunctionTypePtr Brewer::FunctionType::From(const TypePtr& type)
{
    return std::dynamic_pointer_cast<FunctionType>(type);
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
    return From(type);
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
    const auto off = m_Self ? 1 : 0;
    std::vector<llvm::Type*> params(off + m_Params.size());
    if (off) params[0] = llvm::PointerType::get(builder.IRContext(), 0);
    for (size_t i = off; i < params.size(); ++i)
        params[i] = m_Params[i - off]->GenIR(builder);
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
