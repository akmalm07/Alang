#pragma once

#include <vector>
#include <string_view>

#include "ast_nodes.h"
#include "memory_arena.h"

namespace alang
{

	struct FuncInfo
	{
		std::string_view name;
		std::string_view returnType;
		std::vector<std::string_view> parameterTypes;
	};

	enum class SwitchModuleContext
	{
		YES,
		NO
	};	

	class DeclTracker
	{
	public:
		DeclTracker(Arena& arena);

		bool is_valid_name(std::string_view name) const;

		bool add_variable(std::string_view name, Type* type, Atribute* atribute = nullptr);
		bool add_variable(std::string_view name, Type* type, std::vector<Atribute*>& atributes);

		bool add_class(std::string_view name, std::vector<Decl*>& privateMembers, std::vector<Decl*>& publicMembers);

		bool add_module(std::string_view name, std::vector<Decl*>& decls, SwitchModuleContext context = SwitchModuleContext::YES);

		bool add_func(std::string_view name, FunctionType* type, std::vector<std::string_view> paramNames, std::vector<Stmt*> statments);

		bool switch_module(std::string_view name);

		bool variable_exists(std::string_view name) const;
		bool module_exists(std::string_view name) const;

		//VarDecl* resolve_member_variable(const std::vector<std::string_view>& memberAccess) const; // TODO: Implememnt
		//VarDecl* resolve_variable(std::string_view name) const;
		//FuncDecl* resolve_function(std::string_view name) const; // TODO: Implememnt

		/*
		bool add_global_variable(std::string_view name, Type* type, Atribute* atribute = nullptr);
		bool add_global_variable(std::string_view name, Type* type, std::vector<Atribute*>& atributes);
		bool add_global_func(std::string_view name, FunctionType* type, std::vector<std::string_view> paramNames, std::vector<Stmt*> statments);
		bool add_global_class(std::string_view name, std::vector<Decl*> privateMembers, std::vector<Decl*> publicMembers);
		*/
	private:
		Arena& _arena;
		std::vector<ModuleDecl*> _modules;
		
		struct {
			std::string_view name;
			unsigned int index;
		} _currentModule;

		bool _moduleDefined = false;
		
		//std::vector<Decl*> _globalDecls;
	};


}