#include "ErrorHandling.h"

/**Please NOTE: For documentation see "ErrorHandling.h"!!! **/

ErrorTable* InitErrorTable()
{
    ErrorTable* table=(ErrorTable*)calloc(1, sizeof(ErrorTable));
    if(table==NULL)
    {
        FatalMemoryAllocationError();
    }
    table->head=NULL;
    return table;
}

void DeleteErrorTable(ErrorTable* et)
{
    if(et==NULL)
    {
        return;
    }

    while(et->head!=NULL)
    {
        DeleteFirstError(et);
    }
    free(et);
}


void DeleteFirstError(ErrorTable* et)
{
    if(et==NULL || et->head==NULL)
    {
        return;
    }

    free(et->head->data);
    /*RemoveNode frees the first node and returns the new head*/
    et->head = RemoveNode(et->head);
}

void AddError(ErrorTable* et, char descr[ERROR_LEN], int line)
{
    Error* errorPtr;
    assert(et!=NULL);

    errorPtr=(Error*)calloc(1, sizeof(Error));

    if(errorPtr==NULL)
    {
        FatalMemoryAllocationError();
    }
    strcpy((errorPtr->description), descr);

    errorPtr->line=line;

    et->head=AddNode(et->head, errorPtr);
}

void HandleSymbolRedundantDeclaration(ErrorTable* et, char* symbolName, int line)
{
    char descr[ERROR_LEN];
    sprintf(descr, "Line '%d':\tRedundant declaration of symbol '%s'! Symbol was already declared!", line, symbolName);
    AddError(et, descr, line);
}

void HandleSymbolNotDeclared(ErrorTable* et, char* symbolName, int line)
{
    char descr[ERROR_LEN];
    sprintf(descr, "Line '%d':\tSymbol '%s' was not declared!", line, symbolName);
    AddError(et, descr, line);
}

void HandleNonNumericParameterToData(ErrorTable* et, char* paramName, int line)
{
    char descr[ERROR_LEN];
    sprintf(descr, "Line '%d':\tParameter '%s' of the '.data' directive isn't a decimal number!", line, paramName);
    AddError(et, descr, line);
}

void HandleInvalidOperandAddressingType(ErrorTable* et, char* opName, OperandType opType, AddressingType addrType, int line)
{
    char descr[ERROR_LEN];
    sprintf(descr, "Line '%d':\tOperation '%s' doesn't support '%s' addressing type for %s operand!", line, opName, ADDRESSING_TYPE(addrType), opType==Source?"source":"destination");
    AddError(et, descr, line);
}

void HandleWrongParametersCount(ErrorTable* et, char* directionName,  int expected, int actual, int line)
{
    char descr[ERROR_LEN];
    sprintf(descr, "Line '%d':\tDirection '%s' expects '%d' parameters, but was '%d'!", line, directionName, expected, actual);
    AddError(et, descr, line);
}

void HandleWrongOperandsCount(ErrorTable* et, char* opName, int expected, int actual, int line)
{
    char descr[ERROR_LEN];
    sprintf(descr, "Line '%d':\tOperation '%s' expects '%d' parameters, but was '%d'!", line, opName, expected, actual);
    AddError(et, descr, line);
}

void HandleInvalidOperationName(ErrorTable* et, char* opName,int line)
{
     char descr[ERROR_LEN];
     sprintf(descr, "Line '%d':\'%s' is not a valid operation name!", line, opName);
     AddError(et, descr, line);
}

void HandleInvalidDirectiveName(ErrorTable* et, char* name,int line)
{
    char descr[ERROR_LEN];
    sprintf(descr, "Line '%d':\'%s' is not a valid directive name!", line, name);
    AddError(et, descr, line);
}

void HandleIllegalRegisterNumber(ErrorTable* et, char* name, int line)
{
    char descr[ERROR_LEN];
    sprintf(descr, "Line '%d':\'%s' is not a legal register name name! On this computer, only registers numbered between 1-7 are legal!", line, name);
    AddError(et, descr, line);
}

void HandleIllegalOperand(ErrorTable* et, char* operandName, int line)
{
    char descr[ERROR_LEN];
    sprintf(descr, "Line '%d':\'%s' is not a valid operand name! \nIt isn't a decimal number, nor a valid register, nor a valid symbol name, nor a valid dynamic address.", line, operandName);
    AddError(et, descr, line);
}

void  HandleInvalidDynamicAddressing(ErrorTable* et, char* operandName, int line)
{
    char descr[ERROR_LEN];
    sprintf(descr, "Line '%d':\'%s' is not a valid dynamic address!", line, operandName);
    AddError(et, descr, line);
}

void  HandleInvlidDirectAdressing(ErrorTable* et, char* operandName, int line)
{
     char descr[ERROR_LEN];
    sprintf(descr, "Line '%d':\'%s' is not a valid direct address! Direct address should consist of a decimal numeric value", line, operandName);
    AddError(et, descr, line);
}

void HandleInvalidSymbolName(ErrorTable* et, char* symbolName, Boolean isFirstCharADelimiter,int line)
{
    char descr[ERROR_LEN];
    if(isFirstCharADelimiter==True)
    {
        sprintf(descr, "Line '%d': a symbol name can not start with a delimiter (%s)!", line, DELIMITERS);
    }
    else
    {
        sprintf(descr, "Line '%d':\'%s' is not a valid symbol name!", line, symbolName);
    }

    AddError(et, descr, line);
}

/**internal function (not published in header file) for printing a specific error table entry to a fiven file**/
void PrintError(FILE* file, Error* error)
{
    fprintf(file, "\n%s\n", error->description);
}

ErrorStatus PeekOnErrorStatus(ErrorTable* et)
{
    Node* current;

    assert(et!=NULL);
    current=et->head;

    return current==NULL?Ok:ErrorOccured;
}

ErrorStatus ExportErrors(ErrorTable* et, FILE* file)
{
    ErrorStatus result=ErrorOccured;
    Node* current;
    assert(et!=NULL);

    current=et->head;
    if(current==NULL)
    {
        result=Ok;
    }
    else
    {
        while(current!=NULL)
        {
            PrintError(file, (Error*)current->data);
            current=current->next;
        }
    }
    return result;
}
