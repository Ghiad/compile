#ifndef _VARIABLEDECLAST_H
#define _VARIABLEDECLAST_H

#include "Node.h"
#include "TypeAST.h"

class VariableDeclAST : public Node {
public:
	int isRef;//是否是引用
	int isGlobal;
	TypeAST* type;
	std::vector<std::string> variableName;
	
	VariableDeclAST(TypeAST* t, std::vector<std::string> name, int _isRef, int _isGlobal) :isRef(_isRef), isGlobal(_isGlobal), type(t), variableName(name) {}
	VariableDeclAST(const VariableDeclAST& f) :isRef(f.isRef), isGlobal(f.isGlobal), type(f.type), variableName(f.variableName) {}
	Value* Codegen() override;


	NodeKind GetNodeKind() const final { return NodeKind::variableDecl; }
};

#endif