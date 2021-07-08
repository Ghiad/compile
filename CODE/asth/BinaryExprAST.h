#ifndef _BINARYEXPRAST_H
#define _BINARYEXPRAST_H

#include "ExprAST.h"
#include "TypeAST.h"
#include "ConstAST.h"

class BinaryExprAST : public ExprAST {
public:
	BinaryOpKind op;
	ExprAST* LExpr, * RExpr;

	TypeAST* type;//最后表达式代表的类型
	bool hasVar;//是否存在变量
	constValue* constVal;//只在hasVar==false时使用

	BinaryExprAST(BinaryOpKind binaryOp, ExprAST* left, ExprAST* right, int line)
		:op(binaryOp), LExpr(left), RExpr(right)
	{
		type = nullptr;
		hasVar = false;
		constVal = nullptr;
		lineNum = line;
	}
	BinaryExprAST(const BinaryExprAST& f)
		:op(f.op), LExpr(f.LExpr), RExpr(f.RExpr)
	{
		lineNum = f.lineNum;
		type = f.type;
		hasVar = f.hasVar;
		constVal = f.constVal;
	}
	Value* Codegen() override;

};

#endif