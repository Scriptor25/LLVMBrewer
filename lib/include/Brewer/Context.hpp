#pragma once

#include <map>
#include <string>
#include <Brewer/Brewer.hpp>

namespace Brewer
{
    class Context
    {
    public:
        Context();

        TypePtr& GetType(const std::string& name);

        TypePtr GetVoidTy();
        TypePtr GetIntNTy(size_t);
        TypePtr GetInt1Ty();
        TypePtr GetInt8Ty();
        TypePtr GetInt16Ty();
        TypePtr GetInt32Ty();
        TypePtr GetInt64Ty();
        TypePtr GetFloatNTy(size_t);
        TypePtr GetFloat16Ty();
        TypePtr GetFloat32Ty();
        TypePtr GetFloat64Ty();

        TypePtr GetInt8PtrTy();

    private:
        std::map<std::string, TypePtr> m_Types;
    };
}
