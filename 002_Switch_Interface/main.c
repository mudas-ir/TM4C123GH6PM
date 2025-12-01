// --------------------------------------------------------------
// AUTHOR : Mudassir
// DATE   : 28-11-2025
// DAY 	  : Friday
//
// DESCRIPTION OF THE PROGRAM : PROGRAM TO INTERFACE A SWITCH(SW1) to GLOW RED LED
// This program is written for the TM4C123GH6PM (Tiva C) microcontroller.
// It does the following:
// Enables clock for Port F
// So the microcontroller can access GPIOF registers.
// Configures PF1 as output
// PF1 is connected to the RED LED on the LaunchPad.
// Configures PF4 as input with internal pull-up
// PF4 is connected to SW1 push button on the LaunchPad.
// Inside the infinite loop:
// Reads PF4 (the switch)
// If the switch is NOT pressed, the LED is turned OFF
// If the switch IS pressed, the LED is turned ON
// IMPORTANT:
// SW1 is active LOW ?
// When pressed ? PF4 = 0
// When released ? PF4 = 1
// --------------------------------------------------------------

#include <stdint.h>       // Allows the use of fixed-width integers (uint32_t, etc.)
#include "tm4c123gh6pm.h" // Contains register addresses of the microcontroller.

int main(void)
{
    unsigned int value;

    // ---------------------------------------------------------
    // 1. Enable clock for GPIO Port F
    // Bit 5 (0x20) in RCGCGPIO enables Port F.
    // ---------------------------------------------------------
    SYSCTL_RCGCGPIO_R |= 0x20;

    // Wait for clock to be ready (recommended professional step)
    while ((SYSCTL_PRGPIO_R & 0x20) == 0)
    {
    }

    // ---------------------------------------------------------
    // 2. Set PF1 as output (PF1 = Red LED)
    // ---------------------------------------------------------
    GPIO_PORTF_DIR_R |= 0x02; // PF1 = output

    // ---------------------------------------------------------
    // 3. Enable digital function for PF4 (switch) and PF1 (LED)
    // PF4 = 0x10, PF1 = 0x02 ? combined = 0x12
    // ---------------------------------------------------------
    GPIO_PORTF_DEN_R |= 0x12;

    // ---------------------------------------------------------
    // 4. Enable internal pull-up resistor on PF4 (SW1 button)
    // SW1 is active LOW ? gives 1 when not pressed, 0 when pressed
    // ---------------------------------------------------------
    GPIO_PORTF_PUR_R |= 0x10;

    while (1)
    {
        // -----------------------------------------------------
        // 5. Read PF4 switch input
        // Reading the whole PORTF DATA register is fine;
        // we will check if PF4 is HIGH or LOW.
        // -----------------------------------------------------
        value = GPIO_PORTF_DATA_R & 0x10; // Mask only PF4

        // -----------------------------------------------------
        // 6. If switch NOT pressed (PF4 = 1), turn OFF LED
        // -----------------------------------------------------
        if (value != 0)
        {
            GPIO_PORTF_DATA_R &= ~0x02; // Turn off RED LED (PF1 = 0)
        }
        else
        {
            // -------------------------------------------------
            // 7. If switch IS pressed (PF4 = 0), turn ON LED
            // -------------------------------------------------
            GPIO_PORTF_DATA_R |= 0x02; // Turn on RED LED (PF1 = 1)
        }
    }
}
