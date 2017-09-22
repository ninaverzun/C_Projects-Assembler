#include "MachineCodeTranslations.h"
#include "BitOperations.h"

/*For documentation see "MachineCodeTranslations.h"!!! */

unsigned short ShiftAndSetAllocationType(unsigned short number, AllocationType allocType)
{
    unsigned short result=number>0?number:TwosComplement(number);
    result=TurnGivenBitsOn(result<<BITS_SHIFT_TO_FIT_A_R_E, allocType);
    return result;
}

unsigned short SetAllocationType(unsigned short number, AllocationType allocType)
{
     return TurnGivenBitsOn(number, allocType);
}

unsigned short SetOperandAddressingType(unsigned short number, AddressingType allocType, OperandType opType)
{
    unsigned short result = (opType==Source)?
    TurnGivenBitsOn(number, (allocType<<SOURCE_ADDR_BITS_SHIFT)):
    TurnGivenBitsOn(number, (allocType<<DEST_ADDR_BITS_SHIFT));

    return result;
}

unsigned short SetOperandRegisterType(unsigned short number, RegisterType registerType, OperandType opType)
{
    unsigned short result = (opType==Source)?
    TurnGivenBitsOn(number, ((registerType+1)<<SOURCE_REGISTER_BITS_SHIFT)):
    TurnGivenBitsOn(number, ((registerType+1)<<DEST_REGISTER_BITS_SHIFT));

    return result;
}

AddressingType GetOperandAddressingType(unsigned short operationNum, OperandType operandType)
{
    AddressingType result=0;
    int index=operandType==Source?SOURCE_ADDR_BITS_SHIFT:DEST_ADDR_BITS_SHIFT;

    /*get addressing type bits: Immediate=00, Direct=01, Dynamic=10, Register=11*/
    result=GetBitsByIndex(operationNum, index, index+1, False);
    return result;
}

unsigned short SetOperationType(unsigned short number, OperationType operation)
{
    unsigned short result=TurnGivenBitsOn(number, operation<<OPCODE_BITS_SHIFT);
    return result;
}

OperationGroupType GetOperationGroupType(OperationType operation)
{
    OperationGroupType group=None;

    if(operation==mov || operation==cmp || operation==add || operation==sub || operation==lea)
    {
        group=Two;
    }
    else if(operation==rts || operation==stop)
    {
        group=None;
    }
    else
    {
        group=One;
    }
    return group;
}

unsigned short SetOperationGroupType(unsigned short number, OperationType operation)
{
    OperationGroupType group=GetOperationGroupType(operation);

    return TurnGivenBitsOn(number, group<<GROUP_BITS_SHIFT);
}

unsigned short SetUnusedOperationBits(unsigned short number)
{
    return TurnGivenBitsOn(number, UNUSED_BITS_VAL<<UNUSED_BITS_SHIFT);
}

unsigned short GetOperationRepresentation(OperationType opType, OperationGroupType group, AddressingType sourceOperandAddr, AddressingType destOperandAddr)
{
    unsigned short result=0;
    result = SetUnusedOperationBits(result);
    result = SetOperationGroupType(result, opType);
    result = SetOperationType(result, opType);
    if(group==Two)
    {
        result = SetOperandAddressingType(result, sourceOperandAddr, Source);
    }
    if(group!=None)
    {
        result = SetOperandAddressingType(result, destOperandAddr, Dest);
    }
    return result;
}

void TranslateToSpecialOctaBase(unsigned short decimalNum, char* result)
{
    static char* specialOctalBase="!@#$%^&*", octalDigits[MAX_DIGITS];
    unsigned short temp=decimalNum;
    int i=0, j=0;

    if(decimalNum==0)
    {
        strcpy(result,"!");
        return;
    }
    while(temp!=0)
    {
        j=fabs(temp%OCTAL_DIGITS);
        octalDigits[i++]=specialOctalBase[j];
        temp/=OCTAL_DIGITS;
    }

    for(j=0; j<i;j++)
    {
        result[j]=octalDigits[i-j-1];
    }
    result[j]='\0';
}
