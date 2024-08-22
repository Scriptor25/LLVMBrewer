#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>
#include <Brewer/Value.hpp>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>

Brewer::ValuePtr Brewer::Builder::GenEQ(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    llvm::Value* result;
    switch (lhs->GetType()->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateICmpEQ(lhs->Get(), rhs->Get());
        break;
    case Type_Float:
        result = builder.IRBuilder().CreateFCmpOEQ(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, Type::Get(builder.GetContext(), "i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenNE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    llvm::Value* result;
    switch (lhs->GetType()->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateICmpNE(lhs->Get(), rhs->Get());
        break;
    case Type_Float:
        result = builder.IRBuilder().CreateFCmpONE(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, Type::Get(builder.GetContext(), "i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenLT(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    llvm::Value* result;
    switch (lhs->GetType()->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateICmpSLT(lhs->Get(), rhs->Get());
        break;
    case Type_Float:
        result = builder.IRBuilder().CreateFCmpOLT(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, Type::Get(builder.GetContext(), "i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenGT(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    llvm::Value* result;
    switch (lhs->GetType()->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateICmpSGT(lhs->Get(), rhs->Get());
        break;
    case Type_Float:
        result = builder.IRBuilder().CreateFCmpOGT(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, Type::Get(builder.GetContext(), "i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenLE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    llvm::Value* result;
    switch (lhs->GetType()->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateICmpSLE(lhs->Get(), rhs->Get());
        break;
    case Type_Float:
        result = builder.IRBuilder().CreateFCmpOLE(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, Type::Get(builder.GetContext(), "i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenGE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    llvm::Value* result;
    switch (lhs->GetType()->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateICmpSGE(lhs->Get(), rhs->Get());
        break;
    case Type_Float:
        result = builder.IRBuilder().CreateFCmpOGE(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, Type::Get(builder.GetContext(), "i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenLAnd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = builder.IRBuilder().CreateIsNotNull(lhs->Get());
    const auto r = builder.IRBuilder().CreateIsNotNull(rhs->Get());

    const auto result = builder.IRBuilder().CreateAnd(l, r);
    return RValue::Direct(builder, Type::Get(builder.GetContext(), "i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenLOr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = builder.IRBuilder().CreateIsNotNull(lhs->Get());
    const auto r = builder.IRBuilder().CreateIsNotNull(rhs->Get());

    const auto result = builder.IRBuilder().CreateOr(l, r);
    return RValue::Direct(builder, Type::Get(builder.GetContext(), "i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenLXor(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = builder.IRBuilder().CreateIsNotNull(lhs->Get());
    const auto r = builder.IRBuilder().CreateIsNotNull(rhs->Get());

    const auto result = builder.IRBuilder().CreateXor(l, r);
    return RValue::Direct(builder, Type::Get(builder.GetContext(), "i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenAdd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    llvm::Value* result;
    switch (type->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateAdd(lhs->Get(), rhs->Get());
        break;
    case Type_Float:
        result = builder.IRBuilder().CreateFAdd(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, type, result);
}

Brewer::ValuePtr Brewer::Builder::GenSub(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    llvm::Value* result;
    switch (type->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateSub(lhs->Get(), rhs->Get());
        break;
    case Type_Float:
        result = builder.IRBuilder().CreateFSub(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, type, result);
}

Brewer::ValuePtr Brewer::Builder::GenMul(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    llvm::Value* result;
    switch (type->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateMul(lhs->Get(), rhs->Get());
        break;
    case Type_Float:
        result = builder.IRBuilder().CreateFMul(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, type, result);
}

Brewer::ValuePtr Brewer::Builder::GenDiv(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    llvm::Value* result;
    switch (type->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateSDiv(lhs->Get(), rhs->Get());
        break;
    case Type_Float:
        result = builder.IRBuilder().CreateFDiv(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, type, result);
}

Brewer::ValuePtr Brewer::Builder::GenRem(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    llvm::Value* result;
    switch (type->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateSRem(lhs->Get(), rhs->Get());
        break;
    case Type_Float:
        result = builder.IRBuilder().CreateFRem(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, type, result);
}

Brewer::ValuePtr Brewer::Builder::GenAnd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    llvm::Value* result;
    switch (type->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateAnd(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, type, result);
}

Brewer::ValuePtr Brewer::Builder::GenOr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    llvm::Value* result;
    switch (type->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateOr(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, type, result);
}

Brewer::ValuePtr Brewer::Builder::GenXor(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    llvm::Value* result;
    switch (type->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateXor(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, type, result);
}

Brewer::ValuePtr Brewer::Builder::GenShl(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    llvm::Value* result;
    switch (type->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateShl(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, type, result);
}

Brewer::ValuePtr Brewer::Builder::GenLShr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    llvm::Value* result;
    switch (type->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateLShr(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, type, result);
}

Brewer::ValuePtr Brewer::Builder::GenAShr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    llvm::Value* result;
    switch (type->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateAShr(lhs->Get(), rhs->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, type, result);
}

Brewer::ValuePtr Brewer::Builder::GenInc(Builder& builder, const ValuePtr& val)
{
    const auto type = val->GetType();

    llvm::Value* one;
    switch (type->GetID())
    {
    case Type_Integer:
        one = llvm::ConstantInt::get(val->GetIRType(), 1, true);
        break;
    case Type_Float:
        one = llvm::ConstantFP::get(val->GetIRType(), 1.0);
        break;
    default:
        return {};
    }

    return builder.GenBinaryFn("+")(builder, val, RValue::Direct(builder, type, one));
}

Brewer::ValuePtr Brewer::Builder::GenDec(Builder& builder, const ValuePtr& val)
{
    const auto type = val->GetType();

    llvm::Value* one;
    switch (type->GetID())
    {
    case Type_Integer:
        one = llvm::ConstantInt::get(val->GetIRType(), 1, true);
        break;
    case Type_Float:
        one = llvm::ConstantFP::get(val->GetIRType(), 1.0);
        break;
    default:
        return {};
    }

    return builder.GenBinaryFn("-")(builder, val, RValue::Direct(builder, type, one));
}

Brewer::ValuePtr Brewer::Builder::GenNeg(Builder& builder, const ValuePtr& val)
{
    const auto type = val->GetType();

    llvm::Value* result;
    switch (type->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateNeg(val->Get());
        break;
    case Type_Float:
        result = builder.IRBuilder().CreateFNeg(val->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, type, result);
}

Brewer::ValuePtr Brewer::Builder::GenLNot(Builder& builder, const ValuePtr& val)
{
    const auto result = builder.IRBuilder().CreateIsNull(val->Get());
    return RValue::Direct(builder, Type::Get(builder.GetContext(), "i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenNot(Builder& builder, const ValuePtr& val)
{
    const auto type = val->GetType();

    llvm::Value* result;
    switch (type->GetID())
    {
    case Type_Integer:
        result = builder.IRBuilder().CreateNot(val->Get());
        break;
    default:
        return {};
    }

    return RValue::Direct(builder, type, result);
}

Brewer::Builder::Builder(Context& context, const std::string& module_id, const std::string& filename)
    : m_Context(context)
{
    m_IRContext = std::make_unique<llvm::LLVMContext>();
    m_IRBuilder = std::make_unique<llvm::IRBuilder<>>(*m_IRContext);
    m_IRModule = std::make_unique<llvm::Module>(module_id, *m_IRContext);
    m_IRModule->setSourceFileName(filename);

    m_BinaryFns["=="] = GenEQ;
    m_BinaryFns["!="] = GenNE;
    m_BinaryFns["<"] = GenLT;
    m_BinaryFns[">"] = GenGT;
    m_BinaryFns["<="] = GenLE;
    m_BinaryFns[">="] = GenGE;
    m_BinaryFns["&&"] = GenLAnd;
    m_BinaryFns["||"] = GenLOr;
    m_BinaryFns["^^"] = GenLXor;
    m_BinaryFns["+"] = GenAdd;
    m_BinaryFns["-"] = GenSub;
    m_BinaryFns["*"] = GenMul;
    m_BinaryFns["/"] = GenDiv;
    m_BinaryFns["%"] = GenRem;
    m_BinaryFns["&"] = GenAnd;
    m_BinaryFns["|"] = GenOr;
    m_BinaryFns["^"] = GenXor;
    m_BinaryFns["<<"] = GenShl;
    m_BinaryFns[">>"] = GenLShr;
    m_BinaryFns[">>>"] = GenAShr;

    m_UnaryFns["++"] = GenInc;
    m_UnaryFns["--"] = GenDec;
    m_UnaryFns["-"] = GenNeg;
    m_UnaryFns["!"] = GenLNot;
    m_UnaryFns["~"] = GenNot;
}

Brewer::Context& Brewer::Builder::GetContext() const
{
    return m_Context;
}

llvm::LLVMContext& Brewer::Builder::IRContext() const
{
    return *m_IRContext;
}

llvm::IRBuilder<>& Brewer::Builder::IRBuilder() const
{
    return *m_IRBuilder;
}

llvm::Module& Brewer::Builder::IRModule() const
{
    return *m_IRModule;
}

Brewer::BinaryFn& Brewer::Builder::GenBinaryFn(const std::string& operator_)
{
    return m_BinaryFns[operator_];
}

Brewer::UnaryFn& Brewer::Builder::GenUnaryFn(const std::string& operator_)
{
    return m_UnaryFns[operator_];
}

void Brewer::Builder::Dump() const
{
    m_IRModule->print(llvm::errs(), nullptr);
}

void Brewer::Builder::EmitToFile(const std::string& filename) const
{
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    const auto triple = llvm::sys::getDefaultTargetTriple();
    m_IRModule->setTargetTriple(triple);

    std::string error;
    const auto target = llvm::TargetRegistry::lookupTarget(triple, error);

    if (!target)
    {
        llvm::errs() << error;
        return;
    }

    const auto cpu = "generic";
    const auto features = "";

    const llvm::TargetOptions opt;
    const auto machine = target->createTargetMachine(triple, cpu, features, opt, llvm::Reloc::PIC_);

    m_IRModule->setDataLayout(machine->createDataLayout());

    std::error_code ec;
    llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);

    if (ec)
    {
        llvm::errs() << "failed to open file: " << ec.message();
        return;
    }

    llvm::legacy::PassManager pass;
    if (machine->addPassesToEmitFile(pass, dest, nullptr, llvm::CodeGenFileType::ObjectFile))
    {
        llvm::errs() << "failed to emit to file";
        return;
    }

    pass.run(*m_IRModule);
    dest.flush();
}

void Brewer::Builder::Push()
{
    m_Stack.push_back(m_Values);
}

void Brewer::Builder::Pop()
{
    m_Values = m_Stack.back();
    m_Stack.pop_back();
}

Brewer::ValuePtr& Brewer::Builder::operator[](const std::string& name)
{
    return m_Values[name];
}

Brewer::ValuePtr Brewer::Builder::GenCast(const ValuePtr& src, const TypePtr& dst)
{
    const auto src_type = src->GetType();
    if (src_type == dst)
        return src;

    const auto type = dst->GenIR(*this);

    llvm::Value* result = nullptr;
    switch (src_type->GetID())
    {
    case Type_Integer:
        switch (dst->GetID())
        {
        case Type_Integer:
            result = m_IRBuilder->CreateIntCast(src->Get(), type, true);
            break;
        case Type_Float:
            result = m_IRBuilder->CreateSIToFP(src->Get(), type);
            break;
        case Type_Pointer:
            result = m_IRBuilder->CreateIntToPtr(src->Get(), type);
            break;
        default:
            break;
        }
        break;
    case Type_Float:
        switch (dst->GetID())
        {
        case Type_Integer:
            result = m_IRBuilder->CreateFPToSI(src->Get(), type);
            break;
        case Type_Float:
            result = m_IRBuilder->CreateFPCast(src->Get(), type);
            break;
        default:
            break;
        }
        break;
    case Type_Pointer:
        switch (dst->GetID())
        {
        case Type_Integer:
            result = m_IRBuilder->CreatePtrToInt(src->Get(), type);
            break;
        case Type_Pointer:
            result = m_IRBuilder->CreatePointerCast(src->Get(), type);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    if (!result)
        return std::cerr
            << "cannot cast from " << src_type << " to " << dst
            << std::endl
            << ErrMark<ValuePtr>();

    return RValue::Direct(*this, dst, result);
}
