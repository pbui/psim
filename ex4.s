// ex4.s: IO Example

.data

.text

    MOV	    D0, R2, P5	    // Put PReg[5] into RF[2]

    MOV	    R2, #-1	    
    MOV	    D1, R2, P5	    // Put RF[2] into PReg[5]

    END
