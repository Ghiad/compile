#ifndef _ARRAYVARIABLEEXPRAST_H
#define _ARRAYVARIABLEEXPRAST_H

#include "ExprAST.h"
#include "TypeAST.h"

class ArrayVariableExprAST : public ExprAST {
public:
	ExprAST* nameExpr;
	std::vector<ExprAST*> index;

	TypeAST* arrayExprType;//id[id]是什么类型

	ArrayVariableExprAST(ExprAST* variableName, std::vector<ExprAST*> i, int line)
		:nameExpr(variableName), index(i) {
		lineNum = line;
		arrayExprType = nullptr;
	}

	Value* Codegen() override;
};

#endif