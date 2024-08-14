#pragma once

namespace Brewer
{
    class Builder;
    class Parser;
    struct Statement;
    struct Expression;
    struct SourceLocation;
    struct Token;
    class Type;
    class Value;
    class RValue;
    class LValue;

    typedef std::unique_ptr<Statement> StmtPtr;
    typedef std::unique_ptr<Expression> ExprPtr;
    typedef std::shared_ptr<Type> TypePtr;
    typedef std::shared_ptr<Value> ValuePtr;
    typedef std::shared_ptr<RValue> RValuePtr;
    typedef std::shared_ptr<LValue> LValuePtr;
}
