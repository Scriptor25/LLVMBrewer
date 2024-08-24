#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>

Brewer::ValuePtr Brewer::Builder::GenLAnd(Builder& builder,
                                          const ValuePtr& lhs,
                                          const ValuePtr& rhs,
                                          TypePtr* result_type)
{
    if (result_type)
    {
        *result_type = Type::Get(builder.GetContext(), "i1");
        return {};
    }

    const auto l = builder.IRBuilder().CreateIsNotNull(lhs->Get());
    const auto r = builder.IRBuilder().CreateIsNotNull(rhs->Get());

    const auto result = builder.IRBuilder().CreateAnd(l, r);
    return RValue::Direct(builder, Type::Get(builder.GetContext(), "i1"), result);
}
