#include "BasicTypeAST.h"

BasicTypeAST* intTypeAST;
BasicTypeAST* charTypeAST;
BasicTypeAST* boolTypeAST;
BasicTypeAST* realTypeAST;
BasicTypeAST* idTypeAST;

llvm::Type *BasicTypeAST::toLLVMType()
{
		switch (type)
		{
		case TypeName::Integer:
			return llvm::Type::getInt32Ty(*TheContext);
			break;
		case TypeName::Real:
			return llvm::Type::getDoubleTy(*TheContext);
			break;
		case TypeName::Char:
			return llvm::Type::getInt8Ty(*TheContext);
			break;
		case TypeName::Bool:
			return llvm::Type::getInt1Ty(*TheContext);
			break;
		default:
			return llvm::Type::getVoidTy(*TheContext);
			break;
		}
}