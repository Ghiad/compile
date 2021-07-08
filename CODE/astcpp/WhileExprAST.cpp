#include "WhileExprAST.h"
#include "BlockTable.h"

Value *WhileExprAST::Codegen()
    {
        
        //当前正在插入的函数
        Function *TheFunction = builder->GetInsertBlock()->getParent();
        //当前正在插入代码的基本块
        BasicBlock *PreheaderBB = builder->GetInsertBlock();
        
        //循环条件的block
        BasicBlock *startBB = BasicBlock::Create(*TheContext, "startloop", TheFunction);
        //循环部分的block
        BasicBlock *loopBB = BasicBlock::Create(*TheContext, "loopStmt");
        //退出while之后的block
        BasicBlock *exitBB = BasicBlock::Create(*TheContext, "eixtStmt");
        
        // br sloop，转移到while中表达式
        builder->CreateBr(startBB);
        // start insert into conditionBB eg. while(i<=9)；设置了插入点
        builder->SetInsertPoint(startBB);
        
        auto *CondV = whileCond->Codegen();
        Value* Cond_value = nullptr;
        if (!CondV)
            exit(1);
        
        bool isVar = false;
        bool isPtr = false;
        blockStack.top()->GetExprType(whileCond, &isVar, &isPtr);
        
        if(isPtr)
            Cond_value = builder->CreateLoad(CondV,"conditonV");
        else
            Cond_value = CondV;
        
		Cond_value = builder->CreateICmpNE(Cond_value, ConstantInt::get(*TheContext, APInt(1, 0, false)), "whilecond");
        //创建分支
        builder->CreateCondBr(Cond_value, loopBB, exitBB);

        
        //推入loopBB部分
        TheFunction->getBasicBlockList().push_back(loopBB);
        //在loop部分开始插入
        builder->SetInsertPoint(loopBB);
        
        //这里要判body是否为空
        if(body)
            if (!body->Codegen())
            {
                std::cout << "the codegen of whileexpr'body fails! " << std::endl;
                exit(1);
            }
        
        //设置分支到条件判断
        builder->CreateBr(startBB);
        
        //推入exitBB
        TheFunction->getBasicBlockList().push_back(exitBB);
        builder->SetInsertPoint(exitBB);
        return ConstantFP::get(Type::getDoubleTy(*TheContext), 2.0);
    }