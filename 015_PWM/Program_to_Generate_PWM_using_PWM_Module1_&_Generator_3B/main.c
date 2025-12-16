/***************************************************************
 * PROJECT NAME : PWM Generation using Tiva C LaunchPad
 * MCU          : TM4C123GH6PM
 * PWM MODULE   : PWM Module 1
 * PWM OUTPUT   : M1PWM7
 * GPIO PIN     : PF3 (Green LED on LaunchPad)
 *
 * DESCRIPTION :
 * ------------------------------------------------------------
 * This program generates a Pulse Width Modulated (PWM) signal
 * using PWM Module 1, Generator 3, Output B (M1PWM7).
 *
 * The PWM signal is routed to:
 *      PF3 → M1PWM7
 *
 * The PWM duty cycle is controlled by changing the compare
 * register value (PWM1_3_CMPA_R). Varying this value changes
 * the brightness of the green LED connected to PF3.
 *
 * PWM Characteristics:
 *   - Counter Mode  : Down-count
 *   - Load Value    : 0xFFFF (Sets PWM period)
 *   - Duty Cycle    : Controlled using CMPA
 *
 * AUTHOR : Mudassir
 * DATE   : 15/12/2025
 * DAY    : Monday
 ***************************************************************/

#include <stdint.h>
#include "tm4c123gh6pm.h"

int main(void)
{
    /***********************************************************
     * STEP 1: Enable clocks for GPIO Port F and PWM Module 1
     ***********************************************************/
    SYSCTL_RCGCGPIO_R |= 0x20;   // Enable clock to GPIO Port F
    SYSCTL_RCGCPWM_R  |= 0x02;   // Enable clock to PWM Module 1

    /*
     * Optional: PWM Clock Divider
     * --------------------------------------------------------
     * If you want slower PWM frequency, uncomment and select
     * one of the following:
     *
     * SYSCTL_RCC_R |= 0x00100000; // Use PWM clock pre-divider
     * SYSCTL_RCC_R |= 0x000E0000; // Divide PWM clock by 64
     */


    /***********************************************************
     * STEP 2: Configure PF3 for PWM Function (M1PWM7)
     ***********************************************************/
    GPIO_PORTF_AFSEL_R |= 0x08;        // Enable alternate function on PF3

    GPIO_PORTF_PCTL_R &= ~0x0000F000;  // Clear PCTL bits for PF3
    GPIO_PORTF_PCTL_R |=  0x00005000;  // Assign PF3 to PWM1 (M1PWM7)

    GPIO_PORTF_DEN_R  |= 0x08;         // Enable digital function on PF3


    /***********************************************************
     * STEP 3: Configure PWM Generator 3 (Output B)
     ***********************************************************/
    PWM1_3_CTL_R = 0x00;               // Disable PWM generator 3 during setup

    /*
     * GENB Configuration (Down-count mode):
     * -------------------------------------
     *  - Set PWM output HIGH on comparator match
     *  - Set PWM output LOW on load event
     *
     * This defines PWM waveform behavior.
     */
    PWM1_3_GENB_R = 0x000000C4;

    PWM1_3_LOAD_R = 0xFFFF;            // Set PWM period (max resolution)

    PWM1_3_CMPA_R = 0x0000;            // Initial duty cycle = 0%

    PWM1_3_CTL_R = 0x01;               // Enable PWM generator (down-count)

    PWM1_ENABLE_R |= 0x80;             // Enable PWM output M1PWM7


    /***********************************************************
     * STEP 4: Change Duty Cycle in Infinite Loop
     ***********************************************************/
    while (1)
    {
        /*
         * Duty Cycle Control:
         * -------------------
         * CMPA = 0x7FFF → ~50% duty cycle
         * CMPA = 0x0FFF → lower duty cycle (dimmer LED)
         *
         * Duty Cycle Formula:
         * Duty (%) = (CMPA / LOAD) × 100
         */

        //PWM1_3_CMPA_R = 0x7FFF;   // ~50% duty cycle

        PWM1_3_CMPA_R = 0x0FFF;     // Lower duty cycle → Dimmer LED
    }
}
