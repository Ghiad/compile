#ifndef _BEGINENDEXPRAST_H
#define _BEGINENDEXPRAST_H

#include "ExprAST.h"

class BeginEndExprAST : public ExprAST {
public:
	std::vector<ExprAST*> body;

	BeginEndExprAST(std::vector<ExprAST*> bo, int line)
		:body(bo) {
		lineNum = line;
	}
	Value* Codegen() override;
	//void print(int n) override;

};

#endif