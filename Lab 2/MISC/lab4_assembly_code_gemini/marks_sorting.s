;------------------------------------------------------------------------------
; Set up assembly environment
;------------------------------------------------------------------------------
    PRESERVE8       ; preserve 8-byte stack alignment
    THUMB           ; interpret instructions as Thumb instructions

;------------------------------------------------------------------------------
; Data area in RAM with variables
;------------------------------------------------------------------------------
    AREA    counters, DATA, READWRITE
    
count_greater_70  SPACE   4   ; Reserves 4 bytes in RAM to store the count of marks greater than 70
count_range_50_70 SPACE   4   ; Reserves 4 bytes in RAM to store the count of marks between 50 and 70

;------------------------------------------------------------------------------
; Read-Only Data Area in ROM with constants
;------------------------------------------------------------------------------
    AREA    constant_data, DATA, READONLY

marks         DCD     12,35,48,68,98,25,66,41,90,76,35,84,88,76,75,71,55,63,58,50,70
num_of_marks  EQU     21  ; Defines a constant with the value 21

;------------------------------------------------------------------------------
; Code Area
;------------------------------------------------------------------------------
    AREA    |.marks_sorting|, CODE, READONLY
    
    ENTRY              							  ; Highlights the first instruction to be executed
    EXPORT  __main        						  ; Makes the 'main' label visible to the linker

__main
    ;Call the first subroutine to find marks > 70
    LDR     R0, =marks                 			  ; Loads R0 with the starting memory address of the marks. LDR loads constants
    LDR     R1, =num_of_marks           		  ; Load R1 with the number of mark enteries
    BL      count_greater_than_70_subroutine      ; Branch with Link to the 'count_greater_than_70_subroutine' which returns the result in R4

    ;Store the result from the first subroutine
    LDR     R2, =count_greater_70      			  ; Load R2 with the address of the 'count_greater_70' variable
    STR     R4, [R2]                    		  ; Store the result from R4 into the memory location pointed to by R2
    
    ;Call the second subroutine to count marks from 50 to 70
    LDR     R0, =marks                  		  ; Load R0 with the starting address of the list for the next subroutine
    LDR     R1, =num_of_marks           		  ; Load R1 with the count for the next subroutine
    BL      count_range_50_70_subroutine          ; Branch with Link to the 'count_range_50_70_subroutine' which returns the result in R5

    
    ;Store the result from the second subroutine
    LDR     R2, =count_range_50_70      		  ; Load R2 with the address of the 'count_range_50_70' variable
    STR     R5, [R2]                    		  ; Store the result from R5 into the memory location pointed to by R2

STOP
    B       STOP

;------------------------------------------------------------------------------
; count_greater_than_70_subroutine 
; Input: R0 -> Address of the marks list, R1 = Number of marks
; Output: R4 = Count of marks > 70
;------------------------------------------------------------------------------
count_greater_than_70_subroutine    PROC
    MOV     R4, #0                  ; Initialize the counter to zero in R4
    
loop_check_greater
    LDR     R2, [R0], #4            ; Load the mark from the address in R0 into R2, then increment address in  R0 by 4 bytes
    CMP     R2, #70                 ; Compare the mark in R2 with the value 70 and update (N, Z, C, V)
    BLS     skip_inrement_count_1   ; Branch to 'skip_inrement_count_1' if the mark is Lower or Same (<= 70) (Z = 1, or C = 0)
    ADD     R4, R4, #1              ; If not lower or same, it must be greater, so increment the counter
    
skip_inrement_count_1
    SUBS    R1, R1, #1              ; Decrement the loop counter and update the Z flag
    BNE     loop_check_greater      ; Branch back to 'loop_check_greater' if  Z != 1
    
    BX      LR                      ; Return from the subroutine.
    ENDP

;------------------------------------------------------------------------------
; count_range_50_70_subroutine 
; Input: R0 = Address of the marks list, R1 = Number of marks
; Output: R5 = Count of marks in the range [50, 70]
;------------------------------------------------------------------------------
count_range_50_70_subroutine        PROC
    MOV     R5, #0                  ; Initialize the counter to zero in R5

loop_check_range
    LDR     R2, [R0], #4            ; Load the mark from the address in R0 into R2, then increment address in  R0 by 4 bytes
    CMP     R2, #50                 ; Compares the mark with 50
    BLO     skip_inrement_count_2   ; Branch to 'skip_inrement_count_2' if the mark is Lower (< 50)
    CMP     R2, #70                 ; Compares the mark with 70
    BHI     skip_inrement_count_2   ; Branch to 'skip_inrement_count_2' if the mark is Higher (> 70)
    ADD     R5, R5, #1              ; If neither branch was taken, the mark is in range [50, 70]. Increment the counter

skip_inrement_count_2 
    SUBS    R1, R1, #1              ; Decrement the loop counter and update the Z flag
    BNE     loop_check_range        ; Branch back to 'loop_check_range' if R1 is not zero
    
    BX      LR                      ; Return from the subroutine
    ENDP

    END
