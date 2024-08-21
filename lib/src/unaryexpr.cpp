#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>
#include <Brewer/Value.hpp>

Brewer::UnaryExpression::UnaryExpression(const SourceLocation& loc,
                                         std::string operator_,
                                         ExprPtr operand,
                                         const bool lh)
    : Expression(loc), Operator(std::move(operator_)), Operand(std::move(operand)), LH(lh)
{
}

std::ostream& Brewer::UnaryExpression::Dump(std::ostream& stream) const
{
    if (LH)
        return stream << Operator << Operand;
    return stream << Operand << Operator;
}

Brewer::ValuePtr Brewer::UnaryExpression::GenIR(Builder& builder) const
{
    const auto operand = Operand->GenIR(builder);
    if (!operand) return {};

    if (const auto& fn = builder.GenUnaryFn(Operator))
    {
        const bool assign = Operator == "++" || Operator == "--";
        if (auto result = fn(builder, operand))
        {
            if (assign)
            {
                if (auto dest = std::dynamic_pointer_cast<LValue>(operand))
                {
                    llvm::Value* pre;
                    if (!LH) pre = dest->Get();
                    dest->Set(result->Get());
                    if (LH) return dest;
                    return RValue::Direct(builder, operand->GetType(), pre);
                }

                return std::cerr << "at " << Location << ": "
                    << "cannot assign to rvalue"
                    << std::endl
                    << ErrMark<ValuePtr>();
            }
            return result;
        }
    }

    return std::cerr << "at " << Location << ": "
        << "undefined unary operator "
        << "'" << Operator << operand->GetType() << "'"
        << std::endl
        << ErrMark<ValuePtr>();
}
