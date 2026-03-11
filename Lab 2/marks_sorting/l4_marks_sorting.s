    THUMB
    PRESERVE8

;/---------------AREA FOR CONSANT DATA------------------/
    AREA marks_constant_data, DATA, READONLY

marks_list      DCD     12,35,48,68,98,25,66,41,90,76,35,84,88,76,75,71,55,63,58,50,70
marks_count     EQU     21  ; count of marks in the list

;/---------------AREA FOR VARIABLE DATA------------------/
    AREA counters, DATA, READWRITE

count_greater_than_70       SPACE   4
count_between_50_and_70     SPACE   4 

;----------------------CODE AREA------------------------------/
    AREA |.l4_marks_sorting|, CODE, READONLY

    ENTRY
    EXPORT __main

__main

    ; Count the marks greater than 70
    LDR     R0, =marks_list
    LDR     R1, =marks_count
    BL      subroutine_count_greater_than_70 ; Takes input R0 and R1 gives output in R4

    ; Store the output from the subroutine
    LDR     R2, =count_greater_than_70
    STR     R4, [R2]

    ; Count the marks between 50 and 70
    LDR     R0, =marks_list
    LDR     R1, =marks_count
    BL      subroutine_count_between_50_and_70 ; Takes input R0 and R1 gives output in R5

    ; Store the output from the subroutine
    LDR     R2, =count_between_50_and_70
    STR     R5, [R2]

; Stop the program
STOP
    B   STOP


;----------------------SUBROUTINE COUNT GREATER THAN 70------------------------------/
subroutine_count_greater_than_70    PROC

    MOV     R4, #0

check_greater_loop
    LDR     R2, [R0], #4
    CMP     R2, #70
    BLS     skip_increment_1
    ADD     R4, R4, #1

skip_increment_1 
    SUBS    R1, R1, #1
    BNE     check_greater_loop
    BX      LR

    ENDP

subroutine_count_between_50_and_70  PROC
    MOV     R5, #0

check_in_range_loop
    LDR R2, [R0], #4
    CMP R2, #50
    BLO skip_increment_2
    CMP R2, #70
    BHI skip_increment_2
    ADD R5, R5, #1

skip_increment_2
    SUBS R1, R1, #1
    BNE check_in_range_loop
    BX LR

    ENDP

    END

