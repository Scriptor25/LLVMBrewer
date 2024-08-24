#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>

Brewer::ValuePtr Brewer::Builder::GenGE(Builder& builder,
                                        const ValuePtr& lhs,
                                        const ValuePtr& rhs,
                                        TypePtr* result_type)
{
    if (result_type)
    {
        *result_type = Type::Get(builder.GetContext(), "i1");
        return {};
    }

    llvm::Value* result;
    switch (lhs->GetType()->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateICmpSGE(lhs->Get(), rhs->Get());
        break;
    case Type_Float:
        result = builder.IRBuilder().CreateFCmpOGE(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, Type::Get(builder.GetContext(), "i1"), result);
}
