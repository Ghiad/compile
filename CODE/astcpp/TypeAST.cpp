#include "TypeAST.h"
#include "ArrayTypeAST.h"
#include "RecordTypeAST.h"

Constant *toLLVMConst(TypeAST *ast)
{
    TypeName type = ast->GetType();
    auto llvmType = ast->toLLVMType();
    switch (type)
    {
        case TypeName::Integer:
        {
            return builder->getInt32(0);
            break;
        }
        case TypeName::Real:
        {
            return ConstantFP::get(Type::getDoubleTy(*TheContext), 0.0);
            break;
        }
        case TypeName::Char:
        {
            return builder->getInt8('c');
            break;
        }
        case TypeName::Bool:
        {
            return builder->getInt1(false);
            break;
        }
        case TypeName::Record:
        {
            StructType* tmpStruct = dyn_cast<StructType>(llvmType);
            //return tmp->elements();
            RecordTypeAST* tmpAST = (RecordTypeAST*)(ast);
            std::vector<Constant *> const_struct_elems;
            for(int i=0; i<tmpStruct->getNumElements(); i++)
            {
                const_struct_elems.push_back(toLLVMConst((tmpAST->declList[i])->type ) );
            }
            return ConstantStruct::get(tmpStruct, const_struct_elems);
            break;
        }
        case TypeName::Array:
        {
            std::vector<Constant *> const_array_elems;
            ArrayTypeAST* tmpArray = (ArrayTypeAST*)(ast);
                // ArrayTypeAST* tmp = dyn_cast<ArrayTypeAST>(ast);
	    int count = 0;
            for(int i = 0; i < tmpArray->dimenSize.size(); i++) {
				for(int j = 0; j < tmpArray->dimenSize[i]; j++) {
					count++;		
					const_array_elems.push_back(toLLVMConst(tmpArray->type));
				}
			}
	    //std::cout << "---const array elem count:" << count << std::endl;
            return ConstantArray::get(dyn_cast<ArrayType>(llvmType), const_array_elems);
            break;
        }
        default:

            return nullptr;
            break;
    }
}