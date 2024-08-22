#pragma once

#include <functional>
#include <memory>

namespace Brewer
{
    class Context;

    struct SourceLocation;
    struct Token;

    class Parser;

    class Builder;

    class Type;
    class PointerType;
    class ArrayType;
    class StructType;
    class FunctionType;

    typedef std::shared_ptr<Type> TypePtr;
    typedef std::shared_ptr<PointerType> PointerTypePtr;
    typedef std::shared_ptr<ArrayType> ArrayTypePtr;
    typedef std::shared_ptr<StructType> StructTypePtr;
    typedef std::shared_ptr<FunctionType> FunctionTypePtr;

    class Value;
    class RValue;
    class LValue;

    typedef std::shared_ptr<Value> ValuePtr;
    typedef std::shared_ptr<RValue> RValuePtr;
    typedef std::shared_ptr<LValue> LValuePtr;

    struct Statement;
    struct Expression;

    typedef std::unique_ptr<Statement> StmtPtr;
    typedef std::unique_ptr<Expression> ExprPtr;

    class Pipeline;

    typedef std::function<StmtPtr(Parser&)> StmtFn;
    typedef std::function<ExprPtr(Parser&)> ExprFn;
    typedef std::function<ValuePtr(Builder&, const ValuePtr&, const ValuePtr&, TypePtr*)> BinaryFn;
    typedef std::function<ValuePtr(Builder&, const ValuePtr&, TypePtr*)> UnaryFn;
}
