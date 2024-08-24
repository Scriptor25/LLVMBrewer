#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>

Brewer::ValuePtr Brewer::Builder::GenNot(Builder& builder, const ValuePtr& val, TypePtr* result_type)
{
    const auto type = val->GetType();

    if (result_type)
    {
        *result_type = type;
        return {};
    }

    llvm::Value* result;
    switch (type->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateNot(val->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, type, result);
}
