#ifndef BITOPERATIONS_H_INCLUDED
#define BITOPERATIONS_H_INCLUDED

#include <stdio.h>
#include "FrequentlyUsedData.h"

/*PrintBits prints the bits representing a given number, from MSB on the left to LSB on the right*/
void PrintBits(FILE* file, short num);

/*TurnBitOn turns on a specified bit, of a given index */
unsigned short TurnBitOn(unsigned short num, unsigned short bitIndex);

/*Turn few bits on (up to all bits) according to a given mask*/
unsigned short TurnGivenBitsOn(unsigned short num, unsigned short bitMask);

/*'TwosComplement' converts negative numbers to it's TwosComplement representation, but leaves positive numbers as is*/
unsigned short TwosComplement(short num);

/*Returns 'n' bits from position 'start' to the LSB, adjusted to the right.
Please note: rightmost LSB bit index is 0 and leftmost MSB index is 15
If n is larger than the amount of bits between 'start' index and LSB, all bits between 'start' and LSB are returned (adjusted to the right!!)
*/
unsigned short GetNBits(unsigned short num, unsigned short start, unsigned short n);

/*Returns bits from index 'start' to 'end', including the bit at index 'end'.
If 'shouldAdjustSign' is true and the bit at index 'end' is on, all the bits from index
Please note: 'start' and 'end' are indexes from LSB (LSB bit index==0, MSB index=15)!!
If start>end, 0 is returned !!*/
unsigned short GetBitsByIndex(unsigned short num, unsigned short start, unsigned short end, Boolean shouldAdjustSign);
#endif
