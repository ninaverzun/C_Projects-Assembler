#ifndef ERRORHandling_H_INCLUDED
#define ERRORHandling_H_INCLUDED
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LinkedList.h"
#include "FrequentlyUsedData.h"
#include "MachineCodeTranslations.h"

#define ERROR_LEN 200

typedef enum errorStatus {Ok=0, ErrorOccured=1} ErrorStatus;

typedef struct error
{
    int line;
    char description[ERROR_LEN];
} Error;

typedef struct errorTable
{
    Node* head;
} ErrorTable;

/*Allocates the required memory to store the symbol table and returns a pointer to it*/
ErrorTable* InitErrorTable();

/*Deletes the error table and frees all allocated memory*/
void DeleteErrorTable(ErrorTable* et);

/*Deletes the first symbol from the table and frees all memory allocated for it*/
void DeleteFirstError(ErrorTable* et);

/*Adds an error with the given description and line number to the table*/
void AddError(ErrorTable* st, char description[ERROR_LEN], int line);

/*Each symbol may be defined once!
Redundant definitions will cause an error to be added to the error table and will prohibit the creation of any output files (program.ob,program.ent and program.ext)*/
void HandleSymbolRedundantDeclaration(ErrorTable* et, char* symbolName, int line);

/*Each symbol may be defined at least once!
If a declaration isn't found it will cause an error to be added to the error table and will prohibit the creation of any output files (program.ob, program.ent and program.ext)*/
void HandleSymbolNotDeclared(ErrorTable* et, char* symbolName, int line);

/*Only the following register names are legal: "r1", "r2", "r3", "r4", "r5", "r6", "r7", "R1", "R2", "R3", "R4", "R5", "R6" or "R7".
Any other register number will cause an error*/
void HandleIllegalRegisterNumber(ErrorTable* et, char* symbolName, int line);

/*If an operand name doesn't fit none of the possible addressing types, this error will be set*/
void HandleIllegalOperand(ErrorTable* et, char* operandName, int line);

/*A valid dynamically addressed operand (X[5-9] for example) must contain a valid symbol name, adjacent to a '[' and must end with a ']' character.
Between the square brackets, we expect two decimal numbers between 0 and 12, separated by a '-' character.
Otherwise, an error will be set*/
void  HandleInvalidDynamicAddressing(ErrorTable* et, char* operandName, int line);

/*If dynamically addresses operand is not a decimal number*/
void  HandleInvlidDirectAdressing(ErrorTable* et, char* operandName, int line);

/*The parameters of the '.data' directive must be decimal numbers between -(2^15) and (2^15)!
Other types of parameters will cause an error*/
void HandleNonNumericParameterToData(ErrorTable* et, char* paramName, int line);

/*The parameters count mismatches the allowed count for a specific direction (.extern, .entry, .data or .string)*/
void HandleWrongParametersCount(ErrorTable* et, char* directionName,  int expected, int actual, int line);

/*The operands count mismatches the allowed count for a specific operation ("mov", "cmp", "add", "sub" etc...)*/
void HandleWrongOperandsCount(ErrorTable* et, char* operationName,  int expected, int actual, int line);

/*Each operation has a specific set of addressing types for both source and destination operands (if exist).
Invalid addressing type will cause an error. The following error is not relevant for 'rts' and 'stop' operations as they have no operands
-   The forbidden destination operand addressing types:
    For the operations 'mov', 'add', 'sub', 'not', 'clr', 'lea', 'inc', 'dec', 'jmp', 'bne', 'red' and 'jsr': 'Immediate' and 'Dynamic' types are not supported.
-   The forbidden source operand addressing types: 'lea' operation doesn't support 'Immediate', 'Dynamic' and 'Register' types.
    All other operations who require a destination operand, support all addressing types.
*/
void HandleInvalidOperandAddressingType(ErrorTable* et, char* operationName, OperandType opType, AddressingType addrType, int line);

/*According to assembler rules, a symbol name must comply with the following rules:
1. mustn't be a register name: r1, r2, r3, r4, r5, r6, r7, R1, R2, R3, R4, R5, R6 or R7
2. must start with a letter (small or capital)
3. no delimeters allowed as the first letter (no ' ', '\t' etc...)
4. no longer than 30 characters
*/
void HandleInvalidSymbolName(ErrorTable* et, char* symbolName, Boolean isFirstCharADelimiter,int line);

/*The given directive name is not one of the following allowed values:
".entry", ".extern", ".data" or ".string"
*/
void HandleInvalidDirectiveName(ErrorTable* et, char* name,int line);

/*The given operation name is not one of the following allowed values:
"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
*/
void HandleInvalidOperationName(ErrorTable* et, char* opName,int line);

/*Exports all errors present at the error table to the fiven file.
-   Returns 'OK' if no errors are found in the error table
-   Returns 'ErrorOccured' if any error is found in the error table*/
ErrorStatus ExportErrors(ErrorTable* et, FILE* file);

/*Checks the error status:
-   Returns 'OK' if no errors are found in the error table
-   Returns 'ErrorOccured' if any error is found in the error table*/
ErrorStatus PeekOnErrorStatus(ErrorTable* et);

#endif
