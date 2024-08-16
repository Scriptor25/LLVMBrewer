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
    const auto f = Proto.GenIR(builder);
    if (!f || LLVMCountBasicBlocks(f)) return {};

    const auto bb = LLVMAppendBasicBlockInContext(builder.Context(), f, "entry");
    const auto bkp = LLVMGetInsertBlock(builder.IRBuilder());
    LLVMPositionBuilderAtEnd(builder.IRBuilder(), bb);

    builder.Push();
    for (size_t i = 0; i < LLVMCountParams(f); ++i)
    {
        const auto name = Proto.Params[i];
        const auto param = LLVMGetParam(f, i);
        builder[name] = Brewer::RValue::Direct(builder, Brewer::Type::Get("f64"), param);
    }

    const auto return_value = Body->GenIR(builder);
    if (!return_value)
    {
        builder.Pop();
        LLVMPositionBuilderAtEnd(builder.IRBuilder(), bkp);
        LLVMRemoveBasicBlockFromParent(bb);
        return {};
    }

    LLVMBuildRet(builder.IRBuilder(), return_value->Get());
    builder.Pop();
    LLVMPositionBuilderAtEnd(builder.IRBuilder(), bkp);
    return Brewer::RValue::Direct(builder, Brewer::PointerType::Get(Proto.GetType()), f);
}
