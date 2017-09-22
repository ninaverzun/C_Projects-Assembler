.entry LOOP
.entry LENGTH
.extern L3
.extern W

;.entry declaration without an actual declaration
.entry BAD_ENTRY

;invalid destination operand type
MAIN:	mov W,K[2-4]

;invalid Dynamic addressing
MAIN:	mov K[2-4, W
;
;non existing register 
add r67, STR

add r6, STR

;too many parameters
LOOP:	jmp W Bla Bla Bla
prn #-5

MAIN:	mov K[2-4], W
;redeclaration of MAIN
MAIN:	mov K[2-4], W

;invalid immidiate addressing
prn #-5hahaha

;symbol name equals a register name
R7:	sub	r1,r4
inc K

;
mov LOOP[1-13],r3

;non existing operation
bene L3

;non existing directive
.stringing "hhhhhhhhhhhhhhhhh"

bne L3
END: stop
STR: .string "abcdef"
LENGTH: .data 6,-9,15

;non numeric parameter to .data
LENGTH: .data 6,-9,15,find the error here, 3,4,6

K: .data 22

;too few arguments for .data directive
.data 

