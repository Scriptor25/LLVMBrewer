#include <iostream>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>
#include <Test/AST.hpp>

Test::FunctionStatement::FunctionStatement(const Brewer::SourceLocation& loc,
                                           Prototype proto,
                                           Brewer::ExprPtr body)
    : Statement(loc), Proto(std::move(proto)), Body(std::move(body))
{
}

std::ostream& Test::FunctionStatement::Dump(std::ostream& stream) const
{
    return stream << "def " << Proto << ' ' << Body;
}

Brewer::ValuePtr Test::FunctionStatement::GenIR(Brewer::Builder& builder) const
{
    const auto fn = Proto.GenIR(builder);
    if (!fn || !fn->empty()) return {};

    const auto bb = llvm::BasicBlock::Create(builder.Context(), "entry", fn);
    const auto bkp = builder.IRBuilder().GetInsertBlock();
    builder.IRBuilder().SetInsertPoint(bb);

    builder.Push();
    for (size_t i = 0; i < Proto.Params.size(); ++i)
    {
        const auto name = Proto.Params[i];
        const auto param = fn->getArg(i);
        builder[name] = Brewer::RValue::Direct(builder, Brewer::Type::Get("f64"), param);
    }

    const auto return_value = Body->GenIR(builder);
    if (!return_value)
    {
        builder.Pop();
        builder.IRBuilder().SetInsertPoint(bkp);
        fn->erase(fn->begin(), fn->end());
        return {};
    }

    builder.IRBuilder().CreateRet(return_value->Get());
    builder.Pop();
    builder.IRBuilder().SetInsertPoint(bkp);
    return Brewer::RValue::Direct(builder, Brewer::PointerType::Get(Proto.GetType()), fn);
}
