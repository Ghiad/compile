#ifndef _NUMBEREXPRAST_H
#define _NUMBEREXPRAST_H

#include "ExprAST.h"

class NumberExprAST : public ExprAST {
public:
	int val;
	NumberExprAST(int value, int line) :val(value) { lineNum = line; }
	NumberExprAST(const NumberExprAST& f) :val(f.val) { lineNum = f.lineNum; }
	Value* Codegen() override
	{
		return ConstantInt::get(Type::getInt32Ty(*TheContext), val, false);
	}
};

#endif