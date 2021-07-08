#ifndef _FUNCTIONAST_H
#define _FUNCTIONAST_H

#include "Node.h"
#include "BasicTypeAST.h"
#include "ExprAST.h"
#include "ConstAST.h"
#include "VariableDeclAST.h"
#include "ChangeNameTypeAST.h"

enum class BlocksType
{
	function,
	procedure
};

class FunctionAST : public Node {
public:
	std::string name; //函数/过程名
	std::vector<VariableDeclAST*> headerDecl;	//参数
	BasicTypeAST* returnType;					//返回类型	
	std::vector<ConstAST*> consts;					//const_declarations
	std::vector<ChangeNameTypeAST*> selfdefineType;	//type_declarations
	std::vector<VariableDeclAST*> bodyDecl;			//var_declarations
	std::vector<FunctionAST*> functions;			//subporgram_declarations: (嵌套的func/proc)
	std::vector<ExprAST*> body;						//begin..end 里的内容

	Loc declLoc;

	BlocksType type;//区分是procedure还是function
	bool isFunction() { return type == BlocksType::function; }
	bool isProcedure() { return type == BlocksType::procedure; }

	FunctionAST(std::string name, std::vector<VariableDeclAST*>args, BasicTypeAST* returnType,
		std::vector<ConstAST*> con, std::vector<ChangeNameTypeAST*> de,
		std::vector<VariableDeclAST*> funcDecl, std::vector<FunctionAST*> subFun,
		std::vector<ExprAST*> funcBody, BlocksType t)
		:name(name), headerDecl(args), returnType(returnType),
		consts(con), selfdefineType(de), bodyDecl(funcDecl), functions(subFun),
		body(funcBody), type(t) {}
	FunctionAST(const FunctionAST& f)
		:name(f.name), headerDecl(f.headerDecl), returnType(f.returnType), consts(f.consts),
		selfdefineType(f.selfdefineType), bodyDecl(f.bodyDecl), functions(f.functions), body(f.body), type(f.type) {}
    
    Value* Codegen() override;

	NodeKind GetNodeKind() const final { return NodeKind::functionDecl; }
};

#endif