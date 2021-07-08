#ifndef _REALEXPRAST_H
#define _REALEXPRAST_H

#include "ExprAST.h"

class RealExprAST : public ExprAST {
public:
	double val;

	RealExprAST(double value, int line) :val(value) { lineNum = line; }
	RealExprAST(const RealExprAST& f) :val(f.val) { lineNum = f.lineNum; }
	Value* Codegen() override
    {
        return ConstantFP::get(Type::getDoubleTy(*TheContext), val);
    }
	
    
    //void print(int n) override;

};

#endif