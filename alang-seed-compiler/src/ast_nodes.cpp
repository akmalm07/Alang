#include "headers.h"
#include "ast_nodes.h"
#include "compile_time_helpers.h"

namespace alang
{
    void ModuleDecl::add_var_decl(VarDecl* decl)
    {
		varDecls.push_back(decl);
    }

    std::string_view ModuleDecl::get_name() const
    {
        return name;
    }

    const std::vector<VarDecl*>& ModuleDecl::get_var_decls() const
    {
		return varDecls;
    }


    const std::vector<Stmt*>& FuncDecl::get_statments() const
    {
        return statments;
    }

    size_t FuncDecl::get_param_count() const
    {
        return paramNames.size();
    }

    std::string_view VarDecl::get_name() const
    {
        return name;
    }

    void VarDecl::add_initializer(Expr* initializer)
    {
		this->initializer = initializer;
    }

    FuncDecl::FuncDecl(FunctionType* type, std::string_view name, std::vector<std::string_view> paramNames, std::vector<Stmt*> statments)
        : Decl(ASTDeclType::Func), type(type), statments(std::move(statments)), paramNames(std::move(paramNames))
    {
        set_mingled_name(type, name);
    }

    std::string_view FuncDecl::get_mingled_name() const
    {
        return std::string_view();
    }

    void FuncDecl::set_mingled_name(FunctionType* type, std::string_view name)
    {
		mingledName.clear();
        const size_t paramSize = type->get_param_types().size();
        mingledName.reserve(name.size() + 2 + paramSize * 10 + 10); // Reserve enough space to avoid multiple allocations

        mingledName += stringify(type->get_return_type());
        
		size_t sizeBeforeName = mingledName.size();
        mingledName += name;

        mingledName += "(";
        for (size_t i = 0; i < paramSize; i++)
        {
            mingledName += stringify(type->get_param_types()[i]);
            if (i < paramSize - 1)
                mingledName += ",";
        }
        mingledName += ")";

		name = mingledName.substr(sizeBeforeName, name.size());
    }

    std::string_view FuncDecl::get_name() const
    {
        return name;
    }

    const std::vector<Type*>& FunctionType::get_param_types() const
    {
		return parameterTypes;
    }

    Type* const FunctionType::get_return_type() const
    {
        return returnType;
    }

    void Operator::set_value(Expr* val)
    {
        // assert("Value must be null before setting!", value);
        value = val;
	}

    void ClassDecl::add_public_member(Decl* decl)
    {
		publicMembers.push_back(decl);
    }

    void ClassDecl::add_private_member(Decl* decl)
	{
		privateMembers.push_back(decl);
	}

    std::string_view stringify(ASTExprType type)
    {
        switch (type)
        {
        case ASTExprType::Binary: return "Binary";
        case ASTExprType::Unary: return "Unary";
        case ASTExprType::NumberLiteral: return "NumberLiteral";
        case ASTExprType::StringLiteral: return "StringLiteral";
        case ASTExprType::Variable: return "Variable";
        case ASTExprType::Call: return "Call";
        }
    }

    std::string_view stringify(BinaryExprOp op)
    {
        switch (op)
        {
        case BinaryExprOp::Add: return "+";
        case BinaryExprOp::Subtract: return "-";
        case BinaryExprOp::Multiply: return "*";
        case BinaryExprOp::Divide: return "/";
        case BinaryExprOp::Modulo: return "%";
        case BinaryExprOp::And: return "&";
        case BinaryExprOp::Or: return "|";
        case BinaryExprOp::Xor: return "^";
		}
    }

    std::string_view stringify(ASTDeclType type)
    {
        switch (type)
        {
        case ASTDeclType::Var: return "Var";
        case ASTDeclType::Func: return "Func";
		case ASTDeclType::Param: return "Param";
        case ASTDeclType::Class: return "Class";
        case ASTDeclType::Module: return "Module";
        case ASTDeclType::Import: return "Import";
		}
    }

    std::string stringify(Type* type)
    {
        switch (type->type)
        {
        case ASTType::Int: return "Int";
        case ASTType::Float: return "Float";
        case ASTType::Double: return "Double";
        case ASTType::Bool: return "Bool";
        case ASTType::Char: return "Char";
        case ASTType::Class:
        {
            std::string str;
            str += "class<";
            str += static_cast<ClassType*>(type)->get_class_name();
            return str + ">";
        }
        case ASTType::Pointer:
        {
            std::string str;
            str += "ptr<";
            str += stringify(static_cast<PointerType*>(type)->get_pointee_type());
            return str + ">";
        }
    }
}