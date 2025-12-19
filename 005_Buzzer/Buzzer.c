/* ------------------------------------------------------------
 * PROGRAM NAME : Buzzer Control Using Push Button (SW1)
 *
 * AUTHOR :
 * DATE   : 19/12/2025
 * DAY    : Friday
 *
 * DESCRIPTION :
 * This program demonstrates basic GPIO input and output control
 * on the TM4C123GH6PM (Tiva C LaunchPad).
 *
 * - The onboard push button SW1 connected to PF4 is used as input.
 * - A buzzer is connected to PB3 and acts as the output device.
 * - When SW1 is pressed, the buzzer turns ON.
 * - When SW1 is released, the buzzer turns OFF.
 *
 * HARDWARE CONNECTIONS :
 * SW1  -> PF4 (Onboard, active LOW, internal pull-up enabled)
 * Buzzer -> PB3 (External buzzer connected via GPIO)
 *
 * NOTE :
 * SW1 is active LOW because PF4 has an internal pull-up resistor.
 *
 * CONTROLLER : TM4C123GH6PM
 * ------------------------------------------------------------
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"

int main(void)
{
    unsigned int value;

    /* ------------------------------------------------------------
     * ENABLE CLOCKS FOR GPIO PORTS
     * ------------------------------------------------------------ */
    SYSCTL_RCGCGPIO_R |= 0x22;
    /*
     * Bit 1 -> GPIO Port B clock enable
     * Bit 5 -> GPIO Port F clock enable
     */

    /* ------------------------------------------------------------
     * CONFIGURE PF4 AS INPUT (SW1)
     * ------------------------------------------------------------ */
    GPIO_PORTF_DIR_R &= ~0x10; /* Clear bit 4 -> PF4 as input */
    GPIO_PORTF_DEN_R |= 0x10;  /* Enable digital function for PF4 */
    GPIO_PORTF_PUR_R |= 0x10;  /* Enable internal pull-up resistor */

    /*
     * Why pull-up?
     * - SW1 is active LOW
     * - Released  -> PF4 reads '1'
     * - Pressed   -> PF4 reads '0'
     */

    /* ------------------------------------------------------------
     * CONFIGURE PB3 AS OUTPUT (BUZZER)
     * ------------------------------------------------------------ */
    GPIO_PORTB_DIR_R |= 0x08; /* Set PB3 as output */
    GPIO_PORTB_DEN_R |= 0x08; /* Enable digital function for PB3 */

    /* ------------------------------------------------------------
     * MAIN LOOP
     * ------------------------------------------------------------ */
    while (1)
    {
        value = GPIO_PORTF_DATA_R; /* Read PORTF input data */

        /*
         * If PF4 == 1 (switch not pressed)
         * turn OFF buzzer
         */
        if (value & 0x10)
            GPIO_PORTB_DATA_R &= ~0x08; /* Buzzer OFF */

        /*
         * If PF4 == 0 (switch pressed)
         * turn ON buzzer
         */
        else
            GPIO_PORTB_DATA_R |= 0x08; /* Buzzer ON */
    }
}
