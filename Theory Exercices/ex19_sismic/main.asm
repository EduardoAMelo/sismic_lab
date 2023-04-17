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
; 1- Isolar o nibble
; 2- Verificar se é letra ou número
; 3- Aplicar a transf
; 4- Escrever na Saída

w16_ascii:
		PUSH	R4
		PUSH	R5
		MOV		#4, R5

lb1:
		MOV		R12, R4
		AND		#0xF, R4

		CMP		#0xA, R4
		jhs		letra

numero:
		ADD		#0x30, R4
		jmp		continue
letra:
		ADD		#0x37, R4

continue:
		MOV.B	R4, 0(R13)
		INC.W	R13

		RRC		R12
		RRC		R12
		RRC		R12
		RRC		R12

		DEC		R5
		JNZ		lb1
		POP		R5
		POP		R4
		ret
                                            

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
            
