#include <Brewer/AST.hpp>
#include <Brewer/Parser.hpp>
#include <Brewer/Type.hpp>

Brewer::ExprPtr Brewer::Parser::ParseIndex()
{
    auto base = ParseMember();
    if (!base) return {};
    base = ParseIndex(std::move(base));
    if (At(".") || At("!"))
        base = ParseMember(std::move(base));
    return base;
}

Brewer::ExprPtr Brewer::Parser::ParseIndex(ExprPtr base)
{
    while (At("["))
    {
        auto [Location, Type, Value] = Skip();

        auto index = ParseExpr();
        Expect("]");

        TypePtr element;
        if (const auto type = PointerType::From(base->Type)) element = type->GetBase();
        if (const auto type = ArrayType::From(base->Type)) element = type->GetBase();
        base = std::make_unique<IndexExpression>(Location, element, std::move(base), std::move(index));
    }

    return base;
}
