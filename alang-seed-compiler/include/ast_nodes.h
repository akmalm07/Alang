#pragma once

#include <memory>
#include <vector>
#include <string>
#include <string_view>
#include <type_traits>
#include <cassert>

namespace alang {

    // ============================================================================
    // Forward Declarations
    // ============================================================================

    class ASTNode;
    class Expr;
    class Stmt;
    class Decl;
    class Type;
    class Atribute;
    class VarDecl;
    class FuncDecl;
    class FunctionType;
    class NumAtribute;
    class NumLiteralTypeAtribute;

    // ============================================================================
    // RTTI Enums
    // ============================================================================

    enum class ASTNodeKind 
    {
        // Expr range
        ExprBegin,
        BinaryExpr = ExprBegin,
        UnaryExpr,
        NumberLiteralExpr,
        StringLiteralExpr,
        VariableExpr,
        CallExpr,
        ExprEnd,

        // Stmt range
        StmtBegin,
        IfStmt = StmtBegin,
        WhileStmt,
        ForStmt,
        ReturnStmt,
        DeclStmt,
        StmtEnd,

        // Decl range
        DeclBegin,
        VarDecl = DeclBegin,
        FuncDecl,
        ParamDecl,
        ClassDecl,
        ModuleDecl,
        ImportDecl,
        DeclEnd,

        // Type range
        TypeBegin,
        IntType = TypeBegin,
        FloatType,
        DoubleType,
        BoolType,
        CharType,
        PointerType,
        ArrayType,
        FunctionType,
        ClassType,
        TypeEnd,

		AtributeBegin,
		ConstAtribute = AtributeBegin,
		UnsignedAtribute,
		StaticAtribute,
		AtributeEnd,
    };

    enum class NumAtributeKind
    {
        Unsigned,
        Short,
        Long,

	};

    int i = -100z;

    enum class BinaryExprOp 
    {
        Add,
        Subtract,
        Multiply,
        Divide,
        Modulo,
        And,
        Or,
        Xor,
		LogicalAnd,
		LogicalOr,

    };

    enum class UnaryExprOp 
    {
        Negate,
        Plus,
        LogicalNot,
        Dereference,
    };

    enum class PrecedenceLevel 
    {
        Lowest = 0,
        Assignment = 1,
        LogicalOr = 2,
        LogicalAnd = 3,
        BitwiseOr = 4,
        BitwiseXor = 5,
        BitwiseAnd = 6,
        Equality = 7,
        Relational = 8,
        Additive = 9,
        Multiplicative = 10,
        Unary = 11,
    };

    // ============================================================================
    // RTTI Base & CRTP Helpers
    // ============================================================================

    class ASTNode 
    {
    public:
        const ASTNodeKind kind;
        ~ASTNode() = default;

    protected:
        ASTNode(ASTNodeKind k) : kind(k) {}

    private:
    };

    // CRTP helper without subkind
    template <typename Derived, typename Base, ASTNodeKind K>
    class RTTI : public Base 
    {
    public:
        static constexpr ASTNodeKind kindValue = K;

        template <typename... Args>
        RTTI(Args&&... args)
            : Base(K, std::forward<Args>(args)...) {}

        static bool classof(const ASTNode* node) {
            return node && node->kind == K;
        }
    };


    // ============================================================================
    // Casting Utilities
    // ============================================================================

    template <typename T>
    bool isa(const ASTNode* node) 
    {
        return node && T::classof(node);
    }

    template <typename T>
    T* cast(ASTNode* node) {
        assert(isa<T>(node) && "Invalid cast");
        return static_cast<T*>(node);
    }

    template <typename T>
    const T* cast(const ASTNode* node) 
    {
        assert(isa<T>(node) && "Invalid cast");
        return static_cast<const T*>(node);
    }

    template <typename T>
    T* dyn_cast(ASTNode* node) 
    {
        return isa<T>(node) ? static_cast<T*>(node) : nullptr;
    }

    template <typename T>
    const T* dyn_cast(const ASTNode* node) 
    {
        return isa<T>(node) ? static_cast<const T*>(node) : nullptr;
    }

    // ============================================================================
    // Intermediate Base Classes (with Subkinds)
    // ============================================================================

    class Expr : public ASTNode 
    {
    public:

        static bool classof(const ASTNode* node) 
        {
            return node && node->kind >= ASTNodeKind::ExprBegin &&
                   node->kind < ASTNodeKind::ExprEnd;
        }

    protected:
        Expr(ASTNodeKind k) : ASTNode(k) {}

    private:
    };

    class Stmt : public ASTNode 
    {
    public:
        static bool classof(const ASTNode* node) 
        {
            return node && node->kind >= ASTNodeKind::StmtBegin &&
                   node->kind < ASTNodeKind::StmtEnd;
        }

    protected:
        Stmt(ASTNodeKind k) : ASTNode(k) {}

    };

    class Decl : public ASTNode 
    {
    public:

        static bool classof(const ASTNode* node) 
        {
            return node && node->kind >= ASTNodeKind::DeclBegin &&
                   node->kind < ASTNodeKind::DeclEnd;
        }

    protected:
        Decl(ASTNodeKind k) : ASTNode(k) {}
    };

    class Type : public ASTNode 
    {
    public:

        static bool classof(const ASTNode* node)
        {
            return node && node->kind >= ASTNodeKind::TypeBegin &&
                   node->kind < ASTNodeKind::TypeEnd;
        }

    protected:
        Type(ASTNodeKind k) : ASTNode(k) {}
    };

    class Atribute : public ASTNode 
    {
    public:

        static bool classof(const ASTNode* node) 
        {
            return node && node->kind >= ASTNodeKind::AtributeBegin &&
                   node->kind < ASTNodeKind::AtributeEnd;
		}

        virtual ~Atribute() = default;
    protected:
        Atribute(ASTNodeKind k) : ASTNode(k) {}
    };

    // ============================================================================
    // Expression Nodes
    // ============================================================================

    class Operator : public Expr 
    {
    public:
        unsigned int get_precedence() const { return precedence; }
        const Expr* get_value() const { return value; }
        bool is_set() const { return value != nullptr; }
        void set_value(Expr* val) { value = val; }

    protected:
        Operator(ASTNodeKind k, unsigned int prec)
            : Expr(k) {}
    
        Expr* value = nullptr;
    };

    class BinaryExpr : public RTTI<BinaryExpr, Operator, ASTNodeKind::BinaryExpr> 
    {
    public:
        BinaryExpr(Expr* left, BinaryExprOp op)
            : RTTI(), left(left), opType(op) {}

        const Expr* get_left() const { return left; }
        void set_left(Expr* lhs) { left = lhs; }
	    void set_right(Expr* rhs) { set_value(rhs); }
        const Expr* get_right() const { return value; }

        const BinaryExprOp opType;
    private:
        Expr* left;
    };

	unsigned int get_precedence(BinaryExprOp op);

    class UnaryExpr : public RTTI<UnaryExpr, Operator, ASTNodeKind::UnaryExpr> 
    {
    public:
        UnaryExpr(UnaryExprOp op)
            : RTTI(), opType(op) {}

        const UnaryExprOp opType;
    private:
    };


    class LiteralExpr : public Expr 
    {
    protected:
        LiteralExpr(ASTNodeKind k) : Expr(k) {}
    };

    template<typename T>
    class NumberLiteralExpr : public RTTI<NumberLiteralExpr<T>, LiteralExpr, ASTNodeKind::NumberLiteralExpr> 
    {
    public:
        NumberLiteralExpr(T val, NumAtribute* numAttr = nullptr,
                          NumLiteralTypeAtribute* typeAttr = nullptr)
            : RTTI(), value(val), numAtribute(numAttr),
              numLiteralTypeAtribute(typeAttr) {}

        T get_value() const { return value; }

    private:
        T value;
        NumAtribute* numAtribute;
        NumLiteralTypeAtribute* numLiteralTypeAtribute;
    };

    class StringLiteralExpr : public RTTI<StringLiteralExpr, LiteralExpr, ASTNodeKind::StringLiteralExpr> 
    {
    public:
        StringLiteralExpr(std::string val)
            : RTTI(), value(std::move(val)) {}

        const std::string& get_value() const { return value; }

    private:
        std::string value;
    };

    class VariableExpr : public RTTI<VariableExpr, Expr, ASTNodeKind::VariableExpr> 
    {
    public:
        VariableExpr(std::string name, VarDecl* decl)
            : RTTI(), name(std::move(name)), declaration(decl) {}

        const std::string& get_name() const { return name; }
        VarDecl* get_decl() const { return declaration; }

    private:
        std::string name;
        VarDecl* declaration;
    };

    class CallExpr : public RTTI<CallExpr, Expr, ASTNodeKind::CallExpr> 
    {
    public:
        CallExpr(FuncDecl* callee, std::vector<Expr*> args)
            : RTTI(), callee(callee), arguments(std::move(args)) {}

        FuncDecl* get_callee() const { return callee; }
        const std::vector<Expr*>& get_arguments() const { return arguments; }

    private:
        FuncDecl* callee;
        std::vector<Expr*> arguments;
    };

    // ============================================================================
    // Statement Nodes
    // ============================================================================

    class IfStmt : public RTTI<IfStmt, Stmt, ASTNodeKind::IfStmt> 
    {
    public:
        IfStmt(Expr* cond, Stmt* thenBr, Stmt* elseBr = nullptr)
            : RTTI(), condition(cond), thenBranch(thenBr), elseBranch(elseBr) {}

        Expr* get_condition() const { return condition; }
        Stmt* get_then_branch() const { return thenBranch; }
        Stmt* get_else_branch() const { return elseBranch; }

    private:
        Expr* condition;
        Stmt* thenBranch;
        Stmt* elseBranch;
    };

    class WhileStmt : public RTTI<WhileStmt, Stmt, ASTNodeKind::WhileStmt> 
    {
    public:
        WhileStmt(Expr* cond, Stmt* body)
            : RTTI(), condition(cond), body(body) {}

        Expr* get_condition() const { return condition; }
        Stmt* get_body() const { return body; }

    private:
        Expr* condition;
        Stmt* body;
    };

    class ForStmt : public RTTI<ForStmt, Stmt, ASTNodeKind::ForStmt> 
    {
    public:
        ForStmt(Stmt* init, Expr* cond, Expr* inc, Stmt* body)
            : RTTI(), initializer(init), condition(cond), increment(inc), body(body) {}

        Stmt* get_initializer() const { return initializer; }
        Expr* get_condition() const { return condition; }
        Expr* get_increment() const { return increment; }
        Stmt* get_body() const { return body; }

    private:
        Stmt* initializer;
        Expr* condition;
        Expr* increment;
        Stmt* body;
    };

    class ReturnStmt : public RTTI<ReturnStmt, Stmt, ASTNodeKind::ReturnStmt> 
    {
    public:
        ReturnStmt(Expr* val = nullptr)
            : RTTI(), value(val) {}

        Expr* get_value() const { return value; }

    private:
        Expr* value;
    };

    // ============================================================================
    // Declaration Nodes
    // ============================================================================

    class VarDecl : public RTTI<VarDecl, Decl, ASTNodeKind::VarDecl> 
    {
    public:
        VarDecl(Type* type, std::string_view name)
            : RTTI(), varType(type), name(name), initializer(nullptr) {}

        VarDecl(Type* type, std::string_view name, std::vector<Atribute*> attrs)
            : RTTI(), varType(type), name(name),
              atributes(std::move(attrs)), initializer(nullptr) {}

        std::string_view get_name() const { return name; }
        void add_initializer(Expr* init) { initializer = init; }
        Type* get_type() const { return varType; }

    private:
        std::string name;
        Type* varType;
        std::vector<Atribute*> atributes;
        Expr* initializer;
    };

    class FuncDecl : public RTTI<FuncDecl, Decl, ASTNodeKind::FuncDecl>
    {
    public:
        FuncDecl(FunctionType* type, const std::string& name,
                 std::vector<std::string_view> paramNames, std::vector<Stmt*> stmts);

        std::string_view get_mangled_name() const { return mangledName; }
        std::string_view get_name() const { return name; }
        const std::vector<Stmt*>& get_statements() const { return statements; }
        size_t get_param_count() const { return paramNames.size(); }

    private:
        std::string mangledName;
        std::string name;
        FunctionType* type;
        std::vector<std::string_view> paramNames;
        std::vector<Stmt*> statements;

        void set_mangled_name(FunctionType* type, std::string_view name);
    };

    class ParamDecl : public RTTI<ParamDecl, Decl, ASTNodeKind::ParamDecl>
    {
    public:
        ParamDecl(Type* type, std::string_view name)
            : RTTI(), paramType(type), name(name) {}

        std::string_view get_name() const { return name; }
        Type* get_type() const { return paramType; }

    private:
        std::string name;
        Type* paramType;
    };

    class ClassDecl : public RTTI<ClassDecl, Decl, ASTNodeKind::ClassDecl>
    {
    public:
        ClassDecl(std::string name)
            : RTTI(), name(std::move(name)) {}

        void add_private_member(Decl* decl) { privateMembers.push_back(decl); }
        void add_public_member(Decl* decl) { publicMembers.push_back(decl); }
        std::string_view get_name() const { return name; }

    private:
        std::string name;
        std::vector<Decl*> privateMembers;
        std::vector<Decl*> publicMembers;
    };

    class ModuleDecl : public RTTI<ModuleDecl, Decl, ASTNodeKind::ModuleDecl>
    {
    public:
        ModuleDecl(std::string name)
            : RTTI(), name(std::move(name)) {}

        void add_func_decl(FuncDecl* decl) { funcDecls.push_back(decl); }
        void add_class_decl(ClassDecl* decl) { classDecls.push_back(decl); }
        void add_var_decl(VarDecl* decl) { varDecls.push_back(decl); }
		void add_import(class ImportDecl* importDecl) { imports.push_back(importDecl); }

        std::string_view get_name() const { return name; }

        const std::vector<FuncDecl*>& get_func_decls() const { return funcDecls; }
        const std::vector<ClassDecl*>& get_class_decls() const { return classDecls; }
        const std::vector<VarDecl*>& get_var_decls() const { return varDecls; }

    private:
        std::string name;
		std::vector<class ImportDecl*> imports;
        std::vector<FuncDecl*> funcDecls;
        std::vector<ClassDecl*> classDecls;
        std::vector<VarDecl*> varDecls;
    };

    class ImportDecl : public RTTI<ImportDecl, Decl, ASTNodeKind::ImportDecl>
    {
    public:
        ImportDecl(std::string moduleName, ModuleDecl* module)
            : RTTI(), moduleName(std::move(moduleName)), module(module) {}

        std::string_view get_module_name() const { return moduleName; }
        ModuleDecl* get_module() const { return module; }

    private:
        std::string moduleName;
        ModuleDecl* module;
    };

    // ============================================================================
    // Type Nodes
    // ============================================================================

    class PrimitiveType : public Type {
    protected:
        PrimitiveType(ASTNodeKind k) : Type(k) {}
    };

    class IntType : public RTTI<IntType, PrimitiveType, ASTNodeKind::IntType> {
    public:
        IntType() : RTTI() {}
        int val = 0;
    };

    class FloatType : public RTTI<FloatType, PrimitiveType, ASTNodeKind::FloatType> {
    public:
        FloatType() : RTTI() {}
        float val = 0.0f;
    };

    class DoubleType : public RTTI<DoubleType, PrimitiveType, ASTNodeKind::DoubleType> {
    public:
        DoubleType() : RTTI() {}
        double val = 0.0;
    };

    class BoolType : public RTTI<BoolType, PrimitiveType, ASTNodeKind::BoolType> {
    public:
        BoolType() : RTTI() {}
        bool val = false;
    };

    class CharType : public RTTI<CharType, PrimitiveType, ASTNodeKind::CharType> {
    public:
        CharType() : RTTI() {}
        char val = '\0';
    };

    class PointerType : public RTTI<PointerType, Type, ASTNodeKind::PointerType> {
    public:
        PointerType(Type* type)
            : RTTI(), pointeeType(type) {}

        Type* get_pointee_type() const { return pointeeType; }

    private:
        Type* pointeeType;
    };

    class ArrayType : public RTTI<ArrayType, Type, ASTNodeKind::ArrayType> {
    public:
        ArrayType(Type* elemType, size_t size)
            : RTTI(), elementType(elemType), size(size) {}

		size_t get_size() const { return size; }

        Type* get_element_type() const { return elementType; }

    private:
		Type* elementType;
        const size_t size = 0;
    };

    class FunctionType : public RTTI<FunctionType, Type, ASTNodeKind::FunctionType> 
    {
    public:
        FunctionType(Type* returnType, std::vector<Type*> paramTypes)
            : RTTI(), returnType(returnType), parameterTypes(std::move(paramTypes)) {
        }
        Type* get_return_type() const { return returnType; }
        const std::vector<Type*>& get_param_types() const { return parameterTypes; }
    private:
		Type* returnType;
		std::vector<Type*> parameterTypes;
    };

    class ClassType : public RTTI<ClassType, Type, ASTNodeKind::ClassType> {
    public:
        ClassType(std::string className)
            : RTTI(), className(std::move(className)) {}

        std::string_view get_class_name() const { return className; }

    private:
        std::string className;
    };

    // ============================================================================
    // Attribute Nodes
    // ============================================================================

    class ConstAtribute : public Atribute {
    public:
        ConstAtribute() : Atribute(ASTNodeKind::ConstAtribute) {} // Placeholder
    };

    class UnsignedAtribute : public Atribute {
    public:
        UnsignedAtribute() : Atribute(ASTNodeKind::UnsignedAtribute) {}
    };

    class NumAtribute : public Atribute {
    public:
		const NumAtributeKind kind;
        NumAtribute() : Atribute(ASTNodeKind::NumAtribute) {}
    };

    class UnsignedNumAtribute : public NumAtribute {};
    class LongNumAtribute : public NumAtribute {};
    class ShortNumAtribute : public NumAtribute {};

    class NumLiteralTypeAtribute : public Atribute {
    public:
        NumLiteralTypeAtribute() : Atribute(ASTNodeKind::ExprBegin) {}
    };

    class FloatAtribute : public NumLiteralTypeAtribute {};
    class DoubleAtribute : public NumLiteralTypeAtribute {};

    // ============================================================================
    // Concepts
    // ============================================================================

    template<typename T>
    concept NameableDecl = std::is_base_of_v<Decl, T> && requires(T t) {
        { t.get_name() } -> std::convertible_to<std::string_view>;
    };

    template<typename T>
    concept IsBinaryOpDerived = std::derived_from<T, BinaryExpr> && !std::same_as<T, BinaryExpr>;

    template<typename T>
    concept IsUnaryOpDerived = std::derived_from<T, UnaryExpr> && !std::same_as<T, UnaryExpr>;

    template<typename T>
    concept IsOperatorDerived = std::derived_from<T, Operator> && !std::same_as<T, Operator>;

    template<typename T>
    concept IsDeclDerived = std::derived_from<T, Decl> && !std::same_as<T, Decl>;

    // ============================================================================
    // Type Aliases
    // ============================================================================

    using Modules = std::vector<ModuleDecl*>;

    


} // namespace alang

