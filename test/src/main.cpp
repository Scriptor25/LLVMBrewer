#include <fstream>
#include <iostream>
#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Parser.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>
#include <llvm-c/Core.h>

struct Prototype
{
    Prototype(std::string name, const std::vector<std::string>& params)
        : Name(std::move(name)), Params(params)
    {
    }

    LLVMValueRef GenIR(Brewer::Builder& builder) const
    {
        const auto ft = GetType()->GenIR(builder);
        const auto f = LLVMAddFunction(builder.Module(), Name.c_str(), ft);
        if (!f)
            return {};

        for (size_t i = 0; i < LLVMCountParams(f); ++i)
            LLVMSetValueName(LLVMGetParam(f, i), Params[i].c_str());

        builder[Name] = Brewer::RValue::Direct(Brewer::PointerType::Get(GetType()), f);
        return f;
    }

    std::ostream& Dump(std::ostream& stream) const
    {
        using namespace Brewer;
        return stream << Name << '(' << Params << ')';
    }

    [[nodiscard]] Brewer::TypePtr GetType() const
    {
        const std::vector params(Params.size(), Brewer::Type::Get("f64"));
        return Brewer::FunctionType::Get(Brewer::Type::Get("f64"), params, false);
    }

    std::string Name;
    std::vector<std::string> Params;
};

std::ostream& operator<<(std::ostream& stream, const Prototype& proto)
{
    return proto.Dump(stream);
}

struct FunctionStatement : Brewer::Statement
{
    FunctionStatement(const Brewer::SourceLocation& loc,
                      Prototype proto,
                      Brewer::ExprPtr body)
        : Statement(loc), Proto(std::move(proto)), Body(std::move(body))
    {
    }

    Brewer::ValuePtr GenIR(Brewer::Builder& builder) const override
    {
        auto f = LLVMGetNamedFunction(builder.Module(), Proto.Name.c_str());
        if (!f)
        {
            f = Proto.GenIR(builder);
            if (!f) return {};
        }
        if (LLVMCountBasicBlocks(f)) return {};

        const auto bb = LLVMAppendBasicBlockInContext(builder.Context(), f, "entry");
        const auto bkp = LLVMGetInsertBlock(builder.IRBuilder());
        LLVMPositionBuilderAtEnd(builder.IRBuilder(), bb);

        builder.Push();
        for (size_t i = 0; i < LLVMCountParams(f); ++i)
        {
            const auto name = Proto.Params[i];
            const auto param = LLVMGetParam(f, i);
            builder[name] = Brewer::RValue::Direct(Brewer::Type::Get("f64"), param);
        }

        const auto retval = Body->GenIR(builder);
        LLVMBuildRet(builder.IRBuilder(), retval->Get());
        builder.Pop();

        LLVMPositionBuilderAtEnd(builder.IRBuilder(), bkp);
        return Brewer::RValue::Direct(Brewer::PointerType::Get(Proto.GetType()), f);
    }

    std::ostream& Dump(std::ostream& stream) const override
    {
        return stream << "def " << Proto << ' ' << Body;
    }

    Prototype Proto;
    Brewer::ExprPtr Body;
};

struct ExternStatement : Brewer::Statement
{
    ExternStatement(const Brewer::SourceLocation& loc, Prototype proto)
        : Statement(loc), Proto(std::move(proto))
    {
    }

    Brewer::ValuePtr GenIR(Brewer::Builder& builder) const override
    {
        if (LLVMGetNamedFunction(builder.Module(), Proto.Name.c_str()))
            return {};
        Proto.GenIR(builder);
        return {};
    }

    std::ostream& Dump(std::ostream& stream) const override
    {
        return stream << "extern " << Proto;
    }

    Prototype Proto;
};

struct IfExpression : Brewer::Expression
{
    IfExpression(const Brewer::SourceLocation& loc,
                 Brewer::ExprPtr condition,
                 Brewer::ExprPtr then,
                 Brewer::ExprPtr else_)
        : Expression(loc), Condition(std::move(condition)), Then(std::move(then)), Else(std::move(else_))
    {
    }

    Brewer::ValuePtr GenIR(Brewer::Builder& builder) const override
    {
        const auto f = LLVMGetBasicBlockParent(LLVMGetInsertBlock(builder.IRBuilder()));
        auto thenbb = LLVMAppendBasicBlockInContext(builder.Context(), f, "then");
        auto elsebb = LLVMAppendBasicBlockInContext(builder.Context(), f, "else");
        const auto endbb = LLVMAppendBasicBlockInContext(builder.Context(), f, "end");

        const auto condition = Condition->GenIR(builder);
        LLVMBuildCondBr(builder.IRBuilder(), condition->Get(), thenbb, elsebb);

        LLVMPositionBuilderAtEnd(builder.IRBuilder(), thenbb);
        const auto then = Then->GenIR(builder);
        thenbb = LLVMGetInsertBlock(builder.IRBuilder());

        LLVMPositionBuilderAtEnd(builder.IRBuilder(), elsebb);
        const auto else_ = Else->GenIR(builder);
        elsebb = LLVMGetInsertBlock(builder.IRBuilder());

        const auto type = Brewer::Type::GetHigherOrder(then->GetType(), else_->GetType());
        const auto ty = type->GenIR(builder);

        LLVMPositionBuilderAtEnd(builder.IRBuilder(), thenbb);
        auto thenv = builder.GenCast(then, type)->Get();
        LLVMBuildBr(builder.IRBuilder(), endbb);

        LLVMPositionBuilderAtEnd(builder.IRBuilder(), elsebb);
        auto elsev = builder.GenCast(else_, type)->Get();
        LLVMBuildBr(builder.IRBuilder(), endbb);

        LLVMPositionBuilderAtEnd(builder.IRBuilder(), endbb);
        const auto phi = LLVMBuildPhi(builder.IRBuilder(), ty, "");
        LLVMAddIncoming(phi, &thenv, &thenbb, 1);
        LLVMAddIncoming(phi, &elsev, &elsebb, 1);

        return Brewer::RValue::Direct(then->GetType(), phi);
    }

    std::ostream& Dump(std::ostream& stream) const override
    {
        return stream << "if " << Condition << " then " << Then << " else " << Else;
    }

    Brewer::ExprPtr Condition;
    Brewer::ExprPtr Then;
    Brewer::ExprPtr Else;
};

static Prototype parse_proto(Brewer::Parser& p)
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

    return std::make_unique<FunctionStatement>(Location, proto, std::move(body));
}

static Brewer::StmtPtr parse_extern(Brewer::Parser& p)
{
    auto [Location, Type, Value] = p.Expect("extern");
    auto proto = parse_proto(p);
    return std::make_unique<ExternStatement>(Location, proto);
}

static Brewer::ExprPtr parse_if(Brewer::Parser& p)
{
    auto [Location, Type, Value] = p.Expect("if");
    auto condition = p.ParseExpr();
    p.Expect("then");
    auto then = p.ParseExpr();
    p.Expect("else");
    auto else_ = p.ParseExpr();
    return std::make_unique<IfExpression>(Location, std::move(condition), std::move(then), std::move(else_));
}

// small implementation of the kaleidoscope toy language
int main(const int argc, const char** argv)
{
    if (argc < 3)
    {
        std::cerr << "Usage: test <in> <out>" << std::endl;
        return 1;
    }

    std::ifstream stream(argv[1]);
    if (!stream)
    {
        std::cerr << "Failed to open file '" << argv[1] << "'" << std::endl;
        return 1;
    }

    Brewer::Parser parser(stream, argv[1]);
    parser.ParseStmtFn("def") = parse_def;
    parser.ParseStmtFn("extern") = parse_extern;
    parser.ParseExprFn("if") = parse_if;

    Brewer::Builder builder(argv[1]);

    while (!parser.AtEOF())
    {
        auto ptr = parser.Parse();
        std::cout << ptr << std::endl;
        ptr->GenIR(builder);
    }

    builder.Close();
    builder.Dump();
    builder.EmitToFile(argv[2]);

    return 0;
}
