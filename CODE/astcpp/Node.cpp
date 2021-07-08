#include "Node.h"

static ExitOnError ExitOnErr;
static std::unique_ptr<LLVMContext> TheContext = std::make_unique<LLVMContext>();
static std::unique_ptr<Module> TheModule = std::make_unique<Module>("Pascal-S Compiler", *TheContext);
static std::unique_ptr<IRBuilder<>> builder = std::make_unique<IRBuilder<>>(*TheContext);
static std::unique_ptr<legacy::FunctionPassManager> TheFPM = std::make_unique<legacy::FunctionPassManager>(TheModule.get());

static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction,Type* ty, const std::string& VarName) 
{
	IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
	TheFunction->getEntryBlock().begin());
	return TmpB.CreateAlloca(ty, 0, VarName.c_str());
}