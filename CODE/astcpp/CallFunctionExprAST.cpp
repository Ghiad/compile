#include "CallFunctionExprAST.h"
#include "BlockTable.h"

Value *CallFunctionExprAST::Codegen()
    {
        //std::cout << "Here is a function call" << std::endl;
        Function *CalleeF = TheModule->getFunction(callee.c_str());
        if (!CalleeF)
            exit(1);
        
        //参数数量不匹配
        if (CalleeF->arg_size() != args.size()){
	    exit(1);
	}

        //返回值地址
        auto ret_alloca = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), CalleeF->getReturnType(), callee);
        
        //获取调用函数的functiontype
        auto CalleeF_type = CalleeF->getFunctionType();
        
        std::vector<Value *> ArgsV;
        //获取调用函数的每个参数的type
        for (int i = 0; i < CalleeF->arg_size(); i++)
        {
            auto CalleeF_arg_type = CalleeF_type->getParamType(i);
            
            bool isVal = false;
            bool isPtr = false;
            blockStack.top()->GetExprType(args[i],&isVal, &isPtr);
             
            auto args_i_addr = args[i]->Codegen();
            Value* args_i_value = nullptr;
            //常量
            if(!isPtr)
            {
                args_i_value = args_i_addr;
                ArgsV.push_back(args_i_addr);
            }
            //是变量
            else
            {
                
                args_i_value = builder->CreateLoad(args_i_addr,"args_i_value");
                
                //如果是非引用，传值
                if (llvm::Type::getInt32Ty(*TheContext) == CalleeF_arg_type || llvm::Type::getDoubleTy(*TheContext) == CalleeF_arg_type || llvm::Type::getInt8Ty(*TheContext) == CalleeF_arg_type || llvm::Type::getInt1Ty(*TheContext) == CalleeF_arg_type)
                    ArgsV.push_back(args_i_value);
                //如果是引用，传地址
                else if (llvm::Type::getInt32PtrTy(*TheContext) == CalleeF_arg_type || llvm::Type::getDoublePtrTy(*TheContext) == CalleeF_arg_type || llvm::Type::getInt8PtrTy(*TheContext) == CalleeF_arg_type || llvm::Type::getInt1PtrTy(*TheContext) == CalleeF_arg_type)
                    
                    ArgsV.push_back(args_i_addr);
                else{
                    std::cout << "函数调用 参数类型不匹配" << std::endl;
                    ArgsV.push_back(nullptr);
                }
            }
            //检查是否成功
            if (!ArgsV.back())
            {
                std::cout << "para_generate fails" << std::endl;
                exit(1);
            }
        }
        
        
        auto ret_value =  builder->CreateCall(CalleeF, ArgsV, "calltmp");
        builder->CreateStore(ret_value,ret_alloca);
        //std::cout << "call function end" << std::endl;
        return ret_alloca;
    }