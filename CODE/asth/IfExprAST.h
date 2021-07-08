#ifndef _IFEXPRAST_H
#define _IFEXPRAST_H

#include "ExprAST.h"

class IfExprAST : public ExprAST {
public:
	ExprAST* ifCond;
	ExprAST* thenComponent, *elseComponent;

	IfExprAST(ExprAST* cond, ExprAST* thenCo, ExprAST* elseCo, int line)
		:ifCond(cond), thenComponent(thenCo), elseComponent(elseCo) {
		lineNum = line;
	}
	Value* Codegen() override;
};

#endif