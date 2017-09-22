#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MachineCodeTranslations.h"
#include "AssemblerScan.h"

int main(int argc, char *argv[])
{
    FILE* inputFilePtr=NULL;
    char* inputFileName;
    char inputFileNameWithExt[FILE_NAME_LEN];
    unsigned short operations[MEMORY_LEN], symbols[MEMORY_LEN], IC=0, DC=0;
    SymbolTable* symbolTable = NULL;
    ErrorTable* errors=NULL;

    if(argc==1)
    {
        fprintf(stderr, "\nNo file names were provided to assembly for processing!\n\n");
        return 1;
    }

    while(--argc>0)
    {
        symbolTable = InitSymbolTable();
        errors=InitErrorTable();
        inputFileName=*(++argv);

        sprintf(inputFileNameWithExt, "%s.as", inputFileName);

        inputFilePtr=TryOpenFile(inputFileNameWithExt, "r");

        FirstScan(inputFilePtr, operations, &IC, symbols, &DC, symbolTable, errors);
        SecondScan(inputFilePtr, operations, IC, symbols, DC, symbolTable, errors, inputFileName);

        if(inputFilePtr)
        {
            fclose(inputFilePtr);
        }

        DeleteSymbolTable(symbolTable);
        DeleteErrorTable(errors);
    }
    return 0;
}
