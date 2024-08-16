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
    if (!rhs)
    {
        lhs->Erase();
        return {};
    }

    if (Operator == "=")
    {
        if (auto dest = std::dynamic_pointer_cast<LValue>(lhs))
        {
            const auto src = builder.GenCast(rhs, lhs->GetType());
            dest->Set(src->Get());
            return dest;
        }

        lhs->Erase();
        rhs->Erase();
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
        if (!l)
        {
            lhs->Erase();
            rhs->Erase();
            return {};
        }
        r = builder.GenCast(r, type);
        if (!r)
        {
            lhs->Erase();
            rhs->Erase();
            if (l != lhs)l->Erase();
            return {};
        }
    }

    if (const auto& fn = builder.GenBinaryFn(Operator))
    {
        if (auto result = fn(builder, l, r))
            return result;

        lhs->Erase();
        rhs->Erase();
        if (l != lhs) l->Erase();
        if (r != rhs) r->Erase();
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
    {
        lhs->Erase();
        rhs->Erase();
        if (l != lhs) l->Erase();
        if (r != rhs) r->Erase();
        return error<ValuePtr>("at %s(%llu,%llu): undefined binary operator '%s %s %s'\n",
                               Location.Filename.c_str(),
                               Location.Row,
                               Location.Column,
                               lhs->GetType()->Name().c_str(),
                               Operator.c_str(),
                               rhs->GetType()->Name().c_str());
    }

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

            lhs->Erase();
            rhs->Erase();
            if (l != lhs) l->Erase();
            if (r != rhs) r->Erase();
            result->Erase();
            return error<ValuePtr>("at %s(%llu,%llu): cannot assign to rvalue\n",
                                   Location.Filename.c_str(),
                                   Location.Row,
                                   Location.Column);
        }
    }

    lhs->Erase();
    rhs->Erase();
    if (l != lhs) l->Erase();
    if (r != rhs) r->Erase();
    return error<ValuePtr>("at %s(%llu,%llu): undefined binary operator '%s %s %s'\n",
                           Location.Filename.c_str(),
                           Location.Row,
                           Location.Column,
                           lhs->GetType()->Name().c_str(),
                           Operator.c_str(),
                           rhs->GetType()->Name().c_str());
}
