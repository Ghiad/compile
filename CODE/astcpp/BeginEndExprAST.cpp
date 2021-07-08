#include "BeginEndExprAST.h"
#include "BlockTable.h"

Value *BeginEndExprAST::Codegen()
{
    for (auto items : body)
    {
        if(items)
            items->Codegen();
    }
	return ConstantFP::get(Type::getDoubleTy(*TheContext), 2.0);
}
                                             
Value *IfExprAST::Codegen()
    {
		auto *CondV = ifCond->Codegen();
        Value* Cond_value = nullptr;
        
        
		if (!CondV)
			exit(1);
        
        bool isVar = false;
        bool isPtr = false;
        blockStack.top()->GetExprType(ifCond, &isVar, &isPtr);
        
        if(isPtr)
            Cond_value = builder->CreateLoad(CondV,"conditonV");
        else
            Cond_value = CondV;
        
        
		Cond_value = builder->CreateICmpNE(
			Cond_value, ConstantInt::get(*TheContext, APInt(1, 0, false)), "ifcond");

		Function *TheFunction = builder->GetInsertBlock()->getParent();

		BasicBlock *ThenBB = BasicBlock::Create(*TheContext, "then", TheFunction);
		BasicBlock *ElseBB = BasicBlock::Create(*TheContext, "else");
		BasicBlock *MergeBB = BasicBlock::Create(*TheContext, "ifcont");

		builder->CreateCondBr(Cond_value, ThenBB, ElseBB);

		builder->SetInsertPoint(ThenBB);
        
		if(thenComponent){
			Value *ThenV = thenComponent->Codegen();
			if (!ThenV)
				exit(1);

			builder->CreateBr(MergeBB);

			ThenBB = builder->GetInsertBlock();
		}
        else
        {
            builder->CreateBr(MergeBB);
            ThenBB = builder->GetInsertBlock();
        }
		
        
		if(elseComponent){
			TheFunction->getBasicBlockList().push_back(ElseBB);
            
			builder->SetInsertPoint(ElseBB);
            
			Value *ElseV = elseComponent->Codegen();
			if (!ElseV)
				exit(1);

			builder->CreateBr(MergeBB);
			ElseBB = builder->GetInsertBlock();
		}
        else{
            TheFunction->getBasicBlockList().push_back(ElseBB);
            builder->SetInsertPoint(ElseBB);
            
            builder->CreateBr(MergeBB);
        }
        
        TheFunction->getBasicBlockList().push_back(MergeBB);
        builder->SetInsertPoint(MergeBB);
		return ConstantFP::get(Type::getDoubleTy(*TheContext), 2.0);
}