#include "RepeatExprAST.h"
#include "BlockTable.h"

Value *RepeatExprAST::Codegen()
    {

            //当前正在插入的函数
        Function *TheFunction = builder->GetInsertBlock()->getParent();
        
            //循环部分的block
        BasicBlock *loopBB = BasicBlock::Create(*TheContext, "loopStmt", TheFunction);
            //循环条件的block
        BasicBlock *condBB = BasicBlock::Create(*TheContext, "exitcond");
            //退出while之后的block
        BasicBlock *exitBB = BasicBlock::Create(*TheContext, "eixtStmt");
        
            // br sloop，转移到repeat循环中表达式
        builder->CreateBr(loopBB);
            // start insert into conditionBB eg. repeat...；设置了插入点
        builder->SetInsertPoint(loopBB);
        
            //插入body
            //每个表达式——一行，作为表达式AST
        for (int i = 0; i < body.size(); i++)
        {
            if (body[i] && !body[i]->Codegen())
            {
                std::cout << "the codegen of repeat_expr'body fails! " << std::endl;
                exit(1);
            }
        }
        
        //转移到条件BB
        builder->CreateBr(condBB);
        
        TheFunction->getBasicBlockList().push_back(condBB);
        //在condition部分开始插入
        builder->SetInsertPoint(condBB);
        
        
        //Value *CondV = untilCond->Codegen();
        auto *CondV = untilCond->Codegen();
        Value* Cond_value = nullptr;
        if (!CondV)
            exit(1);
        
        bool isVar = false;
        bool isPtr = false;
        blockStack.top()->GetExprType(untilCond, &isVar, &isPtr);
        
        if(isPtr)
            Cond_value = builder->CreateLoad(CondV,"conditonV");
        else
            Cond_value = CondV;
            //getExprType是一个来自符号表的函数
        
            //这里获得条件表达式的bool值
        Cond_value = builder->CreateICmpNE(Cond_value, ConstantInt::get(*TheContext, APInt(1, 0, false)), "untilcond");
            //创建分支
        builder->CreateCondBr(Cond_value, exitBB, loopBB);

        
            //推入exitBB
        TheFunction->getBasicBlockList().push_back(exitBB);
        builder->SetInsertPoint(exitBB);
        return ConstantFP::get(Type::getDoubleTy(*TheContext), 2.0);
    }