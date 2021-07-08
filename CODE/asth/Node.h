#ifndef _NODE_H
#define _NODE_H

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <stack>
#include <map>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <memory>

using namespace llvm;

extern ExitOnError ExitOnErr;
extern std::unique_ptr<LLVMContext> TheContext;
extern std::unique_ptr<Module> TheModule;
extern std::unique_ptr<IRBuilder<>> builder;
extern std::unique_ptr<legacy::FunctionPassManager> TheFPM;

enum class NodeKind
{
	variableDecl,
	functionDecl,
	constDecl,
	typeDecl,
	Expr
};


typedef struct
{
	int row;
	int col;
}Loc;

class Node {
public:
	int lineNum;
	Node() {}
	virtual ~Node() {}
	virtual Value* Codegen() {};
	virtual NodeKind GetNodeKind() const = 0;
};

static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction,Type* ty, const std::string& VarName);

#endif