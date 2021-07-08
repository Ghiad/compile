#ifndef PARSER_H
#define PARSER_H

#include <stack>
#include <set>
#include <iomanip>

#include "Lexer.h"
#include "BlockTable.h"

const int MAX_SIZE = 1000;

class Parser  {                          //语法分析类
public:  
	Lexer               lexer;        //调用词法分析入口
	int                 cur_count;        //当前函数处理到的字元计数
	std::string			name;        //程序名
	TypeAST*            ast_type;        //用于保存接收过程中当前处理的TypeAST的指针
	constValue*         c_value;        //用于保存接收过程中当前处理的constValue的指针
	std::stack<Node*>   n_stack;        //用于保存接收过程中构造的下层AST指针
	std::vector<std::string>        arg;   //保存程序的参数声明
	std::vector<ConstAST*>          cAst_vector;//保存CONST声明
	std::vector<ChangeNameTypeAST*> nameAst_vector;//保存TYPE声明
	std::vector<VariableDeclAST*>   varD_vector;//保存VAR声明
	std::vector<ExprAST*>           boDy;       //保存可执行语句
	std::vector<FunctionAST*>       fun_vector; //保存函数或过程调用声明
	FunctionAST*                    program;    //保存主函数的FunctionAST
public:
	Parser(std::string input);                            //构造函数，初始化

	//接收函数，接收对应非终结符并生成对应AST的函数
	bool COMPOUND_STATEMENT();
	bool STATEMENT_LIST(std::vector<ExprAST*>* exBody);
	bool STATEMENT();
	bool STATEMENT1(std::vector<ExprAST*>* exBody);
	bool ELSE_PART();
	bool CASE_BODY(std::map<ExprAST*, ExprAST*>* body);
	bool BRANCH_LIST(std::map<ExprAST*, ExprAST*>* body);
	bool BRANCH1(std::map<ExprAST*, ExprAST*>* body);
	bool BRANCH(std::map<ExprAST*, ExprAST*>* body);
	bool CONST_LIST(std::vector<ExprAST*>* body);
	bool CONST_LIST1(std::vector<ExprAST*>* body);
	bool FACTOR(); //分析非终结符 factor的函数
	bool VARIABLE();
	bool ID_VARPARTS();
	bool ID_VARPART();
	bool EXP1(std::vector<ExprAST*>*);
	bool EXPRESSION_LIST(std::vector<ExprAST*>*);
	bool EXPRESSION();
	bool TERM1();
	bool TERM();
	bool SEXP1();
	bool SIMPLE_EXPRESSION();
	bool PROCEDURE_CALL(bool*);
	bool UPDOWN(bool*);
	bool PROGRAMSTRUCE();
	bool PROGRAM_HEAD();
	bool PROGRAM_BODY();
	bool IDLIST(std::vector<std::string>*);
	bool IDLIST1(std::vector<std::string>*);
	bool CONST_DECLARATIONS(std::vector<ConstAST*>*,int);
	bool CONST_DECLARATION(std::vector<ConstAST*>*,int);
	bool CONST_VALUE();
	bool CONST1(std::vector<ConstAST*>*);
	bool TYPE_DECLARATIONS(std::vector<ChangeNameTypeAST*>*);
	bool VAR_DECLARATIONS(std::vector<VariableDeclAST*>*);
	bool VALUE_PARAMETER(std::vector<VariableDeclAST*>*,int);
	bool VAR_PARAMETER(std::vector<VariableDeclAST*>*);
	bool PARAMETER1(std::vector<VariableDeclAST*>*);
	bool PARAMETER(std::vector<VariableDeclAST*>*);
	bool PARAMETER_LIST(std::vector<VariableDeclAST*>*);
	bool FORMAL_PARAMETER(std::vector<VariableDeclAST*>*);
	bool SUBPROGRAM_BODY(std::vector<ConstAST*>*c_p, std::vector<ChangeNameTypeAST*>* s_p,
		std::vector<VariableDeclAST*>* v_p, std::vector<FunctionAST*>* f_p);
	bool SUBPROGRAM_HEAD(std::vector<VariableDeclAST*>*,std::string*,int*);
	bool SUBPROGRAM(std::vector<FunctionAST*>* f_p);
	bool SUBPROGRAM_DECLARATIONS(std::vector<FunctionAST*>* f_p);
	bool TYPE();
	bool TYPE_DECLARATION(std::vector<ChangeNameTypeAST*>*);
	bool TYPE_DECLARATION1(std::vector<ChangeNameTypeAST*>*);
	bool BASIC_TYPE();
	bool PERIOD1(std::vector<ConstAST*>*, std::vector<ConstAST*>*);
	bool PERIOD(std::vector<ConstAST*>*, std::vector<ConstAST*>*);
	bool RECORD_BODY(std::vector<VariableDeclAST*>*);

	bool VAR_DECLARATION(std::vector<VariableDeclAST*>*);
	bool VAR1(std::vector<VariableDeclAST*>*);


	//test
	void start();

	void out();//打印整体AST
	void outc(ConstAST*,int);
	void outt(ChangeNameTypeAST*,int);
	void outtab(int);
	void outv(VariableDeclAST*,int);
	void outf(FunctionAST*,int);
	void outb(ExprAST*);
	void out_arr(TypeAST*,int);
	void out_rec(TypeAST*p,int);
	std::string TYPE_S(TypeName n);
	std::string VALUE_S(constValue* v, TypeName n);
};

#endif