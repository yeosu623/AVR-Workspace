#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile int count = 0;
ISR(TIMER0_OVF_vect)
{
	TCNT0 = -125;
	count++;
	if(count>125-1) //500-1
	{
		PORTB = ~PORTB;
		count=0;
		OCR2 += 8;
	}
}

int main(void)
{
	DDRB = 0xff;
	PORTB = 0xff;
	
	TCCR2 = 0b01101011;
	TCCR0 = 0b00000011;
	TCNT0 = -125;
	
	TIMSK=(1<<TOIE0);
	
	sei();
	
	while(1)
	{
		
	}
}