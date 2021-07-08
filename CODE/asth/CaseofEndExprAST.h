#ifndef _CASEOFENDEXPRAST_H
#define _CASEOFENDEXPRAST_H

#include "ExprAST.h"

class CaseofEndExprAST : public ExprAST {
public:
	ExprAST* caseCond;
	std::map<ExprAST*, ExprAST*> body;

	CaseofEndExprAST(ExprAST* cond, std::map<ExprAST*, ExprAST*> bo, int line)
		:caseCond(cond), body(bo) {
		lineNum = line;
	}
	Value* Codegen() override;

};

#endif