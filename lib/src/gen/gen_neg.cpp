#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>

Brewer::ValuePtr Brewer::Builder::GenNeg(Builder& builder, const ValuePtr& val, TypePtr* result_type)
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
        result = builder.IRBuilder().CreateNeg(val->Get());
        break;
    case Type_Float:
        result = builder.IRBuilder().CreateFNeg(val->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, type, result);
}
