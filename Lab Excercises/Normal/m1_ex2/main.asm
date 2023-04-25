;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
EX_2:
			MOV 	#vet, R5
			call	#maior16
			jmp		$
			nop

maior16:
			MOV.B	@R5, R8
			incd	R5
			MOV		#0, R6
			MOV		#0, R7

LB1:
			CMP		@R5, R6
			jeq		LB3
			jhs		LB2
			MOV		@R5, R6
			MOV		#1, R7

LB2:
			incd	R5
			DEC		R8
			jne		LB1
			ret

LB3:
			inc		R7
			jmp		LB2
			nop



			.data
vet:		.word	8,4,14,3,1,14,14,3,0
                                            

;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            
