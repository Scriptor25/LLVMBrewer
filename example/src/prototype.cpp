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

llvm::Function* Test::Prototype::GenIR(Brewer::Builder& builder) const
{
    if (const auto fn = builder.IRModule().getFunction(Name))
        return fn;

    const auto type = GetType(builder.GetContext());
    const auto fn_ty = type->GenIR(builder);
    const auto fn = llvm::Function::Create(fn_ty, llvm::GlobalValue::ExternalLinkage, Name, builder.IRModule());

    for (size_t i = 0; i < Params.size(); ++i)
        fn->getArg(i)->setName(Params[i]);

    builder[Name] = Brewer::RValue::Direct(builder, Brewer::PointerType::Get(type), fn);
    return fn;
}

Brewer::FunctionTypePtr Test::Prototype::GetType(Brewer::Context& context) const
{
    const std::vector params(Params.size(), Brewer::Type::Get(context, "f64"));
    return Brewer::FunctionType::Get(Brewer::Type::Get(context, "f64"), params, false);
}

std::ostream& Test::operator<<(std::ostream& stream, const Prototype& proto)
{
    return proto.Dump(stream);
}
