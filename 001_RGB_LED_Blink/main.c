// --------------------------------------------------------------
// AUTHOR : Mudassir
// DATE   : 28-11-2025
// DAY 	  : Friday
//
// DESCRIPTION OF THE PROGRAM : PROGRAM TO BLINK RGB LED USING GPIO
// This program is written for the TM4C123GH6PM (Tiva C) microcontroller.
// It simply blinks the RGB LEDs on Port F pins
// PF1 (Red),
// PF2 (Blue),
// PF3 (Green)
// one after another with a delay of 500 ms.
// --------------------------------------------------------------

#include <stdint.h>       // Allows the use of fixed-width integers (uint32_t, etc.)
#include "tm4c123gh6pm.h" // Contains register addresses of the microcontroller.

// Function prototype
void delayMs(int n);

int main(void)
{
    // --------------------------------------------------------------
    // STEP 1: Enable clock for PORT F
    // Bit 5 of RCGCGPIO (0x20) corresponds to Port F.
    // Without enabling clock, Port F registers will not work.
    // --------------------------------------------------------------
    SYSCTL_RCGCGPIO_R |= 0x20;

    // Peripheral Ready Register (PRGPIO)
    // It checks Whether the clock has not only been enabled,
    // But also the hardware has completed the clock stabilization for that port. (optional safety)
    while ((SYSCTL_PRGPIO_R & 0x20) == 0)
        ;

    // --------------------------------------------------------------
    // STEP 2: Configure PF1, PF2, PF3 as OUTPUTS
    // PF1 → Red LED
    // PF2 → Blue LED
    // PF3 → Green LED
    // --------------------------------------------------------------
    GPIO_PORTF_DIR_R |= 0x0E; // Binary: 0000 1110 → PF1, PF2, PF3 = output

    // --------------------------------------------------------------
    // STEP 3: Enable DIGITAL functionality on PF1–PF3
    // PF1 = 0x02, PF2 = 0x04, PF3 = 0x08 → combined = 0x0E
    // --------------------------------------------------------------
    GPIO_PORTF_DEN_R |= 0x0E;

    // --------------------------------------------------------------
    // STEP 4: Infinite loop – Blink the RGB LEDs one by one
    // --------------------------------------------------------------
    while (1)
    {
        GPIO_PORTF_DATA_R = 0x02; // Turn ON RED LED (PF1)
        delayMs(500);             // Delay 500 ms

        GPIO_PORTF_DATA_R = 0x04; // Turn ON BLUE LED (PF2)
        delayMs(500);             // Delay 500 ms

        GPIO_PORTF_DATA_R = 0x08; // Turn ON GREEN LED (PF3)
        delayMs(500);             // Delay 500 ms
    }
}

// --------------------------------------------------------------
// delayMs()
// Creates a delay of 'n' milliseconds.
// This delay works correctly when system clock = 16 MHz.
// Inner loop executes 3200 cycles → approx 1 millisecond.
// --------------------------------------------------------------
void delayMs(int n)
{
    int i, j;
    for (i = 0; i < n; i++)        // Loop for 'n' milliseconds
        for (j = 0; j < 3200; j++) // Inner loop = ~1 ms delay
        {
        } // Do nothing (waste time)
}
