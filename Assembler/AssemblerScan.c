#include "AssemblerScan.h"

    /******************************************************************/
   /**                                                              **/
  /**    FOR ADDITIONAL DOCUMENTATION SEE "AssemblerScan.h"!!!!!   **/
 /**                                                              **/
/******************************************************************/

/********************* Internal functions prototypes (not exported in the header file) ***************************/

/******************** For documentation see below (before each internal func implementation) *********************/
void ExportObject(FILE*, unsigned short*, int, unsigned short*, int, Boolean);

ErrorStatus SymbolDeclaration(char*, SymbolType, Boolean, SymbolTable*, ErrorTable*, int);
ErrorStatus SetSymbolEntry(char*, SymbolTable*, ErrorTable*, int);
ErrorStatus SetAddress(char*, unsigned int, SymbolTable*, ErrorTable*, int);
ErrorStatus AddReference(char*, unsigned int, SymbolTable*, ErrorTable*, int);
ErrorStatus GetSymbol(char*, SymbolTable*, ErrorTable*, int, Symbol**);

Boolean IsSymbolName(char*);
Boolean IsDirectiveName(char*);

ErrorStatus GetNumber(const char*, short*);
ErrorStatus GetAddressingType(char*, AddressingType*, ErrorTable*, int);
ErrorStatus ValidateAddressingType(char*, OperandType, AddressingType*, char*, OperationType, ErrorTable*, int);

void AnalyzeDynamicAddressing(char*, char*, unsigned short*, unsigned short*);
ErrorStatus AnalyzeOperand(char*, OperandType, AddressingType, unsigned short*, unsigned short*, unsigned short*, unsigned short, SymbolTable*, ErrorTable*, int);
ErrorStatus AnalyzeSecondRegisterOperand(char*, OperandType, unsigned short*, unsigned short);

unsigned short NextMemoryIndex(unsigned short*);
void PrepareForNextIteration(int* linePtr, int* paramsCountPtr, char* params[LENGTH]);

/************************** External functions (exported to header file) ****************************************/

/*********************************** Operation functions ********************************************************/

void data(char* params[LENGTH], unsigned short paramsCount, unsigned short* DCptr, unsigned short* directives, ErrorTable* errors, int line)
{
    short i, numbers[LENGTH];
    ErrorStatus status=ErrorOccured;

    /*first parameters scan will make sure all parameters are legal numbers, and will set a corresponding error if one isn't*/
    for(i=0; i<paramsCount; i++)
    {
        status=GetNumber(params[i], &(numbers[i]));
        if(status!=Ok)
        {
            HandleNonNumericParameterToData(errors, params[i], line);
            return;
        }
    }

    /*second scan adds all numbers to the directives array, translated to binary 2o's complement if needed*/
    for(i=0;i<paramsCount;i++)
    {
        directives[NextMemoryIndex(DCptr)]=TwosComplement(numbers[i]);
    }
}

void string(char* params, unsigned short* DCptr, unsigned short* directives)
{
    int i;

    for(i=0; i<strlen(params); i++)
    {
        /*Please note: according to the assembler definition, no " characters can be present in the middle of the given string, therefore, the starting and ending '"' characters are ignored*/
        if(params[i]!='"')
        {
            directives[NextMemoryIndex(DCptr)]=params[i];
        }
    }
    directives[NextMemoryIndex(DCptr)]=0;
}

Boolean IsComment(char* params[LENGTH])
{
    return (params!=NULL) && (params[0][0]==COMMENT_TOKEN);
}

Boolean IsBlankLine(int wordCount)
{
    return wordCount==0;
}

void FirstScan(FILE* input, unsigned short* operations, unsigned short* opCount, unsigned short* directives, unsigned short* dataCount, SymbolTable* symbolTable, ErrorTable* errors)
{
    char* params[LENGTH], *symbolName=NULL, *operationName=NULL, *sourceOperand=NULL, *destOperand=NULL;
    int count=0, actualParamsCount, line=0, index=0;
    unsigned short IC=0, DC=0, DcForAddressing=0;
    Boolean isFirstCharADelimiter=False;
    SymbolType symbolType;
    OperationType opType;
    OperationGroupType opGroup;
    AddressingType sourceAddrType=0, destAddrType=0;

    while(ReadLine(input, params, &count, &isFirstCharADelimiter)==ContinueReading)
    {
        /*ensure current line isn't a comment. if it is skip parsing and delete all assigned memory*/
        if(IsBlankLine(count)==False && IsComment(params)==False)
        {
            index=sourceAddrType=destAddrType=0;
            symbolName=sourceOperand=destOperand=NULL;

            /*check if first parameter is a symbol declaration*/
            if(IsSymbolName(params[index]))
            {
                symbolName=params[index++];
            }

            /*The symbol name (if present) shouldn't be counted as a directive/operation parameter. The directive/operation name should also be reduced from the 'actualParamsCount'.
            Therefore, the actual parameters count is: 'total parameters count' minus 'index' minus 'the directive/operation name count'*/
            actualParamsCount=count-index-1;

            if(IsDirectiveName(params[index]))/*chack if the line contains a directive*/
            {
                if(IsValidDirectiveName(params[index])==False)
                {
                    HandleInvalidDirectiveName(errors, params[index], line);
                    PrepareForNextIteration(&line, &count, params);
                    continue;
                }
                DcForAddressing=DC;
                symbolType=Data;

                /*if extern symbol declaration (.entry is ignored in the first scan)*/
                if(strcmp(params[index], ".extern")==0)
                {
                    symbolType=Extern;

                    if(actualParamsCount!=EXTERN_ENTRY_PARAMS)
                    {
                        HandleWrongParametersCount(errors, params[index], EXTERN_ENTRY_PARAMS, actualParamsCount, line);
                        PrepareForNextIteration(&line, &count, params);
                        continue;
                    }

                    symbolName=params[++index];
                    if(SymbolDeclaration(symbolName, symbolType, isFirstCharADelimiter, symbolTable, errors, line)!=Ok)
                    {
                        PrepareForNextIteration(&line, &count, params);
                        continue;
                    }
                }
                /*if string directive*/
                else if(strcmp(params[index], ".string")==0)
                {
                    if(actualParamsCount!=STRING_PARAMS)
                    {
                        HandleWrongParametersCount(errors, ".string", STRING_PARAMS, actualParamsCount, line);
                        PrepareForNextIteration(&line, &count, params);
                        continue;
                    }

                    string(params[++index], &DC, directives);
                }
                /*if data directive*/
                else if(strcmp(params[index], ".data")==0)
                {
                    if(actualParamsCount<DATA_MIN_PARAMS)
                    {
                        HandleWrongParametersCount(errors, ".data", DATA_MIN_PARAMS, actualParamsCount, line);
                        PrepareForNextIteration(&line, &count, params);
                        continue;
                    }
                    data(&(params[++index]), actualParamsCount, &DC, directives, errors, line);
                }

                /*declare symbol, if neceserry and set an address*/
                if(symbolType==Data)
                {
                    if(SymbolDeclaration(symbolName, symbolType, isFirstCharADelimiter, symbolTable, errors, line)!=Ok ||
                        SetAddress(symbolName, DcForAddressing, symbolTable, errors, line)!=Ok)
                    {
                        PrepareForNextIteration(&line, &count, params);
                        continue;
                    }
                }
            }
            else /*not a directive, therefore must be an operation*/
            {
                symbolType=Code;
                operationName=params[index];
                if(IsValidOperationName(operationName, &opType)==False)
                {
                    HandleInvalidOperationName(errors, operationName,line);
                    PrepareForNextIteration(&line, &count, params);
                    continue;
                }
                opGroup=GetOperationGroupType(opType);

                if(actualParamsCount!=opGroup)
                {
                    HandleWrongOperandsCount(errors, operationName, opGroup, actualParamsCount, line);
                    PrepareForNextIteration(&line, &count, params);
                    continue;
                }
                sourceOperand=(opGroup==Two)?params[index+1]:NULL;
                destOperand=(opGroup!=None)?params[index+opGroup]:NULL;

                if(ValidateAddressingType(sourceOperand, Source, &sourceAddrType, operationName, opType, errors, line)==ErrorOccured ||
                   ValidateAddressingType(destOperand, Dest, &destAddrType, operationName, opType, errors, line)==ErrorOccured)
                {
                    PrepareForNextIteration(&line, &count, params);
                    continue;
                }

                if(SymbolDeclaration(symbolName, symbolType, isFirstCharADelimiter, symbolTable, errors, line)!=Ok ||
                   SetAddress(symbolName, IC, symbolTable, errors, line)!=Ok)
                {
                    PrepareForNextIteration(&line, &count, params);
                    continue;
                }

                operations[NextMemoryIndex(&IC)]=GetOperationRepresentation(opType, opGroup, sourceAddrType, destAddrType);
                if(opGroup!=None)
                {
                    operations[NextMemoryIndex(&IC)]=0;
                }
                if(opGroup==Two && (sourceAddrType!=Register || destAddrType!=Register))
                {
                    operations[NextMemoryIndex(&IC)]=0;
                }
            }
        }
        PrepareForNextIteration(&line, &count, params);
    }
    *opCount=IC;
    *dataCount=DC;
}

void SecondScan(FILE* input, unsigned short* operations, unsigned short opCount, unsigned short* directives, unsigned short dataCount, SymbolTable* symbolTable, ErrorTable* errors, char* inputFileName)
{
    char objectFileName[LENGTH], *params[LENGTH], *operationName=NULL, sourceOperand[LENGTH], destOperand[LENGTH];
    FILE* objectFilePtr=NULL;
    int count=0, actualParamsCount, line=0, index=0;
    unsigned short IC=0;
    Boolean isFirstCharADelimiter=False;
    OperationType opType;
    OperationGroupType opGroup;
    AddressingType sourceAddrType=0, destAddrType=0;

    if(PeekOnErrorStatus(errors)!=Ok)
    {
        ExportErrors(errors, stderr);
        return;
    }

    rewind(input);

    while(ReadLine(input, params, &count, &isFirstCharADelimiter)==ContinueReading)
    {
        /*ensure current line isn't a comment. if it is skip parsing and delete all assigned memory*/
        if(IsBlankLine(count)==False && IsComment(params)==False)
        {
            index=sourceAddrType=destAddrType=0;

            /*check if first parameter is a symbol declaration*/
            if(IsSymbolName(params[index]))
            {
                index++;
            }

            /*The symbol name (if present) shouldn't be counted as a directive/operation parameter. The directive/operation name should also be reduced from the 'actualParamsCount'.
            Therefore, the actual parameters count is: 'total parameters count' minus 'index' minus 'the directive/operation name count'*/
            actualParamsCount=count-index-1;

            if(IsDirectiveName(params[index]))/*chack if the line contains a directive*/
            {
                /*.data, .string, .extern, were processed during the first scan, only '.entry' should be processed*/
                /*if extern or entry symbol declaration*/
                if(strcmp(params[index], ".entry")==0)
                {
                    if(actualParamsCount!=EXTERN_ENTRY_PARAMS)
                    {
                        PrepareForNextIteration(&line, &count, params);
                        continue;
                    }

                    if(SetSymbolEntry(params[++index], symbolTable, errors, line)!=Ok)
                    {
                        PrepareForNextIteration(&line, &count, params);
                        continue;
                    }
                }
            }
            else /*not a directive, therefore must be an operation*/
            {
                operationName=params[index];
                if(IsValidOperationName(operationName, &opType)==False)
                {
                    PrepareForNextIteration(&line, &count, params);
                    continue;
                }
                opGroup=GetOperationGroupType(opType);

                if(actualParamsCount!=opGroup)
                {
                    PrepareForNextIteration(&line, &count, params);
                    continue;
                }

                if(opGroup==Two)
                {
                    strcpy(sourceOperand, params[index+1]);
                }
                if(opGroup!=None)
                {
                    strcpy(destOperand, params[index+opGroup]);
                }

                sourceAddrType=GetOperandAddressingType(operations[IC], Source);
                destAddrType=GetOperandAddressingType(operations[IC], Dest);

                /*The operation representation was already set in the first scan. It's address should be increased*/
                NextMemoryIndex(&IC);

                if(opGroup==Two)
                {
                    /*Analyze first operand, set a corresponding additional word to memory (operations) array*/
                    if(AnalyzeOperand(sourceOperand,Source, sourceAddrType, operations, directives, &IC,opCount, symbolTable, errors, line)!=Ok)
                    {
                        PrepareForNextIteration(&line, &count, params);
                        continue;
                    }

                    /*according to assembler's documentation, two register operands must be merged into a single additionam memory (operations) word*/
                    if(sourceAddrType==Register && destAddrType==Register)
                    {
                        AnalyzeSecondRegisterOperand(destOperand, Dest, operations, IC);
                        PrepareForNextIteration(&line, &count, params);
                        continue;
                    }
                }

                /*Analyze destination operand: second in case of 'Two' operation group or first in case of 'One' operation group*/
                if(opGroup!=None)
                {
                    AnalyzeOperand(destOperand, Dest, destAddrType, operations, directives, &IC, opCount, symbolTable, errors, line);
                }
            }
        }
        PrepareForNextIteration(&line, &count, params);
    }

    if(opCount != IC)
    {
        printf("\nFirst scan: %d, Second scan: %d", opCount, IC);
    }

    if(ExportErrors(errors, stderr)==Ok)
    {
        sprintf(objectFileName, "%s.ob", inputFileName);
        objectFilePtr=TryOpenFile(objectFileName, "w");
        ExportObject(objectFilePtr, operations, opCount, directives, dataCount, True);
        ExportSymbols(symbolTable, inputFileName, MEMORY_OFFSET+opCount, MEMORY_OFFSET);
    }

    if(objectFilePtr!=NULL)
    {
        fclose(objectFilePtr);
    }
}

Boolean IsValidDynamicAddressing(char* name, char* symbolName, unsigned short* startBit, unsigned short* endBit)
{
    int length=strlen(name), substringLen;
    char* hyphen = strchr(name, '-');
    char* openingBracket=strchr(name, '[');
    char* closingBracket=strchr(name, ']');
    char* nonNumericString;

    if(name==NULL || openingBracket==NULL || hyphen==NULL || name[length-1]!=']')
    {
        return False;
    }

    substringLen=length-strlen(openingBracket);
    strncpy(symbolName, name, substringLen);
    symbolName[substringLen+1]='\0';

    /*the prefix of the square brackets must be a valid symbol name*/
    if(IsValidSymbolName(symbolName, False)==False)
    {
        return False;
    }

    /*validate the first number between the square brackets*/
    (*startBit)=strtol(++openingBracket, &nonNumericString, DECIMAL);

    /*the indexes between the bracket must be between 0-12, the first index must be followed by a '-' character*/
    if((*startBit)<DYNAMIC_OFFSET_MIN || (*startBit)>DYNAMIC_OFFSET_MAX || nonNumericString==NULL || nonNumericString!=hyphen)
    {
        return False;
    }

    /*the indexes between the bracket must be between 0-12, the second index must be followed by a closing bracket*/
    (*endBit)=strtol(++nonNumericString, &nonNumericString, DECIMAL);
    if((*endBit)<DYNAMIC_OFFSET_MIN || (*endBit)>DYNAMIC_OFFSET_MAX || nonNumericString==NULL || nonNumericString!=closingBracket)
    {
        return False;
    }
    return True;
}

Boolean IsValidAddressingType(OperationType opType, AddressingType addrType, OperandType operandType)
{
    int i, restrictedDestAddressingOperationsCount=12;
    static OperationType restrictedDestAddressingOperations[]={mov, add, sub, not, clr, lea, inc, dec, jmp, bne, red, jsr};

    if(opType==rts || opType==stop)
    {
        return True;
    }

    if(operandType==Source)
    {
        return (opType!=lea || addrType==Direct);
    }

    /*Destination operand validation*/
    for(i=0; i<restrictedDestAddressingOperationsCount;i++)
    {
        if(opType==restrictedDestAddressingOperations[i])
        {
            return addrType==Direct || addrType==Register;
        }
    }
    return True;
}

Boolean IsValidOperationName(char* opName, OperationType* typePtr)
{
    int i;
    static char* operationNames[]={"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
    for(i=0; i< OPERATIONS; i++)
    {
        if(strcmp(opName, operationNames[i])==0)
        {
            (*typePtr)=(OperationType)i;
            return True;
        }
    }
    return False;
}

Boolean IsValidDirectiveName(char* name)
{
    int i;
    static char* directiveNames[]={".entry", ".extern", ".string", ".data"};
    for(i=0; i< DIRECTIVES; i++)
    {
        if(strcmp(name, directiveNames[i])==0)
        {
            return True;
        }
    }
    return False;
}

Boolean IsValidRegisterName(char* name, RegisterType* registerTypePtr)
{
    static char* registerNames[]={"r1", "r2", "r3", "r4", "r5", "r6", "r7", "pc", "sp", "psw", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "PC", "SP", "PSW"};
    int i;
    /*we check "REGISTERS_COUNT*2" because we need to go through register names in small and capital letters*/
    for(i=0; i<REGISTERS_COUNT*2; i++)
    {
        if(strcmp(registerNames[i], name)==0)
        {
            *registerTypePtr=(RegisterType)(i%REGISTERS_COUNT);
            return True;
        }
    }
    return False;
}

Boolean IsRegisterName(char* name)
{
    int i;
    if(name==NULL || name[0]!='r' || name[0]!='R')
    {
        return False;
    }

    for(i=1; i<strlen(name);i++)
    {
        if(!isdigit(name[i]))
        {
            return False;
        }
    }

    return True;
}

Boolean IsValidSymbolName(char* name, Boolean isFirstCharacterADelimiter)
{
    RegisterType type;
    if(isFirstCharacterADelimiter==False && strlen(name)<=SYMBOL_NAME_LEN)
    {
        return IsValidRegisterName(name, &type)==False && IsRegisterName(name)==False;
    }
    return False;
}


/***********************       Internal functions (not exported to the header file)       ******************************/
/*Analyzes the specific type of operand addressing and sets an address pointer to the memory location where the value is stored

-   In case of an Immediate addressing: the value is encoded in the operand name

-   In case of a Direct addressing, a symbol name is encoded in the operand name, it's address should be retrieved from the symbol table.
    The value should be retrieved from the memory (operations) array, at the address of the encoded symbol.

-   In case of a Dynamic addressing, a symbol name should be retrieved from the operand name. Similarly to Direct encoding, the value should also be loaded from the memory
    array, at the symbol's address. Dynamic addressing requires further processing of the value: to extract specified bits.

-   In case of a Regirter addressing, the value should be retrieved from a register*/
ErrorStatus AnalyzeOperand(char* operand, OperandType operandType, AddressingType addressingType, unsigned short* operations,unsigned short* directives,
                           unsigned short* ICptr, unsigned short opCount, SymbolTable* symbolTable, ErrorTable* errors, int line)
{
    unsigned short startIndex=0, endIndex=0, *relevantMemory;
    short value=0;
    char symbolName[SYMBOL_NAME_LEN];
    Symbol* symbolPtr=NULL;
    RegisterType registerType;

    strcpy(symbolName, operand);
    /*Innediate addressing*/
    if(addressingType==Immediate)
    {
        if(GetNumber(&symbolName[1], &value)!=Ok)
        {
            return ErrorOccured;
        }
        value=ShiftAndSetAllocationType(value, Absolute);
    }
    else if(addressingType==Register)
    {
        value=0;
        if(!IsValidRegisterName(symbolName, &registerType))
        {
            return ErrorOccured;
        }
        value=SetOperandRegisterType(value, registerType, operandType);
        value=SetAllocationType(value, Immediate);
    }
    else if(addressingType==Dynamic)
    {
        AnalyzeDynamicAddressing(operand, symbolName, &startIndex, &endIndex);

        /*symbol address loading from symbol table*/
        if(GetSymbol(symbolName, symbolTable, errors, line, &symbolPtr)!=Ok ||
           AddReference(symbolName, *ICptr+MEMORY_OFFSET, symbolTable, errors, line)!=Ok)
        {
            return ErrorOccured;
        }

        if(symbolPtr->isInitialised)
        {
            relevantMemory=symbolPtr->type==Code?operations:directives;
            value=GetBitsByIndex(relevantMemory[symbolPtr->address], startIndex, endIndex, True);
        }
        value=ShiftAndSetAllocationType(value, Immediate);
    }
    else /*Direct addressing*/
    {
        /*symbol address loading from symbol table*/
        if(GetSymbol(symbolName, symbolTable, errors, line, &symbolPtr)!=Ok ||
           AddReference(symbolName, *ICptr+MEMORY_OFFSET, symbolTable, errors, line)!=Ok)
        {
            return ErrorOccured;
        }

        if(symbolPtr->isInitialised)
        {
            value=symbolPtr->address;
            if(symbolPtr->type==Code)
            {
                value+=MEMORY_OFFSET;
            }
            if(symbolPtr->type==Data)
            {
                value+=MEMORY_OFFSET+opCount;
            }
        }
        value=ShiftAndSetAllocationType(value, symbolPtr->type==Extern?External:Relocatable);
    }
    operations[NextMemoryIndex(ICptr)]=value;
    return Ok;
}

/*Analyzes an operand with 'Register' addressing, assuming an previously set additional word exists in the memory (in IC address).
Sets only the relevant bits for the source/destination register name.
Returns 'Ok' if operand is a valid register name, ass assumed or 'ErrorOccured' otherwise*/
ErrorStatus AnalyzeSecondRegisterOperand(char* operand, OperandType operandType, unsigned short* operations, unsigned short IC)
{
    RegisterType registerType;

    if(!IsValidRegisterName(operand, &registerType))
    {
        return ErrorOccured;
    }
    operations[IC]=SetOperandRegisterType(operations[IC], registerType, operandType);
    return Ok;
}

/*Gets a pointer to the current IC(instructionsCounter) or DC (Data Counter) and returns the next index*/
unsigned short NextMemoryIndex(unsigned short* count)
{
    /*according to our computer specifications, we have 1000 addresses in our imaginary CPU.
    It was also stated (page 40) that the input may be unlimited.
    Therefore, i suggest a "circular" manner for memory indexing:
    As defined on page 36, we start from the 100th index in the memory and go up.
    As we reach 1000 (top memory index), we will assign the 0 index and so on and so forth*/
    unsigned short current=((*count)++)%MEMORY_LEN;
    return current;
}

/*PrepareForNextIteration cleans up all memory allocated for the parsing of the current line and promotes the line count*/
void PrepareForNextIteration(int* linePtr, int* paramsCountPtr, char* params[LENGTH])
{
    (*linePtr)++;
    DeleteParameters(params, paramsCountPtr);
}

/*Extracts from a given dynamically addressed operand, the symbol name, start bit index and end bit index.
Assumes valid syntax of a dynamic address and symbol name*/
void AnalyzeDynamicAddressing(char* operand, char* symbolName, unsigned short* startBit, unsigned short* endBit)
{
    int length=strlen(operand), substringLen;
    char* openingBracket=strchr(operand, '[');
    char* nonNumericString;

    substringLen=length-strlen(openingBracket);
    strncpy(symbolName, operand, substringLen);
    symbolName[substringLen]='\0';

    /*the first number between the square brackets*/
    (*startBit)=strtol(++openingBracket, &nonNumericString, DECIMAL);

    /*the second index must be followed by a closing bracket*/
    (*endBit)=strtol(++nonNumericString, &nonNumericString, DECIMAL);
}

/*'ValidateOperationType' gets the addressing type of a given operand and validate's it's compatibility with the given operation
It returns 'Ok' in case of a valid and compatible addressing or reports an error and returns 'ErrorOccured'*/
ErrorStatus ValidateAddressingType(char* operand, OperandType operandType, AddressingType* addrTypePtr, char* operationName, OperationType opType, ErrorTable* errors, int line)
{
    if(operand!=NULL)
    {
        if(GetAddressingType(operand, addrTypePtr, errors, line)!=Ok)
        {
            return ErrorOccured;
        }

        if(IsValidAddressingType(opType, *addrTypePtr, operandType)==False)
        {
            HandleInvalidOperandAddressingType(errors, operationName, operandType, *addrTypePtr, line);
            return ErrorOccured;
        }
    }
    else
    {
        *addrTypePtr=0;
    }
    return Ok;
}

/*Defines the addressing type of an operand according to the string representation of the operand*/
ErrorStatus GetAddressingType(char* operand, AddressingType* addrTypePtr, ErrorTable* errors, int line)
{
    char* dynamicSubstring, *nonNumericString, symbolName[SYMBOL_NAME_LEN];
    unsigned short startBit, endBit;
    RegisterType type;

    if(operand[0]=='#')
    {
        strtol(operand+1, &nonNumericString, DECIMAL);
        if(strcmp(nonNumericString,"\0")==0)
        {
            *addrTypePtr=Immediate;
            return Ok;
        }
        HandleInvlidDirectAdressing(errors, operand, line);
        return ErrorOccured;
    }

    if(IsValidRegisterName(operand, &type))
    {
        *addrTypePtr=Register;
        return Ok;
    }

    if(IsRegisterName(operand))
    {
        HandleIllegalRegisterNumber(errors, operand, line);
        return ErrorOccured;
    }

    dynamicSubstring=strchr(operand, '[');
    if(dynamicSubstring!=NULL)
    {
        if(IsValidDynamicAddressing(operand, symbolName, &startBit, &endBit))
        {
            *addrTypePtr=Dynamic;
            return Ok;
        }
        HandleInvalidDynamicAddressing(errors, operand, line);
        return ErrorOccured;
    }

    if(IsValidSymbolName(operand, False))
    {
        *addrTypePtr=Direct;
        return Ok;
    }
    HandleIllegalOperand(errors, operand, line);
    return ErrorOccured;
}

/*'GetNumber' translates a given string to a decimal number. It sets the result to the 'resultPtr' and returns 'Ok' if translation succeeded or 'ErrorOccured' otherwise*/
ErrorStatus GetNumber(const char* string, short* resultPtr)
{
    char* nonNumericStringPtr;

    *resultPtr=strtol(string,&nonNumericStringPtr, 10);
    if(strcmp(nonNumericStringPtr,"\0")!=0)
    {
        return ErrorOccured;
    }
    return Ok;
}

/*Gets a symbol reference from the symbol table. Handles an error if a symbol wasn't declared
Returns 'Ok' if 'FindSymbol' returned success or 'ErrorOccured' otherwise*/
ErrorStatus GetSymbol(char* symbolName, SymbolTable* symbolTable, ErrorTable* errors, int line, Symbol** symbolPtr)
{

    if(symbolName==NULL)
    {
        return Ok;
    }
    (*symbolPtr)=FindSymbol(symbolTable, symbolName);
    if((*symbolPtr) != NULL)
    {
        return Ok;
    }

    HandleSymbolNotDeclared(errors, symbolName, line);
    return ErrorOccured;
}

/*Sets a symbol reference to the symbol table. Handles an error if a symbol wasn't declared
Returns 'Ok' if 'AddSymbolReference' returned success or 'ErrorOccured' otherwise*/
ErrorStatus AddReference(char* symbolName, unsigned int address, SymbolTable* symbolTable, ErrorTable* errors, int line)
{
    Status status=SymbolNotDeclared;
    if(symbolName==NULL)
    {
        return Ok;
    }
    status=AddSymbolReference(symbolTable, symbolName, address);
    if(status == Success)
    {
        return Ok;
    }

    if(status==SymbolNotDeclared)
    {
        HandleSymbolNotDeclared(errors, symbolName, line);
    }
    return ErrorOccured;
}

/*Sets a symbol address to the symbol table. Handles an error if a symbol wasn't declared
Returns 'Ok' if 'SetSymbolAddress' returned success or 'ErrorOccured' otherwise*/
ErrorStatus SetAddress(char* symbolName, unsigned int address, SymbolTable* symbolTable, ErrorTable* errors, int line)
{
    Status status=SymbolNotDeclared;
    if(symbolName==NULL)
    {
        return Ok;
    }
    status=SetSymbolAddress(symbolTable, symbolName, address);
    if(status == Success)
    {
        return Ok;
    }
    else if(status==SymbolNotDeclared)
    {
        HandleSymbolNotDeclared(errors, symbolName, line);
    }
    return ErrorOccured;
}

/*checks of a given string is a directive name, returns 'True' if it is or 'False' otherwise*/
Boolean IsDirectiveName(char* name)
{
    if(name!=NULL && name[0]=='.')
    {
        return True;
    }
    return False;
}

/*'IsSymbolName' eturns True if the last string character (just before '\0') is the SYMBOL_NAME_LAST_TOKEN or False otherwise.
Parameter: the untrimmes symbol name*/
Boolean IsSymbolName(char* untrimmedSymbolName)
{
    int symTokenIndex=strlen(untrimmedSymbolName)-1;
    return untrimmedSymbolName[symTokenIndex]==SYMBOL_NAME_LAST_TOKEN?True:False;
}

/*'SymbolDeclaration' removes the ':' token from a symbol name, ensures it's valid and then adds it to the symbol table.
In case of a non valid name, an error is added to the error table.
Parameters: symbol name, symbol type, a boolean value that states weather the first character in the parsed line is a delimeter,
a symbol table reference, error table reference and a line number*/
ErrorStatus SymbolDeclaration(char* symbolName, SymbolType symType, Boolean isFirstCharADelimiter, SymbolTable* symbolTable, ErrorTable* errors, int line)
{
    ErrorStatus result=ErrorOccured;
    Status status;
    int length=0;

    if(symbolName==NULL)
    {
        return Ok;
    }
    length=strlen(symbolName);
    if(symbolName[length-1]==SYMBOL_NAME_LAST_TOKEN)
    {
        /*trim the ':' token from the symbol name*/
        symbolName[length-1]='\0';
    }

    if(IsValidSymbolName(symbolName, isFirstCharADelimiter)==True)
    {
        status=AddSymbol(symbolTable, symbolName, symType);
        if(status==Success)
        {
            result=Ok;
        }
        else if(status==RedundantDeclaration)
        {
            HandleSymbolRedundantDeclaration(errors, symbolName, line);
        }
    }
    else
    {
        HandleInvalidSymbolName(errors, symbolName, isFirstCharADelimiter, line);
    }
    return result;
}

/*'SetSymbolEntry' sets 'Entry' symbol type, to a specified symbol.
Returns 'Ok' in case of success or handles a 'symbol was not declared' error and returns 'ErrorOccured'*/
ErrorStatus SetSymbolEntry(char* symbolName, SymbolTable* symbolTable, ErrorTable* errors, int line)
{
    ErrorStatus result=ErrorOccured;

    if(symbolName==NULL)
    {
        return Ok;
    }

    if(MarkSymbolAsEntry(symbolTable, symbolName)==Success)
    {
        result=Ok;
    }
    else
    {
        HandleSymbolNotDeclared(errors, symbolName, line);
    }

    return result;
}

/*Helper function to print a row in 'special' octal base*/
void PrintOctalRow(FILE* file, unsigned short address, unsigned short value)
{
    char octalAddress[LENGTH], octalValue[LENGTH];

    TranslateToSpecialOctaBase(address, octalAddress);
    TranslateToSpecialOctaBase(value, octalValue);
    fprintf(file, "%s\t%s\n", octalAddress, octalValue);
}

/*
'ExportObject' writes the contents of the ".obj" file to the given file pointer.
Parameters: object file pointer, the operations words array and it's count, the directives words array and it's count.

First, it goes through the the 'operations' array, that consists of operation words and their corresponding additional words (2 additional words tops representing the operands of each operation).
The operation's binary encoding and operand values are appended to the object file in the following manner:
'base 8 special address' 'base 8 special code'.

After finishing the 'operations' array, the 'directives' array is also examined.
The 'directives' array consists of the values set by ".data" and ".string" directions from the input file. These values are also appended one by one to the object file,
in a similar manner like the operations.
*/
void ExportObject(FILE* objectFilePtr, unsigned short* operations, int opCount, unsigned short* directives, int dirCount, Boolean isOctal)
{
    int opAddress, dirAddress;

    /*first, append the operations to object file*/
    for(opAddress=0; opAddress<opCount; opAddress++)
    {
        if(isOctal)
        {
             PrintOctalRow(objectFilePtr, opAddress+MEMORY_OFFSET, operations[opAddress]);
        }
        else
        {
            fprintf(objectFilePtr, "%d\t", opAddress+MEMORY_OFFSET);
            PrintBits(objectFilePtr, operations[opAddress]);
        }
    }

    /*last, append directives to the object file*/
    for(dirAddress=0; dirAddress<dirCount; dirAddress++)
    {
        if(isOctal)
        {
             PrintOctalRow(objectFilePtr, opAddress+dirAddress+MEMORY_OFFSET, directives[dirAddress]);
        }
        else
        {
            fprintf(objectFilePtr, "%d\t", opAddress+dirAddress+MEMORY_OFFSET);
            PrintBits(objectFilePtr, directives[dirAddress]);
        }
    }
}
