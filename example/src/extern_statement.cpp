#include <Brewer/Type.hpp>
#include <Brewer/Value.hpp>
#include <llvm/IR/Function.h>
#include <Test/AST.hpp>

using namespace Brewer;

Test::ExternStatement::ExternStatement(const SourceLocation& loc, Prototype proto)
    : Statement(loc), Proto(std::move(proto))
{
}

std::ostream& Test::ExternStatement::Dump(std::ostream& stream) const
{
    return stream << "extern " << Proto;
}

void Test::ExternStatement::GenIRNoVal(Builder& builder) const
{
    Proto.GenIR(builder);
}
