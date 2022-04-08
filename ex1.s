// Example 1

.data

ZERO:   WORD    0
ONE:    WORD    1
RESULT: WORD    -1
	WORD	-20

.text

MAIN:   MOV     R0, ZERO
	MOV     R1, ONE
	ADD     R2, R0, R1
	MOV     RESULT, R2  // Store result
	SUB     R2, R2, R1
	JMPZ    R2, SKIPZ 
	ADD     R2, R2, R1
SKIPZ:	SUB     R2, R2, R1
	JMPN	R2, SKIPN
	ADD     R2, R2, R1
SKIPN:	SUB	R2, R2, R1

	MOV	R0, #-20
	ADD	R0, R0, R0
	SUB	R0, R0, R0
	MOV	RESULT, R0

	MOV	R1, #20
	ADD	R1, R1, R1
	SUB	R1, R1, R1
	MOV	RESULT, R1

	END
