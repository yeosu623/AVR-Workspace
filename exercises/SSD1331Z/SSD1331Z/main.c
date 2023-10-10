#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"
#include "SPI.h"
#include "SSD1331Z.h"
#include "sample_palettes.h"

FILE OUTPUT	= FDEV_SETUP_STREAM(uartPut, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, uartGet, _FDEV_SETUP_READ);

/******************
  MAIN FUNCTIONS
******************/
int main(void)
{
	stdout = &OUTPUT;
	stdin = &INPUT;

	uartInit(57600);
	SPI_MasterInit();
	SSD1331Z_init();
	
	while (1)
	{
		Cmd_MovingSquare(1, 1, 3, Color.blue);
		
		_delay_ms(10);
		
		/*
		for(int i=0; i<10000; i++)
		{
			Cmd_RotatingLine(48, 31, i, Color.white);
			_delay_ms(10);
		}
		*/
		/*
		Cmd_Fill(Color.red);
		_delay_ms(1000);
		Cmd_Fill(Color.orange);
		_delay_ms(1000);
		Cmd_Fill(Color.yellow);
		_delay_ms(1000);
		Cmd_Fill(Color.green);
		_delay_ms(1000);
		Cmd_Fill(Color.blue);
		_delay_ms(1000);
		Cmd_Fill(Color.indigo);
		_delay_ms(1000);
		Cmd_Fill(Color.violet);
		_delay_ms(1000);
		*/
	}
}


