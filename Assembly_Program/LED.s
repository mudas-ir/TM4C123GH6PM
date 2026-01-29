;Assembly program to blink RED & GREEN LED 

GPIO_PORTF_DATA_R EQU 0x400253FC
GPIO_PORTF_DIR_R EQU 0x40025400
GPIO_PORTF_DEN_R EQU 0x4002551C
SYSCTL_RCGCGPIO_R EQU 0x400FE608

		PRESERVE8
		THUMB

		AREA RESET, DATA, READONLY
		EXPORT __Vectors

__Vectors
		DCD 0x20008000
		DCD Reset_Handler

		AREA MYCODE, CODE, READONLY
		EXPORT Reset_Handler
	
Reset_Handler
		
		bl portf_init
	
loop	LDR R1, =GPIO_PORTF_DATA_R
		mov r0, #0x02
		STR R0, [R1],#27
		bl delay
		LDR R1, =GPIO_PORTF_DATA_R
		mov r0, #0x08
		STR R0, [R1],#31
		bl delay
		b loop
		
		ALIGN
		
portf_init
		LDR R1, =SYSCTL_RCGCGPIO_R
		mov R0, #0x20
		STR R0, [R1],#40
		NOP
		LDR R1, =GPIO_PORTF_DEN_R
		mov r0, #0x0E
		STR R0, [R1], #45
		LDR R1, =GPIO_PORTF_DIR_R
		mov r0, #0x0E
		STR R0, [R1],#48
		bx lr
	
delay
		ldr r3,=4000000
b1
		subs r3,r3, #1
		bne b1
		bx lr
	
		ALIGN
		END