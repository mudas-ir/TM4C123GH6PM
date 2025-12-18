/***************************************************************
 *  AUTHOR      :  Mudassir
 *  DATE        :  11-12-2025
 *  DAY         :  Thursday
 *  MCU         :  TM4C123GH6PM  (Tiva C)
 *  MODULE      :  ADC0 – Sample Sequencer 0 (SS0)
 *  INPUT PIN   :  PD3 (AIN4) – Connected to Potentiometer
 *
 *  DESCRIPTION :
 *  ------------------------------------------------------------
 *  This program performs Analog-to-Digital Conversion using
 *  ADC0 on the TM4C123GH6PM microcontroller.
 *
 *  • PD3 is configured as analog input channel AIN4.
 *  • ADC0 Sample Sequencer 0 (SS0) is used.
 *  • Software trigger is used to start conversions.
 *  • The program continuously reads ADC values from PD3 and
 *    stores the 12-bit converted result in a variable.
 *
 *  The result variable will contain values from:
 *        0     -> 0V
 *        4095  -> 3.3V
 *
 *  ------------------------------------------------------------
 *  NOTE:
 *    SS0 can take up to 8 samples, but here we only configure
 *    it to take *one* sample per trigger.
 *
 ****************************************************************/

#include "tm4c123gh6pm.h"
#include <stdint.h>

volatile uint32_t result; // stores ADC conversion output

int main(void)
{
    
    /***********************************************************
     * STEP 1: Enable Clocks
     ***********************************************************/
    SYSCTL_RCGCGPIO_R |= 0x08; // Enable clock for GPIOD (bit 3 -> port D)
    SYSCTL_RCGCADC_R |= 0x01;  // Enable clock for ADC0 module

    /***********************************************************
     * STEP 2: Configure PD3 as Analog Input (AIN4)
     ***********************************************************/
    GPIO_PORTD_AFSEL_R |= 0x08; // Enable Alternate Function on PD3
    GPIO_PORTD_DEN_R &= ~0x08;  // Disable Digital for PD3 (analog pin cannot be digital)
    GPIO_PORTD_AMSEL_R |= 0x08; // Enable Analog Mode for PD3

    /***********************************************************
     * STEP 3: Configure ADC0 Sample Sequencer 0 (SS0)
     ***********************************************************/
    ADC0_ACTSS_R &= ~0x01; // Disable SS0 before configuration

    ADC0_EMUX_R &= ~0x000F; // EMUX = 0 -> Software trigger

    ADC0_SSMUX0_R = 0x04; // SS0 MUX input channel = 4 (AIN4 -> PD3)

    /*
        SSCTL bits:
        - IE0 (bit 2)  : Interrupt enable for sample 0
        - END0 (bit 1) : Marks this as last sample
    */
    ADC0_SSCTL0_R = 0x06; // 0000 0110 -> END0 = 1, IE0 = 1

    ADC0_ACTSS_R |= 0x01; // Enable SS0

    /***********************************************************
     * STEP 4: Start Conversion in Infinite Loop
     ***********************************************************/
    while (1)
    {
        ADC0_PSSI_R |= 0x01; // Start conversion on SS0 (Software trigger)

        while ((ADC0_RIS_R & 0x01) == 0)
            ; // Wait until conversion completes (polling)

        result = ADC0_SSFIFO0_R; // Read 12-bit ADC result from FIFO

        ADC0_ISC_R = 0x01; // Clear the interrupt flag for SS0
    }
}
