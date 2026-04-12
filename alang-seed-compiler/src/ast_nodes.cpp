#include "headers.h"
#include "ast_nodes.h"


namespace alang {

    // ============================================================================
    // FuncDecl Implementation
    // ============================================================================

    FuncDecl::FuncDecl(FunctionType* type, const std::string& name,
        std::vector<std::string_view> paramNames,
        std::vector<Stmt*> stmts)
        : RTTI(), name(name), type(type),
        paramNames(std::move(paramNames)), statements(std::move(stmts))
    {
        set_mangled_name(type, name);
    }

    std::string stringify(Type* type)
    {
        // TODO: Make sure to incorperate this!!!
        // std::string atributes; 
        // if (type->get_atributes());

        switch (type->kind)
        {
        case ASTNodeKind::IntType:
            return "i";
            break;
        case ASTNodeKind::FloatType:
            return "f";
            break;
        case ASTNodeKind::DoubleType:
            return "d";
            break;
        case ASTNodeKind::BoolType:
            return "b";
            break;
        case ASTNodeKind::CharType:
            return "c";
            break;
        case ASTNodeKind::PointerType:
            return std::format("P<{}>", stringify(static_cast<PointerType*>(type)->get_pointee_type()));
            break;
        case ASTNodeKind::ArrayType:
            return std::format("A<{},{}>", stringify(static_cast<ArrayType*>(type)->get_element_type()), static_cast<ArrayType*>(type)->get_size());
            break;
        case ASTNodeKind::FunctionType:
        {
            const auto* func = static_cast<const FunctionType*>(type);
            std::string params;
            for (const auto& paramType : func->get_param_types())
                params += stringify(paramType);

            return std::format("F<{}({})" + stringify(func->get_return_type()));
            break;
        }
        case ASTNodeKind::ClassType:
            return std::format("C<{}>", static_cast<const ClassType*>(type)->get_class_name());
            break;
        }
    }

    void FuncDecl::set_mangled_name(FunctionType* type, std::string_view name)
    {
        std::string str = std::format("_X{}", name);

        // Add parameter types to mangled name

        mangledName = str;
    }


    unsigned int get_precedence(BinaryExprOp op)
    {
        switch (op)
        {
        case BinaryExprOp::Add:
        case BinaryExprOp::Subtract:
			return static_cast<unsigned int>(PrecedenceLevel::Additive);
        case BinaryExprOp::Multiply:
        case BinaryExprOp::Divide:
        case BinaryExprOp::Modulo:
			return static_cast<unsigned int>(PrecedenceLevel::Multiplicative);
        case BinaryExprOp::And:
			return static_cast<unsigned int>(PrecedenceLevel::BitwiseAnd);
        case BinaryExprOp::Or:
			return static_cast<unsigned int>(PrecedenceLevel::BitwiseOr);
        case BinaryExprOp::Xor:
			return static_cast<unsigned int>(PrecedenceLevel::BitwiseXor);
		case BinaryExprOp::LogicalAnd:
			return static_cast<unsigned int>(PrecedenceLevel::LogicalAnd);
		case BinaryExprOp::LogicalOr:
			return static_cast<unsigned int>(PrecedenceLevel::LogicalOr);
        default:
            break;
        }
    }
}

    // ============================================================================
    // ModuleDecl Implementation
    // ============================================================================


} // namespace alang