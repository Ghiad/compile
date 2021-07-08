#ifndef _RECORDVARIABLEEXPRAST_H
#define _RECORDVARIABLEEXPRAST_H

#include "ExprAST.h"
#include "TypeAST.h"

//记录变量表达式，如"a.p"
class RecordVariableExprAST : public ExprAST {
public:
	ExprAST* nameExpr;
	std::string sub;

	TypeAST* recordExprType;//a.p是什么类型

	RecordVariableExprAST(ExprAST* variableName, std::string s, int line)
		:nameExpr(variableName), sub(s) {
		lineNum = line;
		recordExprType = nullptr;
	}
	Value* Codegen() override;

};

#endif