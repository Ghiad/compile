#include "ConstValue.h"

void constValue::NegateValue()
{
    i = -i;
    d = -d;
}

bool constValue::Compare(constValue checkValue) const
{
    if (checkValue.i == i && checkValue.b == b && checkValue.d == d && checkValue.c == c)
        return true;
    else
        return false;
}

void constValue::IntBinOp(int val1, int val2, BinaryOpKind op)
{
    switch (op)
    {
        case BinaryOpKind::plusKind: //+
            i = val1 + val2;
            break;
            
        case BinaryOpKind::minusKind: //-
            i = val1 - val2;
            break;
            
        case BinaryOpKind::orKind: //or
            i = val1 | val2;
            break;
            
        case BinaryOpKind::mulKind: //*
            i = val1 * val2;
            break;
            
        case BinaryOpKind::divRealKind: // /
        {
            if (val2 != 0)
                d = (1.0 * val1) / (1.0 * val2);
            else
                d = 0;
            break;
        }
            
        case BinaryOpKind::divKind: // /
            i = val1 / val2;
            break;
            
        case BinaryOpKind::modKind:
            i = val1 % val2;
            break;
            
        case BinaryOpKind::andKind:
            i = val1 & val2;
            break;
            
        case BinaryOpKind::geKind:
            b = val1 >= val2;
            break;
            
        case BinaryOpKind::gtKind:
            b = val1 > val2;
            break;
            
        case BinaryOpKind::leKind:
            b = val1 <= val2;
            break;
        case BinaryOpKind::ltKind:
            b = val1 < val2;
            break;
        case BinaryOpKind::eqKind:
            b = val1 == val2;
            break;
            
        case BinaryOpKind::ueqKind:
            b = val1 != val2;
            break;
        default:
            break;
    }
    
}

void constValue::RealBinOp(double val1, double val2, BinaryOpKind op)
{
    switch (op)
    {
            
        case BinaryOpKind::plusKind: //+
            d = val1 + val2;
            break;
            
        case BinaryOpKind::minusKind: //-
            d = val1 - val2;
            break;
            
        case BinaryOpKind::mulKind: //*
            d = val1 * val2;
            break;
            
        case BinaryOpKind::divRealKind: // /
        {
            if (val2 != 0)
                d = val1 / val2;
            else
                d = 0;
            break;
        }
            
        case BinaryOpKind::geKind:
            b = val1 >= val2;
            break;
            
        case BinaryOpKind::gtKind:
            b = val1 > val2;
            break;
            
        case BinaryOpKind::leKind:
            b = val1 <= val2;
            break;
        case BinaryOpKind::ltKind:
            b = val1 < val2;
            break;
        case BinaryOpKind::eqKind:
            b = val1 == val2;
            break;
            
        case BinaryOpKind::ueqKind:
            b = val1 != val2;
            break;
        default:
            break;
    }
}

void constValue::CharBinOp(char val1, char val2, BinaryOpKind op)
{
    switch (op)
    {
        case BinaryOpKind::geKind:
            b = val1 >= val2;
            break;
            
        case BinaryOpKind::gtKind:
            b = val1 > val2;
            break;
            
        case BinaryOpKind::leKind:
            b = val1 <= val2;
            break;
        case BinaryOpKind::ltKind:
            b = val1 < val2;
            break;
        case BinaryOpKind::eqKind:
            b = val1 == val2;
            break;
            
        case BinaryOpKind::ueqKind:
            b = val1 != val2;
            break;
        default:
            break;
    }
}

void constValue::BoolBinOp(bool val1, bool val2, BinaryOpKind op)
{
    switch (op)
    {
        case BinaryOpKind::orKind: //or
            b = val1 || val2;
            break;
        case BinaryOpKind::andKind:
            b = val1 && val2;
            break;
            
        case BinaryOpKind::geKind:
            b = val1 >= val2;
            break;
            
        case BinaryOpKind::gtKind:
            b = val1 > val2;
            break;
            
        case BinaryOpKind::leKind:
            b = val1 <= val2;
            break;
        case BinaryOpKind::ltKind:
            b = val1 < val2;
            break;
        case BinaryOpKind::eqKind:
            b = val1 == val2;
            break;
            
        case BinaryOpKind::ueqKind:
            b = val1 != val2;
            break;
        default:
            break;
    }
}