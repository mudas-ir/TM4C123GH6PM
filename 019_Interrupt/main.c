
/*Toggle red LED on PF1 continuously. Upon pressing either SW1, the green LED of PF3 should toggle for three times.
main program toggles red LED while waiting for interrupt from SW1 notice in Table 6-7, IRQ30 is assigned to PORTF */

#include <stdint.h>
#include "tm4c123gh6pm.h"

void delayMs(int n);

int main(void)
{
    SYSCTL_RCGCGPIO_R|= 0x20;   /* enable clock to PORTF */
    
    /* configure PORTF for switch input and LED output */
    GPIO_PORTF_DIR_R &= ~0x11;        /* make PORTF4 input for switch */
    GPIO_PORTF_DIR_R |= 0x0E;         /* make PORTF3, 2, 1 output for LEDs */
    GPIO_PORTF_DEN_R |= 0x1E;         /* make PORTF4-0 digital pins */
    GPIO_PORTF_PUR_R |= 0x10;         /* enable pull up for PORTF4 */
    
    /* configure PORTF4 for falling edge trigger interrupt */
   
    GPIO_PORTF_IEV_R &= ~0x10;        /* falling edge trigger */
    GPIO_PORTF_IM_R  |= 0x10;         /* unmask interrupt */
    
    /* enable interrupt in NVIC and set priority to 3 */
    NVIC_PRI7_R = 0x600000;     /* set interrupt priority to 3 */
    NVIC_EN0_R |= 0x40000000;  /* enable IRQ30 (D30 of ISER[0]) */

    /* toggle the red LED (PF1) continuously */
    while(1)
    {
        GPIO_PORTF_DATA_R |= 0x02;
        delayMs(250);
        GPIO_PORTF_DATA_R &= ~0x02;
        delayMs(250);
    }
}

/* SW1 is connected to PF4 pin, SW2 is connected to PF0. */
/* Both of them trigger PORTF interrupt */
void GPIOF_Handler(void)
{
    int i;
    
    /* toggle green LED (PF3) three times */
    for (i = 0; i < 3; i++)
    {
        GPIO_PORTF_DATA_R = 0x08;
        delayMs(250);
        GPIO_PORTF_DATA_R = 0x00;
        delayMs(250);
    }
    GPIO_PORTF_ICR_R |= 0x11; /* clear the interrupt flag before return */
}

/* delay n milliseconds (16 MHz CPU clock) */
void delayMs(int n)
{
    int i, j;
    for(i = 0 ; i < n; i++)
        for(j = 0; j < 3180; j++)
            {}  /* do nothing for 1 ms */
}

