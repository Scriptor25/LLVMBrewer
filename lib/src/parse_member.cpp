#include <Brewer/AST.hpp>
#include <Brewer/Parser.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>

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

        if (dereference)
        {
            auto ptr_type = std::dynamic_pointer_cast<PointerType>(object->Type);
            type = std::dynamic_pointer_cast<StructType>(ptr_type->GetBase())->GetElement(member, index);
        }
        else
        {
            type = std::dynamic_pointer_cast<StructType>(object->Type)->GetElement(member, index);
        }

        if (!type)
            return std::cerr
                << "at " << Location << ": "
                << "no member " << member << " in type " << object->Type->GetName()
                << std::endl
                << ErrMark<ExprPtr>();

        object = std::make_unique<MemberExpression>(Location, type, std::move(object), member, index, dereference);
    }

    return object;
}
