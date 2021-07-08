#ifndef _CALLPROCEDUREEXPRAST_H
#define _CALLPROCEDUREEXPRAST_H

#include "ExprAST.h"

class CallProcedureExprAST : public ExprAST {
public:
	std::string callee;
	std::vector<ExprAST*> args;

	CallProcedureExprAST(const std::string name, std::vector<ExprAST*> vec, int line)
		:callee(name), args(vec) {
		lineNum = line;
	}
	CallProcedureExprAST(const CallProcedureExprAST& f) : callee(f.callee), args(f.args) { lineNum = f.lineNum; }
	Value* Codegen() override;
	//void print(int n) override;

};

#endif