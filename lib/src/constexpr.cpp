#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>

Brewer::ConstCharExpression::ConstCharExpression(const SourceLocation& loc, const TypePtr& type, const char value)
    : Expression(loc, type), Value(value)
{
}

std::ostream& Brewer::ConstCharExpression::Dump(std::ostream& stream) const
{
    return stream << '\'' << Value << '\'';
}

Brewer::ValuePtr Brewer::ConstCharExpression::GenIR(Builder& builder) const
{
    const auto value = builder.IRBuilder().getInt8(Value);
    return RValue::Direct(builder, Type::Get(builder.GetContext(), "i8"), value);
}

Brewer::ConstFloatExpression::ConstFloatExpression(const SourceLocation& loc, const TypePtr& type, const double value)
    : Expression(loc, type), Value(value)
{
}

std::ostream& Brewer::ConstFloatExpression::Dump(std::ostream& stream) const
{
    return stream << Value;
}

Brewer::ValuePtr Brewer::ConstFloatExpression::GenIR(Builder& builder) const
{
    const auto value = llvm::ConstantFP::get(builder.IRBuilder().getDoubleTy(), Value);
    return RValue::Direct(builder, Type::Get(builder.GetContext(), "f64"), value);
}

Brewer::ConstIntExpression::ConstIntExpression(const SourceLocation& loc, const TypePtr& type, const size_t value)
    : Expression(loc, type), Value(value)
{
}

std::ostream& Brewer::ConstIntExpression::Dump(std::ostream& stream) const
{
    return stream << Value;
}

Brewer::ValuePtr Brewer::ConstIntExpression::GenIR(Builder& builder) const
{
    const auto value = builder.IRBuilder().getInt64(Value);
    return RValue::Direct(builder, Type::Get(builder.GetContext(), "i64"), value);
}

Brewer::ConstStringExpression::ConstStringExpression(const SourceLocation& loc, const TypePtr& type, std::string value)
    : Expression(loc, type), Value(std::move(value))
{
}

std::ostream& Brewer::ConstStringExpression::Dump(std::ostream& stream) const
{
    return stream << '"' << Value << '"';
}

Brewer::ValuePtr Brewer::ConstStringExpression::GenIR(Builder& builder) const
{
    const auto value = builder.IRBuilder().CreateGlobalStringPtr(Value);
    return RValue::Direct(builder, PointerType::Get(Type::Get(builder.GetContext(), "i8")), value);
}
