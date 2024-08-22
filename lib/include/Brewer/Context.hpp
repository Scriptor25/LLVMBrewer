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
        TypePtr& GetSymbol(const std::string& name);

        TypePtr& CurrentResult();

        void Push();
        void Pop();

    private:
        std::map<std::string, TypePtr> m_Types;

        std::vector<std::map<std::string, TypePtr>> m_SymbolStack;
        std::map<std::string, TypePtr> m_Symbols;

        TypePtr m_CurrentResult;
    };
}
