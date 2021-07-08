#include "ArrayTypeAST.h"
#include "BlockTable.h"

bool ArrayTypeAST::CheckLegal()
{
    if (type == nullptr)
        return false;

    if (isChecked)
        return checkResult;

    isChecked = true;

    int startSize = startIndexs.size();
    int endSize = endIndexs.size();

    if (startSize != endSize)
        return false;

    //检查下标是否合法
    for (int i = 0; i < startSize; i++)
    {
        auto startIndex = startIndexs[i];
        auto endIndex = endIndexs[i];

        if (startIndex == nullptr
            || endIndex == nullptr)
        {
            std::cout << "error line" << std::setw(3) << lineNum;
            std::cout << ":第" << i + 1 << "对下标不合法" << std::endl;
            return false;
        }

        //上标和下标类型是否合法
        auto startType = startIndex->type;
        auto endType = endIndex->type;
        if (endType == realTypeAST
            || startType == realTypeAST)
        {
            std::cout << "error line" << std::setw(3) << lineNum;
            std::cout << ":第" << i + 1 << "对下标类型不合法：不允许为Real型" << std::endl;
            return false;
        }
        if (startType != endType)
        {
            std::cout << "error line" << std::setw(3) << lineNum;
            std::cout << ":第" << i + 1 << "对下标类型不匹配：";
            std::cout << "起始类型为" << GetTypeStr(startType)
                << ", 终止类型为" << GetTypeStr(endType) << std::endl;
            return false;
        }

        blockStack.top()->HandleConst(startIndex);
        blockStack.top()->HandleConst(endIndex);

        //上标和下标的值是否合法:上标要小于等于下标
        switch (startType->type)
        {
        case TypeName::Integer:
        {
            if (startIndex->value.i > endIndex->value.i)
            {
                std::cout << "error line" << std::setw(3) << lineNum;
                std::cout << ":第" << i + 1 << "对下标的起始值大于终止值" << std::endl;
                return false;
            }
            int size = endIndex->value.i - startIndex->value.i + 1;
            dimenSize.push_back(size);
            break;
        }
        case TypeName::Char:
        {
            if (startIndex->value.c > endIndex->value.c)
            {
                std::cout << "error line" << std::setw(3) << lineNum;
                std::cout << ":第" << i + 1 << "对下标的起始值大于终止值" << std::endl;
                return false;
            }
            int size = endIndex->value.c - startIndex->value.c + 1;
            dimenSize.push_back(size);
            break;
        }
        default://BasicType::Bool
        {
            if (startIndex->value.b > endIndex->value.b)
            {
                std::cout << "error line" << std::setw(3) << lineNum;
                std::cout << ":第" << i + 1 << "对下标的起始值大于终止值" << std::endl;
                return false;
            }
            int size = endIndex->value.b - startIndex->value.b + 1;
            dimenSize.push_back(size);
            break;
        }
        }
    }
    if (!type->CheckLegal())
        return false;

    checkResult = true;
    return true;
}

bool ArrayTypeAST::Compare(ArrayTypeAST *checkArray) const
{
        //比对下标
    int myDimen = startIndexs.size();
    int checkDimen = checkArray->startIndexs.size();
        //数组维数是否相同
    if (myDimen != checkDimen)
        return false;
    
        //比较各个维度
    for (int i = 0; i < myDimen; i++)
    {
            //各个维度的大小是否相同
        if (dimenSize[i] != checkArray->dimenSize[i])
            return false;
        
            //该维度的下标类型是否相同
        if (startIndexs[i]->type != checkArray->startIndexs[i]->type)
            return false;
        
            //下标的起始值是否相同
        if (!startIndexs[i]->value.Compare(checkArray->startIndexs[i]->value))
            return false;
    }
    
        //比较type
    return blockStack.top()->CompareType(type, checkArray->type, false);
}

    //检查数组的下标使用是否合法
bool ArrayTypeAST::CheckIndex(std::vector<ExprAST*> indexes, int line) const
{
    int indexSize = startIndexs.size();//定义的下标对数
    int checkSize = indexes.size();//要检查的下标对数

    if (indexSize != checkSize)
    {
        //报错，数组下标个数不符合
        std::cout << "error line" << std::setw(3) << line;
        std::cout << ":数组下标对数与定义不符, 期待得到" << indexSize << "对下标, 实际得到" << checkSize << "对下标" << std::endl;
        return false;
    }

    //逐个遍历各个下标是否合法，包含类型和值的范围
    //类型一定要匹配，如果是变量或者函数，可以不匹配值，常量一定要匹配值
    for (int i = 0; i < indexSize; i++)
    {
        BasicTypeAST* indexType = startIndexs[i]->type;//该对下标定义的类型
        constValue startValue = startIndexs[i]->value;
        constValue endValue = endIndexs[i]->value;

        bool isVar = false, isptr = false;

        auto curCheckIndexType = blockStack.top()->GetExprType(indexes[i], &isVar, &isptr);//数组下组类型都是Basic类型

            //下标类型不符合条件
        if (curCheckIndexType != indexType)
        {
            std::cout << "error line" << std::setw(3) << indexes[i]->lineNum;
            std::cout << ":表达式期待是" << GetTypeStr(indexType) << ",得到的是" << GetTypeStr(curCheckIndexType) << std::endl;
            return false;
        }

        //如果是常量，则还需要匹配值的范围
        if (!isVar)
        {
            switch (indexes[i]->expr_type)
            {
            case ExprType::NumberExpr:
            {
                int indexVal = ((NumberExprAST*)indexes[i])->val;
                if (indexVal < startValue.i || indexVal > endValue.i)
                {
                    //报错，不符合的下标值范围
                    std::cout << "error line" << std::setw(3) << indexes[i]->lineNum;
                    std::cout << ":下标范围期待是" << startValue.i << "-" << endValue.i;
                    std::cout << ", 当前下标值为" << indexVal << std::endl;
                    return false;
                }
                break;
            }
            case ExprType::BoolExpr:
            {
                bool indexVal = ((BoolExprAST*)indexes[i])->val;
                if (indexVal < startValue.b || indexVal > endValue.b)
                {
                    //报错，不符合的下标值范围
                    std::cout << "error line" << std::setw(3) << indexes[i]->lineNum;
                    std::cout << ":下标范围期待是" << startValue.b << "-" << endValue.b;
                    std::cout << ", 当前下标值为" << indexVal << std::endl;
                    return false;
                }
                break;
            }
            case ExprType::CharExpr:
            {
                char indexVal = ((CharExprAST*)indexes[i])->val;
                if (indexVal < startValue.c || indexVal > endValue.c)
                {
                    //报错，不符合的下标值范围
                    std::cout << "error line" << std::setw(3) << indexes[i]->lineNum;
                    std::cout << ":下标范围期待是" << startValue.c << "-" << endValue.c;
                    std::cout << ", 当前下标值为" << indexVal << std::endl;
                    return false;
                }
                break;
            }

            case ExprType::Variable:
            {
                auto varExpr = (VariableExprAST*)indexes[i];
                auto varNode = blockStack.top()->SearchAll(varExpr->name);
                if (varNode->GetNodeKind() == NodeKind::constDecl)//如果是常量类型还要匹配值
                {
                    constValue indexVal = ((ConstAST*)varNode)->value;
                    if (!IsBetween(indexType->GetType(), startValue, endValue, indexVal, line))
                        return false;
                }
                break;
            }

            case ExprType::BinaryExpr:
            {
                auto binExpr = (BinaryExprAST*)indexes[i];
                constValue indexVal = *(binExpr->constVal);
                if (!IsBetween(indexType->GetType(), startValue, endValue, indexVal, line))
                    return false;
                break;
            }

            case ExprType::UnaryExpr:
            {
                auto unaryExpr = (UnaryExprAST*)indexes[i];
                constValue indexVal = *(unaryExpr->constVal);
                if (!IsBetween(indexType->GetType(), startValue, endValue, indexVal, line))
                    return false;
                break;
            }

            default://其他类型不合法
            {
                return false;
                break;
            }
            }
        }//end of if

    }//end of for

    return true;
}