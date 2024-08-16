#include <iostream>
#include <Brewer/Builder.hpp>
#include <Brewer/Type.hpp>
#include <Brewer/Util.hpp>
#include <Brewer/Value.hpp>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>

Brewer::ValuePtr Brewer::Builder::GenEQ(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildICmp(builder.IRBuilder(), LLVMIntEQ, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, Type::Get("i1"), result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFCmp(builder.IRBuilder(), LLVMRealOEQ, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, Type::Get("i1"), result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenNE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildICmp(builder.IRBuilder(), LLVMIntNE, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, Type::Get("i1"), result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFCmp(builder.IRBuilder(), LLVMRealONE, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, Type::Get("i1"), result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenLT(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildICmp(builder.IRBuilder(), LLVMIntSLT, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, Type::Get("i1"), result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFCmp(builder.IRBuilder(), LLVMRealOLT, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, Type::Get("i1"), result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenGT(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildICmp(builder.IRBuilder(), LLVMIntSGT, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, Type::Get("i1"), result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFCmp(builder.IRBuilder(), LLVMRealOGT, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, Type::Get("i1"), result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenLE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildICmp(builder.IRBuilder(), LLVMIntSLE, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, Type::Get("i1"), result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFCmp(builder.IRBuilder(), LLVMRealOLE, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, Type::Get("i1"), result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenGE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildICmp(builder.IRBuilder(), LLVMIntSGE, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, Type::Get("i1"), result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFCmp(builder.IRBuilder(), LLVMRealOGE, lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, Type::Get("i1"), result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenLAnd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = LLVMBuildIsNotNull(builder.IRBuilder(), lhs->Get(), "");
    const auto r = LLVMBuildIsNotNull(builder.IRBuilder(), rhs->Get(), "");

    const auto result = LLVMBuildAnd(builder.IRBuilder(), l, r, "");
    return RValue::Direct(builder, Type::Get("i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenLOr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = LLVMBuildIsNotNull(builder.IRBuilder(), lhs->Get(), "");
    const auto r = LLVMBuildIsNotNull(builder.IRBuilder(), rhs->Get(), "");

    const auto result = LLVMBuildOr(builder.IRBuilder(), l, r, "");
    return RValue::Direct(builder, Type::Get("i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenLXor(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto l = LLVMBuildIsNotNull(builder.IRBuilder(), lhs->Get(), "");
    const auto r = LLVMBuildIsNotNull(builder.IRBuilder(), rhs->Get(), "");

    const auto result = LLVMBuildXor(builder.IRBuilder(), l, r, "");
    return RValue::Direct(builder, Type::Get("i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenAdd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildAdd(builder.IRBuilder(), lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, type, result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFAdd(builder.IRBuilder(), lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, type, result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenSub(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildSub(builder.IRBuilder(), lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, type, result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFSub(builder.IRBuilder(), lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, type, result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenMul(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildMul(builder.IRBuilder(), lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, type, result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFMul(builder.IRBuilder(), lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, type, result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenDiv(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildSDiv(builder.IRBuilder(), lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, type, result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFDiv(builder.IRBuilder(), lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, type, result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenRem(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto type = lhs->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildSRem(builder.IRBuilder(), lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, type, result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFRem(builder.IRBuilder(), lhs->Get(), rhs->Get(), "");
        return RValue::Direct(builder, type, result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenAnd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto result = LLVMBuildAnd(builder.IRBuilder(), lhs->Get(), rhs->Get(), "");
    return RValue::Direct(builder, lhs->GetType(), result);
}

Brewer::ValuePtr Brewer::Builder::GenOr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto result = LLVMBuildOr(builder.IRBuilder(), lhs->Get(), rhs->Get(), "");
    return RValue::Direct(builder, lhs->GetType(), result);
}

Brewer::ValuePtr Brewer::Builder::GenXor(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto result = LLVMBuildXor(builder.IRBuilder(), lhs->Get(), rhs->Get(), "");
    return RValue::Direct(builder, lhs->GetType(), result);
}

Brewer::ValuePtr Brewer::Builder::GenShl(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto result = LLVMBuildShl(builder.IRBuilder(), lhs->Get(), rhs->Get(), "");
    return RValue::Direct(builder, lhs->GetType(), result);
}

Brewer::ValuePtr Brewer::Builder::GenShr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto result = LLVMBuildLShr(builder.IRBuilder(), lhs->Get(), rhs->Get(), "");
    return RValue::Direct(builder, lhs->GetType(), result);
}

Brewer::ValuePtr Brewer::Builder::GenAShr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    const auto result = LLVMBuildAShr(builder.IRBuilder(), lhs->Get(), rhs->Get(), "");
    return RValue::Direct(builder, lhs->GetType(), result);
}

Brewer::ValuePtr Brewer::Builder::GenInc(Builder& builder, const ValuePtr& val)
{
    const auto one = LLVMConstInt(val->GetIRType(), 1, true);
    return builder.GenBinaryFn("+")(builder, val, RValue::Direct(builder, val->GetType(), one));
}

Brewer::ValuePtr Brewer::Builder::GenDec(Builder& builder, const ValuePtr& val)
{
    const auto one = LLVMConstInt(val->GetIRType(), 1, true);
    return builder.GenBinaryFn("-")(builder, val, RValue::Direct(builder, val->GetType(), one));
}

Brewer::ValuePtr Brewer::Builder::GenNeg(Builder& builder, const ValuePtr& val)
{
    const auto type = val->GetType();

    if (type->IsInt())
    {
        const auto result = LLVMBuildNeg(builder.IRBuilder(), val->Get(), "");
        return RValue::Direct(builder, type, result);
    }
    if (type->IsFloat())
    {
        const auto result = LLVMBuildFNeg(builder.IRBuilder(), val->Get(), "");
        return RValue::Direct(builder, type, result);
    }

    return {};
}

Brewer::ValuePtr Brewer::Builder::GenLNot(Builder& builder, const ValuePtr& val)
{
    const auto result = LLVMBuildIsNull(builder.IRBuilder(), val->Get(), "");
    return RValue::Direct(builder, Type::Get("i1"), result);
}

Brewer::ValuePtr Brewer::Builder::GenNot(Builder& builder, const ValuePtr& val)
{
    const auto result = LLVMBuildNot(builder.IRBuilder(), val->Get(), "");
    return RValue::Direct(builder, val->GetType(), result);
}

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
    m_BinaryFns[">>"] = GenShr;
    m_BinaryFns[">>>"] = GenAShr;

    m_UnaryFns["-"] = GenNeg;
    m_UnaryFns["!"] = GenLNot;
    m_UnaryFns["~"] = GenNot;
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

Brewer::BinaryFn& Brewer::Builder::GenBinaryFn(const std::string& operator_)
{
    return m_BinaryFns[operator_];
}

Brewer::UnaryFn& Brewer::Builder::GenUnaryFn(const std::string& operator_)
{
    return m_UnaryFns[operator_];
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
