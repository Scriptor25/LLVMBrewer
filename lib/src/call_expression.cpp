#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>
#include <Brewer/Value.hpp>

Brewer::CallExpression::CallExpression(const SourceLocation& loc,
                                       const TypePtr& type,
                                       ExprPtr callee,
                                       std::vector<ExprPtr>& args)
    : Expression(loc, type),
      Callee(std::move(callee))
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
                callee->GetType())->GetBase());
    if (!type)
        return std::cerr
            << "at " << Location << ": "
            << "callee must be a function pointer"
            << std::endl
            << ErrMark<ValuePtr>();

    const auto ty = type->GenIR(builder);
    if (!ty) return {};

    LValuePtr self;
    if (auto self_type = type->GetSelf())
    {
        switch (type->GetMode())
        {
        case FuncMode_Ctor:
            self = LValue::Alloca(builder, type->GetSelf(), "instance");
            break;
        case FuncMode_Member:
            {
                const auto member_callee = dynamic_cast<MemberExpression*>(Callee.get());
                const auto object = member_callee->Object->GenIR(builder);
                if (member_callee->Dereference) self = object->Dereference();
                else self = std::dynamic_pointer_cast<LValue>(object);
            }
            break;
        default:
            break;
        }
    }

    const auto off = self ? 1 : 0;
    std::vector<llvm::Value*> args(off + Args.size());
    if (off) args[0] = self->GetPtr();
    for (size_t i = off; i < args.size(); ++i)
    {
        auto arg = Args[i]->GenIR(builder);
        if (!arg) return {};
        if (auto dest = type->GetParam(i - off))
        {
            arg = builder.GenCast(arg, dest);
            if (!arg) return {};
        }
        args[i] = arg->Get();
    }

    const auto result = builder.IRBuilder().CreateCall(ty, callee->Get(), args);
    if (!result)
        return std::cerr
            << "at " << Location << ": "
            << "failed to create call"
            << std::endl
            << ErrMark<ValuePtr>();

    if (type->GetMode() == FuncMode_Ctor)
        return self;

    return RValue::Direct(builder, type->GetResult(), result);
}
