/*
 * --------------------------------------------------------------
 * AUTHOR : Mudassir
 * DATE   : 28-11-2025
 * DAY    : Friday
 *
 * PROGRAM DESCRIPTION:
 * --------------------------------------------------------------
 * Program to Interface a 16x2 Character LCD using a 74HC595
 * Serial-In Parallel-Out Shift Register in 4-bit mode.
 *
 * The TM4C123GH6PM (Tiva C) microcontroller normally requires
 * many GPIO pins to control a 16x2 LCD (RS, RW, EN, D4–D7 = 7 pins).
 *
 * Using a 74HC595 reduces the total required pins to ONLY THREE:
 *
 *    PF2 → SDATA  (Serial Data Input)
 *    PF3 → SCLK   (Shift Clock)
 *    PE5 → STK    (Latch Clock / Storage Register Clock)
 *
 * The LCD is driven in 4-bit mode by sending high-nibble and
 * low-nibble separately through the shift register.
 *
 * The flow of the program:
 *   1. Initialize GPIO ports for the shift register.
 *   2. Initialize the LCD using standard 4-bit startup sequence.
 *   3. Convert each LCD command/data byte into its 4-bit form.
 *   4. Serially shift the bits into the 74HC595.
 *   5. Latch the data to update the LCD pins.
 *   6. Display text on 1st and 2nd line of LCD.
 *
 * This program demonstrates:
 *   - Bit-level manipulation
 *   - Nibble extraction and remapping for LCD
 *   - Serial data shifting into a shift register
 *   - LCD initialization sequence and display functions
 *
 * The code can be expanded to:
 *   - Create custom characters
 *   - Implement scrolling display
 *   - Interface multiple LCDs using daisy-chained 595s
 *
 * --------------------------------------------------------------
 */

/* Interfacing 16x2 LCD using 74HC595 Shift Register - 4-bit Mode */

#include <stdint.h>
#include "tm4c123gh6pm.h"

// LCD helper macros
#define LCD_clear() LCD_command(0x01)  /* Clear full display */
#define LCD_origin() LCD_command(0x02) /* Return cursor to home position */
#define LCD_row1() LCD_command(0x80)   /* Move cursor to Row 1 */
#define LCD_row2() LCD_command(0xC0)   /* Move cursor to Row 2 */

// Function declarations
void shift_out1(unsigned char byte);
void LCD_command(unsigned char command);
void LCD_putc(unsigned char ascii);
void LCD_puts(unsigned char *lcd_string);
void LCD_init(void);
void delayMs(int n);
void delayUs(int n);

// Global byte sent to 74HC595 (contains control + data bits)
unsigned char PP2 = 0x00;

int main(void)
{
    // Enable PORT E (0x10) and PORT F (0x20)
    SYSCTL_RCGCGPIO_R |= 0x30;

    // PE5 → STK (Latch pin)
    GPIO_PORTE_DIR_R |= 0x20;
    GPIO_PORTE_DEN_R |= 0x20;

    // PF2 → SDATA, PF3 → SCLK
    GPIO_PORTF_DIR_R |= 0x0C;
    GPIO_PORTF_DEN_R |= 0x0C;

    LCD_init(); // Initialize LCD in 4-bit mode

    LCD_row1();          // Set cursor to row 1
    LCD_puts("Welcome"); // Display text

    LCD_row2(); // Set cursor to row 2
    LCD_puts("LCD 16x2");

    delayMs(500);

    while (1)
        ;
}

/* -------------------------------------------------------------
 * LCD_init()
 * Performs the mandatory power-on initialization sequence for
 * 4-bit mode as per HD44780 LCD controller datasheet.
 * -------------------------------------------------------------*/
void LCD_init()
{
    delayMs(20); // LCD power-on delay

    // Initialization sequence: send 0x30 three times
    LCD_command(0x30);
    delayMs(5);

    LCD_command(0x30);
    delayMs(5);

    LCD_command(0x30);
    delayMs(5);

    // Switch to 4-bit mode
    LCD_command(0x20);
    delayMs(5);

    // Function Set: 4-bit, 2-line, 5x7 font
    LCD_command(0x28);
    delayMs(5);

    // Display ON, Cursor OFF
    LCD_command(0x0C);
    delayMs(5);

    // Entry Mode: Auto-increment cursor
    LCD_command(0x06);
    delayMs(5);

    // Clear display
    LCD_command(0x01);
    delayMs(5);
}

/* -------------------------------------------------------------
 * LCD_puts()
 * Sends a null-terminated string to the LCD.
 * -------------------------------------------------------------*/
void LCD_puts(unsigned char *lcd_string)
{
    while (*lcd_string)
    {
        LCD_putc(*lcd_string++);
    }
}

/* -------------------------------------------------------------
 * LCD_command()
 * Sends a command byte to the LCD in two 4-bit transfers.
 * The command byte's bits are rearranged to match 74HC595→LCD wiring.
 * RS = 0, RW = 0 for command mode.
 * -------------------------------------------------------------*/
void LCD_command(unsigned char command)
{
    unsigned char num;

    // Bit remapping due to wiring between 595 and LCD
    /*
    *✅ Meaning of “wiring between 595”

    *📌 It means:

    *The physical connection between the 74HC595 outputs (Q0–Q7) and the LCD pins (D4–D7, RS, RW, EN).

    *Because this wiring is not sequential (Q0 → D4, Q1 → D5, etc.),
    *the software must reorder the bits before shifting them out.
    */
    num = command;
    num = ((num & 0x11) << 3) |
          ((num & 0x22) << 1) |
          ((num & 0x44) >> 1) |
          ((num & 0x88) >> 3);
    command = num;

    // --- Send high nibble ---
    PP2 = (PP2 & 0xF0) | ((command >> 4) & 0x0F); // Load high nibble
    PP2 &= ~(3 << 7);                             // RS=0, RW=0
    PP2 |= 0x20;                                  // EN=1
    shift_out1(PP2);

    PP2 &= ~0x20; // EN=0 (Latch command)
    shift_out1(PP2);

    // --- Send low nibble ---
    PP2 = (PP2 & 0xF0) | (command & 0x0F); // Load low nibble
    PP2 &= ~(3 << 7);                      // RS=0, RW=0
    PP2 |= 0x20;                           // EN=1
    shift_out1(PP2);

    PP2 &= ~0x20; // EN=0
    shift_out1(PP2);
}

/* -------------------------------------------------------------
 * LCD_putc()
 * Sends a data (ASCII) byte to LCD.
 * RS=1 indicates data; RW=0 for write.
 * -------------------------------------------------------------*/
void LCD_putc(unsigned char ascii)
{
    unsigned char num;

    // Bit remapping due to wiring between 595 and LCD
    num = ascii;
    num = ((num & 0x11) << 3) |
          ((num & 0x22) << 1) |
          ((num & 0x44) >> 1) |
          ((num & 0x88) >> 3);
    ascii = num;

    // --- Send high nibble ---
    PP2 = (PP2 & 0xF0) | ((ascii >> 4) & 0x0F);
    PP2 |= 0xA0;  // RS=1, EN=1
    PP2 &= ~0x40; // RW=0
    shift_out1(PP2);

    PP2 &= ~0x20; // EN=0
    shift_out1(PP2);

    // --- Send low nibble ---
    PP2 = (PP2 & 0xF0) | (ascii & 0x0F);
    PP2 |= 0xA0;  // RS=1, EN=1
    PP2 &= ~0x40; // RW=0
    shift_out1(PP2);

    PP2 &= ~0x20; // EN=0
    shift_out1(PP2);
}

/* -------------------------------------------------------------
 * shift_out1()
 * Sends 8 bits (LSB first) to 74HC595 using:
 *   PF2 = SDATA
 *   PF3 = SCLK
 *   PE5 = STK (Latch)
 * -------------------------------------------------------------*/
void shift_out1(unsigned char byte)
{
    unsigned char j, bit;

    // Latch low → prepare for shifting
    GPIO_PORTE_DATA_R = 0x00;

    for (j = 0; j < 8; j++)
    {
        GPIO_PORTF_DATA_R = 0x00; // SCLK = 0

        bit = (byte & (1 << j));

        // Set SDATA according to current bit
        if (bit)
            GPIO_PORTF_DATA_R |= 0x04; // SDATA = 1
        else
            GPIO_PORTF_DATA_R |= 0x00; // SDATA = 0

        // Clock the bit into shift register
        GPIO_PORTF_DATA_R |= 0x08; // SCLK = 1
    }

    // Latch HIGH → output becomes active
    GPIO_PORTE_DATA_R = 0x20; // STK = 1
}

/* Millisecond delay */
void delayMs(int n)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < 2000; j++)
            ;
}
