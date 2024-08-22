#include <Brewer/AST.hpp>
#include <Brewer/Parser.hpp>
#include <Brewer/Type.hpp>

Brewer::ExprPtr Brewer::Parser::ParseCall()
{
    auto base = ParseCall(ParseUnary());
    if (!base) return {};
    if (At("["))
        base = ParseIndex(std::move(base));
    if (At(".") || At("!"))
        base = ParseMember(std::move(base));
    return base;
}

Brewer::ExprPtr Brewer::Parser::ParseCall(ExprPtr callee)
{
    while (At("("))
    {
        auto [Location, Type, Value] = Skip();

        std::vector<ExprPtr> args;
        while (!NextIfAt(")"))
        {
            auto arg = ParseExpr();
            args.push_back(std::move(arg));

            if (!At(")"))
                Expect(",");
        }

        auto type = std::dynamic_pointer_cast<FunctionType>(
            std::dynamic_pointer_cast<PointerType>(callee->Type)->GetBase())->GetResult();

        callee = std::make_unique<CallExpression>(Location, type, std::move(callee), args);
    }

    return callee;
}
