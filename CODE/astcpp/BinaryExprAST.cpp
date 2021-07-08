#include "BinaryExprAST.h"
#include "BlockTable.h"

Value *BinaryExprAST::Codegen()
    {
        //L's allocaInst*/Value*
        auto L = LExpr->Codegen();
        //L's allocaInst*/Value*
        auto R = RExpr->Codegen();
		
		Value* L_value, *R_value;
        bool isVar = false;
        bool isPtr = false;
	auto type_L = blockStack.top()->GetExprType(LExpr, &isVar, &isPtr);

	if(isPtr)
	    L_value =builder->CreateLoad(L, "L_expr");
        else
            L_value = L;
        
        isPtr = false;
        auto type_R = blockStack.top()->GetExprType(RExpr, &isVar, &isPtr);
		if(isPtr)
			R_value =builder->CreateLoad(R, "R_expr");
		else
            R_value = R;
       
	    AllocaInst* ret_ptr = nullptr;
        
        //std::cout << "Creating binary operation "  << std::endl;
        switch (op)
        {
            case BinaryOpKind::plusKind:
                //如果是double
                if (type_L->GetType() == TypeName::Real)
                {
                    //ret_ptr = builder->CreateAlloca(Type::getDoubleTy(*TheContext),nullptr,"result_double");
                    ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getDoubleTy(*TheContext), std::string("result_double of plus"));
                    builder->CreateStore(builder->CreateFAdd(L_value, R_value, "addtmp"), ret_ptr);
                }
                //如果不是double
                else

                {
		            ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), type_L->toLLVMType(), std::string("result_int of plus"));
		            builder->CreateStore(builder->CreateAdd(L_value, R_value, "addtmp"), ret_ptr);
		        }
                break;
			//minus
			case BinaryOpKind::minusKind:
                //如果是double
                if (type_L->GetType() == TypeName::Real)
                {
                    ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getDoubleTy(*TheContext), std::string("result_double of minus"));
                    builder->CreateStore(builder->CreateFSub(L_value, R_value, "minustmp"), ret_ptr);
                }
                //如果不是double
                else
                {
		          ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), type_L->toLLVMType(), std::string("result_int of minus"));
                    builder->CreateStore(builder->CreateSub(L_value, R_value, "addtmp"), ret_ptr);
                }
                break;
            case BinaryOpKind::modKind:
                ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), type_L->toLLVMType(), std::string("result_int of mod"));
                builder->CreateStore(builder->CreateSRem(L_value, R_value, "modtmp"), ret_ptr);
                
                break;
                
            case BinaryOpKind::divKind:
                //整型强转
		        ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), type_L->toLLVMType(), std::string("result_integer of div"));
                builder->CreateStore(builder->CreateSDiv(L_value, R_value, "divtmp"), ret_ptr);
				
                break;
			case BinaryOpKind::divRealKind:
                
                ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getDoubleTy(*TheContext), std::string("result_double of divreal"));
                
                if (!(type_L->GetType() == TypeName::Real))
                {
                    L_value = builder->CreateUIToFP(L_value, Type::getDoubleTy(*TheContext));
                    R_value = builder->CreateUIToFP(R_value, Type::getDoubleTy(*TheContext));
                }

            	builder->CreateStore(builder->CreateFDiv(L_value, R_value, "divrealtmp"), ret_ptr);
                break;
            case BinaryOpKind::mulKind:
                if (type_L->GetType() == TypeName::Real)
				{
                    ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getDoubleTy(*TheContext), std::string("result_double of mul"));
                    builder->CreateStore(builder->CreateFMul(L_value, R_value, "multmp"), ret_ptr);
				}else{
                    ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), type_L->toLLVMType(), std::string("result_integer of mul"));
                    builder->CreateStore(builder->CreateMul(L_value, R_value, "multmp"), ret_ptr);
				}
				break;
            case BinaryOpKind::ltKind:
                if (type_L->GetType() == TypeName::Real)
				{
                    ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt1Ty(*TheContext), std::string("result_double of lt"));
                    builder->CreateStore(builder->CreateFCmpOLT(L_value, R_value, "lttmp"), ret_ptr);
				}else{
                    ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt1Ty(*TheContext), std::string("result_int of lt"));
                    builder->CreateStore(builder->CreateICmpSLT(L_value, R_value, "lttmp"), ret_ptr);
				}
                break;
            case BinaryOpKind::gtKind:
                if (type_L->GetType() == TypeName::Real)
				{
                    ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt1Ty(*TheContext), std::string("result_double of gt"));
                    builder->CreateStore(builder->CreateFCmpOGT(L_value, R_value, "gttmp"), ret_ptr);
				}
                else{
                    ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt1Ty(*TheContext), std::string("result_int of gt"));
                    builder->CreateStore(builder->CreateICmpSGT(L_value, R_value, "gttmp"), ret_ptr);
				}
                break;
            case BinaryOpKind::eqKind:
                if (type_L->GetType() == TypeName::Real)
				{
                    ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt1Ty(*TheContext), std::string("result_double of eq"));
                    builder->CreateStore(builder->CreateFCmpOEQ(L_value, R_value, "eqtmp"), ret_ptr);
				}else{
                    ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt1Ty(*TheContext), std::string("result_int of eq"));
                    builder->CreateStore(builder->CreateICmpEQ(L_value, R_value, "eqtmp"), ret_ptr);	
				}
                break;
            case BinaryOpKind::leKind:
                if (type_L->GetType() == TypeName::Real)
				{
                    ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt1Ty(*TheContext), std::string("result_double of le"));
                    builder->CreateStore(builder->CreateFCmpOLE(L_value, R_value, "letmp"), ret_ptr);
				}else{
                    ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt1Ty(*TheContext), std::string("result_int of le"));
                    builder->CreateStore(builder->CreateICmpSLE(L_value, R_value, "letmp"), ret_ptr);		
				}
                break;
            case BinaryOpKind::geKind:
                if (type_L->GetType() == TypeName::Real)
				{
                    ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt1Ty(*TheContext), std::string("result_double of ge"));
                    builder->CreateStore(builder->CreateFCmpOGE(L_value, R_value, "getmp"), ret_ptr);
				}else{
                    ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt1Ty(*TheContext), std::string("result_int of ge"));
                    builder->CreateStore(builder->CreateICmpSGE(L_value, R_value, "getmp"), ret_ptr);
				}
                break;
            case BinaryOpKind::ueqKind:
                if (type_L->GetType() == TypeName::Real)
				{
                    ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt1Ty(*TheContext), std::string("result_double of ueq"));
                    builder->CreateStore(builder->CreateFCmpONE(L_value, R_value, "ueqtmp"), ret_ptr);
				}else{
                    ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt1Ty(*TheContext), std::string("result_int of ueq"));
                    builder->CreateStore(builder->CreateICmpNE(L_value, R_value, "ueqtmp"), ret_ptr);	
				}
                break;
                
            case BinaryOpKind::andKind:
                ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt1Ty(*TheContext), std::string("result and"));
                builder->CreateStore(builder->CreateAnd(L_value, R_value, "andtmp"), ret_ptr);	
				break;
            case BinaryOpKind::orKind:
                ret_ptr = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt1Ty(*TheContext), std::string("result or"));
                builder->CreateStore(builder->CreateOr(L_value, R_value, "ortmp"), ret_ptr);	
				break;					
            case BinaryOpKind::assignmentKind:
            {
                builder->CreateStore(R_value, L);
                return L;
            }
            default:
                return NULL;
        }
		return ret_ptr;
    }