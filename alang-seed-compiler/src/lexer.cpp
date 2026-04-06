#include "headers.h"
#include "lexer.h"
#include "keywords.h"

namespace alang
{

	std::vector<Token> tokenize(const std::string& source)
	{
		std::vector<Token> tokens;
		unsigned int line = 1;
		for (size_t i = 0; i < source.size(); i++)
		{
			char c = source[i];
			if (std::isspace(c))
				continue;
			else if (std::isalpha(c) || c == '_')
			{
				size_t start = i;

				while (i < source.size() && (std::isalnum(source[i]) || source[i] == '_'))
					i++;

				if (auto keyword = string_to_keyword(source.substr(start, i - start)))
				{
					tokens.emplace_back(keyword.value(), c_keywords[static_cast<size_t>(keyword.value())], line, start);
					continue;
				}

				std::string value = source.substr(start, i - start);
				tokens.emplace_back(TokenType::Identifier, value, line, start);
				i--;
			}
			else if (std::isdigit(c)) // TODO: Must add rule where identifiers cannot start with a digit, otherwise this will cause problems with parsing and error handling, since the lexer will just tokenize something like "123abc" as a number token with value "123" followed by an identifier token with value "abc", which is not ideal. We should probably make the lexer recognize this as an error instead of trying to tokenize it, but for now this will do.
			{
				size_t start = i;
				while (i < source.size() && std::isdigit(source[i]))
					i++;

				std::string value = source.substr(start, i - start);
				tokens.emplace_back(TokenType::Number, value, line, start);
				i--;
			}
			else if (c == '/')
			{
				if (i + 1 < source.size() && source[i + 1] == '/')
				{
					while (i < source.size() && source[i] != '\n')
						i++;
					line++;
				}
				else if (i + 1 < source.size() && source[i + 1] == '*')
				{
					i += 2;
					while (i + 1 < source.size() && !(source[i] == '*' && source[i + 1] == '/'))
					{
						if (source[i] == '\n')
							line++;
						i++;
					}
					i += 2;
				}
				else
					tokens.emplace_back(TokenType::Slash, "/", line, i);
			}
			else
			{
				switch (c)
				{
					case '+':
						tokens.emplace_back(TokenType::Plus, "+", line, i);
						break;
					case '-':
						tokens.emplace_back(TokenType::Minus, "-", line, i);
						break;
					case '*':
						tokens.emplace_back(TokenType::Star, "*", line, i);
						break;
					case '(':
						tokens.emplace_back(TokenType::LeftParen, "(", line, i);
						break;
					case ')':
						tokens.emplace_back(TokenType::RightParen, ")", line, i);
						break;
					case '=':
						tokens.emplace_back(TokenType::Equal, "=", line, i);
						break;
					case ';':
						tokens.emplace_back(TokenType::Semicolon, ";", line, i);
						break;
					case '"':
						tokens.emplace_back(TokenType::DoubleQuote, "\"", line, i);
						break;
					case '\'':
						tokens.emplace_back(TokenType::Quote, "\'", line, i);
						break;
					case '\0':
						tokens.emplace_back(TokenType::EndOfFile, "", line, i);
						break;
					case '\n':
						line++;
						break;
					case '\r':
						break;
				}
			}
		}

		return tokens;
	}

}