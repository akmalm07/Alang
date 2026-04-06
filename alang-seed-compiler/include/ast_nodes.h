#pragma once

#include <memory>
#include <vector>
#include <string>
#include <type_traits>


namespace alang {


	enum class ASTExprType
	{
		Binary,
		Unary,
		NumberLiteral,
		StringLiteral,
		Variable,
		Call,
		Container
	};

	std::string_view stringify(ASTExprType type);

	enum class ASTDeclType
	{
		Var,
		Stmt,
		Func,
		Param,
		Class,
		Module,
		Import,
	};

	std::string_view stringify(ASTDeclType type);

	enum class ASTType
	{
		Int,
		Float,
		Double,
		Bool,
		Char,
		Pointer,
		Array,
		Function,
		Class, // Representing custom types
	};

	enum class ASTStmtType
	{
		If,
		While,
		For,
		Return,
		Decl,
	};

	std::string stringify(class Type* type);

	// TODO: Add Short and Long attributes!!

	// Base class for all AST nodes
	struct ASTNode
	{
	public:
		virtual ~ASTNode() = default;
	};


	// Derived classes for different types of AST nodes
	struct Expr : public ASTNode
	{
	public:
		const ASTExprType type;
		Expr& operator=(const Expr&) = delete; 
		Expr(const Expr&) = delete;

	protected:
		Expr(ASTExprType type) : type(type) {}

	public:
		virtual ~Expr() = default;
	};

	struct Stmt : public ASTNode
	{
	public:
		const ASTStmtType type;
		virtual ~Stmt() = default;
	protected:
		Stmt(ASTStmtType type) : type(type) {}
	};

	struct Decl : public ASTNode
	{
	public:
		const ASTDeclType type;

	protected:
		Decl(ASTDeclType type) : type(type) {}

	public:
		virtual ~Decl() = default;
	};

	struct Type : public ASTNode
	{
	public:
		const ASTType type;

	protected:
		Type(ASTType type) : type(type) {}

	public:
		virtual ~Type() = default;
	};

	struct Atribute : public ASTNode
	{
	public:
		virtual ~Atribute() = default;
	};

	struct Module : public ASTNode
	{
	public:
		virtual ~Module() = default;
	};

	// Important concepts
	template<typename T>
	concept NameableDecl = std::is_base_of_v<Decl, T> && requires(T t) {
		{ t.get_name() } -> std::convertible_to<std::string_view>;
	};

	struct Operator : public Expr
	{
	protected:
		Operator(ASTExprType type, unsigned int precedence) : Expr(type), precedence(precedence) {}
		Expr* value;
	public:
		Expr* get_value() const { return value; }
		bool is_set() const { return value; }
		void set_value(Expr* val);
		virtual ~Operator() = default;
		
		const unsigned int precedence; 
	};

	enum class PrecidenceLevel
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

	// Further derived classes of Expressions
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
	};
	std::string_view stringify(BinaryExprOp op);

	struct BinaryExpr : public Operator
	{
	public:
		BinaryExpr(Expr* left, BinaryExprOp op, unsigned int precedence) 
			: Operator(ASTExprType::Binary, precedence), left(left), right(value), op_type(op) {}
		Expr* get_left() const { return left; }
		void set_left(Expr* lhs) { left = lhs; }
		Expr* get_right() const { return right; }
		virtual ~BinaryExpr() = default;
		
		const BinaryExprOp op_type;
	protected:
		Expr* left;
		Expr*& right;
	};

	struct AddOp : public BinaryExpr
	{
	public:
		AddOp(Expr* left) : BinaryExpr(left, BinaryExprOp::Add, static_cast<unsigned int>(PrecidenceLevel::Additive)) {}
		virtual ~AddOp() = default;
	};

	struct SubtractOp : public BinaryExpr
	{
	public:
		SubtractOp(Expr* left) : BinaryExpr(left, BinaryExprOp::Subtract, static_cast<unsigned int>(PrecidenceLevel::Additive)) {}
		virtual ~SubtractOp() = default;
	};

	struct MultiplyOp : public BinaryExpr
	{
	public:
		MultiplyOp(Expr* left) : BinaryExpr(left, BinaryExprOp::Multiply, static_cast<unsigned int>(PrecidenceLevel::Multiplicative)) {}
		virtual ~MultiplyOp() = default;
	};

	struct DivideOp : public BinaryExpr
	{
	public:
		DivideOp(Expr* left) : BinaryExpr(left, BinaryExprOp::Divide, static_cast<unsigned int>(PrecidenceLevel::Multiplicative)) {}
		virtual ~DivideOp() = default;
	};

	struct ModuloOp : public BinaryExpr
	{
	public:
		ModuloOp(Expr* left) : BinaryExpr(left, BinaryExprOp::Modulo, static_cast<unsigned int>(PrecidenceLevel::Multiplicative)) {}
		virtual ~ModuloOp() = default;
	};

	// Expand ...
	
	template<typename T>
	concept IsBinaryOpDerived = std::derived_from<T, BinaryExpr> && !std::same_as<T, BinaryExpr>;

	enum class UnaryExprOp
	{
		Negate,
		Plus,
		LogicalNot,
		Dereferance,
	};

	struct UnaryExpr : public Operator
	{
	public:
		UnaryExpr(UnaryExprOp op) : Operator(ASTExprType::Unary, static_cast<unsigned int>(PrecidenceLevel::Unary)), operand(value), op_type(op) {}
		const UnaryExprOp op_type;
		virtual ~UnaryExpr() = default;
	private:
		Expr*& operand;
	};

	struct UnaryPlusOp : public UnaryExpr
	{
	public:
		UnaryPlusOp() : UnaryExpr(UnaryExprOp::Plus) {}
		virtual ~UnaryPlusOp() = default;
	};

	struct UnaryNegateOp : public UnaryExpr
	{
	public:
		UnaryNegateOp() : UnaryExpr(UnaryExprOp::Negate) {}
		virtual ~UnaryNegateOp() = default;
	};

	struct UnaryDereferanceOp : public UnaryExpr
	{
	public:
		UnaryDereferanceOp() : UnaryExpr(UnaryExprOp::Dereferance) {}
		virtual ~UnaryDereferanceOp() = default;
	};

	struct UnaryLogicalNotOp : public UnaryExpr
	{
	public:
		UnaryLogicalNotOp() : UnaryExpr(UnaryExprOp::LogicalNot) {}
		virtual ~UnaryLogicalNotOp() = default;
	};

	template<typename T>
	concept IsUnaryOpDerived = std::derived_from<T, UnaryExpr> && !std::same_as<T, UnaryExpr>;

	template<typename T>
	concept IsOperatorDerived = std::derived_from<T, Operator> && !std::same_as<T, Operator>;

	struct LiteralExpr : public Expr
	{
	protected:
		LiteralExpr(ASTExprType type) : Expr(type) {}

	public:
		virtual ~LiteralExpr() = default;
	};

	template<typename T>
	struct NumberLiteralExpr : public LiteralExpr
	{
	public:
		NumberLiteralExpr(T value, NumAtribute* numAtribute = nullptr, NumLiteralTypeAtribute* numLiteralTypeAtribute = nullptr)
			: LiteralExpr(ASTExprType::NumberLiteral), value(value), numAtribute(numAtribute), numLiteralTypeAtribute(numLiteralTypeAtribute) {
		}
		virtual ~NumberLiteralExpr() = default;
	private:
		T value;
		class NumAtribute* numAtribute;
		class NumLiteralTypeAtribute* numLiteralTypeAtribute;
	};

	struct StringLiteralExpr : public LiteralExpr
	{
	public:
		StringLiteralExpr(std::string value) 
			: LiteralExpr(ASTExprType::StringLiteral), value(std::move(value)) {}
		virtual ~StringLiteralExpr() = default;
	private:
		std::string value;
	};

	struct VariableExpr : public Expr
	{
	public:
		VariableExpr(std::string name, VarDecl* decl) 
			: Expr(ASTExprType::Variable), name(std::move(name)), declaration(decl) {}
		virtual ~VariableExpr() = default;
	private:
		std::string name;
		VarDecl* declaration; 
	};

	struct CallExpr : public Expr
	{
	public:
		CallExpr(class FuncDecl* callee, std::vector<Expr*> arguments)
			: Expr(ASTExprType::Call), callee(callee), arguments(std::move(arguments)) {
		}
		virtual ~CallExpr() = default;
	private:
		FuncDecl* callee;
		std::vector<Expr*> arguments;
	};


	// Further derived classes of Statements

	struct IfStmt : public Stmt
	{
	public:
		IfStmt(Expr* condition, Stmt* thenBranch, Stmt* elseBranch = nullptr) 
			: Stmt(ASTStmtType::If), condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {
		}
		virtual ~IfStmt() = default;
	private:
		Expr* condition;
		Stmt* thenBranch;
		Stmt* elseBranch;
	};

	struct WhileStmt : public Stmt
	{
	public:
		WhileStmt(Expr* condition, Stmt* body) 
			: Stmt(ASTStmtType::While), condition(condition), body(body) {
		}
		virtual ~WhileStmt() = default;
	private:
		Expr* condition;
		Stmt* body;
	};

	struct ForStmt : public Stmt
	{
	public:
		ForStmt(Stmt* initializer, Expr* condition, Expr* increment, Stmt* body) 
			: Stmt(ASTStmtType::For), initializer(initializer), condition(condition), increment(increment), body(body) {
		}
		virtual ~ForStmt() = default;
	private:
		Stmt* initializer;
		Expr* condition;
		Expr* increment;
		Stmt* body;
	};

	struct ReturnStmt : public Stmt
	{
	public:
		ReturnStmt(Expr* value = nullptr) : Stmt(ASTStmtType::Return), value(value) {}
		virtual ~ReturnStmt() = default;
	private:
		Expr* value;
	};


	enum class DeclStmtType
	{
		EmptyScope,
		Var,
		Class,
	};

	struct DeclStmt : public Decl, public Stmt
	{
	public:
		const DeclStmtType type;
		virtual ~DeclStmt() = default;
	protected:
	DeclStmt(DeclStmtType type, Stmt* sourceStmt) : Decl(ASTDeclType::Stmt), Stmt(ASTStmtType::Decl), type(type), fatherStmt(sourceStmt) {}
	private:
		Stmt* fatherStmt;
	};

	
	struct EmptyScopeDeclStmt : public DeclStmt
	{
	public:
		EmptyScopeDeclStmt(Stmt* sourceStmt, std::vector<Stmt*> decl) : DeclStmt(DeclStmtType::EmptyScope, sourceStmt), statments(std::move(decl)) {}
		virtual ~EmptyScopeDeclStmt() = default;
	private:
		std::vector<Stmt*> statments;
	};

	struct VarDeclStmt : public DeclStmt
	{
	public:
		VarDeclStmt(VarDecl* decl, Stmt* sourceStmt) : DeclStmt(DeclStmtType::Var, sourceStmt), declaration(decl) {}
		virtual ~VarDeclStmt() = default;
	private:
		VarDecl* declaration;
	};
	
	struct ClassDeclStmt : public DeclStmt
	{
	public:
		ClassDeclStmt(ClassDecl* decl, Stmt* sourceStmt) : DeclStmt(DeclStmtType::Class, sourceStmt), declaration(decl) {}
		virtual ~ClassDeclStmt() = default;
	private:
		ClassDecl* declaration;
	};


	// Further derived classes of Declarations

	struct VarDecl : public Decl
	{
	public:
		VarDecl(Type* type, std::string_view name) 
			: Decl(ASTDeclType::Var), vartype(type), name(name) {}

		VarDecl(Type* type, std::string_view name, Atribute* atribute)
			: Decl(ASTDeclType::Var), vartype(type), name(name), atributes{ atribute } {}
		VarDecl(Type* type, std::string_view name, std::vector<Atribute*> atributes)
			: Decl(ASTDeclType::Var), vartype(type), name(name), atributes(std::move(atributes)) {
		}
		std::string_view get_name() const;
		void add_initializer(Expr* initializer);
		virtual ~VarDecl() = default;
	private:
		std::string name;
		Type* vartype;
		std::vector<Atribute*> atributes;
		Expr* initializer;
	};



	struct FuncDecl : public Decl
	{
	public:
		FuncDecl(FunctionType* type, std::string_view name, std::vector<std::string_view> paramNames, std::vector<Stmt*> statments);
		std::string_view get_mingled_name() const;
		std::string_view get_name() const;
		const std::vector<Stmt*>& get_statments() const;
		size_t get_param_count() const;
		virtual ~FuncDecl() = default;
	private:
		std::string mingledName;
		std::string_view name;

		FunctionType* type;
		std::vector<std::string_view> paramNames;
		std::vector<Stmt*> statments;
	private:
		void set_mingled_name(FunctionType* type, std::string_view name);
	};

	struct ParamDecl : public Decl
	{
	public:
		ParamDecl(Type* type, std::string_view name) 
			: Decl(ASTDeclType::Param), paramtype(type), name(name) {}
		virtual ~ParamDecl() = default;
	private:
		std::string name;
		Type* paramtype;
	};

	struct ClassDecl : public Decl
	{
	public:
		ClassDecl(std::string name)
			: Decl(ASTDeclType::Class), name(std::move(name)) {}

		void add_private_member(Decl* decl);
		void add_public_member(Decl* decl);

		virtual ~ClassDecl() = default;
	private:
		std::string name;
		std::vector<Decl*> privateMembers;
		std::vector<Decl*> publicMembers;
	};

	struct ModuleDecl : public Decl
	{
	public:
		ModuleDecl(std::string name) : Decl(ASTDeclType::Module), name(std::move(name)) {}
		void add_func_decl(FuncDecl* decl);
		void add_class_decl(ClassDecl* decl);
		void add_var_decl(VarDecl* decl);
		void add_import(ImportDecl* importDecl);
		std::string_view get_name() const;
		const std::vector<FuncDecl*>& get_func_decls();
		const std::vector<ClassDecl*>& get_class_decls();
		const std::vector<VarDecl*>& get_var_decls() const;

		virtual ~ModuleDecl() = default;
	private:
		std::string name;
		std::vector<FuncDecl*> funcDecls;
		std::vector<ClassDecl*> classDecls;
		std::vector<VarDecl*> varDecls;
	};

	struct ImportDecl : public Decl
	{
	public:
		ImportDecl(std::string moduleName, ModuleDecl* module) 
			: Decl(ASTDeclType::Import), moduleName(std::move(moduleName)), module(module) {}
		virtual ~ImportDecl() = default;
	private:
		std::string moduleName;
		ModuleDecl* module;
	};

	template<typename T>
	concept IsDeclDerived = std::derived_from<T, Decl> && !std::same_as<T, Decl>;

	// Further derived classes of Types

	struct PrimitiveType : public Type
	{
	protected:
		PrimitiveType(ASTType type) : Type(type) {}

	public:
		virtual ~PrimitiveType() = default;
	};

	struct Int : public PrimitiveType 
	{
	public:
		Int() : PrimitiveType(ASTType::Int) {}
		int val = 0;
	};

	struct Float : public PrimitiveType
	{
	public:
		Float() : PrimitiveType(ASTType::Float) {}
		float val = 0.0f;
	};

	struct Double : public PrimitiveType
	{
	public:
		Double() : PrimitiveType(ASTType::Double) {}
		double val = 0.0;
	};

	struct Bool : public PrimitiveType
	{	
	public:
		Bool() : PrimitiveType(ASTType::Bool) {}
		bool val = false;
	};

	struct Char : public PrimitiveType
	{
	public:
		Char() : PrimitiveType(ASTType::Char) {}
		char val = '\0';
	};

	struct PointerType : public Type
	{
	public:
		PointerType(Type* type) : Type(ASTType::Pointer), pointeeType(type) {}
		Type* get_pointee_type() const { return pointeeType; }
		virtual ~PointerType() = default;
	private:
		Type* pointeeType;
	};

	struct ArrayType : public Type
	{
	public:
		ArrayType(Type* elementType) : Type(ASTType::Array), elementType(elementType) {}
		virtual ~ArrayType() = default;
	private:
		Type* elementType;
	};

	struct FunctionType : public Type
	{
	public:
		FunctionType(std::vector<Type*> parameterTypes, Type* returnType)
			: Type(ASTType::Function), parameterTypes(std::move(parameterTypes)), returnType(returnType) {
		}

		const std::vector<Type*>& get_param_types() const;
		
		Type* const get_return_type() const;

		virtual ~FunctionType() = default;
	private:
		std::vector<Type*> parameterTypes;
		Type* returnType;
	};

	struct ClassType : public Type
	{
	public:
		ClassType(std::string className) : Type(ASTType::Class), className(std::move(className)) {}
		std::string_view get_class_name() const { return className; }
		virtual ~ClassType() = default;
	private:
		std::string className;
	};

	// Further derived classes of Atributes

	struct ConstAtribute : public Atribute
	{};

	struct UnsignedAtribute : public Atribute
	{
	};

	struct SignedAtribute : public Atribute
	{
	};

	struct NumAtribute : public Atribute
	{
	public:
		~NumAtribute() override = default;
	};

	struct UnsignedNumAtribute : public NumAtribute
	{
	public:
	};

	struct LongNumAtribute : public NumAtribute
	{
	public:
	};

	struct ShortNumAtribute : public NumAtribute
	{
	public:
	};

	struct NumLiteralTypeAtribute : public Atribute
	{
	public:
		~NumLiteralTypeAtribute() override = default;
	};

	struct FloatAtribute : public NumLiteralTypeAtribute
	{};

	struct DoubleAtribute : public NumLiteralTypeAtribute
	{};

	using Modules = std::vector<ModuleDecl*>;

}