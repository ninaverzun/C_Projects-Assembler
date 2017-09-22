.entry LOOP
.entry LENGTH
.extern L3
.extern W
MAIN:	mov K[2-4], W
;
;
STR: .string "abcdef"
add r2, STR
LOOP:	jmp W
prn #-5
	sub	r1,r4
LENGTH: .data 6,-9,15
inc K

;
mov LOOP[1-13],r3
K: .data 22
bne L3
END: stop
