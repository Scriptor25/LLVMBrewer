#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Util.hpp>
#include <Brewer/Value.hpp>

#include "Brewer/Type.hpp"

Brewer::MemberExpression::MemberExpression(const SourceLocation& loc,
                                           const TypePtr& type,
                                           ExprPtr object,
                                           std::string member_name,
                                           const size_t member,
                                           const bool dereference)
    : Expression(loc, type),
      Object(std::move(object)),
      MemberName(std::move(member_name)),
      Member(member),
      Dereference(dereference)
{
}

std::ostream& Brewer::MemberExpression::Dump(std::ostream& stream) const
{
    return stream << Object << (Dereference ? "!" : ".") << MemberName;
}

Brewer::ValuePtr Brewer::MemberExpression::GenIR(Builder& builder) const
{
    if (Type->IsFuncPtr())
        return builder.GetFunction(Object->Type, MemberName);

    auto object = Object->GenIR(builder);
    if (!object) return {};
    if (Dereference)
    {
        object = object->Dereference();
        if (!object) return {};
    }

    const auto l_object = LValue::From(object);
    if (!l_object)
        return std::cerr
            << "at " << Location << ": "
            << "cannot get member of constant struct"
            << std::endl
            << ErrMark<ValuePtr>();

    const auto gep = builder.IRBuilder().CreateStructGEP(object->GetIRType(), l_object->GetPtr(), Member);
    return LValue::Direct(builder, Type, gep);
}
