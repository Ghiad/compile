#include "CallProcedureExprAST.h"
#include "BlockTable.h"

Value *CallProcedureExprAST::Codegen()
    {
        //std::cout << "Here is a procedure call:" << std::endl;
        Function *CalleeF = TheModule->getFunction(callee.c_str());
        
        if (!CalleeF)
        {
            std::cout << "Unknown procedure referenced" << std::endl;
            exit(1);
        }
        // If argument mismatch error.
        if (CalleeF->arg_size() != args.size())
        {
            std::cout << "Incorrect # arguments passed" << std::endl;
            exit(1);
        }
        
        //获取函数的functionType
        auto CalleeF_type = CalleeF->getFunctionType();
        std::vector<Value *> ArgsV;
        
        for (int i = 0; i < args.size(); ++i)
        {
            auto CalleeF_arg_type = CalleeF_type->getParamType(i);
            
            bool isVal = false;
            bool isPtr = false;
            blockStack.top()->GetExprType(args[i],&isVal,&isPtr);
            
            
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
        std::cout << "call procedure end" << std::endl;
        
        return builder->CreateCall(CalleeF, ArgsV);
    }