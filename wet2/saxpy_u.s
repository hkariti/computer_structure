	;***************************************    
	;          Computer Architecture
	;                SAXPY 
	;***************************************    

	
	;**************  Code Area ***************    
        .text
main:
        ;*** Initialization
        ld       r10, zero(r0)      ; init x vector pointer
        ld       r13, rounds(r0)    ; Load vector size
        ld       r3,  a(r0)         ; load a
        
        ;*** Start the loop        
loop:   ld      r2, x(r10)    	; r2 <= X[r10]
        ld      r6, y(r10) 		; r6 <= Y[r10]
        ld      r1, x+8(r10)  	; r1 <= X[r10+8]
        dmul   	r4, r2, r3      ; r4 <= r2 * a
        ld      r5, y+8(r10)	; r5 <= Y[r10+8]
        dmul   	r7, r1, r3      ; r7 <= r1 * a
        daddi   r13, r13, -2
        dadd    r6, r4, r6      ; r6 <= r4 + r6
        sd      r6, y(r10)  	; Y[r10] <= r6

        dadd    r5, r7, r5      ; r5 <= r7 + r5
        sd      r5, y+8(r10)  	; Y[r10+8] <= r5

        daddi   r10, r10, 16
        bnez    r13, loop
        nop
        halt
	
	;**************  Data Area ***************
		.data
zero:   .word       0
rounds:	.word       20
a:		.word	    10
x:		.word     1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
y:		.word     5,4,5,4,5,4,5,4,5,4,5,4,5,4,5,4,5,4,5,4




