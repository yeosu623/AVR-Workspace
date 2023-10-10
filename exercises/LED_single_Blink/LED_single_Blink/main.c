// 1. LED 스위치 통제

#define F_CPU 12000000UL // 클럭 속도 결정
#include <avr/io.h>
#include <util/delay.h> // 딜레이 라이브러리


int main(void)
{
	DDRA = 0xff;
	DDRC = 0x00;
	PORTA = 0x00;
	while(1)
	{
		if(PINC & 0x01)
		{
			PORTA = PORTA | (1<<7);
		}
		else
		{
			PORTA = 0x00;
		}
	}
}

