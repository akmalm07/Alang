#pragma once


#include <optional>
#include <string_view>

namespace alang
{
	constexpr std::string_view c_keywords[] = {
		"if",
		"else",
		"while",
		"for",
		"return",
		"break",
		"continue",
		"private",
		"public",
		"int",
		"float",
		"void",
		"bool",
		"double",
		"char",
		"class",
		"module",
	};

	constexpr size_t c_keyword_count = sizeof(c_keywords) / sizeof(c_keywords[0]);

	enum class Keyword
	{
		If,
		Else,
		While,
		For,
		Return,
		Break,
		Continue,
		Private,
		Public,
		Int,
		Float,
		Void,
		Bool,
		Double,
		Char,
		Class,
		Module,
	};

	std::optional<Keyword> string_to_keyword(std::string_view str);

	constexpr std::string_view keyword_to_string(Keyword keyword);
}