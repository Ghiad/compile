#ifndef _FOREXPRAST_H
#define _FOREXPRAST_H

#include "ExprAST.h"

class ForExprAST : public ExprAST {
public:
	std::string varName;
	ExprAST* start, * end;
	ExprAST* body;
	bool increaseDirection;

	ForExprAST(const std::string name, ExprAST* st, ExprAST* en, ExprAST* bo, bool inc, int line)
		:varName(name), start(st), end(en), body(bo), increaseDirection(inc) {
		lineNum = line;
	}
	Value* Codegen() override;
	//void print(int n) override;

};

#endif