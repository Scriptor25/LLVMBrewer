#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>
#include <Test/AST.hpp>

Test::IfExpression::IfExpression(const Brewer::SourceLocation& loc,
                                 Brewer::ExprPtr condition,
                                 Brewer::ExprPtr then,
                                 Brewer::ExprPtr else_)
    : Expression(loc), Condition(std::move(condition)), Then(std::move(then)), Else(std::move(else_))
{
}

std::ostream& Test::IfExpression::Dump(std::ostream& stream) const
{
    return stream << "if " << Condition << " then " << Then << " else " << Else;
}

Brewer::ValuePtr Test::IfExpression::GenIR(Brewer::Builder& builder) const
{
    const auto bkp = LLVMGetInsertBlock(builder.IRBuilder());
    const auto f = LLVMGetBasicBlockParent(bkp);
    auto then_bb = LLVMAppendBasicBlockInContext(builder.Context(), f, "then");
    auto else_bb = LLVMAppendBasicBlockInContext(builder.Context(), f, "else");
    const auto end_bb = LLVMAppendBasicBlockInContext(builder.Context(), f, "end");

    const auto condition = Condition->GenIR(builder);
    if (!condition)
    {
        LLVMPositionBuilderAtEnd(builder.IRBuilder(), bkp);
        LLVMRemoveBasicBlockFromParent(then_bb);
        LLVMRemoveBasicBlockFromParent(else_bb);
        LLVMRemoveBasicBlockFromParent(end_bb);
        return {};
    }
    LLVMBuildCondBr(builder.IRBuilder(), condition->Get(), then_bb, else_bb);

    LLVMPositionBuilderAtEnd(builder.IRBuilder(), then_bb);
    const auto then = Then->GenIR(builder);
    if (!then)
    {
        LLVMPositionBuilderAtEnd(builder.IRBuilder(), bkp);
        LLVMRemoveBasicBlockFromParent(then_bb);
        LLVMRemoveBasicBlockFromParent(else_bb);
        LLVMRemoveBasicBlockFromParent(end_bb);
        return {};
    }
    then_bb = LLVMGetInsertBlock(builder.IRBuilder());

    LLVMPositionBuilderAtEnd(builder.IRBuilder(), else_bb);
    const auto else_ = Else->GenIR(builder);
    if (!else_)
    {
        LLVMPositionBuilderAtEnd(builder.IRBuilder(), bkp);
        LLVMRemoveBasicBlockFromParent(then_bb);
        LLVMRemoveBasicBlockFromParent(else_bb);
        LLVMRemoveBasicBlockFromParent(end_bb);
        return {};
    }
    else_bb = LLVMGetInsertBlock(builder.IRBuilder());

    const auto type = Brewer::Type::GetHigherOrder(then->GetType(), else_->GetType());
    const auto ty = type->GenIR(builder);

    LLVMPositionBuilderAtEnd(builder.IRBuilder(), then_bb);
    auto then_result = builder.GenCast(then, type)->Get();
    LLVMBuildBr(builder.IRBuilder(), end_bb);

    LLVMPositionBuilderAtEnd(builder.IRBuilder(), else_bb);
    auto else_result = builder.GenCast(else_, type)->Get();
    LLVMBuildBr(builder.IRBuilder(), end_bb);

    LLVMPositionBuilderAtEnd(builder.IRBuilder(), end_bb);
    const auto phi = LLVMBuildPhi(builder.IRBuilder(), ty, "");
    LLVMAddIncoming(phi, &then_result, &then_bb, 1);
    LLVMAddIncoming(phi, &else_result, &else_bb, 1);

    return Brewer::RValue::Direct(builder, then->GetType(), phi);
}
