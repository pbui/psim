// Example 2: Frequency Calculator

.data

A0:	WORD	24
	WORD	1
	WORD	24
	WORD	2
	WORD	24
	WORD	3
	WORD	24
	WORD	-1

TGT:	WORD	24

RESULT:	WORD	0

.text
	MOV	R0, TGT	    //	Load target number
	MOV	R1, #0	    //	Set count to 0
	MOV	R2, #1	    //	Load increment

LOAD_A:	MOV	R3, A0	    //	Load array element
	JMPN	R3, END	    //	if negative end
	SUB	R4, R3, R0  //	R4 = R3	- R0

	JMPZ	R4, INC	    //	if R4 == 0 then Increment
	JMP	LOAD_N

INC:	ADD	R1, R1, R2  //	R1 = R1 + R2

LOAD_N:	MOV	R5, LOAD_A  //	Load array access instruction
	ADD	R5, R5, R2  //	R5 = R5 + R2
	MOV	LOAD_A, R5  //	Store array access instruction

	JMP	LOAD_A	    //	Go back to top

END:	MOV	RESULT, R1  //	Store count
	END
