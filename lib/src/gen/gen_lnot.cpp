#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>

Brewer::ValuePtr Brewer::Builder::GenLNot(Builder& builder, const ValuePtr& val, TypePtr* result_type)
{
    if (result_type)
    {
        *result_type = Type::Get(builder.GetContext(), "i1");
        return {};
    }

    const auto result = builder.IRBuilder().CreateIsNull(val->Get());
    return RValue::Direct(builder, Type::Get(builder.GetContext(), "i1"), result);
}
