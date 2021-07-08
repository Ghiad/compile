#ifndef _REPEATEXPRAST_H
#define _REPEATEXPRAST_H

#include "ExprAST.h"

class RepeatExprAST : public ExprAST {
public:
	ExprAST* untilCond;
	std::vector<ExprAST*> body;

	RepeatExprAST(ExprAST* cond, std::vector<ExprAST*> bo, int line)
		:untilCond(cond), body(bo) {
		lineNum = line;
	}
	Value* Codegen() override;
	//void print(int n) override;

};

#endif