#ifndef _EXPRAST_H
#define _EXPRAST_H

#include "Node.h"

enum class ExprType {
	NumberExpr,
	RealExpr,
	BoolExpr,
	CharExpr,
	Variable,
	ArrayVariable,
	RecordVariable,
	UnaryExpr,
	BinaryExpr,
	CallFuncExpr,
	CallProcExpr,
	IfExpr,
	ForExpr,
	WhileExpr,
	CaseExpr,
	BeExpr,
	RepeatExpr
};

class ExprAST : public Node {
public:
	ExprType expr_type;
	ExprAST() {}
	virtual Value* Codegen() {};
	NodeKind GetNodeKind() const final { return NodeKind::Expr; }
};

#endif