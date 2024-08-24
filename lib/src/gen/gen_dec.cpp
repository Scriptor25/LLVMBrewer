#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>

Brewer::ValuePtr Brewer::Builder::GenDec(Builder& builder, const ValuePtr& val, TypePtr* result_type)
{
    const auto type = val->GetType();

    if (result_type)
    {
        *result_type = type;
        return {};
    }

    llvm::Value* one;
    switch (type->GetID())
    {
    case Type_Integer:
        one = llvm::ConstantInt::get(val->GetIRType(), 1, true);
        break;
    case Type_Float:
        one = llvm::ConstantFP::get(val->GetIRType(), 1.0);
        break;
    default:
        return {};
    }

    return builder.GenBinaryFn("-")(builder, val, RValue::Direct(builder, type, one), result_type);
}
