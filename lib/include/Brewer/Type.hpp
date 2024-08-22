#pragma once

#include <memory>
#include <string>
#include <vector>
#include <Brewer/Brewer.hpp>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>

namespace Brewer
{
    enum TypeID
    {
        Type_Void,
        Type_Integer,
        Type_Float,
        Type_Pointer,
        Type_Function,
        Type_Struct,
        Type_Array,
    };

    class Type
    {
    public:
        static TypePtr& Get(Context&, const std::string& name);
        static TypePtr GetHigherOrder(const TypePtr&, const TypePtr&);

        Type(Context&, std::string name, TypeID id, size_t size);
        virtual ~Type();

        virtual llvm::Type* GenIR(Builder&) const;

        [[nodiscard]] Context& GetContext() const;
        [[nodiscard]] const std::string& GetName() const;
        [[nodiscard]] TypeID GetID() const;
        [[nodiscard]] size_t GetSize() const;

        [[nodiscard]] bool IsVoid() const;

        [[nodiscard]] bool IsInt(size_t size = 0) const;
        [[nodiscard]] bool IsInt1() const;
        [[nodiscard]] bool IsInt8() const;
        [[nodiscard]] bool IsInt16() const;
        [[nodiscard]] bool IsInt32() const;
        [[nodiscard]] bool IsInt64() const;

        [[nodiscard]] bool IsFloat(size_t size = 0) const;
        [[nodiscard]] bool IsFloat16() const;
        [[nodiscard]] bool IsFloat32() const;
        [[nodiscard]] bool IsFloat64() const;

        [[nodiscard]] bool IsPointer() const;
        [[nodiscard]] bool IsArray() const;
        [[nodiscard]] bool IsStruct() const;
        [[nodiscard]] bool IsFunction() const;

    private:
        Context& m_Context;
        std::string m_Name;
        TypeID m_ID;
        size_t m_Size;
    };

    class PointerType : public Type
    {
    public:
        static PointerTypePtr Get(const TypePtr& base);

        PointerType(const std::string& name, const TypePtr& base);
        llvm::PointerType* GenIR(Builder&) const override;

        [[nodiscard]] TypePtr Base() const;

    private:
        TypePtr m_Base;
    };

    class ArrayType : public Type
    {
    public:
        static ArrayTypePtr Get(const TypePtr& base, size_t length);

        ArrayType(const std::string& name, const TypePtr& base, size_t length);

        llvm::ArrayType* GenIR(Builder&) const override;

        [[nodiscard]] TypePtr Base() const;
        [[nodiscard]] size_t Length() const;

    private:
        TypePtr m_Base;
        size_t m_Length;
    };

    struct StructElement
    {
        StructElement(const TypePtr&, const std::string&);

        TypePtr Type;
        std::string Name;
    };

    class StructType : public Type
    {
    public:
        static StructTypePtr Get(const std::vector<StructElement>& elements);
        static StructTypePtr Get(Context&);

        StructType(Context&, const std::string& name, size_t size, const std::vector<StructElement>& elements);

        llvm::StructType* GenIR(Builder&) const override;

    private:
        std::vector<StructElement> m_Elements;
    };

    class FunctionType : public Type
    {
    public:
        static FunctionTypePtr Get(const TypePtr& result,
                                   const std::vector<TypePtr>& params,
                                   bool vararg);

        FunctionType(const std::string& name,
                     TypePtr result,
                     const std::vector<TypePtr>& params,
                     bool vararg);
        llvm::FunctionType* GenIR(Builder&) const override;

        TypePtr Result();
        TypePtr Param(size_t i);

    private:
        TypePtr m_Result;
        std::vector<TypePtr> m_Params;
        bool m_VarArg;
    };
}
