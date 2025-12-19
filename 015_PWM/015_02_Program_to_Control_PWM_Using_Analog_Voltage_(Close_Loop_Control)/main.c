/***************************************************************
 * PROJECT NAME : Closed Loop PWM Control using Potentiometer
 * MCU          : TM4C123GH6PM (Tiva C LaunchPad)
 *
 * INPUT DEVICE :
 *   - Potentiometer connected to PD3 (AIN4, ADC0 Channel 4)
 *
 * OUTPUT DEVICE :
 *   - Green LED connected to PF3
 *   - PWM Output : M1PWM7 (PWM Module 1, Generator 3, Output B)
 *
 * DESCRIPTION :
 * ------------------------------------------------------------
 * This program demonstrates a CLOSED-LOOP control system
 * where the duty cycle of a PWM signal is controlled by
 * an analog input (potentiometer).
 *
 * Working Principle:
 *   1. Potentiometer produces an analog voltage (0–3.3 V)
 *   2. ADC0 reads this voltage via Channel 4 (AIN4)
 *   3. ADC digital value (0–4095) is scaled
 *   4. Scaled value is written to PWM compare register
 *   5. PWM duty cycle changes accordingly
 *   6. Brightness of Green LED on PF3 varies smoothly
 *
 * This is a real-world example of:
 *   ADC ? Control Algorithm ? PWM Actuator
 *
 * AUTHOR : 
 * DATE   : 15/12/2025
 * DAY    : Monday
 ***************************************************************/

#include "tm4c123gh6pm.h"
#include <stdint.h>

void delayMs(int n);

int main(void)
{
    /***********************************************************
     * STEP 1: Enable clocks for GPIO, ADC0, and PWM1
     ***********************************************************/
    SYSCTL_RCGCGPIO_R |= 0x28;   // Enable clock to Port D (PD3) and Port F (PF3)
    SYSCTL_RCGCADC_R  |= 0x01;   // Enable clock to ADC0
    SYSCTL_RCGCPWM_R  |= 0x02;   // Enable clock to PWM Module 1

    /*
     * Enable PWM clock divider
     * PWM clock = System clock / divider
     */
    SYSCTL_RCC_R |= 0x00100000;  // Enable PWM clock pre-divider
    // SYSCTL_RCC_R |= 0x000E0000; // Optional: divide PWM clock by 64


    /***********************************************************
     * STEP 2: Configure PD3 as ADC input (AIN4)
     ***********************************************************/
    GPIO_PORTD_AFSEL_R |= 0x08;   // Enable alternate function on PD3
    GPIO_PORTD_DEN_R   &= ~0x08;  // Disable digital function
    GPIO_PORTD_AMSEL_R |= 0x08;   // Enable analog mode


    /***********************************************************
     * STEP 3: Configure PF3 as PWM output (M1PWM7)
     ***********************************************************/
    GPIO_PORTF_AFSEL_R |= 0x08;         // Enable alternate function on PF3

    GPIO_PORTF_PCTL_R &= ~0x0000F000;   // Clear PCTL bits for PF3
    GPIO_PORTF_PCTL_R |=  0x00005000;   // Configure PF3 as M1PWM7

    GPIO_PORTF_DEN_R  |= 0x08;          // Enable digital output on PF3


    /***********************************************************
     * STEP 4: Configure ADC0 Sample Sequencer 0 (SS0)
     ***********************************************************/
    ADC0_ACTSS_R &= ~0x01;        // Disable SS0 during configuration
    ADC0_EMUX_R  &= ~0x000F;      // Software trigger for SS0

    ADC0_SSMUX0_R = 0x04;         // Sample input = AIN4 (PD3)

    ADC0_SSCTL0_R = 0x06;         
    // END0 = 1 ? Single sample
    // IE0  = 1 ? Set flag after conversion

    ADC0_ACTSS_R |= 0x01;         // Enable SS0


    /***********************************************************
     * STEP 5: Configure PWM Module 1 Generator 3 (Output B)
     ***********************************************************/
    PWM1_3_CTL_R = 0x00;          // Disable PWM Generator 3

    /*
     * GENB settings:
     * - PWM output HIGH at load
     * - PWM output LOW at comparator match (down-count)
     */
    PWM1_3_GENB_R = 0x000000C4;

    PWM1_3_LOAD_R = 0x0EFF;       // Set PWM period (frequency)

    PWM1_3_CMPA_R = 0x00;         // Initial duty cycle = 0%

    PWM1_3_CTL_R = 0x01;          // Enable PWM generator

    PWM1_ENABLE_R |= 0x80;        // Enable M1PWM7 output


    /***********************************************************
     * STEP 6: Closed Loop Control (ADC ? PWM)
     ***********************************************************/
    while (1)
    {
        ADC0_PSSI_R |= 0x01;               // Start ADC conversion

        while ((ADC0_RIS_R & 0x01) == 0)
            ;                              // Wait for conversion complete

        /*
         * ADC value range  : 0 – 4095
         * PWM LOAD value   : 0x0EFF
         * Scaling factor   : 0.75 (application-specific)
         *
         * Result:
         * Potentiometer controls PWM duty cycle
         */
        PWM1_3_CMPA_R = 0.75 * ADC0_SSFIFO0_R;

        ADC0_ISC_R = 0x01;                 // Clear ADC interrupt flag
    }
}
