#include <iostream>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>

Brewer::TypePtr Brewer::Type::GetHigherOrder(const TypePtr& a, const TypePtr& b)
{
    if (a == b)
        return a;

    if (a->IsInt())
    {
        if (b->IsInt())
        {
            if (a->GetSize() >= b->GetSize())
                return a;
            return b;
        }

        if (b->IsFloat())
            return b;

        if (b->IsPointer())
            return a;
    }

    if (a->IsFloat())
    {
        if (b->IsInt())
            return a;

        if (b->IsFloat())
        {
            if (a->GetSize() >= b->GetSize())
                return a;
            return b;
        }

        if (b->IsPointer())
            return a;
    }

    if (a->IsPointer())
    {
        if (b->IsInt())
            return b;

        if (b->IsFloat())
            return b;
    }

    return std::cerr
        << "cannot determine higher order type of " << a << " and " << b
        << std::endl
        << ErrMark<TypePtr>();
}
