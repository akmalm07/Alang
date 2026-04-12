#pragma once

#include <vector>
#include <string_view>
#include <functional>

#include "ast_nodes.h"


namespace alang
{

	struct NameToResolve
	{
		std::vector<std::string_view> name; // The vector is so that we can support member access like a.b.c.d.e.f.g
		Decl* whereFrom;
		std::vector<unsigned int> nameAccessIndices;
	};

	class NameResolver
	{
	public:
		NameResolver(std::function<void(std::string_view)> errFunction);

		void add_name_to_resolve(std::vector<std::string_view> name, Decl* whereFrom, std::vector<unsigned int> nameAccessIndices);

		bool resolve_names();
	
	private:
		std::function<void(std::string_view)> _errFunction;
		std::vector<NameToResolve> _namesToResolve;

	private:
		template<IsDeclDerived T>
		VarDecl* search_for_var_decl(const std::vector<std::string_view>& name, unsigned int size, T* whereFrom);
	};

	template<IsDeclDerived T>
	VarDecl* NameResolver::search_for_var_decl(const std::vector<std::string_view>& name, unsigned int size, T* whereFrom)
	{
		if constexpr (std::derived_from<T, FuncDecl> || std::derived_from<T, FuncDecl>)
		{
			const auto& statments = whereFrom->get_statments();

			for (unsigned int i = size - 1; i >= 0; i--)
			{
				if (statments[i]->type == ASTNodeType::VarDecl)
				{
					VarDecl* varDecl = static_cast<VarDecl*>(statments[i]);
					if (varDecl->get_name() == name.back())
						return varDecl;
				}
			}

		// Continue here ...
		}
	}
}