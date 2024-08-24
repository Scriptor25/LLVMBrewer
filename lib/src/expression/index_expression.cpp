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

    if (PointerType::From(Base->Type))
    {
        const auto gep = builder.IRBuilder().CreateGEP(e_ty, base->Get(), {index->Get()});
        return LValue::Direct(builder, Type, gep);
    }

    if (const auto type = ArrayType::From(Base->Type))
    {
        const auto ty = type->GenIR(builder);

        const auto ptr = LValue::From(base)->GetPtr();
        const auto gep = builder.IRBuilder().CreateGEP(ty, ptr, {builder.IRBuilder().getInt64(0), index->Get()});
        return LValue::Direct(builder, Type, gep);
    }

    return std::cerr
        << "at " << Location << ": "
        << "can only index into pointer or array"
        << std::endl
        << ErrMark<ValuePtr>();
}
