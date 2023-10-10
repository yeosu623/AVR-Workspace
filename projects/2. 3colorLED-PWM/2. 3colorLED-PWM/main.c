/*
 * 2. 3colorLED-PWM.c
 * USE PIN A0(red), B3(green), E7(blue)
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>

#include "GPIOtoPWM.h"

int main(void)
{
	GPIOtoPWMInit();
	GPIOtoPWM('A', 0, 0, 0, 100);
	GPIOtoPWM('B', 3, 0, 0, 100);
	GPIOtoPWM('E', 7, 0, 0, 100);

	uint8_t duty = 0;
    while (1)
	{
		// RED
		for(duty=0; duty<100; duty++)
		{
			SET_DUTY('A', 0, duty);
			_delay_ms(0.5);
		}
		SET_DUTY('A', 0, 0);
		
		// GREEN
		for(duty=0; duty<100; duty++)
		{
			SET_DUTY('B', 3, duty);
			_delay_ms(0.5);
		}
		SET_DUTY('B', 3, 0);
		
		// BLUE
		for(duty=0; duty<100; duty++)
		{
			SET_DUTY('E', 7, duty);
			_delay_ms(0.5);
		}
		SET_DUTY('E', 7, 0);
		
		// YELLOW
		for(duty=0; duty<100; duty++)
		{
			SET_DUTY('A', 0, duty);
			SET_DUTY('B', 3, duty);
			_delay_ms(0.5);
		}
		SET_DUTY('A', 0, 0);
		SET_DUTY('B', 3, 0);
		
		// CYAN
		for(duty=0; duty<100; duty++)
		{
			SET_DUTY('B', 3, duty);
			SET_DUTY('E', 7, duty);
			_delay_ms(0.5);
		}
		SET_DUTY('B', 3, 0);
		SET_DUTY('E', 7, 0);
		
		// MAGENTA
		for(duty=0; duty<100; duty++)
		{
			SET_DUTY('A', 0, duty);
			SET_DUTY('E', 7, duty);
			_delay_ms(0.5);
		}
		SET_DUTY('A', 0, 0);
		SET_DUTY('E', 7, 0);
		
		// WHITE
		for(duty=0; duty<100; duty++)
		{
			SET_DUTY('A', 0, duty);
			SET_DUTY('B', 3, duty);
			SET_DUTY('E', 7, duty);
			_delay_ms(0.5);
		}
		SET_DUTY('A', 0, 0);
		SET_DUTY('B', 3, 0);
		SET_DUTY('E', 7, 0);
	}
}