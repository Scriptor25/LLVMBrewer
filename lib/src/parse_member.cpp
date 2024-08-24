#include <Brewer/AST.hpp>
#include <Brewer/Context.hpp>
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

        StructTypePtr struct_type;

        if (dereference)
        {
            const auto ptr_type = std::dynamic_pointer_cast<PointerType>(object->Type);
            struct_type = std::dynamic_pointer_cast<StructType>(ptr_type->GetBase());
        }
        else
        {
            struct_type = std::dynamic_pointer_cast<StructType>(object->Type);
        }

        type = struct_type->GetElement(member, index);
        if (!type) type = m_Context.GetFunction(struct_type, member);
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
