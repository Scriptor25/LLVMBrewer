#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>
#include <Brewer/Value.hpp>

Brewer::BinaryExpression::BinaryExpression(const SourceLocation& loc,
                                           std::string operator_,
                                           ExprPtr lhs,
                                           ExprPtr rhs)
    : Expression(loc), Operator(std::move(operator_)), LHS(std::move(lhs)), RHS(std::move(rhs))
{
}

std::ostream& Brewer::BinaryExpression::Dump(std::ostream& stream) const
{
    return stream << LHS << ' ' << Operator << ' ' << RHS;
}

Brewer::ValuePtr Brewer::BinaryExpression::GenIR(Builder& builder) const
{
    const auto lhs = LHS->GenIR(builder);
    if (!lhs) return {};
    const auto rhs = RHS->GenIR(builder);
    if (!rhs) return {};

    if (Operator == "=")
    {
        if (auto dest = std::dynamic_pointer_cast<LValue>(lhs))
        {
            const auto src = builder.GenCast(rhs, lhs->GetType());
            dest->Set(src->Get());
            return dest;
        }

        return error<ValuePtr>("at %s(%llu,%llu): cannot assign to rvalue\n",
                               Location.Filename.c_str(),
                               Location.Row,
                               Location.Column);
    }

    auto l = lhs;
    auto r = rhs;

    if (l->GetType() != r->GetType())
    {
        const auto type = Type::GetHigherOrder(l->GetType(), r->GetType());
        l = builder.GenCast(l, type);
        if (!l) return {};
        r = builder.GenCast(r, type);
        if (!r) return {};
    }

    if (const auto& fn = builder.GenBinaryFn(Operator))
    {
        if (auto result = fn(builder, l, r))
            return result;

        return error<ValuePtr>("at %s(%llu,%llu): undefined binary operator '%s %s %s'\n",
                               Location.Filename.c_str(),
                               Location.Row,
                               Location.Column,
                               lhs->GetType()->Name().c_str(),
                               Operator.c_str(),
                               rhs->GetType()->Name().c_str());
    }

    const auto pos = Operator.find('=');
    if (pos == std::string::npos)
        return error<ValuePtr>("at %s(%llu,%llu): undefined binary operator '%s %s %s'\n",
                               Location.Filename.c_str(),
                               Location.Row,
                               Location.Column,
                               lhs->GetType()->Name().c_str(),
                               Operator.c_str(),
                               rhs->GetType()->Name().c_str());

    const auto op = Operator.substr(0, pos);
    if (const auto& fn = builder.GenBinaryFn(op))
    {
        if (const auto result = fn(builder, l, r))
        {
            if (auto dest = std::dynamic_pointer_cast<LValue>(lhs))
            {
                const auto src = builder.GenCast(result, lhs->GetType());
                dest->Set(src->Get());
                return dest;
            }

            return error<ValuePtr>("at %s(%llu,%llu): cannot assign to rvalue\n",
                                   Location.Filename.c_str(),
                                   Location.Row,
                                   Location.Column);
        }
    }

    return error<ValuePtr>("at %s(%llu,%llu): undefined binary operator '%s %s %s'\n",
                           Location.Filename.c_str(),
                           Location.Row,
                           Location.Column,
                           lhs->GetType()->Name().c_str(),
                           Operator.c_str(),
                           rhs->GetType()->Name().c_str());
}
