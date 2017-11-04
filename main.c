// B. A. Bryce 2017

#include <atmel_start.h>
#include "xprintf.h"
#define NOP() __asm volatile ( "NOP" )
#define uprintf(fmt, ...) xprintf(fmt, ##__VA_ARGS__); cdcFlush()

void nopDelay(unsigned long delay);

int main(void)
{
	uint8_t adcSampleBuf[2];
	uint16_t adcSample;
	uint8_t commandChar = '\0';

	//Functions for xprintf
	xfunc_out = putCharUSB;
	xfunc_in = getCharUSB;

	//initialize hardware and USB callbacks
	atmel_start_init();
	acm_setup();
	//finish the setup of ADC0 channel 0
	adc_sync_enable_channel(&ADC_0, 0);


	while (1)
	{
		commandChar = getCharUSBnonBlocking();
		switch(commandChar)
		{
		    case 'r':
		    	adc_sync_read_channel(&ADC_0, 0, adcSampleBuf, 2);
				adcSample = (adcSampleBuf[1] << 8) + adcSampleBuf[0];
				uprintf("%d\n", adcSample);
				break;
		    case '\0':
		    	// do nothing
		    	break;
		    default :
		    	uprintf("%c is not a valid command\n", commandChar);
		}
	}
}


//dummy delay function
void nopDelay(unsigned long delay)
{
    unsigned long i, j;
    for(j = 0; j<delay; j++)
    {
        for(i = 0; i<0xFFF; i++)NOP();
    }

}





