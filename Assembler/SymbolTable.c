#include "SymbolTable.h"

/************************  For documentation see "SymbolTable.h"!!! *****************************/

SymbolTable* InitSymbolTable()
{
    SymbolTable* table=(SymbolTable*)calloc(1, sizeof(SymbolTable));
    if(table==NULL)
    {
        FatalMemoryAllocationError();
    }
    table->head=NULL;
    return table;
}

void DeleteSymbolTable(SymbolTable* st)
{
    if(st==NULL)
    {
        return;
    }

    while(st->head!=NULL)
    {
        DeleteFirstSymbol(st);
    }
    free(st);
}

void DeleteFirstSymbol(SymbolTable* st)
{
    if(st==NULL || st->head==NULL)
    {
        return;
    }

    if(((Symbol*)(st->head->data))->references!=NULL)
    {
        free(((Symbol*)(st->head->data))->references);
    }

    free(((Symbol*)(st->head->data))->name);
    free(st->head->data);
    /*RemoveNode frees the first node and returns the new head*/
    st->head = RemoveNode(st->head);
}

Status AddSymbol(SymbolTable* st, char* name, SymbolType symbolType)
{
    Symbol* symbolEntryPtr;
    assert(st!=NULL);

    symbolEntryPtr=FindSymbol(st, name);
    if(symbolEntryPtr!=NULL)
    {
        /*symbol is already found in the table, means a redundant declaration occured*/
        return RedundantDeclaration;
    }

    symbolEntryPtr=(Symbol*)calloc(1, sizeof(Symbol));

    if(symbolEntryPtr==NULL)
    {
        FatalMemoryAllocationError();
    }
    symbolEntryPtr->name=(char*)calloc(strlen(name), sizeof(char));
    strcpy(symbolEntryPtr->name, name);

    symbolEntryPtr->isInitialised=False;
    symbolEntryPtr->isEntry=False;
    symbolEntryPtr->type=symbolType;

    symbolEntryPtr->references=NULL;
    symbolEntryPtr->index=0;

    st->head=AddNode(st->head, symbolEntryPtr);
    return Success;
}

void PrintEntry(int index, Symbol* row)
{
    int i;
    assert(row!=NULL);

    if(row->isInitialised==FALSE)
    {
        printf("\n\t%d) '%s':\t?? (%s) ", index, row->name, SYMBOL_TYPE(row->type));
    }
    else
    {
        printf("\n\t%d) '%s':\t%d (%s) ", index, row->name, row->address, SYMBOL_TYPE(row->type));
    }
    if(row->references!=NULL)
    {
        printf(", references: ");
        for(i=0; i<row->index; i++)
        {
            printf("%d ",(row->references)[i]);
        }
    }
}

void PrintSymbolTable(SymbolTable* st)
{
    Node* current;
    int index=0;
    Symbol* content;

    assert(st!=NULL);
    printf("\nSymbol table content:\n");
    if(st->head==NULL)
    {
        printf("\nSymbol table is empty!\n");
        return;
    }
    current=st->head;
    while(current!=NULL)
    {
        content=(Symbol*)current->data;
        PrintEntry(++index, content);
        current=current->next;
    }
    printf("\n\n");
}

Status AddSymbolReference(SymbolTable* st, char* name, unsigned short address)
{
    Status status = SymbolNotDeclared;
    Symbol* symbolToSet;

    assert(st!=NULL);
    symbolToSet = FindSymbol(st, name);

    if(symbolToSet!=NULL)
    {
        if(symbolToSet->references==NULL)
        {
            (symbolToSet->references)=(unsigned short*)calloc(MEMORY_LEN, sizeof(unsigned short));
            if(symbolToSet->references==NULL)
            {
                FatalMemoryAllocationError();
            }
        }

        symbolToSet->references[(symbolToSet->index)++]=address;
        status=Success;
    }
    return status;
}

Status MarkSymbolAsEntry(SymbolTable* st, char* name)
{
     Status status = SymbolNotDeclared;
    Symbol* symbolToSet;

    assert(st!=NULL);
    symbolToSet = FindSymbol(st, name);

    if(symbolToSet!=NULL)
    {
        symbolToSet->isEntry=True;
        status=Success;
    }
    return status;
}

Status SetSymbolAddress(SymbolTable* st, char* name, short value)
{
    Status status = SymbolNotDeclared;
    Symbol* symbolToSet;

    assert(st!=NULL);
    symbolToSet = FindSymbol(st, name);

    if(symbolToSet!=NULL)
    {
        symbolToSet->address=value;
        symbolToSet->isInitialised=TRUE;
        status=Success;
    }
    return status;
}

Symbol* FindSymbol(SymbolTable* st, char* name)
{
    Node* found;
    assert(st!=NULL);

    found=st->head;
    while(found!=NULL && strcmp(name, ((Symbol*)(found->data))->name)!=0)
    {
        found=found->next;
    }
    return found==NULL?NULL:((Symbol*)(found->data));
}

void PrintExternFileRow(Symbol* symbol, FILE* file)
{
    int i;
    char octal[MAX_DIGITS];

    for(i=0; i<(symbol->index); i++)
    {
        TranslateToSpecialOctaBase(symbol->references[i], octal);
        fprintf(file, "\n%s\t%s", symbol->name, octal);
    }
}

void PrintEntryFileRow(Symbol* symbol, FILE* file, unsigned short dataMemOffset, unsigned short codeMemOffset)
{
    char octal[MAX_DIGITS];
    short normalizedAddress;
    if(symbol->isInitialised)
    {
        if(symbol->type==Data)
        {
            normalizedAddress=symbol->address+dataMemOffset;
        }

        if(symbol->type==Code)
        {
            normalizedAddress=symbol->address+codeMemOffset;
        }
        TranslateToSpecialOctaBase(normalizedAddress, octal);
        fprintf(file, "\n%s\t%s", symbol->name, octal);
    }
}

void ExportSymbols(SymbolTable* st, char* inputFileName, unsigned short dataMemOffset, unsigned short codeMemOffset)
{
    char externFileName[FILE_NAME_LEN], entryFileName[FILE_NAME_LEN];
    FILE* externFilePtr=NULL, *entryFilePtr=NULL;
    Node* current;
    Symbol* symbol;

    assert(st!=NULL);
    sprintf(entryFileName, "%s.ent", inputFileName);
    sprintf(externFileName, "%s.ext", inputFileName);

    current=st->head;
    while(current!=NULL)
    {
        symbol=(Symbol*)current->data;
        if(symbol->type==Extern)
        {
            if(externFilePtr==NULL)
            {
                externFilePtr=TryOpenFile(externFileName, "w");
            }
            if(symbol->references!=NULL)
            {
                PrintExternFileRow(symbol, externFilePtr);
            }
        }
        else if(symbol->isEntry)
        {
            if(entryFilePtr==NULL)
            {
                entryFilePtr=TryOpenFile(entryFileName, "w");
            }
            PrintEntryFileRow(symbol, entryFilePtr, dataMemOffset, codeMemOffset);
        }
        current=current->next;
    }
    if(externFilePtr!=NULL)
    {
        fclose(externFilePtr);
    }
    if(entryFilePtr!=NULL)
    {
        fclose(entryFilePtr);
    }
}
