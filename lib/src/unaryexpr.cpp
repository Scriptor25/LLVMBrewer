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

    ValuePtr result;
    bool assign = false;
    if (Operator == "++")
    {
        const auto one = LLVMConstInt(operand->GetIRType(), 1, true);
        result = builder.GenAdd(operand, RValue::Direct(builder, operand->GetType(), one));
        assign = true;
    }
    else if (Operator == "--")
    {
        const auto one = LLVMConstInt(operand->GetIRType(), 1, true);
        result = builder.GenSub(operand, RValue::Direct(builder, operand->GetType(), one));
        assign = true;
    }
    else if (Operator == "-") result = builder.GenNeg(operand);
    else if (Operator == "!") result = builder.GenLNot(operand);
    else if (Operator == "~") result = builder.GenNot(operand);

    if (result)
    {
        if (assign)
        {
            if (auto dest = std::dynamic_pointer_cast<LValue>(operand))
            {
                LLVMValueRef pre = {};
                if (!LH) pre = dest->Get();
                dest->Set(result->Get());
                if (LH) return dest;
                return RValue::Direct(builder, operand->GetType(), pre);
            }

            result->Erase();
            operand->Erase();
            return error<ValuePtr>("at %s(%llu,%llu): cannot assign to rvalue\n",
                                   Location.Filename.c_str(),
                                   Location.Row,
                                   Location.Column);
        }
        return result;
    }

    operand->Erase();
    return error<ValuePtr>("at %s(%llu,%llu): undefined unary operator '%s%s'\n",
                           Location.Filename.c_str(),
                           Location.Row,
                           Location.Column,
                           Operator.c_str(),
                           operand->GetType()->Name().c_str());
}
