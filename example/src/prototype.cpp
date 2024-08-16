#include <iostream>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>
#include <Brewer/Value.hpp>
#include <Test/AST.hpp>

Test::Prototype::Prototype(std::string name, const std::vector<std::string>& params)
    : Name(std::move(name)), Params(params)
{
}

std::ostream& Test::Prototype::Dump(std::ostream& stream) const
{
    return Brewer::operator<<(stream << Name << '(', Params) << ')';
}

LLVMValueRef Test::Prototype::GenIR(Brewer::Builder& builder) const
{
    auto f = LLVMGetNamedFunction(builder.Module(), Name.c_str());
    if (f) return f;

    f = LLVMAddFunction(builder.Module(), Name.c_str(), GetType()->GenIR(builder));
    if (!f) return {};

    for (size_t i = 0; i < LLVMCountParams(f); ++i)
        LLVMSetValueName(LLVMGetParam(f, i), Params[i].c_str());

    builder[Name] = Brewer::RValue::Direct(builder, Brewer::PointerType::Get(GetType()), f);
    return f;
}

[[nodiscard]] Brewer::TypePtr Test::Prototype::GetType() const
{
    const std::vector params(Params.size(), Brewer::Type::Get("f64"));
    return Brewer::FunctionType::Get(Brewer::Type::Get("f64"), params, false);
}

std::ostream& Test::operator<<(std::ostream& stream, const Prototype& proto)
{
    return proto.Dump(stream);
}
