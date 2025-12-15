/* PE0 is configured as AIN3 ADC0 CH3 that is connected to LDR|LM35D*/
#include "tm4c123gh6pm.h"
#include <stdint.h>

volatile int result;

volatile float V0, temp;

volatile int lightIntensity;


int main(void)
{
    
    /* enable clocks */
    SYSCTL_RCGCGPIO_R |= 0x10; /* enable clock to PORT E (AIN3 is on PE0) */
    SYSCTL_RCGCADC_R |= 1;     /* enable clock to ADC0 */
    /* initialize PE0 for AIN3 input */
    GPIO_PORTE_AMSEL_R |= 1; /* enable analog function */
    /* initialize ADC0 */
    ADC0_ACTSS_R &= ~1;     /* disable SS0 during configuration */
    ADC0_EMUX_R &= ~0x000F; /* software trigger conversion */
    ADC0_SSMUX0_R |= 0x03;  /* get input from channel 3 */
    ADC0_SSCTL0_R |= 0x06;  /* take one sample at a time, set flag at 1st sample */
    ADC0_ACTSS_R |= 0x01;   /* enable ADC0 sequencer 0 */
    while (1)
    {
        ADC0_PSSI_R |= 1; /* start a conversion sequence 0 */
        while ((ADC0_RIS_R & 1) == 0)
            ;                    /* wait for conversion complete at SS0 */
        result = ADC0_SSFIFO0_R; /* read conversion result */
        ADC0_ISC_R = 1;          /* clear completion flag */

        V0 = (float)result * 3.3 / 4095.0; // Convert ADC count to voltage
        temp = V0 * 100;                   // Convert voltage to °C

        lightIntensity = (result * 10) / 3030; // Scale raw ADC vale to 0 -10 (for Light Intensity)       

    }
}
