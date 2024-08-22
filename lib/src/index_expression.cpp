#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>
#include <Brewer/Value.hpp>

Brewer::IndexExpression::IndexExpression(const SourceLocation& loc, const TypePtr& type, ExprPtr base, ExprPtr index)
    : Expression(loc, type), Base(std::move(base)), Index(std::move(index))
{
}

std::ostream& Brewer::IndexExpression::Dump(std::ostream& stream) const
{
    return stream << Base << '[' << Index << ']';
}

Brewer::ValuePtr Brewer::IndexExpression::GenIR(Builder& builder) const
{
    const auto base = Base->GenIR(builder);
    const auto index = Index->GenIR(builder);
    const auto e_ty = Type->GenIR(builder);

    if (Base->Type->IsPointer())
    {
        const auto ptr_type = std::dynamic_pointer_cast<PointerType>(Base->Type);

        llvm::Value* ptr;
        if (const auto b = std::dynamic_pointer_cast<LValue>(base)) ptr = b->GetPtr();
        else ptr = base->Get();

        const auto gep = builder.IRBuilder().CreateGEP(e_ty, ptr, {index->Get()});
        return LValue::Direct(builder, Type, gep);
    }

    if (Base->Type->IsArray())
    {
        const auto arr_type = std::dynamic_pointer_cast<ArrayType>(Base->Type);
        const auto ty = arr_type->GenIR(builder);

        const auto ptr = std::dynamic_pointer_cast<LValue>(base)->GetPtr();

        const auto gep = builder.IRBuilder().CreateGEP(ty, ptr, {builder.IRBuilder().getInt64(0), index->Get()});
        return LValue::Direct(builder, Type, gep);
    }

    return std::cerr
        << "at " << Location << ": "
        << "can only index into pointer or array"
        << std::endl
        << ErrMark<ValuePtr>();
}
