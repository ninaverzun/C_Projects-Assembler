#ifndef ASSEMBLERSCAN_H_INCLUDED
#define ASSEMBLERSCAN_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "FileAndParsingOperations.h"
#include "ErrorHandling.h"
#include "SymbolTable.h"

#define MEMORY_OFFSET 100

#define COMMENT_TOKEN ';'
#define SYMBOL_NAME_LAST_TOKEN ':'
#define DIRET_ADDR_TOKEN '#'

#define EXTERN_ENTRY_PARAMS 1
#define STRING_PARAMS 1
#define DATA_MIN_PARAMS 1

#define SOURCE_OP 1
#define DEST_OP 2
#define DECIMAL 10
#define DYNAMIC_OFFSET_MIN 0
#define DYNAMIC_OFFSET_MAX 14

/*************************** First scan: **************************

Reads the input file tine after line.
Fills operations and symbols arrays according to assembler translation rules.
Adds every declared symbol to the symbol table. Records errors if necesarry.
Validates the syntax and reports all encountered errors

Skipps:
-   Comment lines which start with ";"
-   Empty lines
*/
void FirstScan(FILE* input, unsigned short* operations, unsigned short* opCount, unsigned short* symbols, unsigned short* dataCount, SymbolTable* symbolTable, ErrorTable* errors);

/************************** Second scan: **************************

Reads the input file line after line for the second time.
Fills gaps (symbol values) in the operations array.
No redundant error reporting is done! The only possible errors for the second scan are:
-   Symbol was not declared

After completing the second scan, if no errors found, an ".obj", ".ext" and ".ent" files are created and relevant data is exported*/
void SecondScan(FILE* input, unsigned short* operations, unsigned short opCount, unsigned short* directives, unsigned short dataCount, SymbolTable* symbolTable, ErrorTable* errors, char* inputFileName);

/*Gets a specific line word count. Returns 'True' if it's a blank line or 'False' otherwise*/
Boolean IsBlankLine(int wordCount);

/*Gets the separated words of a specific line. Returns 'True' if the line is a comment, 'False' otherwise*/
Boolean IsComment(char* params[LENGTH]);

/*Checks if a given symbol name is valid. returns True if it does or False otherwise.
According to assembler rules, a symbol name must comply with the following rules:
1. mustn't be a register name: r1, r2, r3, r4, r5, r6, r7, R1, R2, R3, R4, R5, R6 or R7
2. must start with a letter (small or capital)
3. no delimeters allowed as the first letter (no ' ', '\t' etc...)
4. no longer than 30 characters
*/
Boolean IsValidSymbolName(char* name, Boolean);

/*Verifies if a given name is one of the valid register names: r1, r2, r3, r4, r5, r6, r7, pc, sp, R1, R2, R3, R4, R5, R6, R7, PC, SP
Returns 'True' if it is or 'False' otherwise*/
Boolean IsValidRegisterName(char* name, RegisterType* registerTypePtr);

/*Checks if a given name is a register name (consists of a 'r' or 'R' first character and an adjacent decimal number)*/
Boolean IsRegisterName(char* name);

/*A valid dynamic addressing operand (X[5-9] for example) must contain a valid symbol name, adjacent to a '[' and must end with a ']' character.
Between the square brackets, we expect two decimal numbers between 0 and 12, separated by a '-' character
Returns 'True' if valid and 'False' otherwise. Sets incapsulated symbolName, start bit and end bit into the given pointers*/
Boolean IsValidDynamicAddressing(char* name, char* symbolName, unsigned short* startBit, unsigned short* endBit);

/*Checks weather a given string qualifies as a valid operation name. Returns 'True' if it does and 'False' if a non valid name is given.
In case of a valid operation name, a corresponding operation type is set to second parameter's address.
There are only 16 possible operations: "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts" and "stop"
*/
Boolean IsValidOperationName(char*, OperationType*);

/*Checks weather a given string qualifies as a valid directive name. Returns 'True' if it does and 'False' if a non existing name is given.
There are only 4 possible directives: ".entry", ".extern", ".string", ".data"
*/
Boolean IsValidDirectiveName(char*);

/*Checks if a given operation suppoerte the specified source/ dest operand addressing type.
-   The forbidden destination operand addressing types:
    For the operations 'mov', 'add', 'sub', 'not', 'clr', 'lea', 'inc', 'dec', 'jmp', 'bne', 'red' and 'jsr': 'Immediate' and 'Dynamic' types are not supported.
-   The forbidden source operand addressing types: 'lea' operation doesn't support 'Immediate', 'Dynamic' and 'Register' types.
    All other operations who require a destination operand, support all addressing types.
*/
Boolean IsValidAddressingType(OperationType, AddressingType, OperandType);

/*'string' function represents the '.string' directive.
input parameters:
-   params: a string
-   DCptr: a pointer to the current data count (DC)
-   directives: the array of directives
For each character of the 'params' string, the corresponding ASCII value is assigned to the directives array. Including the final '\0' of the string.
After the function end, the data count will increase in the 'params' length plus 1 (for the finl 0).*/
void string(char* params, unsigned short* DCptr, unsigned short* directives);

/*'_data' function represents the '.data' directive.
input parameters:
-   params: an array of decimal numbers (representable by 15 bits, therefore -(2^14)<num<(2^14))
-   paramsCount: the parameters count
-   DCptr: a pointer to the current data count (DC)
-   directives: the array of directives
-   errors: a pointer to the assembler errors table

Each decimal number of the 'params' array, is assigned to the directives array.
After the function end, the data count will increase in the 'params' length.*/
void data(char* params[LENGTH], unsigned short paramsCount, unsigned short* DCptr, unsigned short* directives, ErrorTable* errors, int line);
#endif
