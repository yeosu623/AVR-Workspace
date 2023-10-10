// Brief Description for source code : TurnOnLED on various way.
// It use only Port A0~A7.

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

void TurnOnLED_Sequence();
void TurnOnLED_ReverseSequence();
void TurnOnLED_Alternate();
void TurnOnLED_ShiftBits(uint8_t);
void TurnOnLED_ShiftBitsFaster(uint8_t);
void TurnOnLED_ShiftBitsSuperFaster(uint8_t);
void TurnOnLED_ReverseShiftBits(uint8_t);
void TurnOnLED_ReverseShiftBitsFaster(uint8_t);
void TurnOnLED_ReverseShiftBitsSuperFaster(uint8_t);

int main(void)
{
	uint8_t i = 0;
	DDRA = 0xff;
	
	TurnOnLED_Sequence();
	TurnOnLED_ReverseSequence();
	TurnOnLED_Alternate();
	
	PORTA = 0b11100000;
	TurnOnLED_ShiftBits(PORTA);
	TurnOnLED_ShiftBitsFaster(PORTA);
	TurnOnLED_ShiftBitsFaster(PORTA);
	TurnOnLED_ReverseShiftBitsFaster(PORTA);
	TurnOnLED_ReverseShiftBitsFaster(PORTA);

	PORTA = 0b11001100;
	for(i=0; i<4; i++) TurnOnLED_ShiftBitsFaster(PORTA);
	
	PORTA = 0b11111110;
	for(i=0; i<4; i++) TurnOnLED_ShiftBitsFaster(PORTA);
	
	for(i=0; i<8; i++)
	{
		PORTA = 0b11111110 << i;
		TurnOnLED_ShiftBitsFaster(PORTA);
	}
	
    while (1);
}

// lets make some features.
void TurnOnLED_Sequence()
{
	uint8_t i = 0;
	for(i=0; i<8; i++)
	{
		PORTA = 0x01 << i;
		_delay_ms(500);
	}
}

void TurnOnLED_ReverseSequence()
{
	uint8_t i = 0;
	for(i=0; i<8; i++)
	{
		PORTA = 0b10000000 >> i;
		_delay_ms(500);
	}
}

void TurnOnLED_ShiftBits(uint8_t portdata)
{
	uint8_t i;
	PORTA = portdata;
	for(i=0; i<8; i++)
	{
		if((PORTA & 0b10000000) != 0)
		{
			PORTA = PORTA << 1;
			PORTA = PORTA += 1;
		}
		else PORTA = PORTA << 1;
		
		_delay_ms(500);
	}
}

void TurnOnLED_ShiftBitsFaster(uint8_t portdata)
{
	uint8_t i;
	PORTA = portdata;
	for(i=0; i<8; i++)
	{
		if((PORTA & 0b10000000) != 0)
		{
			PORTA = PORTA << 1;
			PORTA = PORTA += 1;
		}
		else PORTA = PORTA << 1;
		
		_delay_ms(100);
	}
}

void TurnOnLED_ShiftBitsSuperFaster(uint8_t portdata)
{
	uint8_t i;
	PORTA = portdata;
	for(i=0; i<8; i++)
	{
		if((PORTA & 0b10000000) != 0)
		{
			PORTA = PORTA << 1;
			PORTA = PORTA += 1;
		}
		else PORTA = PORTA << 1;
		
		_delay_ms(40);
	}
}

void TurnOnLED_ReverseShiftBits(uint8_t portdata)
{
	uint8_t i;
	PORTA = portdata;
	for(i=0; i<8; i++)
	{
		if((PORTA & 0b00000001) != 0)
		{
			PORTA = PORTA >> 1;
			PORTA = PORTA += 0b10000000;
		}
		else PORTA = PORTA >> 1;
		
		_delay_ms(500);
	}
}

void TurnOnLED_ReverseShiftBitsFaster(uint8_t portdata)
{
	uint8_t i;
	PORTA = portdata;
	for(i=0; i<8; i++)
	{
		if((PORTA & 0b00000001) != 0)
		{
			PORTA = PORTA >> 1;
			PORTA = PORTA += 0b10000000;
		}
		else PORTA = PORTA >> 1;
		
		_delay_ms(100);
	}
}

void TurnOnLED_ReverseShiftBitsSuperFaster(uint8_t portdata)
{
	uint8_t i;
	PORTA = portdata;
	for(i=0; i<8; i++)
	{
		if((PORTA & 0b00000001) != 0)
		{
			PORTA = PORTA >> 1;
			PORTA = PORTA += 0b10000000;
		}
		else PORTA = PORTA >> 1;
		
		_delay_ms(40);
	}
}

void TurnOnLED_Alternate()
{
	uint8_t i = 0;
	for(i=0; i<4; i++)
	{
		PORTA = 0b01010101;
		_delay_ms(500);
		PORTA = 0b10101010;
		_delay_ms(500);
	}
}