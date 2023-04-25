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
NUM		.equ	3498
		mov		#NUM, R5
		mov		#RESP, R6
		call	#alg_rom
		jmp		$
		nop

alg_rom:
		push	R7 ;usado para comparação
		clr		R7
		push	R8
		clr 	R8
		mov		#0, R12
		mov		#0, R13
		mov		#0, R14
		add		#1000, R7


mil_loop:
		sub		R7, R5
		jlo		clr_ram_one
		mov		#'M', 0(R6)
		incd	R6
		jmp		mil_loop

clr_ram_one:
		clr		R7
		clr		R8
		add 	#1000, R5
		add		#100, R7

cent_loop:
		sub		R7, R5
		jlo		cent_save
		inc		R8
		jmp		cent_loop

cent_save:
		mov		R8, R12
		clr		R8
		clr		R7
		add		#100, R5
		add		#10, R7

dec_loop:
		sub		R7, R5
		jlo		dec_save
		inc		R8
		jmp		dec_loop

dec_save:
		mov		R8, R13
		clr		R8
		clr		R7
		add		#10, R5
		add		#1, R7

unit_loop:
		sub		R7, R5
		jlo		unit_save
		inc		R8
		jmp		unit_loop

unit_save:
		mov		R8, R14
		clr		R8
		clr		R7
		add		#1, R5
		pop		R8
		pop		R7

cent_add:
		cmp		#0, R12
		jeq		dec_add
		mov		R12, R4
		mov		#'C', R8
		mov		#'D', R9
		mov		#'M', R10
		call	#save_logic

dec_add:
		cmp		#0, R13
		jeq		unit_add
		mov		R13, R4
		mov		#'X', R8
		mov		#'L', R9
		mov		#'C', R10
		call	#save_logic

unit_add:
		cmp		#0, R14
		jeq		fin
		mov		R14, R4
		mov		#'I', R8
		mov		#'V', R9
		mov		#'X', R10
		call	#save_logic

fin:
		ret


save_logic:
		cmp		#4, R4
		jeq		lb4
		cmp		#9, R4
		jeq		lb9
		push	R7
		mov		#4, R7

lb_less_iv:
		dec		R7
		jz		lb_less_ix
		cmp		R7, R4
		jnz		lb_less_iv
		jz		add_less_iv
		mov		#9, R7
		jmp     lb_less_ix

add_less_iv:
		mov		R8, 0(R6)
		incd	R6
		dec		R7
		jnz		add_less_iv
		pop		R7
		ret

lb_less_ix:
		dec		R7
		cmp		#5,	R7
		jz		add_less_ix_five
		cmp		R7, R4
		jnz		lb_less_ix
		jz		add_less_ix_five

add_less_ix_five:
		mov		R9, 0(R6)
		incd	R6
		sub		#5, R7
		jnz		add_less_ix
		ret

add_less_ix:
		mov		R8, 0(R6)
		incd	R6
		dec		R7
		jnz		add_less_iv
		pop		R7
		ret



lb4:
		mov		R8, 0(R6)
		incd	R6
		mov		R9, 1(R6)
		incd	R6
		ret

lb9:
		mov		R8, 0(R6)
		incd	R6
		mov		R10, 1(R6)
		incd	R6
		ret


		.data
RESP:       .byte       "RRRRRRRRRRRRRRRRRR",0


                                            

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
            
