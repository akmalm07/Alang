#pragma once

#include <string>

namespace alang
{

	enum class TokenType
	{
		// Keywords
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

		// Identifiers
		Identifier,

		// Literals
		Number,
		Plus,
		Minus,
		Star,
		Slash,
		LeftParen,
		RightParen,
		Equal,
		Semicolon,
		DoubleQuote,
		Quote, 
		Dot, // TODO: mkae sure the elexer handles this!
		Comma, // TODO: mkae sure the elexer handles this!
		Colon, // TODO: make sure the lexer handles this!
		ColonColon, // TODO: make sure the lexer handles this!
		LeftBrace, // TODO: make sure the lexer handles this!
		RightBrace, // TODO: make sure the lexer handles this!
		Exclamation, // TODO: make sure the lexer handles this!
		Tilde, // TODO: make sure the lexer handles this! (~)

		Asterisk,
		Percent,
		NotEquals,
		GreaterThan,
		GreaterThanEquals,
		PipePipe,
		Pipe,
		Slash,
		EqualsEquals,
		LessThan,
		LessThanEquals,
		AmpersandAmpersand,
		Ampersand,
		Caret,

		// End of file
		EndOfFile

		// Add more token types as needed

	};

	struct Token
	{
	public:
		TokenType type;
		std::string value;
		unsigned int line;
		unsigned int column;
		Token(TokenType type, const std::string& value, unsigned int line, unsigned int column)
			: type(type), value(value), line(line), column(column) {}
	};

	using Tokens = std::vector<Token>;
}

