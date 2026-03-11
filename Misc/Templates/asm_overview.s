	PRESERVE8
	THUMB
	
	AREA  myDATA, DATA, READWRITE
	
Result  SPACE 8         ;reserves eight bytes for variable result
	
	AREA  |.text|, CODE, READONLY
	
	ENTRY
	EXPORT main
main
	
	LDR  R0, =Value1        ; Load the starting address of
	; first 64-bit number
	LDR  R1, =Value2        ; Load the starting address of
	; second 64-bit number 
	
	BL   SUM64
	
	LDR  R0, =Result
	STR  R6, [R0]           ; Store higher 32-bits to Result
	STR  R7, [R0, #4]       ; Store lower 32-bits to Result
	
	B    STOP
	
SUM64  PROC
	
	LDR  R2, [R0]          ; Load the value of higer 32-bits
	LDR  R3, [R0, #4]      ; Load the value of lower 32-bits
	
	
	LDR  R4, [R1]
	LDR  R5, [R1, #4]
	
	ADDS R7, R3, R5       ; Add with flags update
	MOV   R6, #0xD129 
	; Move the 16-bit hex value to the lower halfword of R6
	
	MOVT  R6, #0xF29A   ; R6 = 
	; Move the 16-bit hex value to the upper halfword of R6
	
	BX   LR
	ENDP
STOP
    B STOP
	
Value1  DCD  0x12A2E640, 0xF2100123
Value2  DCD  0x001019BF, 0x40023F51
	
	END
