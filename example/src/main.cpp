#include <filesystem>
#include <fstream>
#include <Brewer/Parser.hpp>
#include <Brewer/Pipeline.hpp>
#include <Test/AST.hpp>

static Test::Prototype parse_proto(Brewer::Parser& p)
{
    auto [Location, Type, Value] = p.Expect(Brewer::TokenType_Name);
    std::vector<std::string> params;
    p.Expect("(");
    while (!p.NextIfAt(")"))
    {
        auto param = p.Expect(Brewer::TokenType_Name).Value;
        params.push_back(param);
    }
    return {Value, params};
}

static Brewer::StmtPtr parse_def(Brewer::Parser& p)
{
    auto [Location, Type, Value] = p.Expect("def");
    auto proto = parse_proto(p);
    auto body = p.ParseExpr();
    if (!body) return {};
    return std::make_unique<Test::FunctionStatement>(Location, proto, std::move(body));
}

static Brewer::StmtPtr parse_extern(Brewer::Parser& p)
{
    auto [Location, Type, Value] = p.Expect("extern");
    auto proto = parse_proto(p);
    return std::make_unique<Test::ExternStatement>(Location, proto);
}

static Brewer::ExprPtr parse_if(Brewer::Parser& p)
{
    auto [Location, Type, Value] = p.Expect("if");
    auto condition = p.ParseExpr();
    if (!condition) return {};
    p.Expect("then");
    auto then = p.ParseExpr();
    if (!then) return {};
    p.Expect("else");
    auto else_ = p.ParseExpr();
    if (!else_) return {};
    return std::make_unique<Test::IfExpression>(Location, std::move(condition), std::move(then), std::move(else_));
}

// small implementation of the kaleidoscope toy language
int main(const int argc, const char** argv)
{
    if (argc < 3)
    {
        std::cerr << "USAGE: test <in> <out>" << std::endl;
        return 1;
    }

    const std::string input_filename = argv[1];
    const std::string output_filename = argv[2];
    const auto module_id = std::filesystem::path(argv[1])
                           .replace_extension()
                           .filename()
                           .string();

    std::ifstream stream(argv[1]);
    if (!stream)
    {
        std::cerr << "failed to open '" << argv[1] << "'" << std::endl;
        return 1;
    }

    Brewer::Pipeline(stream, input_filename)
        .ParseStmtFn("def", parse_def)
        .ParseStmtFn("extern", parse_extern)
        .ParseExprFn("if", parse_if)
        .DumpAST()
        .ModuleID(module_id)
        .DumpIR()
        .BuildAndEmit(output_filename);

    return 0;
}
