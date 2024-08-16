#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <Brewer/AST.hpp>
#include <Brewer/Builder.hpp>
#include <Brewer/Parser.hpp>
#include <Brewer/Pipeline.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>
#include <Brewer/Value.hpp>
#include <llvm-c/Core.h>

struct Prototype
{
    Prototype(std::string name, const std::vector<std::string>& params)
        : Name(std::move(name)), Params(params)
    {
    }

    std::ostream& Dump(std::ostream& stream) const
    {
        using namespace Brewer;
        return stream << Name << '(' << Params << ')';
    }

    LLVMValueRef GenIR(Brewer::Builder& builder) const
    {
        auto f = LLVMGetNamedFunction(builder.Module(), Name.c_str());
        if (f) return f;

        f = LLVMAddFunction(builder.Module(), Name.c_str(), GetType()->GenIR(builder));
        if (!f) return {};

        for (size_t i = 0; i < LLVMCountParams(f); ++i)
            LLVMSetValueName(LLVMGetParam(f, i), Params[i].c_str());

        builder[Name] = Brewer::RValue::Direct(builder, Brewer::PointerType::Get(GetType()), f);
        return f;
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

    std::ostream& Dump(std::ostream& stream) const override
    {
        return stream << "def " << Proto << ' ' << Body;
    }

    Brewer::ValuePtr GenIR(Brewer::Builder& builder) const override
    {
        const auto f = Proto.GenIR(builder);
        if (!f || LLVMCountBasicBlocks(f)) return {};

        const auto bb = LLVMAppendBasicBlockInContext(builder.Context(), f, "entry");
        const auto bkp = LLVMGetInsertBlock(builder.IRBuilder());
        LLVMPositionBuilderAtEnd(builder.IRBuilder(), bb);

        builder.Push();
        for (size_t i = 0; i < LLVMCountParams(f); ++i)
        {
            const auto name = Proto.Params[i];
            const auto param = LLVMGetParam(f, i);
            builder[name] = Brewer::RValue::Direct(builder, Brewer::Type::Get("f64"), param);
        }

        const auto return_value = Body->GenIR(builder);
        if (!return_value)
        {
            builder.Pop();
            LLVMPositionBuilderAtEnd(builder.IRBuilder(), bkp);
            LLVMRemoveBasicBlockFromParent(bb);
            return {};
        }

        LLVMBuildRet(builder.IRBuilder(), return_value->Get());
        builder.Pop();
        LLVMPositionBuilderAtEnd(builder.IRBuilder(), bkp);
        return Brewer::RValue::Direct(builder, Brewer::PointerType::Get(Proto.GetType()), f);
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

    std::ostream& Dump(std::ostream& stream) const override
    {
        return stream << "extern " << Proto;
    }

    Brewer::ValuePtr GenIR(Brewer::Builder& builder) const override
    {
        const auto f = Proto.GenIR(builder);
        if (!f) return {};
        return Brewer::RValue::Direct(builder, Brewer::PointerType::Get(Proto.GetType()), f);
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

    std::ostream& Dump(std::ostream& stream) const override
    {
        return stream << "if " << Condition << " then " << Then << " else " << Else;
    }

    Brewer::ValuePtr GenIR(Brewer::Builder& builder) const override
    {
        const auto bkp = LLVMGetInsertBlock(builder.IRBuilder());
        const auto f = LLVMGetBasicBlockParent(bkp);
        auto then_bb = LLVMAppendBasicBlockInContext(builder.Context(), f, "then");
        auto else_bb = LLVMAppendBasicBlockInContext(builder.Context(), f, "else");
        const auto end_bb = LLVMAppendBasicBlockInContext(builder.Context(), f, "end");

        const auto condition = Condition->GenIR(builder);
        if (!condition)
        {
            LLVMPositionBuilderAtEnd(builder.IRBuilder(), bkp);
            LLVMRemoveBasicBlockFromParent(then_bb);
            LLVMRemoveBasicBlockFromParent(else_bb);
            LLVMRemoveBasicBlockFromParent(end_bb);
            return {};
        }
        LLVMBuildCondBr(builder.IRBuilder(), condition->Get(), then_bb, else_bb);

        LLVMPositionBuilderAtEnd(builder.IRBuilder(), then_bb);
        const auto then = Then->GenIR(builder);
        if (!then)
        {
            LLVMPositionBuilderAtEnd(builder.IRBuilder(), bkp);
            LLVMRemoveBasicBlockFromParent(then_bb);
            LLVMRemoveBasicBlockFromParent(else_bb);
            LLVMRemoveBasicBlockFromParent(end_bb);
            return {};
        }
        then_bb = LLVMGetInsertBlock(builder.IRBuilder());

        LLVMPositionBuilderAtEnd(builder.IRBuilder(), else_bb);
        const auto else_ = Else->GenIR(builder);
        if (!else_)
        {
            LLVMPositionBuilderAtEnd(builder.IRBuilder(), bkp);
            LLVMRemoveBasicBlockFromParent(then_bb);
            LLVMRemoveBasicBlockFromParent(else_bb);
            LLVMRemoveBasicBlockFromParent(end_bb);
            return {};
        }
        else_bb = LLVMGetInsertBlock(builder.IRBuilder());

        const auto type = Brewer::Type::GetHigherOrder(then->GetType(), else_->GetType());
        const auto ty = type->GenIR(builder);

        LLVMPositionBuilderAtEnd(builder.IRBuilder(), then_bb);
        auto then_result = builder.GenCast(then, type)->Get();
        LLVMBuildBr(builder.IRBuilder(), end_bb);

        LLVMPositionBuilderAtEnd(builder.IRBuilder(), else_bb);
        auto else_result = builder.GenCast(else_, type)->Get();
        LLVMBuildBr(builder.IRBuilder(), end_bb);

        LLVMPositionBuilderAtEnd(builder.IRBuilder(), end_bb);
        const auto phi = LLVMBuildPhi(builder.IRBuilder(), ty, "");
        LLVMAddIncoming(phi, &then_result, &then_bb, 1);
        LLVMAddIncoming(phi, &else_result, &else_bb, 1);

        return Brewer::RValue::Direct(builder, then->GetType(), phi);
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

    const std::string input_filename = argv[1];
    const std::string output_filename = argv[2];
    const auto module_id = std::filesystem::path(argv[1])
                           .replace_extension()
                           .filename()
                           .string();

    std::ifstream stream(argv[1]);
    if (!stream)
    {
        std::cerr << "Failed to open file '" << argv[1] << "'" << std::endl;
        return 1;
    }

    Brewer::Pipeline(stream, input_filename)
        .ParseStmtFn("def", parse_def)
        .ParseStmtFn("extern", parse_extern)
        .ParseExprFn("if", parse_if)
        .DumpAST()
        .ModuleID(module_id)
        .DumpIR()
        .Build();
    //.BuildAndEmit(output_filename);

    return 0;
}
