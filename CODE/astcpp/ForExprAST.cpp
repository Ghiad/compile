#include "ForExprAST.h"
#include "BlockTable.h"

Value *ForExprAST::Codegen()
    {
        //i的地址
        auto startAlloca = blockStack.top()->SearchNameValue(varName);

        bool isVar = false;
        bool isPtr = false;
        auto startPtr = start->Codegen();
        auto endPtr = end->Codegen();
        
        Value *startValue = nullptr;
        Value *endValue = nullptr;
        Value *stepValue = nullptr;
        Value *loopCond = nullptr;
        
        blockStack.top()->GetExprType(start, &isVar, &isPtr);
        
        //startValue
        if(isPtr)
            startValue = builder->CreateLoad(startPtr,"startvalue");
        else
            startValue = startPtr;
        
        //初始化迭代变量 i = 1
        builder->CreateStore(startValue, startAlloca);
        
        //endValue
        blockStack.top()->GetExprType(end, &isVar, &isPtr);
        if(isPtr)
            endValue = builder->CreateLoad(endPtr,"endvalue");
        else
            endValue = endPtr;
        
        
        //stepVal
        if (increaseDirection)
            stepValue = ConstantInt::get(*TheContext, APInt(32, 1, true));
        else
            stepValue = ConstantInt::get(*TheContext, APInt(32, -1, true));
        
        //没生成成功
        if (!startValue || !endValue || !stepValue)
            exit(1);
        
        Function *TheFunction = builder->GetInsertBlock()->getParent();
        
        BasicBlock *testBB = BasicBlock::Create(*TheContext, "test", TheFunction);
        BasicBlock *loopBB = BasicBlock::Create(*TheContext, "loop");
        BasicBlock *loopEndBB = BasicBlock::Create(*TheContext, "loopEnd");
        
        builder->CreateBr(testBB);
        builder->SetInsertPoint(testBB);
		startValue = builder->CreateLoad(startAlloca,"startvalue");
        
		
        if(increaseDirection){
			
            loopCond = builder->CreateICmpSLE(startValue, endValue, "loopcond");
		}
        else
            loopCond = builder->CreateICmpSGE(startValue, endValue, "loopcond");
        
        
        builder->CreateCondBr(loopCond,loopBB,loopEndBB);
        
        TheFunction->getBasicBlockList().push_back(loopBB);
        builder->SetInsertPoint(loopBB);
        
        if(body)
        {
            if (!body->Codegen())
            {
                std::cout << "the codegen of forexpr'body fails! " << std::endl;
                exit(1);
            }
        }
	    startValue = builder->CreateAdd(startValue, stepValue, "for_update");
	    builder->CreateStore(startValue, startAlloca);
	    builder->CreateBr(testBB);		
	    TheFunction->getBasicBlockList().push_back(loopEndBB);
        builder->SetInsertPoint(loopEndBB);
        return Constant::getNullValue(Type::getInt32Ty(*TheContext));

}