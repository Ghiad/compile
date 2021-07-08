#ifndef _CONSTVALUE_H
#define _CONSTVALUE_H

#include "string"

enum class BinaryOpKind {
	plusKind,	// plus
	minusKind,	// minus
	orKind,	// or
	mulKind,	// muliply
	divRealKind, // 运算/，新加的
	divKind,	// div
	modKind,	// mod
	andKind,	// and
	geKind,	// greater equal
	gtKind,	// greater than
	leKind,	// less equal
	ltKind,	// less than
	eqKind,	// equal
	ueqKind,	// unequal
	assignmentKind	// assignment
};

enum class UnaryOpKind {
	notKind,	// not
	negKind	// negative
};

class constValue {
public:
	int i;
	double d;
	char c;
	std::string s;
	bool b;
	constValue(int value) :i(value) { d = 0; c = 0; b = false; }
	constValue(double value) :d(value) { i = 0; c = 0; b = false; }
	constValue(char value) :c(value) { i = 0; d = 0; b = false; }
	constValue(bool value) : b(value) { i = 0; d = 0; c = 0; }

	constValue(const constValue& f) :i(f.i), d(f.d), c(f.c), b(f.b) {}
	void operator=(const constValue& copy)
	{
		i = copy.i;
		d = copy.d;
		c = copy.c;
		b = copy.b;
		s = "";
	}

	void NegateValue();//取负操作
	bool Compare(constValue checkValue) const;//比较两个constValue是否相同
	void IntBinOp(int val1, int val2, BinaryOpKind op);
	void CharBinOp(char val1, char val2, BinaryOpKind op);
	void RealBinOp(double val1, double val2, BinaryOpKind op);
	void BoolBinOp(bool val1, bool val2, BinaryOpKind op);
};

#endif