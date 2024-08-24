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
        TypePtr& GetFunction(const TypePtr& self, const std::string& name);
        TypePtr& GetSymbol(const std::string& name);

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

        TypePtr& CurrentResult();

        void Push();
        void Pop();

    private:
        std::map<std::string, TypePtr> m_Types;

        std::map<TypePtr, std::map<std::string, TypePtr>> m_Functions;

        std::vector<std::map<std::string, TypePtr>> m_Stack;
        std::map<std::string, TypePtr> m_Symbols;

        TypePtr m_CurrentResult;
    };
}
