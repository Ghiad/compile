#include "ChangeNameTypeAST.h"
#include "BlockTable.h"

Value *ChangeNameTypeAST::Codegen()
{
        //该类型声明不会被使用，因此不做codegen，只加入符号表
    if(!blockStack.top()->Insert(newName, this, {0, 0}, originalType))
        //std::cout << "rename fails!" << std::endl;
    
    return nullptr;
    
}