#include <iostream>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>
#include <Brewer/Value.hpp>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>

Brewer::Builder::Builder(const std::string& module_id, const std::string& filename)
{
    m_Context = LLVMContextCreate();
    m_IRBuilder = LLVMCreateBuilderInContext(m_Context);
    m_Module = LLVMModuleCreateWithNameInContext(module_id.c_str(), m_Context);
    LLVMSetSourceFileName(m_Module, filename.c_str(), filename.length());

    const auto ft = LLVMFunctionType(LLVMInt32TypeInContext(m_Context), nullptr, 0, false);
    m_Global = LLVMAddFunction(m_Module, "main", ft);
    const auto bb = LLVMAppendBasicBlock(m_Global, "entry");
    LLVMPositionBuilderAtEnd(m_IRBuilder, bb);
}

Brewer::Builder::~Builder()
{
    LLVMDisposeModule(m_Module);
    LLVMDisposeBuilder(m_IRBuilder);
    LLVMContextDispose(m_Context);
}

const LLVMContextRef& Brewer::Builder::Context() const
{
    return m_Context;
}

const LLVMBuilderRef& Brewer::Builder::IRBuilder() const
{
    return m_IRBuilder;
}

const LLVMModuleRef& Brewer::Builder::Module() const
{
    return m_Module;
}

void Brewer::Builder::Close() const
{
    const auto val = LLVMConstInt(LLVMInt32TypeInContext(m_Context), 0, false);
    LLVMBuildRet(m_IRBuilder, val);
}

void Brewer::Builder::Dump() const
{
    LLVMDumpModule(m_Module);
}

void Brewer::Builder::EmitToFile(const std::string& filename) const
{
    LLVMInitializeAllTargetInfos();
    LLVMInitializeAllTargets();
    LLVMInitializeAllTargetMCs();
    LLVMInitializeAllAsmParsers();
    LLVMInitializeAllAsmPrinters();

    char* error;
    LLVMTargetRef target;

    const auto triple = LLVMGetDefaultTargetTriple();
    if (LLVMGetTargetFromTriple(triple, &target, &error))
    {
        std::cerr << "failed to get target from triple '" << triple << "': " << error << std::endl;
        LLVMDisposeMessage(error);
        return;
    }

    const auto cpu = "generic";
    const auto features = "";

    const auto machine = LLVMCreateTargetMachine(target,
                                                 triple,
                                                 cpu,
                                                 features,
                                                 LLVMCodeGenLevelDefault,
                                                 LLVMRelocPIC,
                                                 LLVMCodeModelDefault);

    LLVMSetModuleDataLayout(m_Module, LLVMCreateTargetDataLayout(machine));

    if (LLVMTargetMachineEmitToFile(machine, m_Module, filename.c_str(), LLVMObjectFile, &error))
    {
        std::cerr << "failed to emit to file '" << filename << "': " << error << std::endl;
        LLVMDisposeMessage(error);
    }
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

Brewer::ValuePtr Brewer::Builder::GenEQ(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildICmp(m_IRBuilder, LLVMIntEQ, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, Type::Get("i1"), result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFCmp(m_IRBuilder, LLVMRealOEQ, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, Type::Get("i1"), result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenNE(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildICmp(m_IRBuilder, LLVMIntNE, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, Type::Get("i1"), result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFCmp(m_IRBuilder, LLVMRealONE, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, Type::Get("i1"), result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenLT(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildICmp(m_IRBuilder, LLVMIntSLT, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, Type::Get("i1"), result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFCmp(m_IRBuilder, LLVMRealOLT, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, Type::Get("i1"), result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenGT(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildICmp(m_IRBuilder, LLVMIntSGT, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, Type::Get("i1"), result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFCmp(m_IRBuilder, LLVMRealOGT, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, Type::Get("i1"), result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenLE(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildICmp(m_IRBuilder, LLVMIntSLE, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, Type::Get("i1"), result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFCmp(m_IRBuilder, LLVMRealOLE, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, Type::Get("i1"), result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenGE(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildICmp(m_IRBuilder, LLVMIntSGE, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, Type::Get("i1"), result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFCmp(m_IRBuilder, LLVMRealOGE, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, Type::Get("i1"), result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenLAnd(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = LLVMBuildIsNotNull(m_IRBuilder, lhs->Get(), "");
    const auto r = LLVMBuildIsNotNull(m_IRBuilder, rhs->Get(), "");

    const auto result = LLVMBuildAnd(m_IRBuilder, l, r, "");
    return RValue::Direct(*this, Type::Get("i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenLOr(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = LLVMBuildIsNotNull(m_IRBuilder, lhs->Get(), "");
    const auto r = LLVMBuildIsNotNull(m_IRBuilder, rhs->Get(), "");

    const auto result = LLVMBuildOr(m_IRBuilder, l, r, "");
    return RValue::Direct(*this, Type::Get("i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenLXor(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = LLVMBuildIsNotNull(m_IRBuilder, lhs->Get(), "");
    const auto r = LLVMBuildIsNotNull(m_IRBuilder, rhs->Get(), "");

    const auto result = LLVMBuildXor(m_IRBuilder, l, r, "");
    return RValue::Direct(*this, Type::Get("i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenAdd(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildAdd(m_IRBuilder, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, type, result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFAdd(m_IRBuilder, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, type, result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenSub(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildSub(m_IRBuilder, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, type, result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFSub(m_IRBuilder, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, type, result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenMul(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildMul(m_IRBuilder, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, type, result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFMul(m_IRBuilder, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, type, result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenDiv(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildSDiv(m_IRBuilder, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, type, result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFDiv(m_IRBuilder, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, type, result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenRem(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildSRem(m_IRBuilder, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, type, result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFRem(m_IRBuilder, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(*this, type, result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenAnd(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto result = LLVMBuildAnd(m_IRBuilder, lhs->Get(), rhs->Get(), "");
    return RValue::Direct(*this, lhs->GetType(), result);
}

Brewer::ValuePtr Brewer::Builder::GenOr(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto result = LLVMBuildOr(m_IRBuilder, lhs->Get(), rhs->Get(), "");
    return RValue::Direct(*this, lhs->GetType(), result);
}

Brewer::ValuePtr Brewer::Builder::GenXor(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto result = LLVMBuildXor(m_IRBuilder, lhs->Get(), rhs->Get(), "");
    return RValue::Direct(*this, lhs->GetType(), result);
}

Brewer::ValuePtr Brewer::Builder::GenShl(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto result = LLVMBuildShl(m_IRBuilder, lhs->Get(), rhs->Get(), "");
    return RValue::Direct(*this, lhs->GetType(), result);
}

Brewer::ValuePtr Brewer::Builder::GenShr(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto result = LLVMBuildLShr(m_IRBuilder, lhs->Get(), rhs->Get(), "");
    return RValue::Direct(*this, lhs->GetType(), result);
}

Brewer::ValuePtr Brewer::Builder::GenAShr(const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto result = LLVMBuildAShr(m_IRBuilder, lhs->Get(), rhs->Get(), "");
    return RValue::Direct(*this, lhs->GetType(), result);
}

Brewer::ValuePtr Brewer::Builder::GenNeg(const ValuePtr& val)
{
    const auto type = val->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildNeg(m_IRBuilder, val->Get(), "");
        return RValue::Direct(*this, type, result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFNeg(m_IRBuilder, val->Get(), "");
        return RValue::Direct(*this, type, result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenLNot(const ValuePtr& val)
{
    const auto result = LLVMBuildIsNull(m_IRBuilder, val->Get(), "");
    return RValue::Direct(*this, Type::Get("i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenNot(const ValuePtr& val)
{
    const auto result = LLVMBuildNot(m_IRBuilder, val->Get(), "");
    return RValue::Direct(*this, val->GetType(), result);
}

Brewer::ValuePtr Brewer::Builder::GenCast(const ValuePtr& value, const TypePtr& type)
{
    const auto value_type = value->GetType();
    if (value_type == type)
        return value;

    const auto val = value->Get();
    const auto ty = type->GenIR(*this);

    if (value_type->IsInt())
    {
        if (type->IsInt())
        {
            const auto result = LLVMBuildIntCast2(m_IRBuilder, val, ty, true, "");
            return RValue::Direct(*this, type, result);
        }

        if (type->IsFloat())
        {
            const auto result = LLVMBuildSIToFP(m_IRBuilder, val, ty, "");
            return RValue::Direct(*this, type, result);
        }

        if (type->IsPointer())
        {
            const auto result = LLVMBuildIntToPtr(m_IRBuilder, val, ty, "");
            return RValue::Direct(*this, type, result);
        }
    }

    if (value_type->IsFloat())
    {
        if (type->IsInt())
        {
            const auto result = LLVMBuildFPToSI(m_IRBuilder, val, ty, "");
            return RValue::Direct(*this, type, result);
        }

        if (type->IsFloat())
        {
            const auto result = LLVMBuildFPCast(m_IRBuilder, val, ty, "");
            return RValue::Direct(*this, type, result);
        }
    }

    if (value_type->IsPointer())
    {
        if (type->IsInt())
        {
            const auto result = LLVMBuildPtrToInt(m_IRBuilder, val, ty, "");
            return RValue::Direct(*this, type, result);
        }

        if (type->IsPointer())
        {
            const auto result = LLVMBuildPointerCast(m_IRBuilder, val, ty, "");
            return RValue::Direct(*this, type, result);
        }
    }

    return error<ValuePtr>("undefined cast from %s to %s\n", value_type->Name().c_str(), type->Name().c_str());
}
