#include <stdint.h>
#include "tm4c123gh6pm.h"
void UART3Tx(char c);
char UART3Rx(void);
void delayMs(int n);
int main(void) 
{
		char c;
	  SYSCTL_RCC_R = 0x02401551; // 40MHz Clock from POISC=16MHz Divisor=0x03(200Mhz/5)
    SYSCTL_RCGCUART_R = 0x08;  /* provide clock to UART3 */
    SYSCTL_RCGCGPIO_R = 0x04;  /* enable clock to PORTC */ 
    /* UART3 initialization */
    UART3_CTL_R = 0;         /* disable UART3 */
    UART3_IBRD_R = 260;      /* 16MHz/(16*9600)=104.1667  9600=>baud rate */ //40000000/(16*9600)=260.4167
    UART3_FBRD_R = 27;       /* fraction part =integer(0.1667*64+0.5)=11.168*/ //0.4167*64+0.5 = 27.16
    UART3_CC_R = 0;          /* use system clock */
    UART3_LCRH_R = 0x60;     /* 8-bit, no parity, 1-stop bit, no FIFO */
    UART3_CTL_R = 0x301;     /* enable UART3, TXE, RXE */
                            /* UART3 TX and RX use PC6=RX and PC7=TX. Set them up. */
    GPIO_PORTC_DEN_R |= 0xC0;      /* Make PC6 and PC7 as digital */
    GPIO_PORTC_AFSEL_R |= 0xC0;    /* Use PC6,PC7 alternate function */
    GPIO_PORTC_PCTL_R |= 0x11000000;     /* configure PC6 and PC7 for UART3 */    
    delayMs(100);             /* wait for output line to stabilize */  
    
	
	while(1)
		{
			//c=UART3Rx();
			//UART3Tx(c);
			UART3Tx('E');
			UART3Tx('N');
			UART3Tx('T');
			UART3Tx('E');    
			UART3Tx('R');  
			UART3Tx(' ');  
			UART3Tx('T');  
			UART3Tx('E');  
			UART3Tx('X');  
			UART3Tx('T');  
			UART3Tx(':');
		}
}

/* UART3 Receive */
/* This function waits until a character is received then returns it. */
char UART3Rx(void)  
{
    char c;
    while((UART3_FR_R & 0x10)!=0); /* wait until the buffer is not empty */
    c = UART3_DR_R;                  /* read the received data */
    return c;                       /* and return it */
}

/* UART3 Transmit */
/* This function waits until the transmit buffer is available then */
/* writes the character in the transmit buffer. It does not wait */
void UART3Tx(char c)  
{
    while((UART3_FR_R & 0x20) != 0); /* wait until Tx buffer not full */
    UART3_DR_R = c;                  /* before giving it another byte */
}


void delayMs(int n)
{
    int i, j;
    for(i = 0 ; i < n; i++)
        for(j = 0; j < 4000; j++);              /* do nothing for 1 ms */
}
