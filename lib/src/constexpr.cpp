#include <Brewer/AST.hpp>
#include <llvm-c/Core.h>

#include "Brewer/Type.hpp"
#include "Brewer/Value.hpp"

Brewer::ConstCharExpression::ConstCharExpression(const SourceLocation& loc, const char value)
    : Expression(loc), Value(value)
{
}

Brewer::ValuePtr Brewer::ConstCharExpression::GenIR(Builder&) const
{
    throw std::runtime_error("TODO");
}

std::ostream& Brewer::ConstCharExpression::Dump(std::ostream& stream) const
{
    return stream << '\'' << Value << '\'';
}

Brewer::ConstFloatExpression::ConstFloatExpression(const SourceLocation& loc, const double value)
    : Expression(loc), Value(value)
{
}

Brewer::ValuePtr Brewer::ConstFloatExpression::GenIR(Builder& builder) const
{
    const auto type = Type::Get("f64");
    const auto ty = type->GenIR(builder);
    const auto value = LLVMConstReal(ty, Value);
    return RValue::Direct(type, value);
}

std::ostream& Brewer::ConstFloatExpression::Dump(std::ostream& stream) const
{
    return stream << Value;
}

Brewer::ConstIntExpression::ConstIntExpression(const SourceLocation& loc, const unsigned long long value)
    : Expression(loc), Value(value)
{
}

Brewer::ValuePtr Brewer::ConstIntExpression::GenIR(Builder& builder) const
{
    const auto type = Type::Get("i64");
    const auto ty = type->GenIR(builder);
    const auto value = LLVMConstInt(ty, Value, true);
    return RValue::Direct(type, value);
}

std::ostream& Brewer::ConstIntExpression::Dump(std::ostream& stream) const
{
    return stream << Value;
}

Brewer::ConstStringExpression::ConstStringExpression(const SourceLocation& loc, std::string value)
    : Expression(loc), Value(std::move(value))
{
}

Brewer::ValuePtr Brewer::ConstStringExpression::GenIR(Builder&) const
{
    throw std::runtime_error("TODO");
}

std::ostream& Brewer::ConstStringExpression::Dump(std::ostream& stream) const
{
    return stream << '"' << Value << '"';
}
