#include "ConstAST.h"
#include "BlockTable.h"

Value *ConstAST::Codegen()
{
    /**
     std::string variableName;
     BasicTypeAST* type;
     constValue value;
     int isGlobal;
     */
    //std::cout << "const" << std::endl;
    Value *val, *alloc;
    
    if (blockStack.top()->Insert(variableName, this, {0, 0}, type))
            // 检查符号表，查看variableName是否存在，如果不存在，将该常量加入符号表, 进行下面的操作
    {
            //生成type的IR，返回值是一个
        switch (type->GetType()) {
            case TypeName::Integer:
                val = ConstantInt::get(Type::getInt32Ty(*TheContext), value.i, false);
                break;
            case TypeName::Real:
                val = ConstantFP::get(Type::getDoubleTy(*TheContext), value.d);
                break;
                
            case TypeName::Char:
                val = ConstantInt::get(Type::getInt8Ty(*TheContext), (int)value.c, false);
                break;
            
            case TypeName::Bool:
                val = ConstantInt::get(Type::getInt1Ty(*TheContext), (int)value.b, false);
                break;
                
            default:
                //报错
                //std::cout << "illegal constType" << std::endl;
                exit(1);
                break;
        }
        //val = type->Codegen();
        

        
            //整型
        if (type->GetType() == TypeName::Integer)
        {
            //std::cout << "new constant integer" << std::endl;
            if (isGlobal)
                alloc = new llvm::GlobalVariable(*TheModule, Type::getInt32Ty(*TheContext), true, GlobalValue::ExternalLinkage, builder->getInt32(0));
	        else
                alloc = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt32Ty(*TheContext), variableName);
            builder->CreateStore(val, alloc);
            //std::cout << "sign up at varTable: " << variableName << std::endl;
        }
            //浮点
        else if (type->GetType() == TypeName::Real)
        {
            //std::cout << "new constant real" << std::endl;
            if (isGlobal)
                alloc = new llvm::GlobalVariable(*TheModule, Type::getDoubleTy(*TheContext), true, GlobalValue::ExternalLinkage, ConstantFP::get(Type::getDoubleTy(*TheContext), 0.0));
            else
                alloc = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getDoubleTy(*TheContext), variableName);
            builder->CreateStore(val, alloc);
            //std::cout << "sign up at varTable: " << variableName << std::endl;
        }
            //bool
        else if (type->GetType() == TypeName::Bool)
        {
           // std::cout << "new constant bool" << std::endl;
            if (isGlobal)
                alloc = new llvm::GlobalVariable(*TheModule, Type::getInt1Ty(*TheContext), true, GlobalValue::ExternalLinkage, builder->getInt1(false));
            else
                alloc = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt1Ty(*TheContext), variableName);
            builder->CreateStore(val, alloc);
            //std::cout << "sign up at varTable: " << variableName << std::endl;
        }
            //char
        else if (type->GetType() == TypeName::Char)
        {
            //std::cout << "new constant char" << std::endl;
            if (isGlobal)
                alloc = new llvm::GlobalVariable(*TheModule, Type::getInt8Ty(*TheContext), true, GlobalValue::ExternalLinkage, builder->getInt8('c'));
            else
                alloc = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt8Ty(*TheContext), variableName);
            
            builder->CreateStore(val, alloc);
            //std::cout << "sign up at varTable: " << variableName << std::endl;
        }
    }
        //插入符号表
    blockStack.top()->InsertNameValue(variableName, alloc);
    return alloc;
}