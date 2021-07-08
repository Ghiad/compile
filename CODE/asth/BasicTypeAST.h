#ifndef _BASICTYPEAST_H
#define _BASICTYPEAST_H

#include "TypeAST.h"

extern BasicTypeAST* intTypeAST;
extern BasicTypeAST* charTypeAST;
extern BasicTypeAST* boolTypeAST;
extern BasicTypeAST* realTypeAST;
extern BasicTypeAST* idTypeAST;

class BasicTypeAST : public TypeAST {
public:
	TypeName type;

	BasicTypeAST(TypeName type) :type(type) {}
	BasicTypeAST(const BasicTypeAST& f) :type(f.type) {}
    
	TypeName GetType() override { return type; }
	bool CheckLegal() override final { return true; }
	llvm::Type *toLLVMType() override;
};

#endif