#ifndef _UNARYEXPRAST_H
#define _UNARYEXPRAST_H

#include "ExprAST.h"
#include "TypeAST.h"
#include "ConstAST.h"

class UnaryExprAST : public ExprAST {
public:
	UnaryOpKind op;
	ExprAST* expr;

	TypeAST* type;//表达式最后代表的类型，如-3.3代表realTypeAST
	bool hasVar;//是否存在变量,如-a,代表有变量
	constValue* constVal;//只在hasVar==false时使用

	UnaryExprAST(UnaryOpKind o, ExprAST* e, int line)
		:op(o), expr(e)
	{
		lineNum = line;
		type = nullptr;
		hasVar = false;
		constVal = nullptr;
	}
	UnaryExprAST(const UnaryExprAST& f)
		:op(f.op), expr(f.expr)
	{
		lineNum = f.lineNum;
		type = f.type;
		hasVar = f.hasVar;
		constVal = f.constVal;
	}
	Value* Codegen() override;

};

#endif