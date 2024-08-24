#include <filesystem>
#include <fstream>
#include <Brewer/Builder.hpp>
#include <Brewer/Context.hpp>
#include <Brewer/Parser.hpp>
#include <Brewer/Pipeline.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>
#include <Test/AST.hpp>

using namespace Brewer;

static Test::Prototype parse_proto(Parser& parser)
{
    auto [Location, Type, Value] = parser.Expect(TokenType_Name);
    std::vector<std::string> params;
    parser.Expect("(");
    while (!parser.NextIfAt(")"))
    {
        auto param = parser.Expect(TokenType_Name).Value;
        params.push_back(param);
    }

    Test::Prototype proto{Value, params};

    parser.GetBuilder().GetFunction({}, Value) = Value::Empty(proto.GetType(parser.GetContext()));
    return proto;
}

static StmtPtr parse_def(Parser& parser)
{
    auto [Location, Type, Value] = parser.Expect("def");
    auto proto = parse_proto(parser);
    parser.GetBuilder().Push();
    for (auto& param : proto.Params)
        parser.GetBuilder().GetSymbol(param) = Value::Empty(parser.GetContext().GetFloat64Ty());
    auto body = parser.ParseExpr();
    parser.GetBuilder().Pop();
    if (!body) return {};
    return std::make_unique<Test::DefStatement>(Location, proto, std::move(body));
}

static StmtPtr parse_extern(Parser& parser)
{
    auto [Location, Type, Value] = parser.Expect("extern");
    auto proto = parse_proto(parser);
    return std::make_unique<Test::ExternStatement>(Location, proto);
}

static ExprPtr parse_if(Parser& parser)
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

    auto type = Type::GetHigherOrder(then->Type, else_->Type);
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

    Pipeline()
        .ParseStmtFn("def", parse_def)
        .ParseStmtFn("extern", parse_extern)
        .ParseExprFn("if", parse_if)
        .DumpAST(true)
        .DumpIR(true)
        .ModuleID(module_id)
        .BuildAndEmit(stream, input_filename, output_filename);

    return 0;
}
