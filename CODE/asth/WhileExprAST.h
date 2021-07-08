#ifndef _WHILEEXPRAST_H
#define _WHILEEXPRAST_H

#include "ExprAST.h"

class WhileExprAST : public ExprAST {
public:
	ExprAST* whileCond;
	ExprAST* body;

	WhileExprAST(ExprAST* cond, ExprAST* bo, int line)
		:whileCond(cond), body(bo) {
		lineNum = line;
	}
	Value* Codegen() override;
	//void print(int n) override;

};

#endif