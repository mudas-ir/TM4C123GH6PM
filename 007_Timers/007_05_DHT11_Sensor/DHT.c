#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

void DHT11_Start(void);
unsigned char DHT11_Check_Response(void);
uint8_t DHT11_Read(void);
void Delay_MicroSecond(int time);
void Delay_MilliSecond(int time);

unsigned char Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
unsigned short int SUM, RH, TEMP;

float Temperature = 0;
float Humidity = 0;
uint8_t Presence = 0;

int main(void)
{
    // Enable GPIO port C
    SYSCTL_RCGCGPIO_R |= 0x04;
    while ((SYSCTL_PRGPIO_R & 0x04) == 0)
        ; // Wait for Port C to be ready

    while (1)
    {
        DHT11_Start();
        Presence = DHT11_Check_Response();
        Rh_byte1 = DHT11_Read();
        Rh_byte2 = DHT11_Read();
        Temp_byte1 = DHT11_Read();
        Temp_byte2 = DHT11_Read();
        SUM = DHT11_Read();

        TEMP = Temp_byte1;
        RH = Rh_byte1;

        Temperature = (float)TEMP;
        Humidity = (float)RH;
    }
}

/*---------------------------------------*/

void DHT11_Start(void)
{
    // PC5 as Digital Output
    GPIO_PORTC_DIR_R |= (1 << 5);
    GPIO_PORTC_DEN_R |= (1 << 5);
    GPIO_PORTC_DATA_R |= (1 << 5);

    Delay_MilliSecond(1000); // 1 second delay

    GPIO_PORTC_DATA_R &= ~(1 << 5); // pull low for 18ms
    Delay_MilliSecond(18);

    GPIO_PORTC_DATA_R |= (1 << 5); // pull high
    Delay_MicroSecond(20);

    // PC5 as Digital Input
    GPIO_PORTC_DIR_R &= ~(1 << 5);
    Delay_MicroSecond(10);
}

/*---------------------------------------*/

unsigned char DHT11_Check_Response(void)
{
    unsigned char Response = 0;
    Delay_MicroSecond(40);

    if (!(GPIO_PORTC_DATA_R & (1 << 5)))
    {
        Delay_MicroSecond(80);
        if (GPIO_PORTC_DATA_R & (1 << 5))
            Response = 1;
        else
            Response = 0;
    }
    while (GPIO_PORTC_DATA_R & (1 << 5))
        ; // wait until pin goes low again

    return Response;
}

/*---------------------------------------*/

uint8_t DHT11_Read(void)
{
    uint8_t i = 0, j;
    for (j = 0; j < 8; j++)
    {
        while (!(GPIO_PORTC_DATA_R & (1 << 5)))
            ; // wait for pin to go high
        Delay_MicroSecond(30);

        if (!(GPIO_PORTC_DATA_R & (1 << 5)))
            i &= ~(1 << (7 - j)); // write 0
        else
            i |= (1 << (7 - j)); // write 1

        while (GPIO_PORTC_DATA_R & (1 << 5))
            ; // wait for pin to go low
    }
    return i;
}

/*---------------------------------------*/

void Delay_MicroSecond(int time)
{
    int i;
    SYSCTL_RCGCTIMER_R |= 2; // enable clock to Timer Block 1
    TIMER1_CTL_R = 0;
    TIMER1_CFG_R = 0x04;
    TIMER1_TAMR_R = 0x02;
    TIMER1_TAILR_R = 16 - 1; // 1us for 16MHz clock
    TIMER1_ICR_R = 0x1;
    TIMER1_CTL_R |= 0x01;

    for (i = 0; i < time; i++)
    {
        while ((TIMER1_RIS_R & 0x1) == 0)
            ;
        TIMER1_ICR_R = 0x1;
    }
}

/*---------------------------------------*/

void Delay_MilliSecond(int time)
{
    int i;
    SYSCTL_RCGCTIMER_R |= 4;
    TIMER2_CTL_R = 0;
    TIMER2_CFG_R = 0x04;
    TIMER2_TAMR_R = 0x02;
    TIMER2_TAILR_R = 16000 - 1; // 1ms for 16MHz clock
    TIMER2_ICR_R = 0x1;
    TIMER2_CTL_R |= 0x01;

    for (i = 0; i < time; i++)
    {
        while ((TIMER2_RIS_R & 0x1) == 0)
            ;
        TIMER2_ICR_R = 0x1;
    }
}
