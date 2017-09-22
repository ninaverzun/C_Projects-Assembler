#ifndef MACHINECODETRANSLATIONS_H_INCLUDED
#define MACHINECODETRANSLATIONS_H_INCLUDED
#include <string.h>
#include <math.h>
#include "BitOperations.h"

#define BITS_SHIFT_TO_FIT_A_R_E 2
#define SOURCE_ADDR_BITS_SHIFT 4
#define DEST_ADDR_BITS_SHIFT 2
#define OPCODE_BITS_SHIFT 6
#define GROUP_BITS_SHIFT 10
#define UNUSED_BITS_SHIFT 12
#define SOURCE_REGISTER_BITS_SHIFT 8
#define DEST_REGISTER_BITS_SHIFT 2

#define UNUSED_BITS_VAL 5
#define OCTAL_DIGITS 8
#define MAX_DIGITS 16

#define OPERATIONS 16
#define DIRECTIVES 4
#define REGISTERS_COUNT 10
#define ADDRESSING_TYPES 4

#define ADDRESSING_TYPE(val) (val==Immediate?"Immediate":(val==Direct?"Direct":(val==Dynamic?"Dynamic":"Register")))

typedef enum operationGroupType{None=0, One=1, Two=2} OperationGroupType;
typedef enum operandType{Source=0, Dest=1} OperandType;
typedef enum allocationType{Absolute=0, External=1, Relocatable=2} AllocationType;
typedef enum addressingType{Immediate=0, Direct=1, Dynamic=2, Register=3} AddressingType;
typedef enum operationType{mov=0, cmp=1, add=2, sub=3, not=4, clr=5, lea=6, inc=7, dec=8, jmp=9, bne=10, red=11, prn=12, jsr=13, rts=14, stop=15} OperationType;
typedef enum registerType{r1=0, r2=1, r3=2, r4=3, r5=4, r6=5, r7=6, PC=7, SP=8, PSW=9} RegisterType;

/*Sets A-R-E bits on for numbers or addresses which represent the operation's second and third memory words
Returns: the modified word
Parameters: the original word and the allocation type*/
unsigned short ShiftAndSetAllocationType(unsigned short number, AllocationType allocType);

/*Sets A-R-E bits on. Works on the operation's first memory word
Returns the modified word
Parameters: the original word and the allocation type*/
unsigned short SetAllocationType(unsigned short number, AllocationType allocType);

/*Sets operand addressing type bits on.
Works on the operation's first memory word:
-   bits 2, 3 for destination operand addressing type
-   bits 4, 5 for source operand addressing type
Returns: the modified word
Parameters: the original word, the addressing type and the operand type*/
unsigned short SetOperandAddressingType(unsigned short number, AddressingType addressingType, OperandType opType);

/*Sets operand register type bits on.
Works on the operation's additional memory words:
-   bits 2-7 for destination operand
-   bits 8-13 for source operand
Returns: the modified word
Parameters: the original word, the register type and the operand type*/
unsigned short SetOperandRegisterType(unsigned short number, RegisterType registerType, OperandType opType);

/*Sets operation type bits on. Works on bits 6-9 of the operation first memory word
Returns: the modified word
Parameters: the original word and operation type*/
unsigned short SetOperationType(unsigned short number, OperationType operation);

/*Sets operation group  type bits on. Works on bits 10, 11 of the operation first memory word.
Returns: the modified word
Parameters: the original word and operation type*/
unsigned short SetOperationGroupType(unsigned short number, OperationType operation);

/*Returns: the operation group type, according to the OperationType
Parameters: the operation type*/
OperationGroupType GetOperationGroupType(OperationType operation);

/*Returns: the operand addressing type, according to the provided operand type and operation representation*/
AddressingType GetOperandAddressingType(unsigned short operationNum, OperandType operandType);

/*Sets unused operation bits to 101. Works on bits 12-14 of the operation first memory word
Returns: the modified word
Parameters: the original word*/
unsigned short SetUnusedOperationBits(unsigned short number);

/*Translates a decimal number to an octal number, represented by digits of the special octal base: "!@#$%^&*"
The translation result is set to the string "result". "result" must be big enough to hold the translated number
Parameters: a decimal number and a char array to save the result into*/
void TranslateToSpecialOctaBase(unsigned short num, char* result);

/*Sets the corresponding bits of a 15 bits representation of an operation according to the assembler rules*/
unsigned short GetOperationRepresentation(OperationType opType, OperationGroupType group, AddressingType sourceOperandAddr, AddressingType destOperandAddr);
#endif
