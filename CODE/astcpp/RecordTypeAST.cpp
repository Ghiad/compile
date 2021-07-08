#include "RecordTypeAST.h"
#include "BlockTable.h"

bool RecordTypeAST::CheckLegal()
{
    if (isChecked)
        return checkResult;
    isChecked = true;

    std::vector<std::string> existName;
    for (auto varList : declList)
    {
        //先检查名字是否重复
        for (auto curVarName : varList->variableName)
        {
            for (auto curName : existName)
            {
                if (curName == curVarName)
                {
                    std::cout << "error line" << std::setw(3) << lineNum;
                    std::cout << ":不允许定义重复的成员变量" << std::endl;
                    return false;
                }
            }
            existName.push_back(curVarName);
        }
        //再检查类型是否合法
        if (!varList->type->CheckLegal())
            return false;
    }
    checkResult = true;
    return true;
}

VariableDeclAST *RecordTypeAST::SearchVar(std::string name) const
{
    for (auto var : declList)
    {
        for (auto curName : var->variableName)
        {
            if (curName == name)
            {
                return var;
            }
        }
    }
    return nullptr;
}