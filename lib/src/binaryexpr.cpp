#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>

Brewer::BinaryExpression::BinaryExpression(const SourceLocation& loc,
                                           std::string op,
                                           ExprPtr lhs,
                                           ExprPtr rhs)
    : Expression(loc), Op(std::move(op)), LHS(std::move(lhs)), RHS(std::move(rhs))
{
}

Brewer::ValuePtr Brewer::BinaryExpression::GenIR(Builder& builder) const
{
    const auto lhs = LHS->GenIR(builder);
    const auto rhs = RHS->GenIR(builder);

    if (Op == "=")
    {
        if (auto dest = std::dynamic_pointer_cast<LValue>(lhs))
        {
            const auto src = builder.GenCast(rhs, lhs->GetType());
            dest->Set(src->Get());
            return dest;
        }
        throw std::runtime_error("cannot assign to rvalue");
    }

    auto l = lhs;
    auto r = rhs;

    if (l->GetType() != r->GetType())
    {
        auto type = Type::GetHigherOrder(l->GetType(), r->GetType());
        l = builder.GenCast(l, type);
        r = builder.GenCast(r, type);
    }

    ValuePtr result;
    if (Op == "==") result = builder.GenEQ(l, r);
    else if (Op == "!=") result = builder.GenNE(l, r);
    else if (Op == "<") result = builder.GenLT(l, r);
    else if (Op == ">") result = builder.GenGT(l, r);
    else if (Op == "<=") result = builder.GenLE(l, r);
    else if (Op == ">=") result = builder.GenGE(l, r);
    else if (Op == "&&") result = builder.GenLAnd(l, r);
    else if (Op == "||") result = builder.GenLOr(l, r);
    else if (Op == "^^") result = builder.GenLXor(l, r);

    if (result)
        return result;

    const auto pos = Op.find('=');
    const auto assign = pos != std::string::npos;
    const auto op = assign ? Op.substr(0, pos) : Op;

    if (Op == "+") result = builder.GenAdd(l, r);
    else if (Op == "-") result = builder.GenSub(l, r);
    else if (Op == "*") result = builder.GenMul(l, r);
    else if (Op == "/") result = builder.GenDiv(l, r);
    else if (Op == "%") result = builder.GenRem(l, r);
    else if (Op == "&") result = builder.GenAnd(l, r);
    else if (Op == "|") result = builder.GenOr(l, r);
    else if (Op == "^") result = builder.GenXor(l, r);
    else if (Op == "<<") result = builder.GenShl(l, r);
    else if (Op == ">>") result = builder.GenShr(l, r);
    else if (Op == ">>>") result = builder.GenAShr(l, r);

    if (result)
    {
        if (assign)
        {
            if (auto dest = std::dynamic_pointer_cast<LValue>(lhs))
            {
                const auto src = builder.GenCast(result, lhs->GetType());
                dest->Set(src->Get());
                return dest;
            }
            throw std::runtime_error("cannot assign to rvalue");
        }
        return result;
    }

    throw std::runtime_error("undefined operator");
}

std::ostream& Brewer::BinaryExpression::Dump(std::ostream& stream) const
{
    return stream << LHS << ' ' << Op << ' ' << RHS;
}
