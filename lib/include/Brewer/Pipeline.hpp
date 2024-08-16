#pragma once

#include <functional>
#include <map>
#include <string>
#include <Brewer/Brewer.hpp>

namespace Brewer
{
    class Pipeline
    {
    public:
        Pipeline(std::istream& stream, std::string input_filename);

        // Intermediate Operations
        Pipeline& ParseStmtFn(const std::string& beg, const std::function<StmtPtr(Parser&)>& fn);
        Pipeline& ParseExprFn(const std::string& beg, const std::function<ExprPtr(Parser&)>& fn);
        Pipeline& ModuleID(const std::string& module_id);
        Pipeline& DumpAST();
        Pipeline& DumpIR();

        // Terminal Operations
        void Build();
        void BuildAndEmit(const std::string& output_filename);

    private:
        void ParseAndBuild();

        std::istream& m_Stream;

        std::string m_InputFilename;
        std::string m_OutputFilename;
        std::string m_ModuleID;

        std::map<std::string, std::function<StmtPtr(Parser&)>> m_StmtFns;
        std::map<std::string, std::function<ExprPtr(Parser&)>> m_ExprFns;

        bool m_DumpAST = false;
        bool m_DumpIR = false;
        bool m_EmitToFile = false;
    };
}
