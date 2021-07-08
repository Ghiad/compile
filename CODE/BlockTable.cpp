#include "BlockTable.h"

bool IsBetween(TypeName type, constValue startValue, constValue endValue, constValue indexVal, int line)
{
    switch (type)
    {
    case TypeName::Integer:
    {
        if (startValue.i > indexVal.i || endValue.i < indexVal.i)
        {
            std::cout << "error line" << std::setw(3) << line;
            std::cout << ":下标范围期待是" << startValue.i << "-" << endValue.i;
            std::cout << ", 当前下标值为" << indexVal.i << std::endl;
            return false;
        }
        break;
    }
    case TypeName::Char:
    {
        if (startValue.c > indexVal.c || endValue.c < indexVal.c)
        {
            std::cout << "error line" << std::setw(3) << line;
            std::cout << ":下标范围期待是" << startValue.c << "-" << endValue.c;
            std::cout << ", 当前下标值为" << indexVal.c << std::endl;
            return false;
        }
        break;
    }
    default://BasicType::Bool
    {
        if (startValue.b > indexVal.b || endValue.b < indexVal.b)
        {
            std::cout << "error line" << std::setw(3) << line;
            std::cout << ":下标范围期待是" << startValue.b << "-" << endValue.b;
            std::cout << ", 当前下标值为" << indexVal.b << std::endl;
            return false;
        }
        break;
    }
    }
    return true;
}

#include <iostream>
#include <fstream>
#include <exception>
#include <memory>
#include <map>
#include "BlockTable.h"

std::stack<BlockTable*> blockStack;

BlockTable::BlockTable(BlockTable* fatherBlock, std::string name)
    :fatherBlock(fatherBlock), name(name)
{
}

BlockTable::~BlockTable()
{
    
}


/*
* 功能：向块表的的标识符表中插入一个新元素
* 参数：symName：要插入的标识符名字，
		symAddr：该标识符的地址，
		firstLoc：第一次出现的位置
* 返回值：插入成功返回true，插入失败返回false
*/
bool BlockTable::Insert(std::string symName, Node* symNode, Loc firstLoc, TypeAST* type)
{
    //先检查名字是否合法
    //如果标识符名与块表名相同
    if (name == symName)
    {
        //program块里定义的变量/常量不允许与program同名
        if (fatherBlock == nullptr)
        {
            if (symNode->GetNodeKind() != NodeKind::functionDecl)
            {
                //错误处理
                std::cout << "error line" << std::setw(3) << symNode->lineNum;
                std::cout << ":重复的标识符" << symName << std::endl;
                return false;
            }
        }
        //如果本块表代表的是函数，则不允许同名
        else
        {
            auto myNode = (FunctionAST*)fatherBlock->SearchThis(name);//父块表插入本函数成功才会生成本块表
            if (myNode->isFunction())
            {
                //错误处理
                std::cout << "error line" << std::setw(3) << symNode->lineNum;
                std::cout << ":重复的标识符" << symName << std::endl;
                return false;
            }
        }

    }

    //在当前块表中查找是否存在同名标识符，存在则失败
    if (SearchThis(symName) != nullptr)
    {
        //错误处理
        std::cout << "error line" << std::setw(3) << symNode->lineNum;
        std::cout << ":重复的标识符" << symName << std::endl;
        return false;
    }

    //如果是ConstAST，需要进行处理
    if (symNode->GetNodeKind() == NodeKind::constDecl)
    {
        if (!HandleConst((ConstAST*)symNode))
            return false;
        type = ((ConstAST*)symNode)->type;
    }

    //检查type是否合法
    if (type != nullptr
        && !type->CheckLegal())
    {
        return false;
    }

    if (type->GetType() == TypeName::Array)
        memberIndex[symName] = new Member((ArrayTypeAST*)type);
    else if (type->GetType() == TypeName::Record)
        memberIndex[symName] = new Member((RecordTypeAST*)type);
    else
        ;

    //条件符合可插入
    symTable[symName] = symNode;
    firstAppear[symName] = firstLoc;
    symTypeTable[symName] = type;
    return true;
}

/*
* 功能：搜索本块表及父块表中是否存在某个标识符
* 参数：std::string symName：要寻找的标识符名字
* 返回值：根据最近原则，若找到标识符，返回改标识符AST的地址（VariableDeclAST*、FunctionAST*、ConstAST*、TypeAST*），否则返回nullptr
*/
Node* BlockTable::SearchAll(std::string symName) 
{
	Node* retPtr = SearchThis(symName);

	BlockTable* curBlock = fatherBlock;
	while (retPtr == nullptr && curBlock != nullptr)
	{
		retPtr = curBlock->SearchThis(symName);
		curBlock = curBlock->fatherBlock;//切换当前搜索的块表为父块表
	}

	return retPtr;
}

/*
* 功能：返回某个标识符的TypeAST
* 参数：std::string symName：要寻找的标识符名字
* 返回值：如果找到符号表可以使用该标识符，返回它的类型树（VariableDeclAST、ConstAST返回的是变量或常量的类型，FunctionAST返回的是函数返回类型）
*       查不到该符号返回nullptr（过程的AST返回的也是nullptr）
*/
TypeAST* BlockTable::GetSymType(std::string symName)
{
	auto iter = symTypeTable.find(symName);
	if (iter == symTypeTable.end())//没找到
	{
		//继续搜索父块表
		TypeAST* retPtr = nullptr;
		BlockTable* curBlock = fatherBlock;
		while (retPtr == nullptr && curBlock != nullptr)
		{
			iter = curBlock->symTypeTable.find(symName);
			if (iter != curBlock->symTypeTable.end())
				retPtr = iter->second;
			curBlock = curBlock->fatherBlock;
		}
		return retPtr;
	}
	else
	{
		return symTypeTable[symName];
	}

}

/*
* 功能：在本块表中查找某个元素
* 参数：std::string symName 元素名
* 返回值：找到返回该元素对象指针(VariableDeclAST*或FunctionAST*)，没找到则返回nullptr
*/
Node* BlockTable::SearchThis(std::string symName)
{
	//搜索本块表
	auto iter = symTable.find(symName);
	if (iter == symTable.end())//没找到
	{
		return nullptr;
	}
	else
	{
		return symTable[symName];
	}
}

/*
* 功能：删除块表内某个标识符
* 参数：symName：标识符名字
* 返回值：删除成功返回true，失败返回false（不存在该元素）
*/
bool BlockTable::Delete(std::string symName)
{
	auto deleteSym = SearchThis(symName);
	if (deleteSym != nullptr)//删除成功
	{
		symTable.erase(symName);
		firstAppear.erase(symName);
		delete deleteSym;
		deleteSym = nullptr;
		return true;
	}
	else//不存在该元素
	{
        std::cout << "错误: 不存在的标识符" << symName << std::endl;
		return false;
	}
}

bool BlockTable::DeleteNameValue(std::string symName)
{
    if (nameValues.find(symName) != nameValues.end())
    {
        nameValues.erase(symName);
        return true;
    }
    else
        return false;
}

/*
* 功能：检查变量表达式的合法性：
        1.是否在符号表中
        2.如果存在同名变量，类型是否是VariableDeclAST、ConstAST
* 参数：VariableExprAST* expr：要检查的表达式
* 返回值：合法返回true，不合法返回false
*/
bool BlockTable::CheckVarExpr(VariableExprAST* expr, bool isAssignLeft)
{
	auto node = SearchAll(expr->name);//先查找符号表里是否存在该变量
	if (node == nullptr)
	{
		//报错，引用了未定义的变量
        std::cout << "error line" << std::setw(3) << expr->lineNum;
        std::cout << ":未定义的标识符" << expr->name << std::endl;
		return false;
	}
	
	//检查AST类型和符号类型是否匹配
	//Variable可以对应的类型有VariableDeclAST、ConstAST、FunctionAST（不含参数的）
	auto nodeKind = node->GetNodeKind();
    switch (nodeKind)
    {
        case NodeKind::variableDecl:
        case NodeKind::constDecl:
        {
            return true;
            break;
        }
        case NodeKind::functionDecl:
        {
            //赋值符号左边: 作为函数返回值, 可以不用检查参数
            auto funcNode = (FunctionAST*)node;
            if (isAssignLeft)
            {
                if (name == funcNode->name)
                    return true;
                else
                {
                    std::cout << "error line" << std::setw(3) << expr->lineNum;
                    std::cout << ":不合法的表达式：标识符" << expr->name << "不能在赋值符号左侧" << std::endl;
                    return false;
                }
            }
            else
            {
                //函数只有在没有参数的情况下可能被构造成VariableExprAST
                if (funcNode->headerDecl.size() == 0)
                {
                    return true;
                }
                else//其余情况都是错的
                {
                    std::cout << "error line" << std::setw(3) << expr->lineNum;
                    std::cout << ":标识符" << expr->name << "接收的参数个数与定义不符" << std::endl;
                    return false;
                }
            }
            break;
        }
        //a:= fun + b; 
        default://ChangeNameTypeAST
        {
            std::cout << "error line" << std::setw(3) << expr->lineNum;
            std::cout << ":标识符" << expr->name << "的使用与定义不符" << std::endl;
            return false;
            break;
        }
    }

}


/*
* 功能：检查数组变量表达式的合法性：
        1.变量定义的类型是否是ArrayTypeAST
        4.检查下标是否都在范围内
* 参数：ArrayVariableExprAST* expr：要检查的表达式
* 返回值：合法返回true，不合法返回false
*/
bool BlockTable::CheckArrayExpr(ArrayVariableExprAST* expr)
{
    //检查名字表达式结构是否合法
    if (!CheckExpression(expr->nameExpr))//检查名字表达式
        return false;

	//检查变量的类型是否是array
    bool isVar = false, isPtr = false;//此处无实际作用
	auto nameType = GetExprType(expr->nameExpr, &isVar, &isPtr);
    if (nameType == nullptr
        || nameType->GetType() != TypeName::Array)
	{
		//报错，变量类型非数组类型
        std::cout << "error line" << std::setw(3) << expr->lineNum;
        std::cout << ":标识符定义的类型不是array类型" << std::endl;
		return false;
	}
    
    //逐个检查下标表达式结构合法性
    for (auto curIndex : expr->index)
    {
        if (!CheckExpression(curIndex))
            return false;
    }
    //检查下标是否都在范围内
    auto arrayType = (ArrayTypeAST*)nameType;
    if (!arrayType->CheckIndex(expr->index, expr->lineNum))
        return false;

    expr->arrayExprType = arrayType->type;//记录id[id]（类似这个形式）代表什么类型，相当于备忘录，减少递归
    return true;
}


/*
* 功能：检查记录变量表达式的合法性：
        1.变量定义的类型是否是RecordTypeAST
        2.检查子表达式是否是合法的成员变量
* 参数：RecordVariableExprAST* expr：要检查的表达式
* 返回值：合法返回true，不合法返回false
*/
bool BlockTable::CheckRecordExpr(RecordVariableExprAST* expr)
{
    //先检查名字表达式是否合法
    if (!CheckExpression(expr->nameExpr))//检查名字表达式
        return false;

    //检查名字表达式的类型与定义的是否相符
    bool isVar = false, isPtr = false;//此处无实际作用
    auto nameType = GetExprType(expr->nameExpr, &isVar, &isPtr);
    if (nameType == nullptr
        || nameType->GetType() != TypeName::Record)
    {
        //报错，不是record类型
        std::cout << "error line" << std::setw(3) << expr->lineNum;
        std::cout << ":标识符" << expr->nameExpr << "定义的类型不是record类型" << std::endl;
        return false;
    }
    auto recordType = (RecordTypeAST*)nameType;

    //检查sub是否是定义里的成员变量
    auto subVarDecl = recordType->SearchVar(expr->sub);
    if (subVarDecl == nullptr)
    {
        //报错，不存在该成员变量
        std::cout << "error line" << std::setw(3) << expr->lineNum;
        std::cout << ":不存在成员变量" << expr->sub << std::endl;
        return false;
    }
    expr->recordExprType = subVarDecl->type;//记录表达式的最终类型，减少后续递归
    
    return true;
}


/*
* 功能：获得某个ExprAST最终代表的type，不如1>2代表的类型为bool型
* 参数：ExprAST* expr：要检查的表达式，isVar是否存在变量
* 返回值：返回改ExprAST的TypeAST，不合法的或无类型的表达式返回nullptr
*/
TypeAST* BlockTable::GetExprType(ExprAST* expr, bool* isVar, bool* isPtr)
{
    *isPtr = true;
    //假设expr的类型都合法
    switch (expr->expr_type)
    {
        //四种常量类型
        case ExprType::NumberExpr:
            *isPtr = false;
            return intTypeAST;
            break;
        case ExprType::RealExpr:
            *isPtr = false;
            return realTypeAST;
            break;
        case ExprType::BoolExpr:
            *isPtr = false;
            return boolTypeAST;
            break;
        case ExprType::CharExpr:
            *isPtr = false;
            return charTypeAST;
            break;

        //单个id，需要查符号表
        case ExprType::Variable:
        {
            VariableExprAST* varExpr = (VariableExprAST*)expr;
            Node* varNode = SearchAll(varExpr->name);
            if (varNode == nullptr)
            {
                //报错，引用了未定义的变量
                std::cout << "error line" << std::setw(3) << expr->lineNum;
                std::cout << ":使用未定义的标识符" << std::endl;
                return nullptr;
            }

            switch (varNode->GetNodeKind())
            {
                case NodeKind::constDecl:
                {
                    return GetSymType(varExpr->name);
                    break;
                }
                case NodeKind::functionDecl:
                case NodeKind::variableDecl:
                {
                    *isVar = true;
                    return GetSymType(varExpr->name);
                    break;
                }
                default:
                    return nullptr;
                    break;
            }
            break;
        }
        
        case ExprType::ArrayVariable://id[id/const]
        {
            *isVar = true;
            return ((ArrayVariableExprAST*)expr)->arrayExprType;
            break;
        }

        case ExprType::RecordVariable://id1.id2.id3...
        {
            *isVar = true;
            return ((RecordVariableExprAST*)expr)->recordExprType;
            break;
        }

        case ExprType::UnaryExpr:
        {
            auto unaryExpr = (UnaryExprAST*)expr;
            if (unaryExpr->hasVar)
                *isVar = true;
            return unaryExpr->type;
            break;
        }

        case ExprType::BinaryExpr:
        {
            auto binExpr = (BinaryExprAST*)expr;
            if (binExpr->hasVar)
                *isVar = true;
            return binExpr->type;
            break;
        }

        case ExprType::CallFuncExpr:
        {
            *isVar = true;
            auto funcExpr = (CallFunctionExprAST*)expr;
            return GetSymType(funcExpr->callee);
            break;
        }

        default:
            return nullptr;
            break;
    }
}

/*
* 功能：比较两种类型是否完全相同（两种类型是否能做二元运算）
* 参数： TypeAST* leftType: 表达式1
        TypeAST* rightType: 表达式2
        bool isAssign: 二元运算是否是赋值
* 返回值：两种类型完全相同/两种类型可以进行二元运算，返回true；否则返回false
*/
bool BlockTable::CompareType(TypeAST* leftType, TypeAST* rightType, bool isAssign) const
{
    switch (leftType->GetType())
    {
        case TypeName::Record:
        {
            return false;
            //record（整体）连赋值都用不了
        }
        case TypeName::Array:
        {
            if (rightType->GetType() != TypeName::Array)
                return false;
            auto arrayLeft = (ArrayTypeAST*)leftType;
            auto arrayRight = (ArrayTypeAST*)rightType;
            return arrayLeft->Compare(arrayRight);
        }
        default://基本类型
        {
            bool result = (leftType == rightType);
            if (isAssign 
                && leftType == realTypeAST
                && rightType == intTypeAST)//赋值运算符的情况下，integer的值可以赋给real
            {
                result = true;
            }
            return result;
            break;
        }
    }
}


bool BlockTable::CheckExpression(ExprAST* expr, bool isAssignLeft)
{
    switch (expr->expr_type)
    {
        //AST的最底层，不存在子ExprAST的情况,递归的base条件
        case ExprType::NumberExpr:
        case ExprType::RealExpr:
        case ExprType::BoolExpr:
        case ExprType::CharExpr:
        {
            return true;
            break;
        }
        case ExprType::Variable:
        {
            return CheckVarExpr((VariableExprAST*)expr, isAssignLeft);
            break;
        }
        
        //需要继续检查子ExprAST，需要进行递归
        case ExprType::ArrayVariable:
            return CheckArrayExpr((ArrayVariableExprAST*)expr);
            break;
        case ExprType::RecordVariable:
            return CheckRecordExpr((RecordVariableExprAST*)expr);
            break;
        case ExprType::UnaryExpr:
            return CheckUnaryExpr((UnaryExprAST*)expr);
            break;
        case ExprType::BinaryExpr:
            return CheckBinExpr((BinaryExprAST*)expr);
            break;
        case ExprType::CallFuncExpr:
            return CheckCallFuncExpr((CallFunctionExprAST*)expr);
            break;

        default://剩余的ExprAST都是statement，不合法
        {
            std::cout << "error line" << std::setw(3) << expr->lineNum;
            std::cout << ":不合法的表达式: 不是expression语句" << std::endl;
            return false;
            break;
        }
    }
}


bool BlockTable::HandleConst(ConstAST* constAst)
{
    auto str = constAst->value.s;
    //判断是否需要把值取负
    bool isMinus = false;
    switch (str[0])
    {
    case '+':
    {
        str = str.erase(0, 1);
        break;
    }
    case '-':
    {
        str = str.erase(0, 1);
        isMinus = true;
        break;
    }
    default:
        break;
    }
    //处理constvalue = id的情况
    if (constAst->type->GetType() == TypeName::Id)
    {
        auto node = SearchAll(str);
        if (node == nullptr
            || node->GetNodeKind() != NodeKind::constDecl)
        {
            //报错,使用未定义的常量名
            std::cout << "error line" << std::setw(3) << constAst->lineNum;
            std::cout << ":使用未定义的常量名" << std::endl;
            return false;
        }

        ConstAST* decl = (ConstAST*)node;
        constAst->type = decl->type;
        constAst->type = decl->type;
        constAst->value = decl->value;
    }

    if (isMinus)
    {
        auto t = constAst->type->GetType();
        if (t != TypeName::Integer
            && t != TypeName::Real)
        {
            std::cout << "error line" << std::setw(3) << constAst->lineNum;
            std::cout << ":不合法的表达式：" << GetTypeStr(constAst->type) << "不允许进行 - 运算" << std::endl;
            return false;
        }
        constAst->value.NegateValue();
    }
    return true;
}


/*
* 功能：检查BinaryExprAST表达式的合法性：
        1.检查左右表达式是否能在一起运算
        2.检查二元运算符是否能支持当前表达式的类型，如char类型不能做+/-等运算
* 参数：BinaryExprAST* binExpr：要检查的表达式
* 返回值：合法返回true，不合法返回false
*/
bool BlockTable::CheckBinExpr(BinaryExprAST* binExpr)
{
    //自顶向下检查子表达式的合法性
    bool isAssignOp = false;
    if (binExpr->op == BinaryOpKind::assignmentKind)
        isAssignOp = true;
    if (!CheckExpression(binExpr->LExpr, isAssignOp)
        || !CheckExpression(binExpr->RExpr))
    {
        return false;
    }

    //子表达式都合法，求子表达式的类型及是否存在变量
    bool isVar = false, isPtr = false;
    bool isLeftVar = false;//记录左表达式是否是常量
    auto leftExprType = GetExprType(binExpr->LExpr, &isVar, &isPtr);
    isLeftVar = isVar;
    auto rightExprType = GetExprType(binExpr->RExpr, &isVar, &isPtr);
    binExpr->hasVar = isVar;

    //二元表达式左右的类型不允许为空
    if (leftExprType == nullptr
        || rightExprType == nullptr)
    {
        std::cout << "error line" << std::setw(3) << binExpr->lineNum;
        std::cout << ":二元表达式左右的类型不合法" << std::endl;
        return false;
    }

    //先匹配类型是否相同
    if (!CompareType(leftExprType, rightExprType, isAssignOp))//存在record整体类型或类型不匹配
    {
        std::cout << "error line" << std::setw(3) << binExpr->lineNum;
        std::cout << ":二元表达式左右的类型不允许进行计算" << std::endl;
        return false;
    }

    //record类型已经被过滤
    auto leftTypeName = leftExprType->GetType();
    switch (binExpr->op)
    {
        //+、*、-,允许的类型：integer、real
        case BinaryOpKind::plusKind://+
        case BinaryOpKind::mulKind://*
        case BinaryOpKind::minusKind://-
        {
            if (leftTypeName != TypeName::Integer
                && leftTypeName != TypeName::Real)
            {
                std::cout << "error line" << std::setw(3) << binExpr->lineNum;
                std::cout << ":不合法的表达式：" 
                        << GetTypeStr(leftExprType) << "不允许进行" << GetOpStr(binExpr->op) << "运算" << std::endl;
                return false;
            }
            binExpr->type = leftExprType;
            break;
        }
        // 运算/得到的结果都是real
        case BinaryOpKind::divRealKind:
        {
            if (leftTypeName != TypeName::Integer
                && leftTypeName != TypeName::Real)
            {
                std::cout << "error line" << std::setw(3) << binExpr->lineNum;
                std::cout << ":不合法的表达式："
                    << GetTypeStr(leftExprType) << "不允许进行" << GetOpStr(binExpr->op) << "运算" << std::endl;
                return false;
            }
            binExpr->type = realTypeAST;
            break;
        }
            
        //or、and允许的类型：integer、bool
        case BinaryOpKind::orKind://or
        case BinaryOpKind::andKind://and
        {
            if (leftTypeName != TypeName::Integer
                && leftTypeName != TypeName::Bool)
            {
                std::cout << "error line" << std::setw(3) << binExpr->lineNum;
                std::cout << ":不合法的表达式："
                    << GetTypeStr(leftExprType) << "不允许进行" << GetOpStr(binExpr->op) << "运算" << std::endl;
                return false;
            }
            binExpr->type = leftExprType;
            break;
        }
        
        //div、mod允许的类型: integer
        case BinaryOpKind::divKind://div
        case BinaryOpKind::modKind://mod
        {
            if (leftTypeName != TypeName::Integer)
            {
                std::cout << "error line" << std::setw(3) << binExpr->lineNum;
                std::cout << ":不合法的表达式："
                    << GetTypeStr(leftExprType) << "不允许进行" << GetOpStr(binExpr->op) << "运算" << std::endl;
                return false;
            }

            binExpr->type = leftExprType;
            break;
        }

        case BinaryOpKind::assignmentKind://:=
        {
            if ((binExpr->LExpr->expr_type == ExprType::Variable && isLeftVar)
                || binExpr->LExpr->expr_type == ExprType::ArrayVariable
                || binExpr->LExpr->expr_type == ExprType::RecordVariable)
            {
                binExpr->type = nullptr;
            }
            else
            {
                //报错，不是var类型
                std::cout << "error line" << std::setw(3) << binExpr->LExpr->lineNum;
                std::cout << ":左表达式要求是变量" << std::endl;
                return false;
            }
            break;
        }
            
        default://relop
        {
            if (leftTypeName == TypeName::Array)
            {
                std::cout << "error line" << std::setw(3) << binExpr->lineNum;
                std::cout << ":不合法的表达式："
                    << GetTypeStr(leftExprType) << "不允许进行" << GetOpStr(binExpr->op) << "运算" << std::endl;
                return false;
            }
            binExpr->type = boolTypeAST;
            break;
        }
            
    }//end of switch

    //如果二元表达式是常量，保存结果指，如2+3，保存5
    if (!binExpr->hasVar
        && binExpr->op != BinaryOpKind::assignmentKind)
    {
        auto leftValue = GetConstValue(binExpr->LExpr);
        auto rightValue = GetConstValue(binExpr->RExpr);
        binExpr->constVal = new constValue(0);
        switch (leftTypeName)
        {
        case TypeName::Integer:
            binExpr->constVal->IntBinOp(leftValue.i, rightValue.i, binExpr->op);
            break;
        case TypeName::Real:
            binExpr->constVal->RealBinOp(leftValue.d, rightValue.d, binExpr->op);
            break;
        case TypeName::Char:
            binExpr->constVal->CharBinOp(leftValue.c, rightValue.c, binExpr->op);
            break;
        case TypeName::Bool:
            binExpr->constVal->BoolBinOp(leftValue.b, rightValue.b, binExpr->op);
            break;
        default:
            break;
        }
    }

    return true;
}


/*
* 功能：检查CallFunctionExprAST表达式的合法性：
        1.检查该函数名是否在符号表中
        2.符号表中的同名标识符是否是函数类型
        3.匹配参数：参数的类型与定义时的类型是否相同
* 参数：CallFunctionExprAST* expr：要检查的表达式
* 返回值：合法返回true，不合法返回false
*/
bool BlockTable::CheckCallFuncExpr(CallFunctionExprAST* expr)
{
    auto funcNode = SearchAll(expr->callee);
	//auto funcAST = (FunctionAST*)funcNode;//对应的符号表里的functionAST
	
    if (funcNode == nullptr)
    {
        //报错，不存在
        std::cout << "error line" << std::setw(3) << expr->lineNum;
        std::cout << ":未定义过的标识符" << std::endl;
        return false;
    }
    if (funcNode->GetNodeKind() != NodeKind::functionDecl)
    {
        //报错，类型不是函数类型
        std::cout << "error line" << std::setw(3) << expr->lineNum;
        std::cout << ":标识符" << expr->callee << "定义的类型不是函数" << std::endl;
        return false;
    }
    
    auto funcAST = (FunctionAST*)funcNode;//对应的符号表里的functionAST

    if (!funcAST->isFunction())
    {
        //不是函数类型
        std::cout << "error line" << std::setw(3) << expr->lineNum;
        std::cout << ":标识符" << expr->callee << "定义的类型不是函数" << std::endl;
        return false;
    }
    
    //开始匹配参数
    //先计算定义的参数个数
    int declArgSize = funcAST->headerDecl.size();//定义里的参数个数
    int exprArgSize = expr->args.size();
    if (declArgSize != exprArgSize)
    {
        //报错，参数个数不匹配
        std::cout << "error line" << std::setw(3) << expr->lineNum;
        std::cout << ":与过程定义的参数个数不符"
            << "期待得到" << declArgSize << "个参数, "
            << "实际得到" << exprArgSize << "个参数" << std::endl;
        return false;
    }

    //逐个检查每个参数表达式的合法性以及每个参数的类型与定义的是否匹配
    for (int i = 0; i < declArgSize; i++)
    {
        //先检查表达式的合法性
        if (!CheckExpression(expr->args[i]))
            return false;

        //表达式合法，检查类型与定义的是否匹配
        bool isVar = false, isPtr = false;//此处无具体作用
        auto curCheckArgType = GetExprType(expr->args[i],&isVar, &isPtr);//当前要检查的参数的类型
        auto declArgType = funcAST->headerDecl[i]->type;//参数定义的类型，都是BasicType
        //函数的参数都是基本类型，直接比对指针地址是不是一样即可
        if (curCheckArgType != declArgType)
        {
            std::cout << "error line" << std::setw(3) << expr->lineNum;
            std::cout << ":错误：过程定义的第" << i << "个参数类型为" << GetTypeStr(declArgType)
                << "，得到的类型为" << GetTypeStr(curCheckArgType) << std::endl;
            return false;
        }
    }
    return true;
}


/*
* 功能：检查CallProcedureExprAST表达式的合法性：
        1.检查该函数名是否在符号表中
        2.符号表中的同名标识符是否是过程类型
        3.匹配参数：参数的类型与定义时的类型是否相同
* 参数：CallProcedureExprAST* expr：要检查的表达式
* 返回值：合法返回true，不合法返回false
*/
bool BlockTable::CheckCallProcExpr(CallProcedureExprAST* expr)
{
    auto procNode = SearchAll(expr->callee);
    if (procNode == nullptr)
    {
        //报错，不存在
        std::cout << "error line" << std::setw(3) << expr->lineNum;
        std::cout << ":错误：未定义过的标识符" << expr->callee << std::endl;
        return false;
    }
    if (procNode->GetNodeKind() != NodeKind::functionDecl)
    {
        //报错，类型不是函数类型
        std::cout << "error line" << std::setw(3) << expr->lineNum;
        std::cout << ":错误：标识符" << expr->callee << "定义的类型不是过程" << std::endl;
        return false;
    }

    auto procAST = (FunctionAST*)procNode;//对应的符号表里的functionAST
    if (!procAST->isProcedure())
    {
        //不是过程类型
        std::cout << "error line" << std::setw(3) << expr->lineNum;
        std::cout << ":错误：标识符" << expr->callee << "定义的类型不是过程" << std::endl;
        return false;
    }

    int declArgSize = procAST->headerDecl.size();//定义里的参数个数    
    int exprArgSize = expr->args.size();
    if (declArgSize != exprArgSize)
    {
        //报错，参数个数不匹配
        std::cout << "error line" << std::setw(3) << expr->lineNum;
        std::cout << ":与过程定义的参数个数不符" 
                << "期待得到" << declArgSize << "个参数, " 
                << "实际得到" << exprArgSize << "个参数" << std::endl;
        return false;
    }

    //逐个检查每个参数的类型与定义的是否匹配
    for (int i = 0; i < declArgSize; i++)
    {
        //先检查表达式的合法性
        if (!CheckExpression(expr->args[i]))
            return false;

        //表达式合法，检查类型与定义的是否匹配
        bool isVar = false, isPtr = false;//此处无具体作用
        auto curCheckArgType = GetExprType(expr->args[i], &isVar, &isPtr);
        auto declArgType = procAST->headerDecl[i]->type;//参数定义的类型，都是BasicType
        //函数的参数都是基本类型，直接比对指针地址是不是一样即可
        if (curCheckArgType != declArgType)
        {
            std::cout << "error line" << std::setw(3) << expr->lineNum;
            std::cout << ":过程定义的第" << i << "个参数类型为" << GetTypeStr(declArgType)
                << "，得到的类型为" << GetTypeStr(curCheckArgType) << std::endl;
            return false;
        }
    }
    return true;
}


constValue BlockTable::GetConstValue(ExprAST* expr)
{
    //获得case的常量值
    switch (expr->expr_type)
    {
        case ExprType::NumberExpr:
        {
            int val = ((NumberExprAST*)expr)->val;
            return constValue(val);
            break;
        }
        case ExprType::BoolExpr:
        {
            bool val = ((BoolExprAST*)expr)->val;
            return constValue(val);
            break;
        }
        case ExprType::CharExpr:
        {
            char val = ((CharExprAST*)expr)->val;
            return constValue(val);
            break;
        }
        case ExprType::RealExpr:
        {
            double val = ((RealExprAST*)expr)->val;
            return constValue(val);
            break;
        }
        case ExprType::Variable:
        {
            auto varExpr = (VariableExprAST*)expr;
            auto varNode = SearchAll(varExpr->name);
            return ((ConstAST*)varNode)->value;
            break;
        }

        case ExprType::BinaryExpr:
        {
            auto binExpr = (BinaryExprAST*)expr;
            return (*(binExpr->constVal));
            break;
        }

        case ExprType::UnaryExpr:
        {
            auto unaryExpr = (UnaryExprAST*)expr;
            return (*(unaryExpr->constVal));
            break;
        }

        default://其他类型不合法（一般不会到这里）
        {
            return constValue(0);
            break;
        }
    }
}


/*
* 功能：检查UnaryExprAST表达式的合法性：
        1.检查一元运算符是否能支持当前表达式的类型
* 参数：UnaryExprAST* expr：要检查的表达式
* 返回值：合法返回true，不合法返回false
*/
bool BlockTable::CheckUnaryExpr(UnaryExprAST* expr)
{
    //先检查子表达式的合法性
    if (!CheckExpression(expr->expr))
        return false;

    bool isVar = false, isPtr = false;//记录该一元表达式中是否存在变量
    auto exprType = GetExprType(expr->expr, &isVar, &isPtr);
    expr->hasVar = isVar;

    //类型不允许为空
    if (exprType == nullptr)
    {
        std::cout << "error line" << std::setw(3) << expr->lineNum;
        std::cout << ":不合法的表达式" << std::endl;
        return false;
    }

    //过滤record、array类型
    auto exprTypeName = exprType->GetType();
    if (exprTypeName == TypeName::Array
        || exprTypeName == TypeName::Record)
    {
        std::cout << "error line" << std::setw(3) << expr->lineNum;
        std::cout << ":不合法的表达式 : "
                    << GetTypeStr(exprType) <<"不允许进行一元运算"<< std::endl;
        return false;
    }

    //根据一元操作数判断表达式类型是否符合条件, 并标记type
    switch (expr->op)
    {
        case UnaryOpKind::negKind://-
        {
            if (exprTypeName != TypeName::Integer
                && exprTypeName != TypeName::Real)
            {
                std::cout << "error line" << std::setw(3) << expr->lineNum;
                std::cout << ":不合法的表达式 : "
                    << GetTypeStr(exprType) << "不允许进行一元-运算" << std::endl;
                return false;
            }
            expr->type = exprType;//记录一元表达式最终类型
            break;
        }
        default://not
        {
            if (exprTypeName == TypeName::Real)
            {
                std::cout << "error line" << std::setw(3) << expr->lineNum;
                std::cout << ":不合法的表达式 : "
                    << GetTypeStr(exprType) << "不允许进行一元not运算" << std::endl;
                return false;
            }

            if (exprTypeName == TypeName::Bool)
                expr->type = boolTypeAST;
            else
                expr->type = intTypeAST;//允许not 'c'，最终结果将存为int

            break;
        }
    }//end of switch

    //如果表达式只包含常量，计算出该一元表达式所表示的常量值，如-(3+4),保存-7
    if (!expr->hasVar)
    {
        auto exprValue = GetConstValue(expr->expr);
        expr->constVal = new constValue(0);
        if (expr->op == UnaryOpKind::negKind)
        {
            if (exprTypeName == TypeName::Integer)
                expr->constVal->i = -exprValue.i;
            else
                expr->constVal->d = -exprValue.d;
        }
        else //UnaryOpKind::notKind
        {
            if (exprTypeName == TypeName::Bool)
                expr->constVal->b = !exprValue.b;
            else if(exprTypeName == TypeName::Integer)
                expr->constVal->i = ~exprValue.i;
            else
                expr->constVal->i = ~exprValue.c;
        }
    }

    return true;
}


/*
* 功能：在本块表中查找某个标识符的Value
* 参数：std::string symName 元素名
* 返回值：找到返回该元素Value指针，没找到则返回nullptr
*/
Value* BlockTable::SearchThisValue(std::string symName)
{
    //搜索本块表
    auto iter = nameValues.find(symName);
    if (iter == nameValues.end())//没找到
    {
        return nullptr;
    }
    else
    {
        return nameValues[symName];
    }
}


/*
* 功能：在本块表及父块表中查找某个标识符的Value
* 参数：std::string symName 元素名
* 返回值：找到返回该元素Value指针，没找到则返回nullptr
*/
Value* BlockTable::SearchNameValue(std::string symName)
{
    Value* retPtr = SearchThisValue(symName);

    BlockTable* curBlock = fatherBlock;
    while (retPtr == nullptr && curBlock != nullptr)
    {
        retPtr = curBlock->SearchThisValue(symName);
        curBlock = curBlock->fatherBlock;//切换当前搜索的块表为父块表
    }
    return retPtr;
}


/*
* 功能：往当前块表中插入某个标识符的Value*
* 参数：std::string symName 元素名
*       Value* value 值
* 返回值：插入成功返回true，不成功返回false
*/
bool BlockTable::InsertNameValue(std::string symName, Value* value)
{
    //program块里通过var定义的变量不允许与program同名
    if (fatherBlock == nullptr && name == symName)
    {
        //错误处理
        std::cout << "错误：重复定义的变量名: " << symName << std::endl;
        return false;
    }

    //在当前块表中查找是否存在同名标识符，存在则失败
    if (SearchThisValue(symName) != nullptr)
    {
        //错误处理
        std::cout << "错误：重复定义的变量名: " << symName << std::endl;
        return false;
    }

    //条件符合可插入
    nameValues[symName] = value;
    return true;
}


/*
* 功能：检查文法中statement的合法性：
        1.statement中只包含以下几种表达式：
            1.赋值语句（BinaryExpr）
            2.过程调用/函数调用（CallExpr）
            3.if语句
            4.for语句
            5.while语句
            6.repeat语句
            7.begin..end语句
            8.case语句
        2.若表达式是以上类型，则调用对应函数检查语句的合法性
* 参数：ExprAST* expr：要检查的表达式
* 返回值：合法返回true，不合法返回false
*/
bool BlockTable::CheckStatement(ExprAST* expr)
{
    if (expr == nullptr)
        return true;

    switch (expr->expr_type)
    {
        case ExprType::BinaryExpr:
        {
            auto binExpr = (BinaryExprAST*)expr;
            auto op = binExpr->op;
            if (op != BinaryOpKind::assignmentKind)
            {
                std::cout << "error line" << std::setw(3) << expr->lineNum;
                std::cout << ":不合法的表达式: 表达式进行" << GetOpStr(op) << "运算， 非赋值语句" << std::endl;
                return false;
            }
            return CheckBinExpr(binExpr);
            break;
        }
        /*case ExprType::CallFuncExpr:
        {
            auto callFuncExpr = (CallFunctionExprAST*)expr;
            return CheckCallFuncExpr(callFuncExpr);
            break;
        }*/
        case ExprType::CallProcExpr:
        {
            auto callProcExpr = (CallProcedureExprAST*)expr;
            return CheckCallProcExpr(callProcExpr);
            break;
        }
        case ExprType::IfExpr:
        {
            auto ifExpr = (IfExprAST*)expr;
            return CheckIfExpr(ifExpr);
            break;
        }

        case ExprType::ForExpr:
        {
            auto forExpr = (ForExprAST*)expr;
            if (CheckForExpr(forExpr))
                return true;
            else
                return false;
            break;
        }

        case ExprType::WhileExpr:
        {
            auto whileExpr = (WhileExprAST*)expr;
            return CheckWhileExpr(whileExpr);
            break;
        }

        case ExprType::RepeatExpr:
        {
            auto repeatExpr = (RepeatExprAST*)expr;
            return CheckRepeatExpr(repeatExpr);
            break;
        }
        case ExprType::BeExpr:
        {
            auto beExpr = (BeginEndExprAST*)expr;
            return CheckBeginEndExpr(beExpr);
            break;
        }

        case ExprType::CaseExpr:
        {
            auto caseExpr = (CaseofEndExprAST*)expr;
            return CheckCaseExpr(caseExpr);
            break;
        }

        default:
            std::cout << "error line" << std::setw(3) << expr->lineNum;
            std::cout << ":不合法的表达式：不是statement语句" << std::endl;
            return false;
            break;
    }//end of switch
}

/*
* 功能：检查BeginEndExprAST表达式的合法性：
        1.逐个检查body部分的statement语句
* 参数：BeginEndExprAST* beExpr：要检查的表达式
* 返回值：合法返回true，不合法返回false
*/
bool BlockTable::CheckBeginEndExpr(BeginEndExprAST* beExpr)
{
    auto body = beExpr->body;
    //逐个检查body中的statement
    for (unsigned int i = 0; i < body.size(); i++)
    {
        if (!CheckStatement(body[i]))
            return false;
    }
    return true;
}

std::string GetTypeStr(TypeAST* type)
{
    switch (type->GetType())
    {
    case TypeName::Array:
        return "array型";
    case TypeName::Bool:
        return "Boolean型";
    case TypeName::Char:
        return "char型";
    case TypeName::Integer:
        return "integer型";
    case TypeName::Real:
        return "real型";
    case TypeName::Record:
        return "record型";
    default:
        return GetTypeStr(((ChangeNameTypeAST*)type)->originalType);
        break;
    }
}

/*
* 功能：检查IfExprAST表达式的合法性：
        1.检查ifCond是否是bool类型
        2.逐个检查thenComponent部分的statement语句
        3.逐个检查elseComponent部分的statement语句
* 参数：IfExprAST* ifExpr：要检查的表达式
* 返回值：合法返回true，不合法返回false
*/
bool BlockTable::CheckIfExpr(IfExprAST* ifExpr)
{
    //先判断子表达式的合法性
    if (!CheckExpression(ifExpr->ifCond))
        return false;

    //ifCond得是bool型
    bool isVar = false, isPtr = false;
    TypeAST* condType = GetExprType(ifExpr->ifCond, &isVar, &isPtr);
    if (condType != boolTypeAST)
    {
        std::cout << "error line" << std::setw(3) << ifExpr->lineNum;
        std::cout << ":表达式期待是Boolean型，得到的是" << GetTypeStr(condType) << std::endl;
        return false;
    }

    //检查thenComponent
    if (!CheckStatement(ifExpr->thenComponent))
        return false;
   
    //检查elseComponent
    if (!CheckStatement(ifExpr->elseComponent))
        return false;

    return true;
}


/*
* 功能：检查ForExprAST表达式的合法性：
        1.检查循环变量是否合法
        2.检查循环条件是否合法
        3.逐个检查body部分的statement语句
* 参数：ForExprAST* forExpr：要检查的表达式
* 返回值：合法: 循环变量类型为int返回1，为char返回2，为bool返回3；不合法返回0
*/
bool BlockTable::CheckForExpr(ForExprAST* forExpr)
{
    auto idNode = SearchAll(forExpr->varName);//先查找符号表里是否存在该变量
    if (idNode == nullptr)
    {
        //报错，引用了未定义的变量
        std::cout << "error line" << std::setw(3) << forExpr->lineNum;
        std::cout << ":未定义的标识符 " << forExpr->varName << std::endl;
        return false;
    }
    if (idNode->GetNodeKind() != NodeKind::variableDecl)
    {
        //报错，期待变量类型
        std::cout << "error line" << std::setw(3) << forExpr->lineNum;
        std::cout << ":标识符 " << forExpr->varName << " 期待是变量" << std::endl;
        return false;
    }

    auto counterType = ((VariableDeclAST*)idNode)->type;
    if (counterType != intTypeAST
        && counterType != boolTypeAST
        && counterType != charTypeAST)
    {
        //counter的类型不合法
        std::cout << "error line" << std::setw(3) << forExpr->lineNum;
        std::cout << ":循环变量 " << forExpr->varName << " 的类型不允许为" << GetTypeStr(counterType) << std::endl;
        return false;
    }

    //检查start和end表达式合法性
    if (!CheckExpression(forExpr->start)
        || !CheckExpression(forExpr->end))
        return false;

    //检查start和end的类型与counter是否相符
    bool isVar = false, isPtr = false;
    auto startType = GetExprType(forExpr->start, &isVar, &isPtr);
    auto endType = GetExprType(forExpr->end, &isVar, &isPtr);
    if (startType != counterType
        || endType != counterType)
    {
        //与counter的类型不合
        std::cout << "error line" << std::setw(3) << forExpr->lineNum;
        std::cout << ":范围期待类型为" << GetTypeStr(counterType) 
                << ", 得到类型为" << GetTypeStr(startType) << "、" << GetTypeStr(endType) << std::endl;
        return false;
    }

    //检查body部分的合法性
    if (!CheckStatement(forExpr->body))
        return false;

    return true;
}


/*
* 功能：检查WhileExprAST表达式的合法性：
        1.检查whileCond是否是bool类型
        2.逐个检查body部分的statement语句
* 参数：WhileExprAST* whileExpr：要检查的表达式
* 返回值：合法返回true，不合法返回false
*/
bool BlockTable::CheckWhileExpr(WhileExprAST* whileExpr)
{
    //检查条件表达式结构是否合法
    if (!CheckExpression(whileExpr->whileCond))
        return false;

    //检查条件表达式是否为bool类型
    bool isVar = false, isPtr = false;
    auto condType = GetExprType(whileExpr->whileCond, &isVar, &isPtr);
    if (condType != boolTypeAST)
    {
        //报错,条件期待为bool型
        std::cout << "error line" << std::setw(3) << whileExpr->lineNum;
        std::cout << ":表达式期待是Boolean型，得到的是" << GetTypeStr(condType) << std::endl;
        return false;
    }

    //逐个检查body中的表达式
    if (!CheckStatement(whileExpr->body))
        return false;
    return true;
}


/*
* 功能：检查RepeatExprAST表达式的合法性：
        1.检查untilCond是否是bool类型
        2.逐个检查body部分的statement语句
* 参数：RepeatExprAST* repeatExpr：要检查的表达式
* 返回值：合法返回true，不合法返回false
*/
bool BlockTable::CheckRepeatExpr(RepeatExprAST* repeatExpr)
{
    //检查条件表达式结构是否合法
    if (!CheckExpression(repeatExpr->untilCond))
        return false;

    //检查条件表达式是否为bool类型
    bool isVar = false, isPtr = false;
    auto condType = GetExprType(repeatExpr->untilCond, &isVar, &isPtr);
    if (condType != boolTypeAST)
    {
        //报错,条件期待为bool型
        std::cout << "error line" << std::setw(3) << repeatExpr->lineNum;
        std::cout << ":表达式期待是Boolean型，得到的是" << GetTypeStr(condType) << std::endl;
        return false;
    }

    //逐个检查body中的表达式
    int size = repeatExpr->body.size();
    for (int i = 0; i < size; i++)
    {
        if (!CheckStatement(repeatExpr->body[i]))
            return false;
    }
    return true;
}


/*
* 功能：获得某函数/过程的FunctionAST
* 参数：std::string funcName 名字
* 返回值：找到返回该元素FunctionAST指针，没找到则返回nullptr
*/
FunctionAST* BlockTable::GetFunctionAST(std::string funcName)
{
    auto funcNode = SearchAll(funcName);
    if (funcNode != nullptr
        && funcNode->GetNodeKind() == NodeKind::functionDecl)
        return (FunctionAST*)funcNode;
    else
        return nullptr;
}


/*
* 功能：检查CaseofEndExprAST表达式的合法性：
        1.检查caseCond是否是int/bool/char类型
        2.逐个检查body部分的语句: body[const] = statement，const部分和statement都要合法
* 参数：CaseofEndExprAST* caseExpr：要检查的表达式
* 返回值：合法返回true，不合法返回false
*/
bool BlockTable::CheckCaseExpr(CaseofEndExprAST* caseExpr)
{
    //检查表达式结构是否合法
    if (!CheckExpression(caseExpr->caseCond))
        return false;

    bool isVar = false, isPtr = false;
    auto condType = GetExprType(caseExpr->caseCond, &isVar, &isPtr);

    //condType不可以是real类型和非基础类型
    if (condType != intTypeAST
        && condType != boolTypeAST
        && condType != charTypeAST)
    {
        //报错
        std::cout << "error line" << std::setw(3) << caseExpr->caseCond->lineNum;
        std::cout << ":表达式类型不允许是" << GetTypeStr(condType) << std::endl;
        return false;
    }

    //遍历body，body[const] = statement
    std::vector<constValue> caseValues;
    for (auto curBody : caseExpr->body)
    {
        if (!CheckExpression(curBody.first))
            return false;

        bool isVar = false, isPtr = false;
        auto caseType = GetExprType(curBody.first, &isVar, &isPtr);
        if (caseType != condType)
        {
            //报错，case类型与条件类型不符
            std::cout << "error line" << std::setw(3) << curBody.first->lineNum;
            std::cout << ":表达式期待是" << GetTypeStr(condType) << ",得到的是" << GetTypeStr(caseType) << std::endl;
            return false;
        }
        if (isVar)
        {
            //case期望是常量
            std::cout << "error line" << std::setw(3) << curBody.first->lineNum;
            std::cout << ":表达式期待是常量" << std::endl;
            return false;
        }

        //查重
        auto value = GetConstValue(curBody.first);//该条件分支的常量值
        for (auto curCaseVal : caseValues)
        {
            if (curCaseVal.Compare(value))
            {
                //报错，重复的条件分支
                std::cout << "error line" << std::setw(3) << curBody.first->lineNum;
                std::cout << ":不允许重复的条件分支" << std::endl;
                return false;
            }
        }
        caseValues.push_back(value);

        //检查statement部分是否合法
        if (!CheckStatement(curBody.second))
            return false;
    }//end of for

    return true;
}


Member::Member(RecordTypeAST* type)
{
    for (auto varDecl : type->declList)
    {
        Member* memType = nullptr;
        switch (varDecl->type->GetType())
        {
        case TypeName::Array:
        {
            memType = new Member((ArrayTypeAST*)varDecl->type);
            break;
        }
        case TypeName::Record:
        {
            memType = new Member((RecordTypeAST*)varDecl->type);
            break;
        }
        default:
            break;
        }
        for (auto varName : varDecl->variableName)
        {
            memberIndex.push_back(varName);
            memberType.push_back(memType);
        }
    }
}

Member::Member(ArrayTypeAST* type)
{
    Member* memType = nullptr;
    switch (type->type->GetType())
    {
    case TypeName::Array:
    {
        memType = new Member((ArrayTypeAST*)type->type);
        break;
    }
    case TypeName::Record:
    {
        memType = new Member((RecordTypeAST*)type->type);
        break;
    }
    default:
        break;
    }
    memberIndex.push_back("IS_ARRAY");
    memberType.push_back(memType);
}


Member* BlockTable::SearchThisMember(std::string symName)
{
    //搜索本块表
    auto iter = memberIndex.find(symName);
    if (iter == memberIndex.end())//没找到
    {
        return nullptr;
    }
    else
    {
        return memberIndex[symName];
    }
}

Member* BlockTable::SearchAllMember(std::string symName)
{
    Member* retPtr = SearchThisMember(symName);

    BlockTable* curBlock = fatherBlock;
    while (retPtr == nullptr && curBlock != nullptr)
    {
        retPtr = curBlock->SearchThisMember(symName);
        curBlock = curBlock->fatherBlock;//切换当前搜索的块表为父块表
    }

    return retPtr;
}

int BlockTable::GetMemberIndex(ExprAST* searchExpr)
{
    int res = -1;
    std::stack<std::string> memNameStack;

    //先检查表达式各部分合法
    auto curExpr = searchExpr;
    //递归到最顶层，获得最顶层的名字，即存在符号表里的名字
    while (curExpr->expr_type != ExprType::Variable)
    {
        switch (curExpr->expr_type)
        {
        case ExprType::ArrayVariable:
        {
            auto arrayExpr = (ArrayVariableExprAST*)curExpr;
            memNameStack.push("IS_ARRAY");
            curExpr = arrayExpr->nameExpr;
            break;
        }
        case ExprType::RecordVariable:
        {
            auto recordExpr = (RecordVariableExprAST*)curExpr;
            memNameStack.push(recordExpr->sub);
            curExpr = recordExpr->nameExpr;
            break;
        }

        default:
            break;
        }
    }

    //找index
    auto varExpr = (VariableExprAST*)curExpr;
    auto curMemberPtr = SearchAllMember(varExpr->name);//顶层的Member*
    if (curMemberPtr != nullptr)
    {
        while (!memNameStack.empty())
        {
            auto curName = memNameStack.top();
            memNameStack.pop();

            int index = -1;
            if (curMemberPtr != nullptr)
            {
                //寻找成员变量所在的下标
                for (unsigned int i = 0; i < curMemberPtr->memberIndex.size(); i++)
                {
                    if (curName == curMemberPtr->memberIndex[i])
                    {
                        index = i;
                        break;
                    }
                }
                if (index >= 0)
                    curMemberPtr = curMemberPtr->memberType[index];//更新为下一层Member*
                else//没找到，不合法
                    break;
            }
            res = index;
        }// end of while
    }// end of if
    else
        ;

    return res;
}

std::string GetOpStr(BinaryOpKind op)
{
    switch (op)
    {
    case BinaryOpKind::plusKind:
        return "+";
        break;
    case BinaryOpKind::minusKind:
        return "-";
        break;
    case BinaryOpKind::orKind:
        return "or";
        break;
    case BinaryOpKind::mulKind:
        return "*";
        break;
    case BinaryOpKind::divRealKind:
        return "/";
        break;
    case BinaryOpKind::divKind:
        return "div";
        break;
    case BinaryOpKind::modKind:
        return "mod";
        break;
    case BinaryOpKind::andKind:
        return "and";
        break;
    case BinaryOpKind::geKind:
        return ">=";
        break;
    case BinaryOpKind::gtKind:
        return ">";
        break;
    case BinaryOpKind::leKind:
        return "<=";
        break;
    case BinaryOpKind::ltKind:
        return "<";
        break;
    case BinaryOpKind::eqKind:
        return "=";
        break;
    case BinaryOpKind::ueqKind:
        return "<>";
        break;
    case BinaryOpKind::assignmentKind:
        return "赋值";
        break;
    default:
        return "balabala";
        break;
    }
}
