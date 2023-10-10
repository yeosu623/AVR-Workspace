#define F_CPU 16000000
#define baudrate 9600
#define MYUBRR F_CPU / 16 / baudrate - 1

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "twi.h"


int main(void)
{
	uartInit(MYUBRR);
	TWI_Init();
	
    while (1) 
    {
		TWI_Master_Write(0x55, 0x27);
		_delay_ms(100);
		uartPuts("data transmitted.\n");
    }
}

