#include "ArrayVariableExprAST.h"
#include "BlockTable.h"

Value *ArrayVariableExprAST::Codegen()
{
    //每个维度的偏移量 定义[2..5]——>a[4]   offset: 2
    std::vector<int> offset;   
    //索引类型
    TypeName indexType; 
    
    bool hasVar = false;
    bool hasPtr = false;
    //表达式是否存在变量，此函数中无意义
    
    ArrayTypeAST *arr = (ArrayTypeAST *)(blockStack.top()->GetExprType(nameExpr, &hasVar, &hasPtr)); //从符号表中获取到该数组的原始定义AST
    
    int constIntValue = 0;                                                                        //暂存从Value *取得的整型数值
     
    for (int j = 0; j < index.size(); j++)
    {
        Value* val = nullptr;
// d[d[5]]
	indexType = (blockStack.top()->GetExprType(index[j], &hasVar,&hasPtr))->GetType(); //获取索引的类型
        
        //获取索引的类型
        bool isVal = false;
        bool isPtr = false;
        auto indexExpr = index[j]->Codegen();
        
	//indexType = (blockStack.top()->GetExprType(indexExpr, &hasVar,&hasPtr))->GetType(); //获取索引的类型
        blockStack.top()->GetExprType(index[j], &isVal, &isPtr);
        
        if(isPtr){
	    //std::cout<<"aaaaaaaaaaaaaaaaa"<<std::endl;
            val = builder->CreateLoad(indexExpr,"indexExpr");
	}
        else
            val = indexExpr;
        
            
        //计算索引表达式
        switch (indexType)
        {
            case TypeName::Integer:
                if (ConstantInt *CI = dyn_cast<ConstantInt>(val))
                {
                    if (CI->getBitWidth() <= 32)
                   {
		//std::cout<<"aaaaaaaaaaaaaaaaa!!:"<<std::endl;
                        constIntValue = CI->getSExtValue();
                    }
                }
                //std::cout<<"aaaaaaaaaaaaaaaaa!!:"<<constIntValue<<std::endl;
                offset.push_back(abs(constIntValue - ((arr->startIndexs[j]->value).i))); //计算该维度的偏移量，用int类型存储
                break;
            case TypeName::Char:
                if (ConstantInt *CI = dyn_cast<ConstantInt>(val))
                {
                    if (CI->getBitWidth() <= 8)
                    {
                        constIntValue = CI->getSExtValue();
                    }
                }
                offset.push_back(abs(constIntValue - (int)((arr->startIndexs[j]->value).c))); //计算该维度的偏移量，用int类型存储
                break;
            case TypeName::Bool:
                if (ConstantInt *CI = dyn_cast<ConstantInt>(val))
                {
                    if (CI->getBitWidth() <= 1)
                    {
                        constIntValue = CI->getSExtValue();
                    }
                }
                offset.push_back(abs(constIntValue - (int)((arr->startIndexs[j]->value).b))); //计算该维度的偏移量，用int类型存储
            default:
	{
                break;
	}
        }
    }
    
    
    auto idxList = std::vector<llvm::Value *>();
    Value *ArrayPtr = nameExpr->Codegen();
    idxList.push_back(llvm::ConstantInt::get( llvm::Type::getInt32Ty(*TheContext), APInt(32, 0) ) ); //第一个元素是0
    
    //每个维度偏移量插入vector
    for (int i = 0; i < offset.size(); i++)
    {
        idxList.push_back(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*TheContext),APInt(32,  offset[i])));
        //std::cout << "offset " << i << ":" << offset[i] << std::endl;
    }
	
    //std::cout << "---create array ref success --[]--" << std::endl;
	
    return GetElementPtrInst::CreateInBounds(ArrayPtr, idxList, "tempname", builder->GetInsertBlock());

}