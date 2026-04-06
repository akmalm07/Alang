#pragma once

#include "tokens.h"
#include "memory_arena.h"
#include "error_handler.h"
#include "ast_nodes.h"
#include "auto_check_iterator.h"
#include "decl_tracker.h"

#include <type_traits>

namespace alang
{

	class Parser
	{
	public:

		Parser();

		Errors parse(const Tokens& tokens);

	private:
		Arena _memArena;
		Modules _modules;
		Errors _errors;

		Tokens _tokens;

		DeclTracker _declTracker;

	private:
		enum class WhereParsingExprFrom
		{
			VariableInitializer,
			CallArgument,
			ReturnStatement,
			IfCondition,
			WhileCondition,
			ForInitializer,
			ForCondition,
			ForIncrement,
			Container,
		};
	private:
		bool add_err(ErrorType type, std::string_view errStr, size_t tokenItr); // make this return an error!!!
		bool has_next(size_t i);

		template<typename T>
			requires std::is_arithmetic_v<T>
		T numify(std::string_view value) const noexcept;

		bool is_native_type(TokenType type) const;

		Atribute* parse_atribute(Iterator& i);

		Expr* parse_expression(WhereParsingExprFrom context, Decl* whereFrom, Iterator& i);

		//Expr* find_root_operator(WhereParsingExprFrom context, Iterator& i);

		// Possibility top put this in compile time...
		bool eval_stop_condition_for_expr(WhereParsingExprFrom context, Iterator& i) const;

		//bool is_unary_operator(Expr* expr) const;

		std::vector<Expr*> parse_call_arguments(Iterator& i);

		template<BinaryExprOp T> 
		Expr* parse_arithmitic(Expr* const currentExpr, const Iterator& i);

		LiteralExpr* parse_num_literal(std::string_view value, Iterator& i);

		bool parse_one_round(ModuleDecl* module, size_t& startIndex); // Should start index be passed by reference? I think so, since we want to update it as we parse through the tokens

		void parse_class_decl(ModuleDecl* module, ClassDecl* classDecl, Iterator& index);

		Decl* parse_variable_or_function_decl(Iterator& i);

		VarDecl* parse_variable_decl(Iterator& i);

		ModuleDecl* parse_module(Iterator& index);

		FuncDecl* parse_function_decl(Iterator& i);

		VarDecl* parse_native_variable_decl(Iterator& i);
		VarDecl* parse_custom_variable_decl(Iterator& i);

		template<IsUnaryOpDerived UnaryOp, IsBinaryOpDerived BinaryOp>
		Expr* make_operator(Expr* current, bool isItemExpr);

		template<IsBinaryOpDerived BinaryOp>
		Expr* Parser::make_operator(Expr* current, bool isItemExpr);
	};

	template<IsUnaryOpDerived UnaryOp, IsBinaryOpDerived BinaryOp>
	Expr* Parser::make_operator(Expr* current, bool isItemExpr)
	{
		if (!isItemExpr)
			return _memArena.alloc<UnaryOp>();
		return arena.alloc<BinaryOp>(current);
	}

	template<IsBinaryOpDerived BinaryOp>
	Expr* Parser::make_operator(Expr* current, bool isItemExpr)
	{
		if (!bool isItemExpr)
			return _memArena.alloc<BinaryOp>(current);
		add_err(ErrorType::SyntaxError, c_expected_expr_before_arithmatic_operator(BinaryExprOp::Add), i);
		return nullptr;
	}

	template<typename T>
		requires std::is_arithmetic_v<T> 
	inline T Parser::numify(std::string_view value) const noexcept
	{
		if constexpr (std::is_integral_v<T>)
		{
			T result = 0;
			for (char c : value)
			{
				if (c == '.')
					break;
				if (!std::isdigit(c))
					return 0;
				result = result * 10 + (c - '0');
			}
			return result;
		}
		else if constexpr(std::is_floating_point_v<T>)
		{
			T result = 0;
			bool floatPointArith = false;
			T divisor = 10;
			for (char c : value)
			{
				if (c == '.')
				{
					floatPointArith = true;
					continue;
				}
				if (!std::isdigit(c))
					return 0;
				if (!floatPointArith)
					result = result * 10 + (c - '0');
				else
				{
					result = result + (c - '0') / divisor;
					divisor *= 10;
				}
			}
			return result;
		}
	}

	template<BinaryExprOp T>
	Expr* Parser::parse_arithmitic(Expr* const currentExpr, const Iterator& i)
	{
		if constexpr (T == BinaryExprOp::Add)
		{
			if (is_operator(currentExpr))
			{
				auto op = static_cast<Operator*>(currentExpr);
				if (!op->is_set())
				{
					add_err(ErrorType::SyntaxError, c_expected_expr_before_arithmatic_operator(BinaryExprOp::Add), i);
				}
				if (op->precedence >= static_cast<unsigned int>(PrecidenceLevel::Additive))
				{
					return _memArena.alloc<AddOp>(currentExpr)
				}
			}
			if (is_item_expr(currentExpr))
				return _memArena.alloc<AddOp>(currentExpr);
			else if (is_unary_operator(currentExpr, i))
				return _memArena.alloc<UnaryPlusOp>(currentExpr);
			else
			{
				add_err(ErrorType::SyntaxError, c_expected_expr_before_arithmatic_operator(BinaryExprOp::Add), i);
				return nullptr;
			}
		}
		else if constexpr (T == BinaryExprOp::Subtract)
		{
			if (is_item_expr(currentExpr))
				return _memArena.alloc<SubtractOp>(currentExpr);
			else if (is_unary_operator(currentExpr, i))
				return _memArena.alloc<UnaryNegateOp>(currentExpr);
			else
			{
				add_err(ErrorType::SyntaxError, c_expected_expr_before_arithmatic_operator(BinaryExprOp::Subtract), i);
				return nullptr;
			}
		}
		else if constexpr (T == BinaryExprOp::Multiply)
		{
			if (is_item_expr(currentExpr))
				return _memArena.alloc<MultiplyOp>(currentExpr);
			else
			{
				add_err(ErrorType::SyntaxError, c_expected_expr_before_arithmatic_operator(BinaryExprOp::Multiply), i);
				return nullptr;
			}
		}
		else if constexpr (T == BinaryExprOp::Divide)
		{
			if (is_item_expr(currentExpr))
				return _memArena.alloc<DivideOp>(currentExpr);
			else
			{
				add_err(ErrorType::SyntaxError, c_expected_expr_before_arithmatic_operator(BinaryExprOp::Divide), i);
				return nullptr;
			}
		}
		else if constexpr (T == BinaryExprOp::Modulo)
		{
			if (is_item_expr(currentExpr))
				return _memArena.alloc<ModuloOp>(currentExpr);
			else
			{
				add_err(ErrorType::SyntaxError, c_expected_expr_before_arithmatic_operator(BinaryExprOp::Modulo), i);
				return nullptr;
			}
		}
	}


}