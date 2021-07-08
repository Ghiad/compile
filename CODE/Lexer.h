#ifndef _LEXER_H
#define _LEXER_H

#include <iostream>
#include <fstream>
#include <exception>
#include <map>
#include <vector>

#include "BlockTable.h"


#define idmax 8						//标识符最大长度8
const int BUF_SIZE = 1024;			//半个缓冲区长度
const int TOTAL_SIZE = 2 * BUF_SIZE;//缓冲区完成长度

enum  Token_type {                  //记号种类
	programstruce = 1, program_head, program_body, const_declarations,
	type_declarations, var_declarations, subprogram_declarations, compound_statement,
	idlist, idlist1, const_declaration, const_value, const1, type_declaration,
	type_declaration1, var_declaration, var1, type, basic_type, period, period1, record_body,
	subprogram, subprogram_head, subprogram_body, formal_parameter,
	paramerter_list, paramerter1, var_paramerter, value_paramerter,
	statement_list, statement, statement1, variable, expression,
	procedure_call, else_part, case_body, id_varparts, branch_list, branch,
	branch1, const_list, const_list1, updowm,
	expression_list, variable1, id_varpart, exp1, simple_exp, term, sexp1,
	factor, term1,//非终结符

	//终结符
	tok_eof, tok_def, tok_delimiter,
	tok_identifier, tok_number, tok_mulop,
	tok_if, tok_case, tok_while, tok_repeat, tok_for,
	tok_begin, tok_array, tok_boolean, tok_real,
	tok_integer, tok_record, tok_assignop/*赋值符号token*/, tok_procedure,
	tok_relop, tok_addop, tok_program, tok_keys, tok_const, tok_function,
	tok_var, tok_end, tok_type, tok_not,

	//增添
	tok_letter, tok_char, tok_boolc
};


typedef struct {        //保存已经读取过的记号的结构体，方便回溯时获取之前的记号
	Token_type type;
	std::string body;
	Loc loc;
}sToken;

class Lexer {           //词法分析主要类
private:
	int f_p;			//向前指针
	Loc loc;            //当前位置
	int c_col;          //当前字符所在列
	int pre_col;		//暂存之前的列数
	int not_num;

	int t_count;        //已经读取的token计数

	bool empty;         //表明当前TOKEN是否完成过读取
	bool loaded;        //表明下一个 半缓冲区是否已加载过 
						//(防止因为retract使得某个半区未被处理被覆盖)

	char last_C;        //保存上一个处理的字符
	char C;             //当前读取字符
	char buffer[TOTAL_SIZE];  //缓冲区

	Token_type iskey(std::string id);  //检测当前获取的字符串是否是关键词
	bool letter(char s);          //检测字符 s 是否是字母
	bool digit(char s);           //检测字符 s 是否是数字
	void get_char(int);           //从缓冲区获取下一个字符 并完成相关属性如行号变更
	void get_nbc(int);            //获取下一个非空白的字符
	void retract();               //回退一个字符
	void getnextLine();           //跳过当前行剩余部分 用于过滤 // 型注释

	std::string     body;              //字元内容
	Token_type type;              //字元种类

	std::ifstream f_in;           //输入文件
	std::ofstream f_out;          //报错文件

	std::map<std::string, Token_type> keys;//关键字表
	std::vector<sToken>t_flow;        //记号流，方便递归

public:
	bool result;                    //是否遇到非法字符

	Lexer();                        //构造函数
	void close();                   //关闭输入文件
	void file_initialize(std::string s); //初始化输入文件路径
	void initKeys();                //初始化关键字表
	void loadBuffer();              //加载下一个 半缓冲区
	void getTOKEN(int);             //获取参数指定的的记号
	int get_row();                  //获取当前行号
	int get_col();                  //获取当前记号所在列号
	int get_c_col();                //获取当前字符所在列号
	bool get_empty();               //返回 empty 当前值
	std::string get_body();              //返回当前记号的std::string形态
	Token_type get_type();          //返回当前记号的种类
};


#endif