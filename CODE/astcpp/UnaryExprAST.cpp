#include "UnaryExprAST.h"
#include "BlockTable.h"

Value *UnaryExprAST::Codegen()
    {
        bool isVar = false;
        bool isPtr = false;
		auto tt = blockStack.top()->GetExprType(expr, &isVar, &isPtr);
        auto ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), tt->toLLVMType(), std::string("unary_result"));
        
        Value *result;
        //expr代表的AST的codegen
		auto eexpr = expr->Codegen();
		
		if(isPtr)
			result = builder->CreateLoad(eexpr, "eexpr");
        else
            result = eexpr;

		if (op == UnaryOpKind::notKind)
        {
			builder->CreateStore(builder->CreateNot(result, "unary_variable"), ret_ptr);
        }
        else if(op == UnaryOpKind::negKind){	
			builder->CreateStore(builder->CreateNeg(result, "unary_variable"), ret_ptr);
		}
		return ret_ptr;
    }