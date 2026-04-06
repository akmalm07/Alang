#pragma once

#include "ast_nodes.h"
#include "auto_check_iterator.h"
#include "error_handler.h"
#include "tokens.h"

#include <functional>

namespace alang {

	// This class is used to build expressions in a more convenient way. 
	
	// It keeps track of the current expression being built and allows adding new
	// expressions to it. It also provides a method to get the final expression once 
	// all sub-expressions have been added.

	class ExprBuilder
	{
	public:
		ExprBuilder(Iterator& iterator, const Tokens& tokens, std::function<void(std::string_view)> errCallback);

		Expr* const get_current_expr();

		void add_expr(Expr* expr);
		
		bool is_item_expr(Expr* expr) const;
		bool is_operator(Expr* expr) const;
	private:
		Expr* _currentExpr = nullptr;
		Iterator& _iterator;
		const Tokens& _tokens;
		std::function<void(std::string_view)> _errCallback;
		Expr** _nextPlacement = nullptr; // This is used to keep track of where to place the next expression in the tree. It starts as a pointer to the root expression and moves down the tree as we add expressions.
	private:

		
		bool is_unary_operator(Expr* expr) const;
		bool is_binary_operator(Expr* expr) const;

		bool is_item_token(const Token& token) const;

		bool is_unary_op_token(const Token& token) const;
		
		bool is_binary_op_token(const Token& token) const;

		template<IsOperatorDerived T>
		void eval_order_of_operations(T* newOperator);
	};

	template<IsOperatorDerived T>
	void ExprBuilder::eval_order_of_operations(T* newOperator)
	{
		Operator* currentOp = static_cast<Operator*>(_currentExpr);
		if (currentOp->precedence >= newOperator->precedence) // easy case
		{
			if constexpr (std::derived_from<T, BinaryExpr>)
			{
				auto binaryExpr = static_cast<BinaryExpr*>(newOperator);
				currentOp->set_value(binaryExpr->get_left());
				binaryExpr->set_left(currentOp);
				_nextPlacement = &binaryExpr->get_right();
				_currentExpr = binaryExpr;
			}
			else if constexpr (std::derived_from<T, UnaryExpr>)
			{
				_nextPlacement = &newOperator->get_value();
				currentOp->set_value(newOperator);
				_currentExpr = newOperator;
			}
		}
		else // harder case - new operator has higher precedence
		{
			if constexpr (std::derived_from<T, BinaryExpr>)
			{
				auto binaryExpr = static_cast<BinaryExpr*>(newOperator);
				binaryExpr->set_left(currentOp->get_value());
				_nextPlacement = &binaryExpr->get_right();
				currentOp->set_value(binaryExpr);
			}
			else if constexpr (std::derived_from<T, UnaryExpr>)
			{
				_nextPlacement = &newOperator->get_value();
				currentOp->set_value(newOperator);
			}
		}
	}
}