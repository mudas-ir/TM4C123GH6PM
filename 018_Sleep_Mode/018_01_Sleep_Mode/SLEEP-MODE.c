
/* SLEEP-MODE */
/*Toggle GREEN LED for 5 times then processor enters SLEEP MODE by executing "WFI" instruction. 
Upon pressing SW1 interrupt event occurs and processor return back from sleep mode and toggle RED LED times 
During Execution of ISR BLUE LED will toggle for 3 times*/

#include <stdint.h>
#include "tm4c123gh6pm.h"

void delayMs(int n);

int main(void)
{
	  int j;
    SYSCTL_RCGCGPIO_R|= 0x20;   /* enable clock to PORTF */
    
    /* configure PORTF for switch1 input and LED output */
    GPIO_PORTF_DIR_R |= 0x0E;         /* make PORTF PIN3,2&1 output for LEDs */
    GPIO_PORTF_DEN_R |= 0x1E;         /* make PORTF PIN4,3,1 digital pins */
    GPIO_PORTF_PUR_R |= 0x10;         /* enable pull up for PORTF4 */
    
    /* configure PORTF4 for falling edge trigger interrupt */
   
    GPIO_PORTF_IEV_R &= ~0x10;        /* falling edge trigger */
    GPIO_PORTF_IM_R  |= 0x10;         /* unmask interrupt */
    
    /* enable interrupt in NVIC and set priority to 3 */
    //NVIC_PRI7_R = 0x600000;     /* set interrupt priority to 3 */
    NVIC_EN0_R |= 0x40000000;  /* enable IRQ30 (D30 of ISER[0]) */
 
	 /* toggle the GREEN LED 5 times */
	  for(j=0;j<5;j++)
    {
        GPIO_PORTF_DATA_R |= 0x08;
        delayMs(250);
        GPIO_PORTF_DATA_R &= ~0x08;
        delayMs(250);
    }
		
   __asm("WFI");   // Enters Sleep Mode. Wait for Interrupt instruction will bring back MCU from Sleep mode
    
    /* toggle the red LED (PF1) continuously */
   while(1)
    {
        GPIO_PORTF_DATA_R = 0x02;
        delayMs(250);
        GPIO_PORTF_DATA_R &= ~0x02;
        delayMs(250);
    }
}

/* SW1 is connected to PF4 pin */
/* SW1 trigger PORTF interrupt */
void GPIOF_Handler(void)
{
    int i;
             /* toggle the Blue LED (PF2) 3 times */
	 for(i=0;i<3;i++)
    {
    		GPIO_PORTF_DATA_R = 0x04;
        delayMs(250);
        GPIO_PORTF_DATA_R &= ~0x04;
        delayMs(250);
		}
    GPIO_PORTF_ICR_R |= 0x10; /* clear the interrupt flag before return */
  
}

/* delay n milliseconds (16 MHz CPU clock) */
void delayMs(int n)
{
    int i, j;
    for(i = 0 ; i < n; i++)
        for(j = 0; j < 3180; j++)
            {}  /* do nothing for 1 ms */
}

