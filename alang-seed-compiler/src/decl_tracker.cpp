#include "headers.h"
#include "decl_tracker.h"
#include "keywords.h"


namespace alang
{

	DeclTracker::DeclTracker(Arena& arena)
		: _arena(arena)
	{}

	bool DeclTracker::is_valid_name(std::string_view name) const
	{
		if (!_moduleDefined) // No name is valid if a module is not defined yet
			return false;

		if (name.empty())
			return false;
		else if (name.size() > 64)
			return false;

		auto c = static_cast<unsigned char>(name[0]);
		if (!(std::isalpha(c) || c == '_'))
			return false;

		for (size_t i = 1; i < name.size(); ++i)
		{
			c = static_cast<unsigned char>(name[i]);
			if (!(std::isalnum(c) || c == '_'))
				return false;
		}

		// Disallow keywords
		if (string_to_keyword(name).has_value())
			return false;

		return true;
	}

	bool DeclTracker::add_variable(std::string_view name, Type* type, Atribute* atribute)
	{
		if (!is_valid_name(name))
			return false;
		if (atribute)
			_modules[_currentModule.index]->add_var_decl(_arena.alloc<VarDecl>(name, type, atribute));
		else
			_modules[_currentModule.index]->add_var_decl(_arena.alloc<VarDecl>(name, type));
		return true;
	}

	bool DeclTracker::add_variable(std::string_view name, Type* type, std::vector<Atribute*>& atributes)
	{
		if (!is_valid_name(name))
			return false;
		_modules[_currentModule.index]->add_var_decl(_arena.alloc<VarDecl>(name, type, std::move(atributes)));
		return true;
	}

	bool DeclTracker::add_class(std::string_view name, std::vector<Decl*>& privateMembers, std::vector<Decl*>& publicMembers)
	{
		if (!is_valid_name(name))
			return false;
		_modules[_currentModule.index]->add_class_decl(_arena.alloc<ClassDecl>(name, std::move(privateMembers), std::move(publicMembers)));
		return true;
	}

	bool DeclTracker::add_module(std::string_view name, std::vector<Decl*>& decls, SwitchModuleContext context)
	{
		if (is_valid_name(name))
			return false;

		if (!_moduleDefined)
		{
			_moduleDefined = true;
			context = SwitchModuleContext::YES;
		}
		if (context == SwitchModuleContext::YES)
		{
			_currentModule.name = name;
			_currentModule.index = 0;
		}
		_modules.push_back(_arena.alloc<ModuleDecl>(name, std::move(decls), context));
		return true;
	}

	bool DeclTracker::add_func(std::string_view name, FunctionType* type, std::vector<std::string_view> paramNames, std::vector<Stmt*> statments)
	{
		if (!is_valid_name(name))
			return false;
		_modules[_currentModule.index]->add_func_decl(_arena.alloc<FuncDecl>(type, name, std::move(paramNames), std::move(statments)));
		return true;
	}

	bool DeclTracker::switch_module(std::string_view name)
	{

		for (size_t i = 0; i < _modules.size(); i++)
		{
			if (_modules[i]->get_name() == name)
			{
				_currentModule.name = name;
				_currentModule.index = i;
				return true;
			}
		}
		return false;
	}

	bool DeclTracker::variable_exists(std::string_view name) const
	{
		auto vars = _modules[_currentModule.index]->get_var_decls();
		for (const auto& var : vars)
		{
			if (var->type == DeclKind::Var && static_cast<const VarDecl*>(var)->get_name() == name)
				return true;
		}
		return false;
	}

	bool DeclTracker::module_exists(std::string_view name) const
	{
		for (const auto& mod : _modules)
		{
			if (mod->get_name() == name)
				return true;
		}
		return false;
	}
	//VarDecl* DeclTracker::resolve_member_variable(const std::vector<std::string_view>& memberAccess) const
	//{
	//	return nullptr;
	//}
	//VarDecl* DeclTracker::resolve_variable(std::string_view name) const
	//{
	//	auto vars = _modules[_currentModule.index]->get_var_decls();
	//	for (const auto& var : vars)
	//	{
	//		if (var->type() == DeclKind::Var && static_cast<const VarDecl*>(var)->get_name() == name)
	//			return static_cast<VarDecl*>(var);
	//	}
	//	return nullptr;
	//}
	//FuncDecl* DeclTracker::resolve_function(std::string_view name) const
	//{
	//	return nullptr;
	//}
}