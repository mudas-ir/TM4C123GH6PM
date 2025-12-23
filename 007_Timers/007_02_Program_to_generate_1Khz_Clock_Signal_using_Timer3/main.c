/*****************************************************************************************
 * FILE NAME : timer3_pwm_1khz_pb2.c
 *
 * DATE      : 23/12/2025
 *
 * TARGET    : TM4C123GH6PM (Tiva C Series)
 *
 * DESCRIPTION:
 * This program configures Timer 3A of the TM4C123GH6PM microcontroller to
 * generate a 1 kHz PWM (square wave) signal with 50% duty cycle.
 *
 * The PWM output is generated using the Timer Capture/Compare/PWM (CCP)
 * functionality and is routed to GPIO Port B pin PB2.
 *
 * Hardware Mapping:
 *   - Timer Used      : Timer 3A
 *   - Timer Mode      : 16-bit periodic PWM mode (down counter)
 *   - Output Pin      : PB2
 *   - Alternate Func  : T3CCP0
 *
 * Signal Characteristics:
 *   - System Clock    : 16 MHz (default PIOSC)
 *   - PWM Frequency  : 1 kHz
 *   - PWM Period     : 1 ms
 *   - Duty Cycle     : 50%
 *
 * Calculation:
 *   Timer clock = 16 MHz
 *   Counts for 1 ms = 16,000
 *   Period Load     = 16000 - 1
 *   Match Value     = 8000  - 1  → 50% duty cycle
 *
 * This program demonstrates:
 *   - Timer PWM configuration
 *   - GPIO alternate function selection
 *   - CCP pin routing
 *****************************************************************************************/

#include <stdint.h>
#include "tm4c123gh6pm.h"

int main(void)
{
    /* Optional: System clock configuration (commented out)
     * SYSCTL_RCC_R = 0x01C01551; // 50 MHz system clock
     */

    /* Enable clock for GPIO Port B */
    SYSCTL_RCGCGPIO_R |= 0x02;

    /*
     * Configure PB2 for Timer 3A CCP output:
     * PB2 → T3CCP0
     */
    GPIO_PORTB_DIR_R = 0x04;         /* Set PB2 as output */
    GPIO_PORTB_AFSEL_R = 0x04;       /* Enable alternate function on PB2 */
    GPIO_PORTB_PCTL_R |= 0x00000700; /* Assign PB2 to T3CCP0 function */
    GPIO_PORTB_DEN_R = 0x04;         /* Enable digital function on PB2 */

    /* Enable clock for Timer 3 */
    SYSCTL_RCGCTIMER_R |= 0x08;

    /* Disable Timer3A before configuration */
    TIMER3_CTL_R = 0x00;

    /* Configure Timer3 as 16-bit timer */
    TIMER3_CFG_R = 0x04;

    /*
     * Configure Timer3A Mode:
     * - PWM mode enabled
     * - Periodic mode
     * - Down-counter
     *
     * 0x0A = 1010b
     */
    TIMER3_TAMR_R = 0x0A;

    /*
     * Load value for 1 kHz PWM period:
     * 16 MHz / 1000 Hz = 16,000 counts
     */
    TIMER3_TAILR_R = 16000 - 1;

    /*
     * Match value for 50% duty cycle:
     * Output toggles when counter reaches match value
     */
    TIMER3_TAMATCHR_R = 8000 - 1;

    /* Clear Timer3A timeout flag */
    TIMER3_ICR_R = 0x01;

    /* Enable Timer3A */
    TIMER3_CTL_R |= 0x01;

    while (1)
    {
        /*
         * Timer hardware generates PWM automatically.
         * CPU is not required here — this loop is optional.
         * Kept only for demonstration/debugging.
         */

        while ((TIMER3_RIS_R & 0x01) == 0)
            ; /* Wait for timeout flag */

        TIMER3_ICR_R = 0x01; /* Clear timeout flag */
    }
}
