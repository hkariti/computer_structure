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
        
        ; SW Pipeline Preample (4 stages)
        ld      r2, x(r10)    	; r2 <= X[r10] (stage1, itr 1)
        ld      r6, y(r10) 		; r6 <= Y[r10] (stage3, itr 1)
        dmul   	r4, r2, r3      ; r4 <= r2 * a (stage2, itr 1)
        ld      r2, x+8(r10)   	; r2 <= X[r10] (stage1, itr 2)
        daddi   r13, r13, -3    ; decrease 3 iterations so we clear the pipeline
        dadd    r6, r4, r6      ; r6 <= r4 + r6 (stage3, itr 1)
        dmul   	r4, r2, r3      ; r4 <= r2 * a (stage2, itr 2)
        ld      r2, x+16(r10)  	; r2 <= X[r10] (stage1, itr 3)
        ;*** Start the loop
loop:  
        sd      r6, y(r10)  	; Y[r10] <= r6 (stage4, itr i)
        ld      r6, y+8(r10) 	; r6 <= Y[r10] (stage3, itr i+1)
        daddi   r13, r13, -1      	
        daddi   r10, r10, 8     
        dadd    r6, r4, r6      ; r6 <= r4 + r6 (stage3, itr i+1)
        dmul   	r4, r2, r3      ; r4 <= r2 * a (stage2, itr i+2)
        bnez    r13, loop
        ld      r2, x+16(r10)    	; r2 <= X[r10] (stage1, itr i+3)
		; SW Pipeline emptying
        sd      r6, y(r10)  	; Y[r10] <= r6 (stage4, itr n-2)
        ld      r6, y+8(r10) 	; r6 <= Y[r10] (stage3, itr n-1)
        dadd    r6, r4, r6      ; r6 <= r4 + r6 (stage3, itr n-1)
        dmul   	r4, r2, r3      ; r4 <= r2 * a (stage2, itr n)
        sd      r6, y+8(r10)  	; Y[r10] <= r6 (stage4, itr n-1)
        ld      r6, y+16(r10) 	; r6 <= Y[r10] (stage3, itr n)
        dadd    r6, r4, r6      ; r6 <= r4 + r6 (stage3, itr n)
        sd      r6, y+16(r10)  	; Y[r10] <= r6 (stage4, itr n)
        halt
	
	;**************  Data Area ***************
		.data
zero:   .word       0
rounds:	.word       20
a:		.word	    10
x:		.word     1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
y:		.word     5,4,5,4,5,4,5,4,5,4,5,4,5,4,5,4,5,4,5,4




