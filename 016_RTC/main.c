/*****************************************************************************************
 * FILE NAME  : i2c_rtc_ds1307_7segment_display.c
 *
 * AUTHOR     : Mudassir
 *
 * TARGET     : TM4C123GH6PM (Tiva C Series)
 *
 * DESCRIPTION:
 * This program interfaces a Real-Time Clock (RTC) device (DS1307 / DS3231 compatible)
 * with the TM4C123GH6PM microcontroller using I2C module 2.
 *
 * The RTC time (hours, minutes, seconds) is periodically read via I2C and displayed
 * on a multi-digit 7-segment display using a shift register (e.g., 74HC595).
 *
 * FEATURES:
 *  - I2C2 configured in Master mode (100 kHz)
 *  - RTC time initialization (HH:MM:SS)
 *  - Continuous RTC time read
 *  - BCD to decimal digit extraction
 *  - 7-segment display driving using shift register
 *
 * HARDWARE CONNECTIONS:
 *  I2C:
 *    PE4 → I2C2SCL
 *    PE5 → I2C2SDA (Open-drain)
 *
 *  Shift Register / 7-Segment:
 *    PF2 → Serial Data (DS)
 *    PF3 → Shift Clock (SHCP)
 *    PC4 → Latch Clock (STCP)
 *
 * RTC I2C ADDRESS:
 *    Slave Address = 0x68
 *
 * NOTE:
 *  - RTC stores time in BCD format
 *  - Software delays are used for simplicity
 *
 *****************************************************************************************/

#include <stdint.h>
#include "tm4c123gh6pm.h"

/* Utility macro for bit manipulation */
#define BIT(n) (1U << (n))

/* RTC (DS1307) I2C Definitions */
#define SLAVE_ADDR 0x68
#define SECONDS_ADDR 0x00
#define MINTUES_ADDR 0x01
#define HOUR_ADDR 0x02
#define WRITE 0x0
#define READ 0x1

/* Function Prototypes */
void I2C_init(void);
void sevenSegment_init(void);
int I2C_wait(void);
void delayMs(int n);
void byteWrite(unsigned char value, unsigned char address);
unsigned char byteRead(unsigned char address);
void shift_out(unsigned char data);

/* RTC Time Variables (BCD format) */
volatile unsigned char seconds = 0x56;
volatile unsigned char minutes = 0x39;
volatile unsigned char hour = 0x06;

/* 7-Segment Lookup Table (Common Cathode) */
static const unsigned char SevenSegmentTable[12] =
    {
        0xFC, 0x60, 0xDA, 0xF2, 0x66,
        0xB6, 0xBE, 0xE0, 0xFE, 0xF6,
        0x02, 0x00};

/* Display and Control Variables */
volatile unsigned char RTC_Data[8] = {0};
volatile unsigned char temp = 60;
volatile unsigned char d, check;

/*****************************************************************************************
 * MAIN FUNCTION
 *****************************************************************************************/
int main(void)
{
    /* Initialize peripherals */
    sevenSegment_init();
    I2C_init();

    /* Initialize RTC time */
    byteWrite(seconds, SECONDS_ADDR);
    delayMs(700);
    byteWrite(minutes, MINTUES_ADDR);
    delayMs(700);
    byteWrite(hour, HOUR_ADDR);
    delayMs(700);

    while (1)
    {
        /* Read RTC Time */
        seconds = byteRead(SECONDS_ADDR);
        delayMs(700);
        minutes = byteRead(MINTUES_ADDR);
        delayMs(700);
        hour = byteRead(HOUR_ADDR);
        delayMs(700);

        /* Update display only if seconds change */
        if (seconds != temp)
        {
            temp = seconds;

            /* Extract BCD digits */
            RTC_Data[0] = minutes & 0x0F;
            RTC_Data[1] = (minutes >> 4) & 0x0F;
            RTC_Data[2] = hour & 0x0F;
            RTC_Data[3] = (hour >> 4) & 0x0F;

            /* Send digits to 7-segment display */
            for (d = 0; d < 4; d++)
            {
                shift_out(SevenSegmentTable[RTC_Data[d]]);
            }

            /* Display seconds (optional) */
            shift_out(seconds);
        }
    }
}

/*****************************************************************************************
 * I2C INITIALIZATION (I2C2)
 *****************************************************************************************/
void I2C_init(void)
{
    SYSCTL_RCGCI2C_R |= BIT(2);  /* Enable I2C2 clock */
    SYSCTL_RCGCGPIO_R |= BIT(4); /* Enable GPIO Port E */

    while ((SYSCTL_PRGPIO_R & BIT(4)) == 0)
        ;

    GPIO_PORTE_AFSEL_R |= BIT(4) | BIT(5);
    GPIO_PORTE_DEN_R |= BIT(4) | BIT(5);
    GPIO_PORTE_ODR_R |= BIT(5); /* SDA open-drain */

    GPIO_PORTE_PCTL_R &= ~(0x00FF0000);
    GPIO_PORTE_PCTL_R |= (0x00330000);

    I2C2_MCR_R |= BIT(4); /* Enable I2C Master mode */
    I2C2_MTPR_R = 0x07;   /* 100 kHz @ 16 MHz */
}

/*****************************************************************************************
 * WRITE SINGLE BYTE TO RTC REGISTER
 *****************************************************************************************/
void byteWrite(unsigned char value, unsigned char address)
{
    I2C2_MSA_R = (SLAVE_ADDR << 1) | WRITE;
    I2C2_MDR_R = address;
    I2C2_MCS_R = 0x03; /* START + RUN */
    I2C_wait();

    I2C2_MDR_R = value;
    I2C2_MCS_R = 0x05; /* RUN + STOP */
    I2C_wait();
}

/*****************************************************************************************
 * READ SINGLE BYTE FROM RTC REGISTER
 *****************************************************************************************/
unsigned char byteRead(unsigned char address)
{
    I2C2_MSA_R = (SLAVE_ADDR << 1) | WRITE;
    I2C2_MDR_R = address;
    I2C2_MCS_R = 0x03;
    I2C_wait();

    I2C2_MSA_R = (SLAVE_ADDR << 1) | READ;
    I2C2_MCS_R = 0x07;
    I2C_wait();

    return I2C2_MDR_R;
}

/*****************************************************************************************
 * WAIT FOR I2C TRANSACTION COMPLETION
 *****************************************************************************************/
int I2C_wait(void)
{
    while (I2C2_MCS_R & 0x01)
        ;
    return (I2C2_MCS_R & 0x0E) ? -1 : 0;
}

/*****************************************************************************************
 * INITIALIZE 7-SEGMENT CONTROL PINS
 *****************************************************************************************/
void sevenSegment_init(void)
{
    SYSCTL_RCGCGPIO_R |= BIT(2) | BIT(5);

    while ((SYSCTL_PRGPIO_R & (BIT(2) | BIT(5))) == 0)
        ;

    GPIO_PORTC_DIR_R |= BIT(4);
    GPIO_PORTF_DIR_R |= BIT(2) | BIT(3);

    GPIO_PORTC_DEN_R |= BIT(4);
    GPIO_PORTF_DEN_R |= BIT(2) | BIT(3);
}

/*****************************************************************************************
 * SHIFT DATA TO 7-SEGMENT USING SHIFT REGISTER
 *****************************************************************************************/
void shift_out(unsigned char data)
{
    for (unsigned char j = 0; j < 8; j++)
    {
        GPIO_PORTC_DATA_R &= ~BIT(4); /* Latch LOW */
        GPIO_PORTF_DATA_R &= ~BIT(3); /* Clock LOW */

        if (data & (1 << j))
            GPIO_PORTF_DATA_R |= BIT(2);
        else
            GPIO_PORTF_DATA_R &= ~BIT(2);

        GPIO_PORTF_DATA_R |= BIT(3); /* Clock HIGH */
    }
    GPIO_PORTC_DATA_R |= BIT(4); /* Latch HIGH */
}

/*****************************************************************************************
 * SOFTWARE DELAY (Approx. 1 ms)
 *****************************************************************************************/
void delayMs(int n)
{
    volatile int i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < 400; j++)
            ;
}
