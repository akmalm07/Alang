#pragma once

#include <string>

#include "keywords.h"
#include "compile_time_helpers.h"

namespace alang
{

	constexpr std::string_view c_missing_module = "Expected 'module' declaration at the beginning of the file ";
	constexpr std::string_view c_expected_type_and_id_before_decl = "Expected type and identifier before declaration";
	constexpr std::string_view c_vars_cant_be_void = "Variables cannot be of type 'void'";
	constexpr std::string_view c_variables_cannot_be_keywords = "Variables cannot have a name that is a keyword";

	constexpr std::string_view c_expected_expr_before_arithmatic_operator(BinaryExprOp op)
	{
		ConstexprString<64> result;
		result.append("Expected expression before '");
		result.append(stringify(op));
		result.append("' operator");
		return result.view();
	}

	std::string_view no_closing_perenthesis_after(std::string_view word)
	{
		std::string result;
		result.reserve(34 + word.size());
		result += "Expected ')' after '";
		result += word;
		result += "'";
		return result;
	}
	
	std::string type_before_identifier(std::string_view name)
	{
		std::string result;
		result.reserve(34 + name.size());
		result += "Expected type before identifier '";
		result += name;
		result += "'";
		return result;
	}

	constexpr std::string_view c_unexpected_token(std::string_view token)
	{
		ConstexprString<64> result;
		result.append("Unexpected token '");
		result.append(token);
		result.append("'");
		
		return result.view();
	}

	std::string expected_token_after(std::string_view word, std::string_view expected)
	{
		std::string result;
		result.reserve(12 + expected.size() + word.size());

		result += "Expected '";
		result += expected;
		result += "' after '";
		result += word;
		result += "'";

		return result;
	}

	constexpr std::string_view c_expected_keyword_idenifier(Keyword word)
	{
		ConstexprString<64> result;
		result.append("Expected identifier after '");
		result.append(keyword_to_string(word));
		result.append("' keyword");
		
		return result.view();
	}

	constexpr std::string_view c_expected_left_brace(Keyword word)
	{
		ConstexprString<64> result;
		result.append("Expected '{' after '");
		result.append(keyword_to_string(word));
		result.append("' declaration");
		
		return result.view();
	}

	constexpr std::string_view c_expected_right_brace(Keyword word)
	{
		ConstexprString<64> result;
		result.append("Expected '}' at the end of '");
		result.append(keyword_to_string(word));
		result.append("' declaration");
		
		return result.view();
	}

	constexpr std::string_view c_expected_expr_after_arithmatic_operator(std::string_view operatorToken)
	{
		ConstexprString<64> result;
		result.append("Expected expression after '");
		result.append(operatorToken);
		result.append("' operator");
		
		return result.view();
	}




	enum class ErrorType
	{
		SyntaxError,
		SemanticError,
		TypeError,
		Warning
	};

	struct Error
	{
		ErrorType type;
		std::string message;
		unsigned int line;
		unsigned int column;
		Error(ErrorType type, const std::string& message, unsigned int line, unsigned int column)
			: type(type), message(message), line(line), column(column) {}
	};

	using Errors = std::vector<Error>;
}