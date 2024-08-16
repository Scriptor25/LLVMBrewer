#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>
#include <Brewer/Value.hpp>

Brewer::CallExpression::CallExpression(const SourceLocation& loc,
                                       ExprPtr callee,
                                       std::vector<ExprPtr>& args)
    : Expression(loc), Callee(std::move(callee))
{
    Args.reserve(args.size());
    for (auto& arg : args)
        Args.push_back(std::move(arg));
}

std::ostream& Brewer::CallExpression::Dump(std::ostream& stream) const
{
    return stream << Callee << '(' << Args << ')';
}

Brewer::ValuePtr Brewer::CallExpression::GenIR(Builder& builder) const
{
    const auto callee = Callee->GenIR(builder);
    if (!callee) return {};

    const auto type =
        std::dynamic_pointer_cast<FunctionType>(
            std::dynamic_pointer_cast<PointerType>(
                callee->GetType())->Base());
    if (!type)
    {
        callee->Erase();
        return error<ValuePtr>("at %s(%ull,%ull): callee must be of type function pointer",
                               Location.Filename.c_str(),
                               Location.Row,
                               Location.Column);
    }

    const auto ty = type->GenIR(builder);
    if (!ty)
    {
        callee->Erase();
        return {};
    }

    std::vector<LLVMValueRef> args(Args.size());
    for (size_t i = 0; i < args.size(); ++i)
    {
        auto arg = Args[i]->GenIR(builder);
        arg = builder.GenCast(arg, type->Param(i));
        if (!arg)
        {
            callee->Erase();
            return {};
        }
        args[i] = arg->Get();
    }

    const auto result = LLVMBuildCall2(builder.IRBuilder(), ty, callee->Get(), args.data(), args.size(), "");
    if (!result)
    {
        callee->Erase();
        return error<ValuePtr>("at %s(%llu,%llu): failed to create call",
                               Location.Filename.c_str(),
                               Location.Row,
                               Location.Column);
    }

    return RValue::Direct(builder, type->Result(), result);
}
