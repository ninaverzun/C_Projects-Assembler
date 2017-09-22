assembler: main.o AssemblerScan.o BitOperations.o LinkedList.o SymbolTable.o MachineCodeTranslations.o ErrorHandling.o FatalErrorHandling.o FileAndParsingOperations.o
	gcc -g -ansi -Wall main.o AssemblerScan.o LinkedList.o SymbolTable.o BitOperations.o MachineCodeTranslations.o ErrorHandling.o FatalErrorHandling.o FileAndParsingOperations.o -o assembler
main.o: main.c
	gcc -c -ansi -Wall -pedantic main.c -o main.o
BitOperations.o: BitOperations.c
	gcc -c -ansi -Wall -pedantic BitOperations.c -o BitOperations.o
SymbolTable.o: SymbolTable.c
	gcc -c -ansi -Wall -pedantic SymbolTable.c -o SymbolTable.o
LinkedList.o: LinkedList.c
	gcc -c -ansi -Wall -pedantic LinkedList.c -o LinkedList.o
MachineCodeTranslations.o: MachineCodeTranslations.c
	gcc -c -ansi -Wall -pedantic MachineCodeTranslations.c -o MachineCodeTranslations.o
FatalErrorHandling.o: FatalErrorHandling.c
	gcc -c -ansi -Wall -pedantic FatalErrorHandling.c -o FatalErrorHandling.o
ErrorHandling.o: ErrorHandling.c
	gcc -c -ansi -Wall -pedantic ErrorHandling.c -o ErrorHandling.o
FileAndParsingOperations.o: FileAndParsingOperations.c
	gcc -c -ansi -Wall -pedantic FileAndParsingOperations.c -o FileAndParsingOperations.o
AssemblerScan.o: AssemblerScan.c
	gcc -c -ansi -Wall -pedantic AssemblerScan.c -o AssemblerScan.o
clean: rm *~

