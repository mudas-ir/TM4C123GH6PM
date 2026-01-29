;https://circuitcellar.com/research-design-hub/programming-the-cortex-m4-in-assembly/	

		PRESERVE8
		THUMB

; Vector Table Mapped to Address 0 at Reset
; Linker requires __Vectors to be exported

		AREA RESET, DATA, READONLY
		EXPORT __Vectors

__Vectors
		DCD 0x20008000 ; initial Stack Pointer
		DCD Reset_Handler ; reset vector

		space 8
test 	DCD 0xCDEF
	
		ALIGN

	; Linker requires Reset_Handler
	
		AREA MYCODE, CODE, READONLY

		ENTRY
		EXPORT Reset_Handler
	
Reset_Handler
	ldr r7, =test
	ldr r8, [r7], #4
	ldr r3, =0x20000000
	mov r4, #0xAB
	str r8, [r3], #4 
	str r4, [r3]
		
STOP 	B STOP

		end