/***************************************************************
 * PROJECT NAME : Interfacing Analog HEX Keypad
 * MCU          : TM4C123GH6PM (Tiva C LaunchPad)
 * MODULES USED :
 *      - ADC0 Sample Sequencer 0 (SS0)
 *      - Analog Input on PD2 (AIN5)
 *      - Custom Analog Hex-Keypad Voltage Mapping
 *
 * DESCRIPTION :
 * ------------------------------------------------------------
 * This program reads an analog voltage coming from a HEX keypad
 * where each key produces a unique voltage level using a
 * resistor network.
 *
 * The ADC measures this voltage using:
 *      - ADC0
 *      - Sample Sequencer 0 (SS0)
 *      - Analog Channel AIN5 (PD2)
 *
 * After conversion, the ADC value is passed to the function
 * key_scan(), which compares the received ADC value with
 * predefined ranges (shifted right by 4 bits) and returns the
 * corresponding HEX key ('0'->'9', 'A'->'F').
 *
 * The returned key can be displayed on LCD, UART, or further used
 * based on application design.
 *
 * Author	: 
 * DATE     : 12/12/2025
 * DAY      : Thursday
 * *************************************************************/

#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"

// Function prototype
void delayMs(int n);
unsigned char key_scan(unsigned int volatile rec_val);

// Global variables
unsigned char Dig_val;          // Stores detected key value
volatile unsigned int ADCValue; // Stores raw ADC result

int main(void)
{
    /***********************************************************
     * STEP 1: Enable clocks for Port D and ADC0
     ***********************************************************/
    SYSCTL_RCGCGPIO_R |= 0x08; // Enable clock to GPIOD
    SYSCTL_RCGCADC_R |= 1;     // Enable clock to ADC0

    /***********************************************************
     * STEP 2: Configure PD2 for analog input (AIN5)
     ***********************************************************/
    GPIO_PORTD_AFSEL_R |= 4; // Enable alternate function on PD2
    GPIO_PORTD_DEN_R &= ~4;  // Disable digital functionality
    GPIO_PORTD_AMSEL_R |= 4; // Enable analog mode (essential for ADC)

    /***********************************************************
     * STEP 3: Configure ADC0 Sample Sequencer 0 (SS0)
     ***********************************************************/
    ADC0_ACTSS_R &= ~1;   // Disable SS0 during configuration
    ADC0_EMUX_R &= ~0x0F; // Select software trigger for SS0

    ADC0_SSMUX0_R = 0x05; // Select AIN5 for sample step 0
    // NOTE: SSMUX0_R is written, not OR'ed - ensures correct channel

    ADC0_SSCTL0_R |= 0x06;
    // Bit 1 (END0) ? Marks this sample as last sample
    // Bit 2 (IE0)  ? Enables raw interrupt flag after sample

    ADC0_ACTSS_R |= 1; // Enable SS0

    /***********************************************************
     * MAIN LOOP : Continuously read ADC & decode keypad
     ***********************************************************/
    while (1)
    {
        ADC0_PSSI_R |= 1; // Start ADC conversion on SS0

        while ((ADC0_RIS_R & 1) == 0)
            ; // Wait for conversion to complete

        ADCValue = ADC0_SSFIFO0_R; // Read ADC result (0 to 4095)

        ADC0_ISC_R = 1; // Clear conversion complete flag

        Dig_val = key_scan(ADCValue); // Convert ADC value to HEX key

        // You can print Dig_val on LCD, UART etc.
        // Example: LCD_display(Dig_val);
    }
}

/***************************************************************
 * delayMs()
 * Simple blocking delay routine (approx 1 ms based on 16 MHz)
 ***************************************************************/
void delayMs(int n)
{
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < 3000; j++)
            ; // Waste time (1 ms approx)
}

/***************************************************************
 * key_scan()
 * Converts ADC reading into corresponding HEX key
 *
 * The analog HEX keypad outputs fixed voltage levels.
 * ADC value is right-shifted by 4 bits before comparison
 * to reduce sensitivity and stabilize detection.
 *
 * Returns: Character '0'->'9', 'A'->'F'
 ***************************************************************/
unsigned char key_scan(unsigned int volatile rec_val)
{
    rec_val = rec_val >> 4; // Scale down ADC value

    if (rec_val == 0xB7 || rec_val == 0xB6)
        return '0';
    else if (rec_val == 0xB3 || rec_val == 0xB2)
        return '1';
    else if (rec_val == 0xAE || rec_val == 0xAD)
        return '2';
    else if (rec_val == 0xAA || rec_val == 0xA9)
        return '3';
    else if (rec_val == 0xA6 || rec_val == 0xA7)
        return '4';
    else if (rec_val == 0xA0 || rec_val == 0x9F)
        return '5';
    else if (rec_val == 0x98 || rec_val == 0x97)
        return '6';
    else if (rec_val == 0x92 || rec_val == 0x91)
        return '7';
    else if (rec_val == 0x8C || rec_val == 0x8B)
        return '8';
    else if (rec_val == 0x81 || rec_val == 0x80)
        return '9';
    else if (rec_val == 0x73 || rec_val == 0x72)
        return 'A';
    else if (rec_val == 0x66 || rec_val == 0x65)
        return 'B';
    else if (rec_val == 0x5A)
        return 'C';
    else if (rec_val == 0x41)
        return 'D';
    else if (rec_val == 0x20)
        return 'E';
    else if (rec_val == 0x00)
        return 'F';

    return 'G'; // Undefined / noise
}
