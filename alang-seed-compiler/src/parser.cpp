#include "headers.h"
#include "parser.h"
#include "compile_time_helpers.h"
#include "expr_builder.h"

namespace alang {

	struct ParsingModule // Use this!!!
	{
		std::string name;
		std::vector<Token> tokens; // Declarations that need to be resolved after parsing
	};

	Parser::Parser() 
		: _declTracker(_memArena)
	{}


	bool Parser::add_err(ErrorType type, std::string_view errStr, size_t tokenItr)
	{
		_errors.emplace_back(type, errStr, _tokens[tokenItr].line, _tokens[tokenItr].column);
	}


	bool Parser::has_next(size_t i)
	{
		return i + 1 < _tokens.size();
	}

	bool Parser::is_native_type(TokenType type) const
	{
		return type == TokenType::Int || type == TokenType::Float || type == TokenType::Void ||
			type == TokenType::Bool || type == TokenType::Double || type == TokenType::Char;
	}

	Atribute* Parser::parse_atribute(Iterator& i)
	{
		if (_tokens[i].value == "const")
			return _memArena.alloc<ConstAtribute>();
		else if (_tokens[i].value == "unsigned")
			return _memArena.alloc<UnsignedAtribute>();
		else if (_tokens[i].value == "long")
			return _memArena.alloc<UnsignedAtribute>();
		else if (_tokens[i].value == "short")
			return _memArena.alloc<UnsignedAtribute>();
		/*else if (_tokens[i].value == "signed")
			return _memArena.alloc<SignedAtribute>();*/
		//else if (_tokens[i].value == "static")
		//	return _memarena.alloc<staticatribute>();
		//else if (_tokens[i].value == "extern")
		//	return _memarena.alloc<externatribute>();	
		else
		{
			add_err(ErrorType::SyntaxError, c_unexpected_token(_tokens[i].value), i);
			return nullptr;
		}

	}



	Expr* Parser::parse_expression(WhereParsingExprFrom context, Decl* whereFrom, Iterator& i)
	{
		//Expr* rootOperator = find_root_operator(context, i);
		Expr* nextExpr = nullptr;
		ExprBuilder exprBuilder(i, _tokens, [this, &i](std::string_view errStr) { add_err(ErrorType::SyntaxError, errStr, i); });
		while (eval_stop_condition_for_expr(context, i))
		{
			if (exprBuilder.is_operator(nextExpr))
			{
				exprBuilder.add_expr(nextExpr);
				nextExpr = nullptr;
			}

			bool isItemExpr = exprBuilder.is_item_expr(nextExpr);

			switch (_tokens[i].type)
			{
			case TokenType::Number:
				if (isItemExpr)
				{
					add_err(ErrorType::SyntaxError, c_unexpected_token(_tokens[i].value), i);
					return nullptr;
				}
				nextExpr = parse_num_literal(_tokens[i].value, i);
				if (!nextExpr) return nullptr;
				break;
			case TokenType::Identifier:
			{
				if (isItemExpr)
				{
					add_err(ErrorType::SyntaxError, c_unexpected_token(_tokens[i].value), i);
					return nullptr;
				}

				std::vector<std::string_view> members;
				while (_tokens[i + 1].type == TokenType::Dot && _tokens[i].type == TokenType::Identifier)
				{
					members.push_back(_tokens[i].value);
					i++;
				}

				if (_tokens[i + 1].type == TokenType::LeftParen)
				{
					FuncDecl* callee;
					if (members.empty())
					{
						// make parsing and resolving 2 differnt stages in the pipeline,
						// and add pointers to variubles and function that need to be resolved
						callee = find_declaration_func(whereFrom, _tokens[i].value);
					}
					else
					{
						callee = find_declaration_class_func(whereFrom, members);
					}
					if (!callee) return nullptr;
					// Function declaration syntax: retType identifier ( param1:type param2:type param3:type ) { body }
					// Function call syntax: identifier ( arg1, arg2, arg3)
					i += 2;
					nextExpr = _memArena.alloc<CallExpr>(callee, parse_call_arguments(i));
					if (!nextExpr) return nullptr;
				}

				VarDecl* decl = nullptr;
				if (members.empty())
					decl = find_declaration_class(_tokens[i].value);
				else
					decl = find_declaration_var(members)
					if (!decl) return nullptr;
				nextExpr = _memArena.alloc<VariableExpr>(_tokens[i].value, decl);

				break;
			}
			case TokenType::LeftParen:
				if (isItemExpr)
				{
					// make this equal multiplication operator, since in c++ ( and ) can be used for grouping and for function calls, so we need to check if the current expression is an item expression or an operator expression, if it's an item expression then we are in the grouping case and if it's an operator expression then we are in the function call case, since in c++ you can do things like 2(3) which is valid and means 2 * 3
					break;
				}
				i++;
				nextExpr = parse_expression(WhereParsingExprFrom::Container, i);
				if (!nextExpr) return nullptr;
				if (_tokens[i].type != TokenType::RightParen)
				{
					add_err(ErrorType::SyntaxError, no_closing_perenthesis_after(_tokens[i].value), i);
					return nullptr;
				}
				break;
			case TokenType::Plus:
				nextExpr = make_operator<UnaryPlusOp, AddOp>(nextExpr, isItemExpr);
				break;
			case TokenType::Minus:
				nextExpr = make_operator<UnaryNegateOp, SubtractOp>(nextExpr, isItemExpr);
				break;
			case TokenType::Star:
				nextExpr = make_operator<UnaryDereferanceOp, MultiplyOp>(nextExpr, isItemExpr);
				break;
			case TokenType::Ampersand:
				nextExpr = make_operator<UnaryAdressOp, BitwiseAndOp>(nextExpr, isItemExpr);
				break;
			case TokenType::Caret:
				nextExpr = make_operator<BitwiseXorOp>(nextExpr, isItemExpr);
				break;
			case TokenType::AmpersandAmpersand:
				nextExpr = make_operator<LogicalAndOp>(nextExpr, isItemExpr);
				break;
			case TokenType::Pipe:
				nextExpr = make_operator<UnaryBitwiseOrOp, BitwiseOrOp>(nextExpr, isItemExpr);
				break;
			case TokenType::PipePipe:
				nextExpr = make_operator<LogicalOrOp>(nextExpr, isItemExpr);
				break;
			case TokenType::EqualsEquals:
				nextExpr = make_operator<EqualsOp>(nextExpr, isItemExpr);
				break;
			case TokenType::NotEquals:
				nextExpr = make_operator<NotEqualsOp>(nextExpr, isItemExpr);
				break;
			case TokenType::LessThan:
				nextExpr = make_operator<LessThanOp>(nextExpr, isItemExpr);
				break;
			case TokenType::GreaterThan:
				nextExpr = make_operator<GreaterThanOp>(nextExpr, isItemExpr);
				break;
			case TokenType::LessThanEquals:
				nextExpr = make_operator<LessThanEqualsOp>(nextExpr, isItemExpr);
				break;
			case TokenType::GreaterThanEquals:
				nextExpr = make_operator<GreaterThanEqualsOp>(nextExpr, isItemExpr);
				break;
			case TokenType::Percent:
				nextExpr = make_operator<ModuloOp>(nextExpr, isItemExpr);
				break;
			case TokenType::Dot:



			case TokenType::Slash:
			}
		}
	}



	//Expr* Parser::parse_expression(Iterator& i)
	//{
	//	Expr* expr = nullptr;
	//	//std::vector<ContainerExpr*> perenContainers;
	//	while (_tokens[i].type != TokenType::Semicolon || _tokens[i].type != TokenType::Comma)
	//	{
	//		switch (_tokens[i].type)
	//		{
	//		case TokenType::Number:
	//			if (is_item_expr(expr))
	//			{
	//				add_err(ErrorType::SyntaxError, c_unexpected_token(_tokens[i].value), i);
	//				return nullptr;
	//			}
	//			expr = parse_num_literal(_tokens[i].value, i);
	//			if (!expr) return nullptr;
	//		case TokenType::Identifier:
	//			if (is_item_expr(expr))
	//			{
	//				add_err(ErrorType::SyntaxError, c_unexpected_token(_tokens[i].value), i);
	//				return nullptr;
	//			}
	//			if (_tokens[i + 1].type == TokenType::LeftParen)
	//			{
	//				FuncDecl* callee = _declTracker.resolve_function(_tokens[i].value);
	//				if (!callee) return nullptr;
	//				// Function declaration syntax: retType identifier ( param1:type param2:type param3:type ) { body }
	//				// Function call syntax: identifier ( arg1, arg2, arg3)
	//				i += 2;
	//				expr = _memArena.alloc<CallExpr>(callee, parse_call_arguments(i));
	//				if (!expr) return nullptr;
	//			}
	//			
	//			VarDecl* decl = _declTracker.resolve_variable(_tokens[i].value);
	//			if (!decl) return nullptr;
	//			expr = _memArena.alloc<VariableExpr>(_tokens[i].value, decl);

	//			break;
	//		case TokenType::Plus:
	//			if (is_item_expr(expr))
	//			{
	//				expr = parse_arithmitic<BinaryExprOp::Add>(expr, i);
	//				if (!expr) return nullptr;
	//			}
	//			else if (is_unary_operator(expr))
	//			{
	//				UnaryExpr* unaryExpr = _memArena.alloc<UnaryExpr>(UnaryExprOp::Plus);
	//				unaryExpr->set_value(expr);
	//				expr = unaryExpr;
	//			}
	//			else
	//			{
	//				add_err(ErrorType::SyntaxError, c_expected_expr_before_arithmatic_operator(BinaryExprOp::Add), i);
	//				return nullptr;
	//			}
	//		case TokenType::Minus:
	//			if (is_item_expr(expr))
	//			{
	//				expr = parse_arithmitic<BinaryExprOp::Subtract>(expr, i);
	//				if (!expr) return nullptr;
	//			}
	//			else
	//			{
	//				add_err(ErrorType::SyntaxError, c_expected_expr_before_arithmatic_operator(BinaryExprOp::Subtract), i);
	//				return nullptr;
	//			}
	//		}

	//		break;
	//	}
	//	
	//	return expr;
	//}



	//Expr* Parser::find_root_operator(WhereParsingExprFrom context, Iterator& i)
	//{
	//	Expr* rootOperator = nullptr;
	//	while (eval_stop_condition_for_expr(context, i))
	//	{
	//		switch (_tokens[i].type)
	//		{
	//		case TokenType::LeftParen:
	//			i++;
	//			/*
	//				FUTURE:
	//				Create a type for handling weather we are handling with an operator or 
	//				an item expression, since this is important for handling the consumption 
	//				of the tokens correctly, for example if we have a plus operator and 
	//				we encounter a left parenthesis, we know that we are dealing with an 
	//				operator and not an item expression, so we can handle the consumption 
	//				of the tokens correctly, for example if we have a plus operator and we 
	//				encounter a left parenthesis, we know that we are dealing with an operator
	//				and not an item expression, so we can handle the consumption of the tokens
	//				correctly, for example if we have a plus operator and we encounter a left 
	//				parenthesis, we know that we are dealing with an operator and not an item
	//				expression, so we can handle the consumption of the tokens correctly

	//				TOO MUCH CODE BLOUT!!!
	//			*/
	//			// Check if this is an operator to handle the consumption correctly
	//			rootOperator = find_root_operator(WhereParsingExprFrom::Container, i);
	//			if (!rootOperator) return nullptr;
	//		case TokenType::Number:
	//			if (is_item_expr(rootOperator))
	//			{
	//				add_err(ErrorType::SyntaxError, c_unexpected_token(_tokens[i].value), i);
	//				return nullptr;
	//			}
	//			else if (is_operator(rootOperator))
	//			{
	//				if (!static_cast<Operator*>(rootOperator)->is_set())
	//					static_cast<Operator*>(rootOperator)->set_value(parse_num_literal(_tokens[i].value, i));
	//				else
	//				{
	//					add_err(ErrorType::SyntaxError, c_unexpected_token(_tokens[i].value), i);
	//					return nullptr;
	//				}
	//			}
	//			rootOperator = parse_num_literal(_tokens[i].value, i);
	//			if (!rootOperator) return nullptr;
	//		case TokenType::Plus:
	//			rootOperator = parse_arithmitic<BinaryExprOp::Add>(rootOperator, i);
	//			if (!rootOperator) return nullptr;
	//		case TokenType::Minus:
	//			rootOperator = parse_arithmitic<BinaryExprOp::Subtract>(rootOperator, i);
	//			if (!rootOperator) return nullptr;
	//		case TokenType::Star:
	//			rootOperator = parse_arithmitic<BinaryExprOp::Multiply>(rootOperator, i);
	//			if (!rootOperator) return nullptr;
	//		case TokenType::Slash:
	//			rootOperator = parse_arithmitic<BinaryExprOp::Divide>(rootOperator, i);
	//			if (!rootOperator) return nullptr;
	//		case TokenType::Identifier:
	//			if (is_item_expr(rootOperator))
	//			{
	//				add_err(ErrorType::SyntaxError, c_unexpected_token(_tokens[i].value), i);
	//				return nullptr;
	//			}
	//			if (_tokens[i + 1].type == TokenType::LeftParen)
	//			{
	//				FuncDecl* callee = _declTracker.resolve_function(_tokens[i].value);
	//				if (!callee) return nullptr;
	//				i += 2;
	//				rootOperator = _memArena.alloc<CallExpr>(callee, parse_call_arguments(i));
	//				if (!rootOperator) return nullptr;
	//			}
	//			else
	//			{
	//				VarDecl* decl = _declTracker.resolve_variable(_tokens[i].value);
	//				if (!decl) return nullptr;
	//				rootOperator = _memArena.alloc<VariableExpr>(_tokens[i].value, decl);
	//			}
	//			break;
	//		case TokenType::Number:
	//		}

	//	}

	//	return rootOperator;
	//}

	bool Parser::eval_stop_condition_for_expr(WhereParsingExprFrom context, Iterator& i) const
	{
		switch (context)
		{
		case WhereParsingExprFrom::ReturnStatement:
		case WhereParsingExprFrom::VariableInitializer:
		case WhereParsingExprFrom::ForInitializer:
			return _tokens[i].type != TokenType::Semicolon;
		case WhereParsingExprFrom::CallArgument:
			return _tokens[i].type != TokenType::RightParen || _tokens[i].type != TokenType::Comma;
		case WhereParsingExprFrom::IfCondition:
		case WhereParsingExprFrom::WhileCondition:
		case WhereParsingExprFrom::Container:
		case WhereParsingExprFrom::ForIncrement:
			return _tokens[i].type != TokenType::RightParen;

		}
	}

	//bool Parser::is_unary_operator(Expr* expr) const
	//{
	//	if (!expr) return false;
	//	ASTExprType type = expr->type;
	//	return type != ASTExprType::Binary || type == ASTExprType::Container;
	//}

	std::vector<Expr*> alang::Parser::parse_call_arguments(Iterator& i)
	{
		std::vector<Expr*> arguments;
		i += 2; // Skip the identifier and the left parenthesis
		while (_tokens[i].type != TokenType::RightParen)
		{
			Expr* arg = parse_expression(WhereParsingExprFrom::CallArgument, i);
			// TODO: Add future logic for passing by referance and stuff like that here, but for now we will just assume all arguments are passed by value.
			if (!arg)
			{
				add_err(ErrorType::SyntaxError, c_unexpected_token(_tokens[i].value), i);
				return {};
			}
			arguments.push_back(arg);
			if (_tokens[i + 1].type != TokenType::Comma)
				add_err(ErrorType::SyntaxError, expected_token_after(_tokens[i].value, ","), i + 1);
			i += 2;
		}
		return arguments;
	}



	//bool Parser::parse_one_round(ModuleDecl* module, size_t& startIndex)
	//{

	//	for (Iterator i = { startIndex, _tokens.size() }; i < _tokens.size(); i++)
	//	{
	//		try {
	//			switch (_tokens[i].type)
	//			{
	//			case TokenType::Module:
	//				if (_tokens[i + 1].type == TokenType::Identifier)
	//				{
	//					std::string moduleName = _tokens[i + 1].value;
	//					ModuleDecl* newModule = _memArena.alloc<ModuleDecl>(moduleName);
	//					module->add_decl(newModule);

	//					if (_tokens[i + 1].type != TokenType::LeftBrace)
	//						add_err(ErrorType::SyntaxError, c_expected_left_brace(Keyword::Module), i + 1);
	//					i += 2;

	//					bool metRightBrace = false;
	//					for (size_t j = i; j < _tokens.size(); j++)
	//					{
	//						if (_tokens[j].type == TokenType::RightBrace)
	//						{
	//							i = j;
	//							metRightBrace = true;
	//							break;
	//						}
	//						parse_one_round(newModule, i.as_index()); // Recursively parse the inner module
	//					}
	//					if (!metRightBrace)
	//						add_err(ErrorType::SyntaxError, c_expected_right_brace(Keyword::Module), i);
	//				}
	//				else
	//					add_err(ErrorType::SyntaxError, c_expected_keyword_idenifier(Keyword::Module), i);

	//				break;
	//			case TokenType::Class:
	//				if (_tokens[i + 1].type == TokenType::Identifier)
	//				{
	//					std::string className = _tokens[i + 1].value;
	//					ClassDecl* newClass = _memArena.alloc<ClassDecl>(className);
	//					module->add_decl(newClass);
	//					if (_tokens[i + 1].type != TokenType::LeftBrace)
	//						add_err(ErrorType::SyntaxError, c_expected_left_brace(Keyword::Class), i + 1);
	//					i += 2;
	//					bool metRightBrace = false;
	//				
	//					parse_class_decl(module, newClass, i); // Recursively parse the inner class
	//					
	//					if (!metRightBrace)
	//						add_err(ErrorType::SyntaxError, c_expected_right_brace(Keyword::Class), i);
	//				}
	//				else
	//					add_err(ErrorType::SyntaxError, c_expected_keyword_idenifier(Keyword::Class), i);
	//				break;
	//			case TokenType::Int:
	//			case TokenType::Float:
	//			case TokenType::Void:
	//			case TokenType::Bool:
	//			case TokenType::Double:
	//			case TokenType::Char:
	//				// Handle variable or function declaration
	//				break;
	//			case TokenType::Identifier:
	//				// Handle identifier (could be variable, function call, etc.)
	//				break;
	//			case TokenType::If:
	//			case TokenType::Else:
	//			case TokenType::While:
	//			case TokenType::For:
	//			case TokenType::Return:
	//			case TokenType::Break:
	//			case TokenType::Continue:
	//			case TokenType::EndOfFile:

	//				// Handle control flow statements
	//				break;
	//			}
	//		}
	//		catch (const std::out_of_range& e)
	//		{
	//			add_err(ErrorType::SyntaxError, "Unexpected end of tokens while parsing", i); // TODO: Make the string into a constexpr string_view in error_handler.h
	//			return false;
	//		}
	//	}
	//}

	LiteralExpr* Parser::parse_num_literal(std::string_view value, Iterator& i)
	{
		std::string trueNumberVal = _tokens[i].value;
		int inc = 1;
		if (_tokens[i + 1].type == TokenType::Dot)
		{
			trueNumberVal += ".";
			inc++;
			if (_tokens[i + 2].type == TokenType::Number)
			{
				trueNumberVal += _tokens[i + 2].value;
				inc++;
			}
			else
			{
				add_err(ErrorType::SyntaxError, "Expected number after '.' in floating point literal", i + 2);
				return nullptr;
			}
		}
		else if (_tokens[i + inc].type == TokenType::Identifier)
		{
			// TODO: This is problamatic because the lexer does not recondize what is a specifier and what is an identifier, so we have to do it here in the parser, which is not ideal. We should probably change the lexer to recognize specifiers and keywords better, but for now this will do.
			bool unsignedSpecifierUsed = false;
			if (_tokens[i + inc].value == "u" || _tokens[i + inc].value == "U")
			{
				return _memArena.alloc<NumberLiteralExpr<unsigned int>>(numify<unsigned int>(trueNumberVal)); // Fix this!!!
				unsignedSpecifierUsed = true;
			}
			else if (_tokens[i + inc].value == "l" || _tokens[i + inc].value == "L")
			{
				if (unsignedSpecifierUsed)
					return _memArena.alloc<NumberLiteralExpr<unsigned long>>(numify<unsigned long>(trueNumberVal));
				else
				{
					return _memArena.alloc<NumberLiteralExpr<unsigned long>>(numify<unsigned long>(trueNumberVal));
					if (_tokens[i + 2].value == "u" || _tokens[i + 2].value == "U")
					{
						add_err(ErrorType::SyntaxError, "Unsigned specifier 'u' cannot be used after long specifier 'l'", i + 2); // TODO: Make the string into a constexpr string_view in error_handler.h
						return nullptr;
					}
				}
			}
			else if (_tokens[i + inc].value == "f" || _tokens[i + inc].value == "F")
				return _memArena.alloc<NumberLiteralExpr<float>>(numify<float>(trueNumberVal));
			else if (_tokens[i + inc].value == "d" || _tokens[i + inc].value == "D")
				return _memArena.alloc<NumberLiteralExpr<double>>(numify<double>(trueNumberVal));
		}
		else
			return _memArena.alloc<NumberLiteralExpr<int>>(numify<int>(trueNumberVal));
	}

	void Parser::parse_class_decl(ModuleDecl* module, ClassDecl* classDecl, Iterator& index)
	{
		bool publicMode = true;
		while (index < _tokens.size() || _tokens[index].type != TokenType::RightBrace)
		{
			switch (_tokens[index].type)
			{
			case TokenType::Public:
				publicMode = true;
				index++;
				if (_tokens[index + 1].type == TokenType::Colon)
					index++;
				add_err(ErrorType::SyntaxError, expected_token_after(":", "public"), index);
				break;
			case TokenType::Private:
				publicMode = false;
				index++;
				if (_tokens[index + 1].type == TokenType::Colon)
					index++;
				add_err(ErrorType::SyntaxError, expected_token_after(":", "private"), index);
				break;
			case TokenType::Int:
			case TokenType::Float:
			case TokenType::Void:
			case TokenType::Bool:
			case TokenType::Double:
			case TokenType::Char:
				if (publicMode)
					classDecl->add_public_member(parse_variable_or_function_decl(index)); // TODO: implement this function to parse variable or function declarations
				else
					classDecl->add_private_member(parse_variable_or_function_decl(index)); // TODO: implement this function to parse variable or function declarations
				if (_tokens[index].type == TokenType::RightBrace)
				{
					index++; // Move past the closing brace
					return;
				}
			}

			add_err(ErrorType::SyntaxError, c_expected_right_brace(Keyword::Class), index);
			return;
		}
	}

	Errors Parser::parse(const Tokens& tokens)
	{
		if (tokens.empty()) return { Error(ErrorType::SyntaxError, "No tokens to parse", 0, 0) };

		_tokens = tokens;

		_memArena.reset();
		_modules.clear();

		Modules modules;
		
		std::vector<std::thread> parsingModules;

		if (tokens[0].type != TokenType::Module)
		{
			add_err(ErrorType::SyntaxError, c_missing_module, 0);
			return _errors;
		}

		size_t startIndex = 0;

		while (startIndex < _tokens.size())
		{
			std::string moduleName;
			unsigned increment = 1;

			if (has_next(startIndex) && _tokens[startIndex + 1].type == TokenType::Identifier)
			{
				moduleName = _tokens[startIndex + 1].value;
				increment = 2;
			}

			ModuleDecl* module = nullptr;

			for (auto* mod : modules)
			{
				if (mod->get_name() == moduleName)
				{
					module = mod;
					break;
				}
			}

			if (!module)
			{
				module = _memArena.alloc<ModuleDecl>(moduleName);
				modules.push_back(module);
			}

			startIndex += increment;

			for (size_t i = startIndex; i < _tokens.size(); i++)
			{
				if (_tokens[i].type == TokenType::EndOfFile)
				{
					startIndex = i + 1;
					break;
				}

				parse_one_round(module, i);
			}
		}
	}


	Decl* Parser::parse_variable_or_function_decl(Iterator& i)
	{
		bool nativeType = false;
		if (is_native_type(_tokens[i].type))
			nativeType = true;
		else if (_tokens[i].type != TokenType::Identifier)
		{
			add_err(ErrorType::SyntaxError, c_expected_type_and_id_before_decl, i);
			return nullptr;
		}

		i++;

		if (_tokens[i].type == TokenType::Identifier)
		{
			std::string name = _tokens[i].value;
			i++;
			if (_tokens[i].type == TokenType::LeftParen)
			{
				return parse_function_decl(i);
			}
			else if (_tokens[i].type == TokenType::Equal || _tokens[i].type == TokenType::Semicolon ||
				_tokens[i].type == TokenType::Star || _tokens[i].type == TokenType::Colon)
			{
				if (nativeType)
					return parse_native_variable_decl(i);
				else 
					return parse_custom_variable_decl(i);
			}
			else
			{
				add_err(ErrorType::SyntaxError, c_unexpected_token(_tokens[i].value), i);
				return nullptr;
			}
		}
		else
		{
			add_err(ErrorType::SyntaxError, c_unexpected_token(_tokens[i].value), i);
			return nullptr;
		}
	}

	VarDecl* Parser::parse_variable_decl(Iterator& i)
	{
		return nullptr;
	}

	ModuleDecl* Parser::parse_module(Iterator& index)
	{
		ModuleDecl* module = nullptr;
		if (has_next(index) && _tokens[index + 1].type == TokenType::Identifier)
		{
			std::string moduleName = _tokens[index + 1].value;
			index += 2; // Move index past the module declaration
			module = _memArena.alloc<ModuleDecl>(moduleName);
		}
		else
		{
			module = _memArena.alloc<ModuleDecl>(""); // Create an unnamed module to continue parsing				
			index++; // Move index past the module declaration
		}

		for (size_t i = index; i < _tokens.size(); i++)
		{
			parse_one_round(module, i);
			if (_tokens[i].type == TokenType::RightBrace)
			{
				index = i + 1; // Move index past the closing brace
				return module;
			}
			else if (_tokens[i].type == TokenType::EndOfFile)
			{
				add_err(ErrorType::SyntaxError, c_expected_right_brace(Keyword::Module), i);
				return module;
			}
		}
		return nullptr;
	}

	FuncDecl* Parser::parse_function_decl(Iterator& i)
	{
		return nullptr;
	}

	VarDecl* Parser::parse_native_variable_decl(Iterator& i)
	{
		TokenType type = _tokens[i - 2].type; 
		if (type == TokenType::Void)
		{
			add_err(ErrorType::SyntaxError, c_vars_cant_be_void, i - 2);
			return nullptr;
		}
		else
		{
			std::string name = _tokens[i - 1].value;

			VarDecl* varDecl = nullptr;
			if (string_to_keyword(name).has_value())
			{
				add_err(ErrorType::SyntaxError, c_variables_cannot_be_keywords, i - 1);
				return nullptr;
			}

			if (_tokens[i].type == TokenType::Star)
			{
				varDecl = _memArena.alloc<VarDecl>(name, PointerType());
				i++;
			}
			else
			{
				switch (type)
				{
				case TokenType::Int:
					VarDecl* varDecl = _memArena.alloc<VarDecl>(name, Int());
				case TokenType::Float:
					varDecl = _memArena.alloc<VarDecl>(name, Float());
					break;
				case TokenType::Bool:
					varDecl = _memArena.alloc<VarDecl>(name, Bool());
					break;
				case TokenType::Double:
					varDecl = _memArena.alloc<VarDecl>(name, Double());
					break;
				case TokenType::Char:
					varDecl = _memArena.alloc<VarDecl>(name, Char());
					break;
				}
			}

			if (_tokens[i].type == TokenType::Equal)
				i++;
			else if (_tokens[i].type == TokenType::Colon)
			{
				// Handle attributes here (TODO)
				i++;
				while (_tokens[i].type == TokenType::Identifier)
				{
					if (Atribute* parse = parse_atribute(i))
					{
						varDecl->add_atribute(parse);
					}
					else
						add_err(ErrorType::SyntaxError, c_unexpected_token(_tokens[i].value), i);
					i++;
					if (_tokens[i].type == TokenType::Comma)
						i++;
					else
						break;
				}
				if (_tokens[i].type == TokenType::Equal)
					i++;
				parse_expression(i);
			}
			else if (_tokens[i].type != TokenType::Semicolon)
				return varDecl;
			return nullptr; // TODO: return the actual VarDecl after parsing the initializer if there is one

		}

	}

	VarDecl* Parser::parse_custom_variable_decl(Iterator& i)
	{
		return nullptr;
	}

}