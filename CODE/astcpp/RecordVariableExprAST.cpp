#include "RecordVariableExprAST.h"
#include "BlockTable.h"

Value *RecordVariableExprAST::Codegen()
{
    int sub_index = blockStack.top()->GetMemberIndex(this);
    //如果找不到sub，报错就行，并且这里直接shut down
    if (sub_index == -1)
    {
        //std::cout << "Error! This member " << sub << " is not in the current struct!" << std::endl;
        exit(1);
    }
    //获取当前的结构体的基地址
    auto basePtr = nameExpr->Codegen();
    
    //通过基地址+sub_index 获取当前的元素的地址
    //std::cout << "---[] create record ref success[]-- "<< sub_index << std::endl;
    //TheModule->print(errs(), NULL);
    return GetElementPtrInst::CreateInBounds(basePtr, ArrayRef<llvm::Value *>{ConstantInt::get(*TheContext, APInt(32, 0)),ConstantInt::get(*TheContext, APInt(32, sub_index))},sub.c_str(), builder->GetInsertBlock());
}