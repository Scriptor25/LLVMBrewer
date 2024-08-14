#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
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

Brewer::ValuePtr Brewer::CallExpression::GenIR(Builder& builder) const
{
    const auto callee = Callee->GenIR(builder);
    const auto type =
        std::dynamic_pointer_cast<FunctionType>(
            std::dynamic_pointer_cast<PointerType>(
                callee->GetType())->Base());
    const auto ty = type->GenIR(builder);

    std::vector<LLVMValueRef> args(Args.size());
    for (size_t i = 0; i < args.size(); ++i)
    {
        auto arg = Args[i]->GenIR(builder);
        arg = builder.GenCast(arg, type->Param(i));
        args[i] = arg->Get();
    }

    const auto result = LLVMBuildCall2(builder.IRBuilder(), ty, callee->Get(), args.data(), args.size(), "");
    return RValue::Direct(type->Result(), result);
}

std::ostream& Brewer::CallExpression::Dump(std::ostream& stream) const
{
    return stream << Callee << '(' << Args << ')';
}
