#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Parser.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>
#include <Brewer/Value.hpp>

Brewer::ExprPtr Brewer::Parser::ParseMember()
{
    auto object = ParsePrimary();
    if (!object) return {};
    return ParseMember(std::move(object));
}

Brewer::ExprPtr Brewer::Parser::ParseMember(ExprPtr object)
{
    while (At(".") || At("!"))
    {
        auto [Location, Type, Value] = Skip();
        auto member = Expect(TokenType_Name).Value;
        auto dereference = Value == "!";

        TypePtr type;
        size_t index;

        StructTypePtr struct_type;

        if (dereference) struct_type = StructType::From(PointerType::From(object->Type)->GetBase());
        else struct_type = StructType::From(object->Type);

        type = struct_type->GetElement(member, index);
        if (!type)
        {
            const auto func = m_Builder.GetFunction(struct_type, member);
            type = func ? func->GetType() : nullptr;
        }
        if (!type)
            return std::cerr
                << "at " << Location << ": "
                << "no member '" << member << "' in type " << struct_type->GetName()
                << std::endl
                << ErrMark<ExprPtr>();

        object = std::make_unique<MemberExpression>(Location, type, std::move(object), member, index, dereference);
    }

    return object;
}
