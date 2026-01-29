	PRESERVE8
	THUMB

; Vector Table Mapped to Address 0 at Reset
; Linker requires __Vectors to be exported

	AREA RESET, DATA, READONLY
	EXPORT __Vectors

__Vectors
	DCD 0x00000200 ; initial Stack Pointer
	DCD Reset_Handler ; reset vector

	; space 8

	ALIGN

	; Linker requires Reset_Handler
	
	AREA MYCODE, CODE, READONLY

	ENTRY
	EXPORT Reset_Handler
	
Reset_Handler
		mov r0, #3
		mov r1, #5
		add r2, r0, r1
		
STOP 	B STOP

		end