#ifndef _CHAREXPRAST_H
#define _CHAREXPRAST_H

#include "ExprAST.h"

class CharExprAST : public ExprAST {
public:
	char val;
	CharExprAST(char value, int line) :val(value) { lineNum = line; }
	CharExprAST(const CharExprAST& f) :val(f.val) { lineNum = f.lineNum; }
	Value* Codegen() override
	{
		return ConstantInt::get(Type::getInt8Ty(*TheContext), (int)val, false);
	}
};

#endif