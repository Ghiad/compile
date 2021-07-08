#ifndef _VARIABLEEXPRAST_H
#define _VARIABLEEXPRAST_H

#include "ExprAST.h"

class VariableExprAST : public ExprAST {
public:
	std::string name;

	VariableExprAST(const std::string variableName, int line) :name(variableName) { lineNum = line; }
	VariableExprAST(const VariableExprAST& f) :name(f.name) { lineNum = f.lineNum; }
	Value* Codegen() override;
};

#endif