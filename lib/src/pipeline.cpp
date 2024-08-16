#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Parser.hpp>
#include <Brewer/Pipeline.hpp>

Brewer::Pipeline::Pipeline(std::istream& stream, std::string input_filename)
    : m_Stream(stream), m_InputFilename(std::move(input_filename))
{
}

Brewer::Pipeline& Brewer::Pipeline::ParseStmtFn(const std::string& beg, const std::function<StmtPtr(Parser&)>& fn)
{
    m_StmtFns[beg] = fn;
    return *this;
}

Brewer::Pipeline& Brewer::Pipeline::ParseExprFn(const std::string& beg, const std::function<ExprPtr(Parser&)>& fn)
{
    m_ExprFns[beg] = fn;
    return *this;
}

Brewer::Pipeline& Brewer::Pipeline::DumpAST()
{
    m_DumpAST = true;
    return *this;
}

Brewer::Pipeline& Brewer::Pipeline::ModuleID(const std::string& module_id)
{
    m_ModuleID = module_id;
    return *this;
}

Brewer::Pipeline& Brewer::Pipeline::DumpIR()
{
    m_DumpIR = true;
    return *this;
}

void Brewer::Pipeline::Build()
{
    ParseAndBuild();
}

void Brewer::Pipeline::BuildAndEmit(const std::string& output_filename)
{
    m_EmitToFile = true;
    m_OutputFilename = output_filename;
    ParseAndBuild();
}

void Brewer::Pipeline::ParseAndBuild()
{
    Parser parser(m_Stream, m_InputFilename);
    Builder builder(m_ModuleID, m_InputFilename);

    for (const auto& [beg, fn] : m_StmtFns)
        parser.ParseStmtFn(beg) = fn;
    for (const auto& [beg, fn] : m_ExprFns)
        parser.ParseExprFn(beg) = fn;

    while (!parser.AtEOF())
    {
        const auto stmt_ptr = parser.Parse();
        if (m_DumpAST) stmt_ptr->Dump(std::cerr) << std::endl;
        stmt_ptr->GenIR(builder);
    }

    builder.Close();

    if (m_DumpIR) builder.Dump();
    if (m_EmitToFile) builder.EmitToFile(m_OutputFilename);
}
