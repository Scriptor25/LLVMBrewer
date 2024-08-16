#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>
#include <Test/AST.hpp>

Test::ExternStatement::ExternStatement(const Brewer::SourceLocation& loc, Prototype proto)
    : Statement(loc), Proto(std::move(proto))
{
}

std::ostream& Test::ExternStatement::Dump(std::ostream& stream) const
{
    return stream << "extern " << Proto;
}

Brewer::ValuePtr Test::ExternStatement::GenIR(Brewer::Builder& builder) const
{
    const auto f = Proto.GenIR(builder);
    if (!f) return {};
    return Brewer::RValue::Direct(builder, Brewer::PointerType::Get(Proto.GetType()), f);
}
