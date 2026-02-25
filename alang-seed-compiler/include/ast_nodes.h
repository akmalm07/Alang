#pragma once

#include <memory>


namespace alang {
	

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
		virtual ~Expr() = default;
	};	

	struct Stmt : public ASTNode
	{
	public:
		virtual ~Stmt() = default;
	};

	struct Decl : public ASTNode
	{
	public:
		virtual ~Decl() = default;
	};

	struct Type : public ASTNode
	{
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


	// Further derived classes of Expressions

	struct BinaryExpr : public Expr
	{
	public:
		virtual ~BinaryExpr() = default;
	protected:
		std::unique_ptr<Expr> left;
		std::unique_ptr<Expr> right;
	};

	struct UnaryExpr : public Expr
	{
	public:
		virtual ~UnaryExpr() = default;
	private:
		std::unique_ptr<Expr> operand;
	};

	struct LiteralExpr : public Expr
	{
	public:
		virtual ~LiteralExpr() = default;
	};

	struct VariableExpr : public Expr
	{
	public:
		virtual ~VariableExpr() = default;
	};

}