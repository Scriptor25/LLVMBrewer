#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Util.hpp>

Brewer::SymbolExpression::SymbolExpression(const SourceLocation& loc, const TypePtr& type, std::string name)
    : Expression(loc, type), Name(std::move(name))
{
}

std::ostream& Brewer::SymbolExpression::Dump(std::ostream& stream) const
{
    return stream << Name;
}

Brewer::ValuePtr Brewer::SymbolExpression::GenIR(Builder& builder) const
{
    if (const auto& value = builder.GetSymbol(Name)) return value;
    if (const auto& value = builder.GetFunction({}, Name)) return value;
    return std::cerr
        << "at " << Location << ": "
        << "no such symbol '" << Name << "'"
        << std::endl
        << ErrMark<ValuePtr>();
}
