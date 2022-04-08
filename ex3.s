// ex3.s: Sum of Array using MOVR

.data

A:  WORD    1
    WORD    2
    WORD    3
    WORD    4
    WORD    5
    WORD    -1

R:  WORD    0

.text

	MOV	R0, @A	    // Load base address
	MOV	R1, #1	    // INC = 1
	MOV	R2, #0	    // Sum = 0

Loop:	MOVR    R3, R0, #0  // Load Array Element
	JMPN	R3, Done    // Check for -1
	ADD	R2, R2, R3  // Sum = Sum + Array Element
	ADD	R0, R0, R1  // Array address = Array Address + 1
	JMP	Loop	    // Do it again

Done:	MOV	R, R2	    // Store sum in R
	END
