#ifndef _BLOCKTABLE_H
#define _BLOCKTABLE_H

#include "ArrayTypeAST.h"
#include "ArrayVariableExprAST.h"
#include "BasicTypeAST.h"
#include "BeginEndExprAST.h"
#include "BinaryExprAST.h"
#include "BoolAST.h"
#include "CallFunctionExprAST.h"
#include "CallProcedureExprAST.h"
#include "CaseofEndExprAST.h"
#include "ChangeNameTypeAST.h"
#include "CharExprAST.h"
#include "ConstAST.h"
#include "ConstValue.h"
#include "ExprAST.h"
#include "ForExprAST.h"
#include "FunctionAST.h"
#include "IfExprAST.h"
#include "Node.h"
#include "NumberExprAST.h"
#include "RealExprAST.h"
#include "RecordTypeAST.h"
#include "RecordVariableExprAST.h"
#include "RepeatExprAST.h"
#include "SelfdefineTypeAST.h"
#include "TypeAST.h"
#include "UnaryExprAST.h"
#include "VariableDeclAST.h"
#include "VariableExprAST.h"
#include "WhileExprAST.h"

class Member
{
public:
    std::vector<std::string> memberIndex;
    std::vector<Member*> memberType;//array、record有

    Member(RecordTypeAST*);
    Member(ArrayTypeAST*);
};

//按块建立符号表，可以用来确定作用域，一个块只能访问本层及上层的符号
//只通过program/procedure/function创建
class BlockTable
{
    std::string name;//块名，program/procedure/function后面跟的名字
    BlockTable* fatherBlock; // 该块嵌套在哪个块里，最底层的fatherBlock设置为nullptr

    //标识符列表
    std::map<std::string, Node*> symTable;//Node的种类四种，var/fun/const/type
    std::map<std::string, Loc> firstAppear;//它第一次出现的位置（在发出一个有关该标 识符的错误消息时需要使用这个信息
    std::map<std::string, TypeAST*> symTypeTable;//func存的是retType
    std::map<std::string, Member*> memberIndex;
    std::map<std::string, Value*> nameValues;


    //
    Node* SearchThis(std::string name);//只查找本块表内的table
    Value* SearchThisValue(std::string symName);//只查找本块表内的nameValues
    Member* SearchThisMember(std::string name);
    Member* SearchAllMember(std::string name);
    constValue GetConstValue(ExprAST* expr); //获得表达式的常量值
    TypeAST* GetSymType(std::string);        //获得某个标识符的类型树
    FunctionAST* GetFunctionAST(std::string funcName);//获得某个函数的FunctionAST指针


    //检查各类ExprAST的合法性
    bool CheckExpression(ExprAST* expr, bool isAssignLeft = false);
    bool CheckVarExpr(VariableExprAST* expr, bool isAssignLeft);
    bool CheckArrayExpr(ArrayVariableExprAST* expr);
    bool CheckRecordExpr(RecordVariableExprAST* expr);
    bool CheckUnaryExpr(UnaryExprAST* expr);
    bool CheckBinExpr(BinaryExprAST* binExpr);

    bool CheckCallFuncExpr(CallFunctionExprAST* expr);
    bool CheckCallProcExpr(CallProcedureExprAST* expr);
    bool CheckIfExpr(IfExprAST* ifExpr);
    bool CheckForExpr(ForExprAST* forExpr);
    bool CheckWhileExpr(WhileExprAST* whileExpr);
    bool CheckRepeatExpr(RepeatExprAST* repeatExpr);
    bool CheckCaseExpr(CaseofEndExprAST* caseExpr);
    bool CheckBeginEndExpr(BeginEndExprAST* beExpr);

public:
    BlockTable(BlockTable* fatherBlock, std::string name);//创建新块表
    ~BlockTable();//释放一个块表

    //增删查
    bool Insert(std::string symName, Node* symNode, Loc firstLoc, TypeAST* type); //插入成功返回true，插入失败（即之前声明过该变量）返回false
    bool InsertNameValue(std::string symName, Value* value);//插入nameValue
    bool Delete(std::string symName);   //删除某个标识符
    bool DeleteNameValue(std::string symName);
    Value* SearchNameValue(std::string symName);//获得某个Value指针，查不到返回nullptr
    Node* SearchAll(std::string symName);//搜索本块表及父块表中是否存在某个标识符

    bool CompareType(TypeAST* leftType, TypeAST* rightType, bool isAssign)const;//比较两个表达式的类型是否可以一起运算
    bool HandleConst(ConstAST* constAst);

    //Get
    int GetMemberIndex(ExprAST* searchExpr);
    TypeAST* GetExprType(ExprAST* expr, bool* isVar, bool* isPtr);//获得某个ExprAST最终代表的type
    std::string GetName() const { return name; }//get func name
    //检查合法性
    bool CheckStatement(ExprAST* expr);
};

extern std::stack<BlockTable*> blockStack;//块表/符号表栈

std::string GetTypeStr(TypeAST* type);
std::string GetOpStr(BinaryOpKind op);
bool IsBetween(TypeName type, constValue startValue, constValue endValue, constValue indexVal, int line);//判断数组下标值是否在范围内

#endif