#ifndef _CONSTAST_H
#define _CONSTAST_H

#include "Node.h"
#include "BasicTypeAST.h"
#include "ConstValue.h"

class ConstAST : public Node {
public:
	std::string variableName;
	BasicTypeAST* type;
	constValue value;
	int isGlobal;

	ConstAST(std::string name, BasicTypeAST* ty, constValue val)
		:variableName(name), type(ty), value(val) {}
	ConstAST(const ConstAST& f) : variableName(f.variableName), type(f.type), value(f.value) {}
	~ConstAST() {}
	Value* Codegen() override;

	NodeKind GetNodeKind() const final { return NodeKind::constDecl; }
};

#endif