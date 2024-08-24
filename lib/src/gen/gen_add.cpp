#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>

Brewer::ValuePtr Brewer::Builder::GenAdd(Builder& builder,
                                         const ValuePtr& lhs,
                                         const ValuePtr& rhs,
                                         TypePtr* result_type)
{
    const auto type = lhs->GetType();

    if (result_type)
    {
        *result_type = type;
        return {};
    }

    llvm::Value* result;
    switch (type->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateAdd(lhs->Get(), rhs->Get());
        break;
    case Type_Float:
        result = builder.IRBuilder().CreateFAdd(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, type, result);
}
