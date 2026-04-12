#include "headers.h"
#include "name_resolver.h"

namespace alang
{
	NameResolver::NameResolver(std::function<void(std::string_view)> errFunction)
		: _errFunction(errFunction)
	{}
	
	void NameResolver::add_name_to_resolve(std::vector<std::string_view> name, Decl* whereFrom, std::vector<unsigned int> nameAccessIndices)
	{
		_namesToResolve.push_back({ std::move(name), whereFrom, std::move(nameAccessIndices) });
	}
	bool NameResolver::resolve_names()
	{
		for (const auto& nameToResolve : _namesToResolve)
		{
			switch (nameToResolve.whereFrom->type)
			{
			case DeclKind::Func:
				if (nameToResolve.name.size() == 0)
				{
					for (size_t i = nameToResolve.nameAccessIndices.size() - 1; i >= 0; i--)
					{
						unsigned int size = nameToResolve.nameAccessIndices[i];
						VarDecl* varDecl = search_for_var_decl(nameToResolve.name, size, static_cast<FuncDecl*>(nameToResolve.whereFrom));
					}
				}
			case DeclKind::Stmt: // This hnadles declarations that are also statments, and we will catagorize emptry scopes { ... } as Decl Stmts
				if (nameToResolve.name.size() == 0)
				{ // make int a = a illigal!!! -- never mind, we can support this by first resolving the name and then adding it to the scope, but for now we will just make it illigal
					for (size_t i = nameToResolve.nameAccessIndices.size() - 1; i >= 0; i--)
					{
						unsigned int size = nameToResolve.nameAccessIndices[i];
						switch (static_cast<DeclStmt*>(nameToResolve.whereFrom)->type)
						{
						case DeclStmtType::Var:
							VarDecl* varDecl = search_for_var_decl(nameToResolve.name, size, static_cast<VarDeclStmt*>(nameToResolve.whereFrom));
						case DeclStmtType::Class:
							VarDecl* varDecl = search_for_var_decl(nameToResolve.name, size, static_cast<ClassDeclStmt*>(nameToResolve.whereFrom));
						case DeclStmtType::EmptyScope:
							VarDecl* varDecl = search_for_var_decl(nameToResolve.name, size, static_cast<DeclStmt*>(nameToResolve.whereFrom));
					}
				}

			}
		}

		return true;
	}

}
