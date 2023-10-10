#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


int main(void)
{
    DDRA = (1<<PINA0)|(1<<PINA1)|(1<<PINA2); //R,G,B 순서
	
    while (1) 
    {
		PORTA = 1<<PINA0;
		_delay_ms(500);
		PORTA = 1<<PINA1;
		_delay_ms(500);
		PORTA = 1<<PINA2;
		_delay_ms(500);
		
		PORTA = (1<<PINA0 | 1<<PINA1);
		_delay_ms(500);
		PORTA = (1<<PINA0 | 1<<PINA2);
		_delay_ms(500);
		PORTA = (1<<PINA1 | 1<<PINA2);
		_delay_ms(500);
		
		PORTA = (1<<PINA0 | 1<<PINA1 | 1<<PINA2);
		_delay_ms(1000);
    }
}

