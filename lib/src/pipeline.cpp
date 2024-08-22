#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Context.hpp>
#include <Brewer/Parser.hpp>
#include <Brewer/Pipeline.hpp>

Brewer::Pipeline::Pipeline()
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

Brewer::Pipeline& Brewer::Pipeline::GenBinaryFn(const std::string& operator_, const BinaryFn& fn)
{
    m_BinaryFns[operator_] = fn;
    return *this;
}

Brewer::Pipeline& Brewer::Pipeline::GenUnaryFn(const std::string& operator_, const UnaryFn& fn)
{
    m_UnaryFns[operator_] = fn;
    return *this;
}

Brewer::Pipeline& Brewer::Pipeline::DumpAST(const bool mode)
{
    m_DumpAST = mode;
    return *this;
}

Brewer::Pipeline& Brewer::Pipeline::ModuleID(const std::string& module_id)
{
    m_ModuleID = module_id;
    return *this;
}

Brewer::Pipeline& Brewer::Pipeline::DumpIR(const bool mode)
{
    m_DumpIR = mode;
    return *this;
}

void Brewer::Pipeline::Build(std::istream& stream, const std::string& input_filename)
{
    Context context;

    Builder builder(context, m_ModuleID, input_filename);
    Parser parser(builder, stream, input_filename);

    for (const auto& [beg, fn] : m_StmtFns)
        parser.ParseStmtFn(beg) = fn;
    for (const auto& [beg, fn] : m_ExprFns)
        parser.ParseExprFn(beg) = fn;

    for (const auto& [op, fn] : m_BinaryFns)
        builder.GenBinaryFn(op) = fn;
    for (const auto& [op, fn] : m_UnaryFns)
        builder.GenUnaryFn(op) = fn;

    while (!parser.AtEOF())
    {
        const auto ptr = parser.Parse();
        if (!ptr) continue;

        if (m_DumpAST) std::cerr << ptr->Location << ": " << std::endl << ptr << std::endl;
        ptr->GenIRNoVal(builder);
    }

    if (m_DumpIR) builder.Dump();
    if (m_EmitToFile) builder.EmitToFile(m_OutputFilename);
}

void Brewer::Pipeline::BuildAndEmit(std::istream& stream,
                                    const std::string& input_filename,
                                    const std::string& output_filename)
{
    m_EmitToFile = true;
    m_OutputFilename = output_filename;
    Build(stream, input_filename);
}
