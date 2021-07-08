#include "CaseofEndExprAST.h"
#include "BlockTable.h"

Value *CaseofEndExprAST::Codegen()
    {
        auto *CondV = caseCond->Codegen();
        Value* Cond_value = nullptr;
        
        
        if (!CondV)
            exit(1);
        
        bool isVar = false;
        bool isPtr = false;
        blockStack.top()->GetExprType(caseCond, &isVar, &isPtr);
        
        if(isPtr)
            Cond_value = builder->CreateLoad(CondV,"conditonV");
        else
            Cond_value = CondV;
        
        
        Function *Thefunction = builder->GetInsertBlock()->getParent();
        
        BasicBlock *DestBB = BasicBlock::Create(*TheContext, "switch-part-end");
        std::vector<BasicBlock *> caseBB(body.size()); 
        SwitchInst *new_part = builder->CreateSwitch(Cond_value, DestBB, body.size());
        
        for(int i = 0 ; i < body.size(); i++){
		caseBB[i] = BasicBlock::Create(*TheContext, "switch-parts");
	}
        
        auto t = caseBB.begin();
        for (auto i : body)
        {
	        auto i_first_addr = i.first->Codegen();
            Value* i_first_value = nullptr;
            bool isVar = false;
            bool isPtr = false;
            blockStack.top()->GetExprType(i.first, &isVar, &isPtr);
            if(isPtr)
                i_first_value = builder->CreateLoad(i_first_addr, "conditonV");
            else
                i_first_value = i_first_addr;
            
            new_part->addCase(dyn_cast<ConstantInt>(i_first_value), *t);
	        Thefunction->getBasicBlockList().push_back(*t);
            t++;
        }
        auto f = caseBB.begin();
	for(auto i : body){
		    builder->SetInsertPoint(*f);
            if(i.second)
            	if (!i.second->Codegen())
            	{
                	std::cout << "case语句条件执行表达式翻译失败" << std::endl;
                	exit(1);
           	 }
            
            builder->CreateBr(DestBB);
	    f++;
	}
	    Thefunction->getBasicBlockList().push_back(DestBB);
        builder->SetInsertPoint(DestBB);
        return ConstantFP::get(Type::getDoubleTy(*TheContext), 2.0);
    }