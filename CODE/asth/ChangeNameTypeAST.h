#ifndef _CHANGENAMETYPEAST_H
#define _CHANGENAMETYPEAST_H

#include "SelfdefineTypeAST.h"
#include "TypeAST.h"

class ChangeNameTypeAST : public SelfdefineTypeAST {
public:
	std::string newName;
	TypeAST* originalType;

	ChangeNameTypeAST(std::string name, TypeAST* old) :newName(name), originalType(old) {}
	Value* Codegen() override;

	TypeName GetType() override { return TypeName::changeType; }
	bool CheckLegal() override { return originalType->CheckLegal(); }
};

#endif