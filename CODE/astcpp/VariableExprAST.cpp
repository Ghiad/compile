#include "VariableExprAST.h"
#include "BlockTable.h"

Value *VariableExprAST::Codegen()
{
    //查ast
    Node *var = blockStack.top()->SearchAll(name);
    if (var->GetNodeKind() == NodeKind::constDecl || var->GetNodeKind() == NodeKind::variableDecl)
    {
        //从符号表中查找本该变量，获取变量的Value *值 
        auto val = blockStack.top()->SearchNameValue(name); 
        //std::cout << "pick variable:" << name << std::endl;
        return val;
    }
    else if(var->GetNodeKind() == NodeKind::functionDecl)
    {
        auto val = blockStack.top()->SearchNameValue(name); 
        //std::cout << "pick the return value's addr :" << name << std::endl;
        return val;
    }
    else
    {
        //std::cout << "Not a valid variable type." << std::endl;
    }
}