#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>

Brewer::ConstCharExpression::ConstCharExpression(const SourceLocation& loc, const char value)
    : Expression(loc), Value(value)
{
}

std::ostream& Brewer::ConstCharExpression::Dump(std::ostream& stream) const
{
    return stream << '\'' << Value << '\'';
}

Brewer::ValuePtr Brewer::ConstCharExpression::GenIR(Builder& builder) const
{
    const auto value = builder.IRBuilder().getInt8(Value);
    return RValue::Direct(builder, Type::Get("i8"), value);
}

Brewer::ConstFloatExpression::ConstFloatExpression(const SourceLocation& loc, const double value)
    : Expression(loc), Value(value)
{
}

std::ostream& Brewer::ConstFloatExpression::Dump(std::ostream& stream) const
{
    return stream << Value;
}

Brewer::ValuePtr Brewer::ConstFloatExpression::GenIR(Builder& builder) const
{
    const auto value = llvm::ConstantFP::get(builder.IRBuilder().getDoubleTy(), Value);
    return RValue::Direct(builder, Type::Get("f64"), value);
}

Brewer::ConstIntExpression::ConstIntExpression(const SourceLocation& loc, const unsigned long long value)
    : Expression(loc), Value(value)
{
}

std::ostream& Brewer::ConstIntExpression::Dump(std::ostream& stream) const
{
    return stream << Value;
}

Brewer::ValuePtr Brewer::ConstIntExpression::GenIR(Builder& builder) const
{
    const auto value = builder.IRBuilder().getInt64(Value);
    return RValue::Direct(builder, Type::Get("i64"), value);
}

Brewer::ConstStringExpression::ConstStringExpression(const SourceLocation& loc, std::string value)
    : Expression(loc), Value(std::move(value))
{
}

std::ostream& Brewer::ConstStringExpression::Dump(std::ostream& stream) const
{
    return stream << '"' << Value << '"';
}

Brewer::ValuePtr Brewer::ConstStringExpression::GenIR(Builder& builder) const
{
    const auto value = builder.IRBuilder().CreateGlobalStringPtr(Value);
    return RValue::Direct(builder, PointerType::Get(Type::Get("i8")), value);
}
