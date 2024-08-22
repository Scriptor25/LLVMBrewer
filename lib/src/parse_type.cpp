#include <iostream>
#include <Brewer/AST.hpp>
#include <Brewer/Context.hpp>
#include <Brewer/Parser.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>

Brewer::TypePtr Brewer::Parser::ParseType()
{
    TypePtr type;

    if (NextIfAt("struct"))
    {
        if (!NextIfAt("{"))
        {
            type = StructType::Get(m_Context);
        }
        else
        {
            std::vector<StructElement> elements;
            while (!NextIfAt("}"))
            {
                auto element_type = ParseType();
                auto element_name = Expect(TokenType_Name).Value;
                elements.emplace_back(element_type, element_name);
                if (!At("}")) Expect(",");
            }
            type = StructType::Get(elements);
        }
    }
    else
    {
        auto [Location, Type, Value] = Expect(TokenType_Name);
        type = m_Context.GetType(Value);
        if (!type)
            return std::cerr
                << "at " << Location << ": "
                << "undefined type " << Value
                << std::endl
                << ErrMark<TypePtr>();
    }

    while (true)
    {
        if (NextIfAt("*"))
        {
            type = PointerType::Get(type);
            continue;
        }

        if (NextIfAt("["))
        {
            const auto length = dynamic_pointer_cast<ConstIntExpression>(ParseExpr());
            auto [Location, Type, Value] = Expect("]");
            if (!length)
                return std::cerr
                    << "at " << Location << ": "
                    << "array length must be a constant int"
                    << std::endl
                    << ErrMark<TypePtr>();
            type = ArrayType::Get(type, length->Value);
            continue;
        }

        if (NextIfAt("("))
        {
            std::vector<TypePtr> params;
            bool vararg = false;
            while (!NextIfAt(")"))
            {
                if (NextIfAt("?"))
                {
                    vararg = true;
                    Expect(")");
                    break;
                }

                params.push_back(ParseType());
                if (!At(")")) Expect(",");
            }
            type = FunctionType::Get(type, params, vararg);
            continue;
        }

        return type;
    }
}
