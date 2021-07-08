#ifndef _RECORDTYPEAST_H
#define _RECORDTYPEAST_H

#include "SelfdefineTypeAST.h"
#include "ConstAST.h"
#include "VariableDeclAST.h"

class RecordTypeAST : public SelfdefineTypeAST {
public:
	std::vector<VariableDeclAST*> declList;

	bool isChecked;
	bool checkResult;
	RecordTypeAST(std::vector<VariableDeclAST*> v) :declList(v) {isChecked = false; checkResult=false;}
	Value* Codegen() override
	{
		return NULL;
	}

	TypeName GetType() override { return TypeName::Record; }
	VariableDeclAST* SearchVar(std::string name) const;
	bool CheckLegal() override;
	Type *toLLVMType() override
	{
		llvm::StructType* structType = llvm::StructType::create(*TheContext, "newStructure");
            //内部成员type的vector
        std::vector<llvm::Type*> elements;
        
            //逐个加入
        for(size_t i = 0; i<declList.size(); ++i)
                //struct和array待定
            elements.push_back( declList[i]->type->toLLVMType() );
        //插入所有的身体
        structType->setBody(elements);
        //返回创建的struct实体
        return structType;
	}
};

#endif