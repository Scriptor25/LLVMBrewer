#pragma once

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
        Pipeline& ParseStmtFn(const std::string& beg, const StmtFn& fn);
        Pipeline& ParseExprFn(const std::string& beg, const ExprFn& fn);
        Pipeline& GenBinaryFn(const std::string& operator_, const BinaryFn& fn);
        Pipeline& GenUnaryFn(const std::string& operator_, const UnaryFn& fn);
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

        std::map<std::string, StmtFn> m_StmtFns;
        std::map<std::string, ExprFn> m_ExprFns;
        std::map<std::string, BinaryFn> m_BinaryFns;
        std::map<std::string, UnaryFn> m_UnaryFns;

        bool m_DumpAST = false;
        bool m_DumpIR = false;
        bool m_EmitToFile = false;
    };
}
