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
    const auto bkp = builder.IRBuilder().GetInsertBlock();
    const auto f = bkp->getParent();
    auto then_bb = llvm::BasicBlock::Create(builder.Context(), "then", f);
    auto else_bb = llvm::BasicBlock::Create(builder.Context(), "else", f);
    const auto end_bb = llvm::BasicBlock::Create(builder.Context(), "end", f);

    const auto condition = Condition->GenIR(builder);
    if (!condition) return {};
    builder.IRBuilder().CreateCondBr(condition->Get(), then_bb, else_bb);

    builder.IRBuilder().SetInsertPoint(then_bb);
    const auto then = Then->GenIR(builder);
    if (!then) return {};
    then_bb = builder.IRBuilder().GetInsertBlock();

    builder.IRBuilder().SetInsertPoint(else_bb);
    const auto else_ = Else->GenIR(builder);
    if (!else_) return {};
    else_bb = builder.IRBuilder().GetInsertBlock();

    const auto type = Brewer::Type::GetHigherOrder(then->GetType(), else_->GetType());
    const auto ty = type->GenIR(builder);

    builder.IRBuilder().SetInsertPoint(then_bb);
    const auto then_result = builder.GenCast(then, type)->Get();
    builder.IRBuilder().CreateBr(end_bb);

    builder.IRBuilder().SetInsertPoint(else_bb);
    const auto else_result = builder.GenCast(else_, type)->Get();
    builder.IRBuilder().CreateBr(end_bb);

    builder.IRBuilder().SetInsertPoint(end_bb);
    const auto phi = builder.IRBuilder().CreatePHI(ty, 2);
    phi->addIncoming(then_result, then_bb);
    phi->addIncoming(else_result, else_bb);

    return Brewer::RValue::Direct(builder, then->GetType(), phi);
}
