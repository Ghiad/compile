#ifndef _ARRAYTYPEAST_H
#define _ARRAYTYPEAST_H

#include "SelfdefineTypeAST.h"
#include "ConstAST.h"
#include "ExprAST.h"

class ArrayTypeAST : public SelfdefineTypeAST {
public:
	TypeAST* type;
	//ConstAST* startIndex, * endIndex;
	std::vector<ConstAST*> startIndexs;
	std::vector<ConstAST*> endIndexs;

	bool isChecked;
	bool checkResult;
	std::vector<int> dimenSize;//各个维度的跨度

	ArrayTypeAST(TypeAST* ty, std::vector<ConstAST*> s, std::vector<ConstAST*> e) :type(ty), startIndexs(s), endIndexs(e) {isChecked = false; checkResult = false;}
	ArrayTypeAST() { startIndexs.clear(); endIndexs.clear(); type = nullptr; isChecked = false;checkResult = false;}

	bool CheckIndex(std::vector<ExprAST*> indexes, int line) const;//检查下标是否合法
	bool Compare(ArrayTypeAST* checkType) const;//比较某个ArrayTypeAST与本AST是否相同
	bool CheckLegal() override;//判断内容是否合法
	TypeName GetType() override { return TypeName::Array; }
	llvm::Type *toLLVMType() override
	{
		auto innerType = llvm::ArrayType::get(type->toLLVMType(), dimenSize[dimenSize.size()-1]);
		//std::cout<<"---deal with dimen:"<<dimenSize.size()-1<<std::endl;
		for(int i=dimenSize.size()-2;i>=0;i--){
			//std::cout<<"---deal with dimen:"<<i<<std::endl;
			innerType = llvm::ArrayType::get(innerType, dimenSize[i]);
		}
		return innerType;
	}
};

#endif