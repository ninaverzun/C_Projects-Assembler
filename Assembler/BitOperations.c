#include <stdio.h>
#include <math.h>
#include "BitOperations.h"

#define SIZEOF_BIT 8
#define SIZEOF_WORD 15
#define IS_BIT_ON(var,bitIndex) (var&(1<<bitIndex))!=0

/*** For documentation see "BitOperations.h"!!! ***/

void PrintBits(FILE* file, short num)
{
    int i;
    unsigned short binary=num>0?(unsigned short)num:TwosComplement(num);

    for(i=SIZEOF_WORD-1; i>=0; i--)
    {
        fprintf(file, "%u", IS_BIT_ON(binary,i));
    }
    fprintf(file,"\n");
}

unsigned short TurnBitOn(unsigned short num, unsigned short bitIndex)
{
    if(bitIndex>SIZEOF_BIT*sizeof(num))
    {
        fprintf(stderr, "\nTurn bit on function parameter is out of range!\n");
        return num;
    }

    return num|(1<<bitIndex);
}

unsigned short TurnGivenBitsOn(unsigned short num, unsigned short bitMask)
{
    return num|bitMask;
}

unsigned short TwosComplement(short num)
{
    unsigned short absoluteVal;

    if(num>=0)
    {
        return (unsigned short)num;
    }
    absoluteVal=fabs(num);
    return ((~absoluteVal)+1);
}

/*start is an index from LSB (LSB bit index==0!!)*/
unsigned short GetNBits(unsigned short num, unsigned short start, unsigned short n)
{
    return (num>>(start+1-n)) & ~(~0 <<n);
}

/*'start' and 'end' are indexes from LSB (LSB bit index==0, MSB index=15!!)
If start>end, 0 is returned !!*/
unsigned short GetBitsByIndex(unsigned short num, unsigned short start, unsigned short end, Boolean shouldAdjustSign)
{
    unsigned short amountOfBits=0, result=0;

    if(end<start)
    {
        return 0;
    }
    amountOfBits=end-start+1;
    result= GetNBits(num, end, amountOfBits);

    if(shouldAdjustSign && (IS_BIT_ON(num, end)))
    {
        result|=(~0 <<amountOfBits);
    }
    return result;
}
