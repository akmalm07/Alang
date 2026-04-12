#include "expr_builder.h"


namespace alang {
	ExprBuilder::ExprBuilder(Iterator& iterator, const Tokens& tokens, std::function<void(std::string_view)> errCallback)
		: _errCallback(std::move(errCallback)), _tokens(tokens), _iterator(iterator)
	{
	}

	Expr* const ExprBuilder::get_current_expr()
	{
		return _currentExpr;
	}


	void ExprBuilder::add_expr(Expr* expr)
	{  
		if (is_operator(_currentExpr))
		{
			if (!static_cast<Operator*>(_currentExpr)->is_set())
			{
				_errCallback("Operator is already set");
				return;
			}
			
			if (expr->kind == ASTNodeKind::BinaryExpr)
				eval_order_of_operations(static_cast<BinaryExpr*>(expr));
			else if (expr->kind == ASTNodeKind::UnaryExpr)
				eval_order_of_operations(static_cast<UnaryExpr*>(expr));
			else
			{
				if (_nextPlacement)
				{
					*_nextPlacement = expr;
					_nextPlacement = nullptr;
				}
				else
					_errCallback("Invalid expression placement");
			}
		}
		else // Not operator branch
			_currentExpr = expr;
	}

	bool ExprBuilder::is_operator(Expr* expr) const
	{
		if (!expr) return false;
		return expr->kind == ASTNodeKind::BinaryExpr || expr->kind == ASTNodeKind::UnaryExpr;
	}

	bool ExprBuilder::is_item_expr(Expr* expr) const
	{
		if (!expr) return false;
		return expr->kind != ASTNodeKind::BinaryExpr || expr->kind != ASTNodeKind::UnaryExpr;
	}

	bool ExprBuilder::is_unary_operator(Expr* expr) const
	{
		if (!expr)
			return is_unary_op_token(_tokens[_iterator]);
		else if (is_binary_op_token(_tokens[_iterator - 1]) && is_unary_op_token(_tokens[_iterator])) return true;
		return false;
	}

	bool ExprBuilder::is_binary_operator(Expr* expr) const
	{
		if (!expr) return false;
		else if (is_item_token(_tokens[_iterator - 1]) && is_binary_op_token(_tokens[_iterator])) return true;
	}

	bool ExprBuilder::is_item_token(const Token& token) const
	{
		return token.type == TokenType::Identifier || token.type == TokenType::Number || 
			token.type == TokenType::DoubleQuote || token.type == TokenType::RightParen;
	}

	bool ExprBuilder::is_unary_op_token(const Token& token) const
	{
		return token.type == TokenType::Plus || token.type == TokenType::Minus || 
			token.type == TokenType::Exclamation || token.type == TokenType::Tilde;
	}

	bool alang::ExprBuilder::is_binary_op_token(const Token& token) const
	{
		return token.type == TokenType::Plus || token.type == TokenType::Minus || 
		       token.type == TokenType::Asterisk || token.type == TokenType::Slash || 
		       token.type == TokenType::Percent || token.type == TokenType::EqualsEquals || 
		       token.type == TokenType::NotEquals || token.type == TokenType::LessThan || 
		       token.type == TokenType::GreaterThan || token.type == TokenType::LessThanEquals || 
		       token.type == TokenType::GreaterThanEquals || token.type == TokenType::AmpersandAmpersand || 
		       token.type == TokenType::PipePipe || token.type == TokenType::Ampersand || 
		       token.type == TokenType::Pipe || token.type == TokenType::Caret;
	}
}