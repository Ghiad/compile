#ifndef _BOOLAST_H
#define _BOOLAST_H

#include "ExprAST.h"

class BoolExprAST : public ExprAST {
public:
	bool val;

	BoolExprAST(bool value, int line) :val(value) { lineNum = line; }
	BoolExprAST(const BoolExprAST& f) :val(f.val) { lineNum = f.lineNum; }
	Value* Codegen() override
	{
		return ConstantInt::get(Type::getInt1Ty(*TheContext), (int)val, false);
	}

};

#endif