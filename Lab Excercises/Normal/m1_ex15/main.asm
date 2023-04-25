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
                                            ; and retain curRENt section.
            .retainrefs                     ; And retain any sections that have
                                            ; refeRENces to curRENt section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
	bis.b		#BIT0, &P1DIR

	bic.b		#BIT1, &P2DIR
    bis.b		#BIT1, &P2REN
    bis.b		#BIT1, &P2OUT

    bis.b		#BIT7, &P4DIR

	bic.b		#BIT1, &P1DIR
    bis.b		#BIT1, &P1REN
    bis.b		#BIT1, &P1OUT

lb1:
	BIT.b		#BIT1, &P2IN
	jz			lb2
	bic.b		#BIT0, &P1OUT

lb4:
	bit.b		#BIT1, &P1IN
	jz			lb3
	bic.b		#BIT7, &P4OUT
	jmp			lb1

lb2:
	bis.b		#BIT0, &P1OUT
	jmp			lb4

lb3:
	bis.b		#BIT7, &P4OUT
	jmp			lb1
	nop


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
            
