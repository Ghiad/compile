#ifndef _CALLFUNCTIONEXPRAST_H
#define _CALLFUNCTIONEXPRAST_H

#include "ExprAST.h"

class CallFunctionExprAST : public ExprAST {
public:
	std::string callee;
	std::vector<ExprAST*> args;
	bool isSystemCall;

	CallFunctionExprAST(const std::string name, std::vector<ExprAST*> vec, bool is, int line)
		:callee(name), args(vec), isSystemCall(is) {
		lineNum = line;
	}
	CallFunctionExprAST(const CallFunctionExprAST& f) : callee(f.callee), args(f.args), isSystemCall(f.isSystemCall) { lineNum = f.lineNum; }
	Value* Codegen() override;
	//void print(int n) override;

};

#endif