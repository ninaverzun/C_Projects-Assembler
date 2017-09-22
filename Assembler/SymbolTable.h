#ifndef SYMBOLTABLE_H_INCLUDED
#define SYMBOLTABLE_H_INCLUDED
#include <stdlib.h>
#include <string.h>
#include "FatalErrorHandling.h"
#include "LinkedList.h"
#include "FileAndParsingOperations.h"
#include "MachineCodeTranslations.h"

#define TRUE 1
#define FALSE 0
#define ENTRY_TYPE_LEN 7
#define FILE_NAME_LEN 50
#define REF_SIZE 1000

#define SYMBOL_TYPE(type) ((type==Data)?"Data":(type==Code?"Code":"Extern"))
#define SYMBOL_FILE_EXTENTION(type) (type==Entry?".ent":".ext")

typedef enum status {Success=0, SymbolNotDeclared=1, RedundantDeclaration=2} Status;
typedef enum symbolType {Code, Data, Extern} SymbolType;

/*struct symbol holds all info about a specific "Label" that was defined in the assembly input file
For each input file:
- symbol name must be unique in the whole symbol table and should be declared once (although it may be assigned few times)
- upon a symbol declaration, "isInitialised" field is set to FALSE (==0)*/
typedef struct symbol
{
    short address;
    char* name;
    Boolean isInitialised;
    Boolean isEntry;
    SymbolType type;
    unsigned short* references;
    unsigned short index;
}
Symbol;

typedef struct symbolTable
{
    Node* head;
}SymbolTable;

/*Allocates the required memory to store the symbol table and returns a pointer to it*/
SymbolTable* InitSymbolTable();

/*Deletes the symbol table and frees all allocated memory*/
void DeleteSymbolTable(SymbolTable* st);

/*prints the contents of the symbol table*/
void PrintSymbolTable();

/*Adds a symbol with the given name and type to the table. "isInitialised" and "value" properties are set to 0 by default*/
Status AddSymbol(SymbolTable* st, char* name, SymbolType type);

/*Finds a symbol in the symbol table by name and adds an address to it's addresses array (part of SymbolReferences struct)*/
Status AddSymbolReference(SymbolTable* st, char* name, unsigned short address);

/*Deletes the first symbol from the table and frees all memory allocated for it*/
void DeleteFirstSymbol(SymbolTable* st);

/*Sets the value of an existing symbol in the table. In case the symbol hasn't been added yet, adds a new symbol to the table and
set's it's value to the given value*/
Status SetSymbolAddress(SymbolTable* st, char* name, short value);

/*Sets 'True' to the 'IsEntry' property of a specified symbol. Usefull to distinguish and process '.entry' declarations
Returns 'Success' if operation succeeded or a 'SymbolNotDeclared' error otherwise*/
Status MarkSymbolAsEntry(SymbolTable* st, char* name);

/*Returns a pointer to a symbol from the table that matches the given name.
If no symbol is found with the given name, a NULL value is returned*/
Symbol* FindSymbol(SymbolTable* st, char* name);

/*ExportSymbols goes through the symbol table:
-   if "extern" type symbols are found, a "name.ext" file is created and the relevant symbols are written to it:
    row is added for every 'Extern' symbol reference, consisting of the symbol name and the memory address where it's being referenced.
    all numbers are represented in the 'special' octal base.

-   if "entry" type symbols are found, a "name.ent" file is created and the relevant symbols are written to it:
    row is added for each symbol defined as 'entry', sonsisting of the symbol name and it's memory address
    all numbers are represented in the 'special' octal base.

    As code and data are kept apart and symbol addresses are relative to the relevant memory type, memory offsets should be provided to normalize the
    'entry' addresses*/
void ExportSymbols(SymbolTable* st, char* inputFileName, unsigned short dataMemOffset, unsigned short codeMemOffset);

#endif
