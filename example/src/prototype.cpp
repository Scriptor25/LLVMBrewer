#include <iostream>
#include <Brewer/Builder.hpp>
#include <Brewer/Context.hpp>
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
    const auto fn_ty = llvm::cast<llvm::FunctionType>(type->GetBase()->GenIR(builder));
    const auto fn = llvm::Function::Create(fn_ty, llvm::GlobalValue::ExternalLinkage, Name, builder.IRModule());

    for (size_t i = 0; i < Params.size(); ++i)
        fn->getArg(i)->setName(Params[i]);

    builder[Name] = Brewer::RValue::Direct(builder, type, fn);
    return fn;
}

Brewer::PointerTypePtr Test::Prototype::GetType(Brewer::Context& context) const
{
    const auto flt_ty = context.GetFloat64Ty();
    const std::vector params(Params.size(), flt_ty);
    return Brewer::Type::GetFunPtr(Brewer::FuncMode_Normal, {}, flt_ty, params, false);
}

std::ostream& Test::operator<<(std::ostream& stream, const Prototype& proto)
{
    return proto.Dump(stream);
}
