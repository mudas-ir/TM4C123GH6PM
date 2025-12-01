// --------------------------------------------------------------
// AUTHOR : Mudassir
// DATE   : 28-11-2025
// DAY 	  : Friday
//
//? DESCRIPTION OF THE PROGRAM : Program to Interface LED and 7-Segment LED using 74HC595 Shift Register
// This program is written for the TM4C123GH6PM (Tiva C) microcontroller.
/*
 * Program Description:
 * --------------------
 * This program demonstrates how to interface a 7-segment display using a
 * serial-in parallel-out shift register (such as 74HC595) with the
 * TM4C123GH6PM microcontroller. The shift register reduces the GPIO pin
 * usage by allowing 8 segment-control lines to be driven using only
 * three microcontroller pins:
 *
 *   PF2 → SDATA (Serial Data Input)
 *   PF3 → SHCP  (Shift Clock)
 *   PC4 → STCP  (Latch Clock)
 *
 * The software loads segment data serially (LSB first) into the shift
 * register using the shift_out1() function. After shifting all 8 bits,
 * a latch pulse transfers the data to the shift register’s output pins,
 * which drive the segments of the 7-segment display.
 *
 * A lookup table is used to store segment patterns for digits 1, 2, 3, 4
 * and a full-segment test pattern (all segments ON). Inside the main loop,
 * these patterns are shifted out sequentially to display different digits.
 * A delay function is used to hold each digit on the display for visible
 * duration. The code also demonstrates clearing the display by shifting out
 * 0x00 for all digits.
 *
 * This program demonstrates:
 *   - Enabling GPIO clocks for Port C and Port F
 *   - Configuring MCU pins for digital output
 *   - Serial bit-banging to a shift register (LSB-first transmission)
 *   - Using latch control to update display outputs
 *   - Creating delays using nested loops
 *
 * The code can be extended for:
 *   - Multiplexed multi-digit displays
 *   - Dynamic animations
 *   - Timer-based refreshing instead of blocking delays
 */

// --------------------------------------------------------------

#include <stdint.h>       // Standard integer definitions (uint8_t, uint32_t, etc.)
#include "tm4c123gh6pm.h" // MCU register definitions

void delayMs(int n);
void shift_out1(unsigned char data_byte);

// Lookup table: Segment values for digits 1,2,3,4 + all segments ON
unsigned char a[5] = {0x60, 0xDA, 0xF2, 0x66, 0xFF};

unsigned int T = 500;

int main(void)
{
    // ------------------------------------------------------------
    // Enable clock for PORT C (0x04) and PORT F (0x20)
    // PC3 → STCP (Latch) for shift register
    // PF2 → SDATA (Data pin)
    // PF3 → SHCP (Shift Clock)
    // ------------------------------------------------------------
    SYSCTL_RCGCGPIO_R |= 0x24;

    // Configure output pins
    GPIO_PORTC_DIR_R |= 0x10; // PC4 (0x10) used as STCP latch pin
    GPIO_PORTF_DIR_R |= 0x0C; // PF3 (0x08) = SHCP, PF2 (0x04) = SDATA

    // Enable digital mode
    GPIO_PORTC_DEN_R |= 0x10;
    GPIO_PORTF_DEN_R |= 0x0C;

    while (1)
    {
        // Output digits 4,3,2,1 and 8 (all ON)
        shift_out1(a[3]);
        shift_out1(a[2]);
        shift_out1(a[1]);
        shift_out1(a[0]);
        shift_out1(a[4]);

        delayMs(T);

        // Clear (turn off all segments)
        shift_out1(0);
        shift_out1(0);
        shift_out1(0);
        shift_out1(0);
        shift_out1(0);

        delayMs(T);
    }
}

// ==================================================================
// FUNCTION: shift_out1()
// PURPOSE : Sends 1 byte serially (LSB first) to the shift register
// CONNECTIONS:
//   PF2 → SDATA (Serial Data Input)
//   PF3 → SHCP  (Shift Clock)
//   PC4 → STCP  (Latch to output)
// ==================================================================
void shift_out1(unsigned char data_byte)
{
    unsigned char j = 0;
    unsigned char bit_state = 0;

    // ------------------------------------------
    // Latch LOW – prepare for shifting 8 bits
    // ------------------------------------------
    GPIO_PORTC_DATA_R = 0x00; // STCP LOW

    // ------------------------------------------
    // Shift out 8 bits (LSB → MSB)
    // ------------------------------------------
    for (j = 0; j < 8; j++)
    {
        GPIO_PORTF_DATA_R = 0x00; // SHCP LOW, SDATA cleared first

        // Extract j-th bit of data_byte
        bit_state = (data_byte & (1 << j));

        // Set SDATA (PF2) according to bit value
        if (bit_state)
            GPIO_PORTF_DATA_R |= 0x04; // SDATA = 1 (PF2 HIGH)
        else
            GPIO_PORTF_DATA_R |= 0x00; // SDATA = 0

        // Generate shift clock pulse
        GPIO_PORTF_DATA_R |= 0x08; // SHCP HIGH → shift bit into shift register
    }

    // ------------------------------------------
    // Latch HIGH → Output all 8 bits to LEDs
    // ------------------------------------------
    GPIO_PORTC_DATA_R = 0x10; // STCP HIGH (PC4)
}

// ==================================================================
// delayMs()
// Blocks CPU for 'n' milliseconds (approx at 16 MHz clock)
// ==================================================================
void delayMs(int n)
{
    int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < 3200; j++)
        {
            // Intentional empty loop for ~1 ms delay
        }
}
