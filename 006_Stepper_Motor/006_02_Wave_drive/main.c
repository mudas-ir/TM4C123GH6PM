/* Program to Drive Stepper Motor in Wave Drive or Single Coil Excitation Method */
#include <stdint.h>
#include "tm4c123gh6pm.h"
#define N 5
void delayMs(int n);
int main(void)
{
    SYSCTL_RCGCGPIO_R |= 0x10; /* enable clock to GPIO PORTE at clock gating control register */
    GPIO_PORTE_DIR_R = 0x1E;   /* enable the GPIO pins for the PE1/PE2/PE3/PE4 as output */
    GPIO_PORTE_DEN_R = 0x1E;
    while (1)
    {
        GPIO_PORTE_DATA_R = 0x10; // 1 0000 - L4
        delayMs(N);
        GPIO_PORTE_DATA_R = 0x08; // 0 1000 - L3
        delayMs(N);
        GPIO_PORTE_DATA_R = 0x04; // 0 0100 - L2
        delayMs(N);
        GPIO_PORTE_DATA_R = 0x02; // 1 0000 - L1
        delayMs(N);
    }
}
/* delay n milliseconds (16 MHz CPU clock) */
void delayMs(int n)
{
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < 2100; j++)
        {
        } /* do nothing for 1 ms */
}