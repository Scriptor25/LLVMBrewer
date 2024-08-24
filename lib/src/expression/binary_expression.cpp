#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>
#include <Brewer/Value.hpp>

Brewer::BinaryExpression::BinaryExpression(const SourceLocation& loc,
                                           const TypePtr& type,
                                           std::string operator_,
                                           ExprPtr lhs,
                                           ExprPtr rhs)
    : Expression(loc, type),
      Operator(std::move(operator_)),
      LHS(std::move(lhs)),
      RHS(std::move(rhs))
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
        if (auto dest = LValue::From(lhs))
        {
            const auto src = builder.GenCast(rhs, lhs->GetType());
            dest->Set(src->Get());
            return dest;
        }

        return std::cerr
            << "at " << Location << ": "
            << "cannot assign to rvalue"
            << std::endl
            << ErrMark<ValuePtr>();
    }

    auto l = lhs;
    auto r = rhs;

    if (l->GetType() != r->GetType())
    {
        const auto type = Type::GetHigherOrder(l->GetType(), r->GetType());
        if (!type) return {};
        l = builder.GenCast(l, type);
        if (!l) return {};
        r = builder.GenCast(r, type);
        if (!r) return {};
    }

    if (const auto& fn = builder.GenBinaryFn(Operator))
    {
        if (auto result = fn(builder, l, r, {}))
            return result;

        return std::cerr
            << "at " << Location << ": "
            << "undefined binary operator "
            << "'" << lhs->GetType() << " " << Operator << rhs->GetType() << "'"
            << std::endl
            << ErrMark<ValuePtr>();
    }

    if (const auto pos = Operator.find('='); pos != std::string::npos)
    {
        const auto op = Operator.substr(0, pos);
        if (const auto& fn = builder.GenBinaryFn(op))
        {
            if (const auto result = fn(builder, l, r, {}))
            {
                if (auto dest = LValue::From(lhs))
                {
                    const auto src = builder.GenCast(result, lhs->GetType());
                    dest->Set(src->Get());
                    return dest;
                }

                return std::cerr
                    << "at " << Location << ": "
                    << "cannot assign to rvalue"
                    << std::endl
                    << ErrMark<ValuePtr>();
            }
        }
    }

    return std::cerr
        << "at " << Location << ": "
        << "undefined binary operator "
        << "'" << lhs->GetType() << " " << Operator << rhs->GetType() << "'"
        << std::endl
        << ErrMark<ValuePtr>();
}
