#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>
#include <Test/AST.hpp>

using namespace Brewer;

Test::IfExpression::IfExpression(const SourceLocation& loc,
                                 const TypePtr& type,
                                 ExprPtr condition,
                                 ExprPtr then,
                                 ExprPtr else_)
    : Expression(loc, type), Condition(std::move(condition)), Then(std::move(then)), Else(std::move(else_))
{
}

std::ostream& Test::IfExpression::Dump(std::ostream& stream) const
{
    return stream << "if " << Condition << " then " << Then << " else " << Else;
}

ValuePtr Test::IfExpression::GenIR(Builder& builder) const
{
    const auto bkp = builder.IRBuilder().GetInsertBlock();
    const auto f = bkp->getParent();
    auto then_bb = llvm::BasicBlock::Create(builder.IRContext(), "then", f);
    auto else_bb = llvm::BasicBlock::Create(builder.IRContext(), "else", f);
    const auto end_bb = llvm::BasicBlock::Create(builder.IRContext(), "end", f);

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

    const auto type = Type::GetHigherOrder(then->GetType(), else_->GetType());
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

    return RValue::Direct(builder, then->GetType(), phi);
}
