#include "VariableDeclAST.h"
#include "BlockTable.h"

Value *VariableDeclAST::Codegen()
{
    /**
     int isRef;//是否是引用
     int isGlobal;
     TypeAST* type;
     std::vector<std::string> variableName;
     */
    //std::cout << "var" << std::endl;
    Value *alloc;
    
    for (int i = 0; i < variableName.size(); i++)
    {
        if (blockStack.top()->Insert(variableName[i], this, {0, 0}, type))
                // 检查符号表，查看variableName[i]是否存在，如果不存在，加入符号表，进行下面的操作
        {
            if (type->GetType() == TypeName::Integer || type->GetType() == TypeName::Real || type->GetType() == TypeName::Char || type->GetType() == TypeName::Bool)
            {
                //如果是基本类型
                if (type->GetType() == TypeName::Integer)
                {
                    //std::cout << "new integer" << std::endl;
                    Value *val = builder->getInt32(0);
                    if (isGlobal)
                        alloc = new llvm::GlobalVariable(*TheModule, Type::getInt32Ty(*TheContext), false, GlobalValue::ExternalLinkage, builder->getInt32(0),variableName[i].c_str());
                    else
                    {
                        if (isRef){
						//std::cout <<"var " <<variableName[i] <<" is ref"<<std::endl;
                        // 该变量是否是引用（&）
                        alloc = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt32PtrTy(*TheContext), variableName[i]);
						}
                        else
                            alloc = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt32Ty(*TheContext), variableName[i]);
                    }
                    
                    builder->CreateStore(val, alloc);
                    //std::cout << "sign up at varTable: " << variableName[i] << std::endl;
                }
                else if (type->GetType() == TypeName::Real)
                {
                    //std::cout << "new real" << std::endl;
                    Value *val = ConstantFP::get(Type::getDoubleTy(*TheContext), 0.0);
                    if (isGlobal)
                        alloc = new llvm::GlobalVariable(*TheModule, Type::getDoubleTy(*TheContext), false, GlobalValue::ExternalLinkage, ConstantFP::get(Type::getDoubleTy(*TheContext), 0.0), variableName[i].c_str());
                    else
                    {
                        // 该变量是否是引用（&）
                        if (isRef)
                            alloc = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getDoublePtrTy(*TheContext), variableName[i]);
                        else
                            alloc = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getDoubleTy(*TheContext), variableName[i]);
                    }
                    
                    builder->CreateStore(val, alloc);
                    //std::cout << "sign up at varTable: " << variableName[i] << std::endl;
                }
                else if (type->GetType() == TypeName::Bool)
                {
                    //std::cout << "new bool" << std::endl;
                    Value *val = builder->getInt1(false);
                    if (isGlobal)
                        alloc = new llvm::GlobalVariable(*TheModule, Type::getInt1Ty(*TheContext), false, GlobalValue::ExternalLinkage, builder->getInt1(0),variableName[i].c_str());
                    else
                    {
                        // 该变量是否是引用（&）
                        if (isRef)
                            alloc = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt1PtrTy(*TheContext), variableName[i]);
                            
                        else
                            alloc = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt1Ty(*TheContext), variableName[i]);
                    }
                    builder->CreateStore(val, alloc);
                    //std::cout << "sign up at varTable: " << variableName[i] << std::endl;
                }
                else if (type->GetType() == TypeName::Char)
                {
                    //std::cout << "new char" << std::endl;
                    Value *val = builder->getInt8('c');
                    if (isGlobal)
                        alloc = new llvm::GlobalVariable(*TheModule, Type::getInt8Ty(*TheContext), false, GlobalValue::ExternalLinkage, builder->getInt8('c'),variableName[i].c_str());
                    else
                    {
                        // 该变量是否是引用（&）
                        if (isRef)
                            alloc = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt8PtrTy(*TheContext), variableName[i]);
                        else
                            alloc = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), Type::getInt8Ty(*TheContext), variableName[i]);
                    }
                    builder->CreateStore(val, alloc);
                    //std::cout << "sign up at varTable: " << variableName[i] << std::endl;
                    
                }
            }
            else if (type->GetType() == TypeName::Record || type->GetType() == TypeName::Array)
            { 
                //如果是自定义类型（用ChangeNameTypeAST存）
                if (type->GetType() == TypeName::Record)
                { 
                    //如果是Record
                    //std::cout << "new Record：" << variableName[i] << std::endl;
                    auto structType = type->toLLVMType();
                    if(!isGlobal)
                        alloc = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), structType, variableName[i]);
                    else
                    {
                        auto global_struct = toLLVMConst(this->type);
                        
                        alloc = new llvm::GlobalVariable(*TheModule, structType, false, GlobalValue::ExternalLinkage, global_struct, variableName[i].c_str());
                    }
                }
                else if (type->GetType() == TypeName::Array)
                { 
                    //如果是Array
                    //std::cout << "new Array：========" << variableName[i] << std::endl;
                    if (isGlobal)
                    {
                        //生成一个LLVM ArrayType
                        auto array_type = this->type->toLLVMType(); 
                        //用这个数组的AST构造一个LLVM数组常量
                        Constant *const_array = toLLVMConst(this->type); 
                        alloc = new llvm::GlobalVariable(*TheModule, array_type, false, GlobalValue::ExternalLinkage, const_array, variableName[i].c_str());
                    }
                    else
                    {
                        auto array = (ArrayTypeAST *) type;
                        auto array_type = llvm::ArrayType::get( array -> type -> toLLVMType(), array->dimenSize[0]);
                        alloc = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), array_type, variableName[i]);
                    }
                }
            }
            //往当前块表中插入该标识符的Value*
            blockStack.top()->InsertNameValue(variableName[i], alloc);         
        }
    }
    return alloc;
}