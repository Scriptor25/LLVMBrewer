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

Brewer::ValuePtr& Brewer::Builder::GetFunction(const TypePtr& self, const std::string& name)
{
    return m_Functions[self][name];
}

Brewer::ValuePtr Brewer::Builder::GetCtor(const TypePtr& type)
{
    for (const auto& [self, func] : m_Functions[{}])
    {
        const auto fun_type = FunctionType::From(PointerType::From(func->GetType())->GetBase());
        if (fun_type->GetMode() != FuncMode_Ctor) continue;
        if (fun_type->GetSelf() != type) continue;
        return func;
    }

    return {};
}

Brewer::ValuePtr& Brewer::Builder::GetSymbol(const std::string& name)
{
    return m_Symbols[name];
}

void Brewer::Builder::Push()
{
    m_Stack.push_back(m_Symbols);
}

void Brewer::Builder::Pop()
{
    m_Symbols = m_Stack.back();
    m_Stack.pop_back();
}

Brewer::TypePtr& Brewer::Builder::CurrentResult()
{
    return m_CurrentResult;
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
            << "cannot cast from " << src_type->GetName() << " to " << dst->GetName()
            << std::endl
            << ErrMark<ValuePtr>();

    return RValue::Direct(*this, dst, result);
}
