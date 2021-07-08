#ifndef _TYPEAST_H
#define _TYPEAST_H

#include "Node.h"

enum class TypeName {
	Integer,
	Real,
	Char,
	Bool,
	// 语法分析新增属性
	Record,
	Array,
    Id,
	changeType
};

class TypeAST : public Node {
public:
	TypeAST() {}
//	virtual void print(int i) {};

	virtual TypeName GetType() = 0;
	virtual bool CheckLegal() = 0;
    virtual llvm::Type *toLLVMType(){}
	NodeKind GetNodeKind() const final { return NodeKind::typeDecl; }
};

Constant *toLLVMConst(TypeAST *ast);

#endif