#include "FunctionAST.h"
#include "BlockTable.h"

Value *FunctionAST::Codegen()
{        
    //std::cout << "function: " << name << std::endl;
    std::vector<llvm::Type *> arg_types;
        
    //为本函数新建符号表的堆栈，该函数符号表地址为tmps
	BlockTable *tmps = nullptr;
	if(blockStack.size())
    	tmps = new BlockTable(blockStack.top(), name);
	else
	    tmps = new BlockTable(nullptr, name);

        blockStack.push(tmps);
    auto toLLVMType = [](TypeName type, int ifref) -> Type * {
            if (!ifref)
            {
                switch (type)
                {
                    case TypeName::Integer:
                        return llvm::Type::getInt32Ty(*TheContext);
                        break;
                    case TypeName::Real:
                        return llvm::Type::getDoubleTy(*TheContext);
                        break;
                    case TypeName::Char:
                        return llvm::Type::getInt8Ty(*TheContext);
                        break;
                    case TypeName::Bool:
                        return llvm::Type::getInt1Ty(*TheContext);
                        break;
                    default:
                        return llvm::Type::getVoidTy(*TheContext);
                        break;
                }
            }
            else
            {
                switch (type)
                {
                    case TypeName::Integer:
                        return llvm::Type::getInt32PtrTy(*TheContext);
                        break;
                    case TypeName::Real:
                        return llvm::Type::getDoublePtrTy(*TheContext);
                        break;
                    case TypeName::Char:
                        return llvm::Type::getInt8PtrTy(*TheContext);
                        break;
                    case TypeName::Bool:
                        return llvm::Type::getInt1PtrTy(*TheContext);
                        break;
                    default:
                        return llvm::Type::getVoidTy(*TheContext);
                        break;
                }
            }
        };
        
        //处理函数签名
        for (int i = 0; i < headerDecl.size(); i++)
        {
            //std::cout << "args decl" << std::endl;
            for (int j = 0; j < headerDecl[i]->variableName.size(); j++)
            {
                int IsRef = headerDecl[i]->isRef;
                arg_types.push_back(toLLVMType(headerDecl[i]->type->GetType(), IsRef));
            }
        }
		
        
        //函数返回值类型
       // std::cout << "create f_type" << std::endl;
        auto f_type = llvm::FunctionType::get(this->isProcedure() ? llvm::Type::getVoidTy(*TheContext) : returnType->toLLVMType(), llvm::makeArrayRef(arg_types), false);
        
       // std::cout << "create function" << std::endl;
        
        //创建函数声明
        Function *Thefunc = llvm::Function::Create(f_type, llvm::GlobalValue::ExternalLinkage, name.c_str(), TheModule.get());
        
        // 加入引用属性
        for (int i = 0; i < headerDecl.size(); i++)
        {
           // std::cout << "args attributes added : " << std::endl;
         
            int IsRef = headerDecl[i]->isRef;
            if (IsRef)
            {
               Thefunc->addDereferenceableParamAttr(i, 8);
            }
            
        }
        
        builder->SetInsertPoint(BasicBlock::Create(*TheContext, name + "_entry", Thefunc));
        
        // 处理参数 ： 引用、传值
        llvm::Value *arg_value;
        //std::cout << "create args_values" << std::endl;
        auto args_values = Thefunc->arg_begin();
        for (int i = 0; i < headerDecl.size(); i++)
        {
           // std::cout << "create code for headerDecl" << std::endl;
            auto tmp = headerDecl[i]->Codegen();
           // std::cout << "set argValue" << std::endl;
            for (int j = 0; j < headerDecl[i]->variableName.size(); j++)
            {
                
               // std::cout << "args_values++" << std::endl;
                arg_value = args_values++;
                
               // std::cout << "set Name" << std::endl;
                arg_value->setName(headerDecl[i]->variableName[j].c_str());
                
               // std::cout << "Create store" << std::endl;
                builder->CreateStore(arg_value, tmp);
            }
        }

        
        // Ret 部分
        
        if (this->isFunction())
        {
            //std::cout << "Creating function return value declaration" << std::endl;
            auto retVal = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), returnType->toLLVMType(), std::string("ret"));
            
            if (tmps->InsertNameValue(this->name, retVal))
            {
               // std::cout << "The " << name << " has inserted successfully\n";
            }
        }
        
        
        
            // deal with constant define
        for (int i = 0; i < consts.size(); i++)
        {
            consts[i]->Codegen();
        }

        
        for (int i = 0; i < selfdefineType.size(); i++)
        {
            selfdefineType[i]->Codegen();
        }
        
        // deal with variable declaration
        for (int i = 0; i < bodyDecl.size(); i++)
        {
            bodyDecl[i]->Codegen();
        }

        //处理内嵌子函数
        for (int i = 0; i < functions.size(); i++)
        {
            blockStack.top()->Insert(functions[i]->name, functions[i], {0,0}, functions[i]->returnType);
		    functions[i]->Codegen();
        }



        
        // deal with program statements
        for (int i = 0; i < body.size(); i++)
        {
	     	if (!blockStack.top()->CheckStatement(body[i])){
			std::cout << name << "one body illegal" << std::endl;	
			exit(1);
	    	}
	   
            if(body[i])
			{
			
		        if(!body[i]->Codegen())
                {
                    std::cout << "函数"<<name<<"的body"<<i<<"生成失败" << std::endl;
                    exit(1);
                }
		    }
	    }
        
        // return value
        if (this->isFunction())
        {
           // std::cout << "Generating return value for function" << std::endl;
            
            auto load_ret = builder->CreateLoad(tmps->SearchNameValue(this->name), "return_value_final");
          //  std::cout << "func "<< name << " load ret value success! "<< std::endl; 
            builder->CreateRet(load_ret);
            /* free return value */
          //  std::cout << "func "<< name << " create ret success! "<< std::endl;
           //处理
            tmps->DeleteNameValue(this->name);
        }
        else
        {
           // std::cout << "Generating return void for procedure" << std::endl;
            builder->CreateRetVoid();
        }
        
        blockStack.pop();
	if(!blockStack.empty()){
	    Function* fatherFunc = TheModule->getFunction((blockStack.top()->GetName()).c_str());
	    builder->SetInsertPoint(&(fatherFunc->back()));
        } 
	
	delete tmps;
	// Validate the generated code, checking for consistency.
        verifyFunction(*Thefunc);

    // Optimize the function.
        TheFPM->run(*Thefunc);
	//std::cout << "TheFPM-------------------" << std::endl;
        return Thefunc;
    }