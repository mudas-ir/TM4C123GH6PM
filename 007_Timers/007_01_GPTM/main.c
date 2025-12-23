/*****************************************************************************************
 * FILE NAME : rgb_led_blink_timer1.c
 *
 *
 * DATE      : 23/12/2025
 *
 * TARGET    : TM4C123GH6PM (Tiva C Series)
 *
 * DESCRIPTION:
 * This program demonstrates how to blink the on-board RGB LED present on the
 * TM4C123GH6PM LaunchPad using Timer1 to generate a precise delay of 1 second.
 *
 * The RGB LED is connected to GPIO Port F:
 *   - PF1 → Red LED
 *   - PF2 → Blue LED
 *   - PF3 → Green LED
 *
 * Timer1 is configured in 16-bit periodic down-counting mode to generate a
 * 1 ms delay. This 1 ms delay is repeatedly called 1000 times to obtain
 * a total delay of 1 second.
 *
 * The LEDs blink in the following sequence:
 *   Red → Green → Blue → repeat
 *
 * This program helps in understanding:
 *   - GPIO configuration
 *   - Timer configuration
 *   - Hardware-based delay generation
 *****************************************************************************************/

#include <stdint.h>
#include "tm4c123gh6pm.h"

/* Function prototype */
void delayMs(int time);

int main(void)
{
    /* Enable clock for GPIO Port F */
    SYSCTL_RCGCGPIO_R |= 0x20;

    /*
     * Configure GPIO Port F:
     * PF1, PF2, PF3 → Output (RGB LEDs)
     * Binary: 0000 1110 = 0x0E
     */
    GPIO_PORTF_DIR_R = 0x0E;

    /* Enable digital function for PF1, PF2, PF3 */
    GPIO_PORTF_DEN_R = 0x0E;

    while (1)
    {
        /* Turn ON Red LED (PF1) */
        GPIO_PORTF_DATA_R = 0x02;
        delayMs(1000); /* 1 second delay */

        /* Turn ON Green LED (PF3) */
        GPIO_PORTF_DATA_R = 0x08;
        delayMs(1000); /* 1 second delay */

        /* Turn ON Blue LED (PF2) */
        GPIO_PORTF_DATA_R = 0x04;
        delayMs(1000); /* 1 second delay */
    }
}

/*****************************************************************************************
 * FUNCTION NAME : delayMs
 *
 * DESCRIPTION:
 * Generates a delay in milliseconds using Timer1.
 *
 * Timer1 is configured as:
 *   - 16-bit timer
 *   - Periodic mode
 *   - Down counter
 *
 * The timer is loaded with a value corresponding to 1 ms delay assuming
 * a 16 MHz system clock.
 *
 * ARGUMENT:
 *   time → number of milliseconds to delay
 *****************************************************************************************/
void delayMs(int time)
{
    int i;

    /* Enable clock for Timer1 */
    SYSCTL_RCGCTIMER_R |= 0x02;

    /* Disable Timer1 before configuration */
    TIMER1_CTL_R = 0x00;

    /* Configure Timer1 as 16-bit timer */
    TIMER1_CFG_R = 0x04;

    /* Configure Timer1A:
     * - Periodic mode
     * - Down counter
     */
    TIMER1_TAMR_R = 0x02;

    /*
     * Load value for 1 ms delay:
     * 16 MHz clock → 16,000 cycles per millisecond
     */
    TIMER1_TAILR_R = 16000 - 1;

    /* Clear TimerA timeout flag */
    TIMER1_ICR_R = 0x01;

    /* Enable Timer1A */
    TIMER1_CTL_R |= 0x01;

    /* Loop for required number of milliseconds */
    for (i = 0; i < time; i++)
    {
        /* Wait until timeout flag is set */
        while ((TIMER1_RIS_R & 0x01) == 0)
            ;

        /* Clear timeout flag for next iteration */
        TIMER1_ICR_R = 0x01;
    }
}
