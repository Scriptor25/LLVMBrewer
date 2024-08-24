#include <filesystem>
#include <fstream>
#include <Brewer/Parser.hpp>
#include <Brewer/Pipeline.hpp>
#include <Test/AST.hpp>

#include "Brewer/Context.hpp"
#include "Brewer/Type.hpp"

static Test::Prototype parse_proto(Brewer::Parser& parser)
{
    auto [Location, Type, Value] = parser.Expect(Brewer::TokenType_Name);
    std::vector<std::string> params;
    parser.Expect("(");
    while (!parser.NextIfAt(")"))
    {
        auto param = parser.Expect(Brewer::TokenType_Name).Value;
        params.push_back(param);
    }
    Test::Prototype proto{Value, params};
    parser.GetContext().GetSymbol(Value) = proto.GetType(parser.GetContext());
    return proto;
}

static Brewer::StmtPtr parse_def(Brewer::Parser& parser)
{
    auto [Location, Type, Value] = parser.Expect("def");
    auto proto = parse_proto(parser);
    parser.GetContext().Push();
    for (auto& param : proto.Params)
        parser.GetContext().GetSymbol(param) = parser.GetContext().GetType("f64");
    auto body = parser.ParseExpr();
    parser.GetContext().Pop();
    if (!body) return {};
    return std::make_unique<Test::DefStatement>(Location, proto, std::move(body));
}

static Brewer::StmtPtr parse_extern(Brewer::Parser& parser)
{
    auto [Location, Type, Value] = parser.Expect("extern");
    auto proto = parse_proto(parser);
    return std::make_unique<Test::ExternStatement>(Location, proto);
}

static Brewer::ExprPtr parse_if(Brewer::Parser& parser)
{
    auto [Location, Type, Value] = parser.Expect("if");
    auto condition = parser.ParseExpr();
    if (!condition) return {};
    parser.Expect("then");
    auto then = parser.ParseExpr();
    if (!then) return {};
    parser.Expect("else");
    auto else_ = parser.ParseExpr();
    if (!else_) return {};

    auto type = Brewer::Type::GetHigherOrder(then->Type, else_->Type);
    if (!type) return {};

    return std::make_unique<Test::IfExpression>(Location,
                                                type,
                                                std::move(condition),
                                                std::move(then),
                                                std::move(else_));
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

    Brewer::Pipeline()
        .ParseStmtFn("def", parse_def)
        .ParseStmtFn("extern", parse_extern)
        .ParseExprFn("if", parse_if)
        .DumpAST(true)
        .DumpIR(true)
        .ModuleID(module_id)
        .BuildAndEmit(stream, input_filename, output_filename);

    return 0;
}
