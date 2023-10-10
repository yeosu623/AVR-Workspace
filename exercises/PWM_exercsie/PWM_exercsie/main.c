#define F_CPU 10000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void)
{
	unsigned char i;
	DDRB = 0xff;
	PORTB = 0xff;
	
	TCCR0 = 0b01101010;
	
	while(1)
	{
		for(i=0; i<256; i++)
		{
			OCR0 = i;
			_delay_ms(10);			
		}
		i=0;
	}
}