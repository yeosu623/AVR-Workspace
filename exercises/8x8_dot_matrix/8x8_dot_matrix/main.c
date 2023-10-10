// 1588BHG 8x8 dot matrix 기준이다.
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

char timer_1s = 0; //false
char timer_150ms = 0; //false
ISR(TIMER0_OVF_vect)
{
	static short count_1s = 0;
	static short count_150ms = 0;
	
	TCNT0 = -250;
	count_1s++;
	count_150ms++;
	if(count_1s>999) //1초. 1000-1 이다.
	{
		count_1s = 0;
		timer_1s = 1; // true
	}
	
	if(count_150ms>149) // 150ms
	{
		count_150ms = 0;
		timer_150ms = 1;
	}
}

void timer0_setting()
{
	TCCR0 = ((1<<CS02)|(0<<CS01)|(0<<CS00));
	TCNT0 = -250; //1ms
	TIMSK=(1<<TOIE0);
}

/* global variables */
// alphabet[0]은 A, alphabet[1]은 B, ... alphabet[25]는 Z
unsigned char blank[8] = {0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111};
unsigned char alphabet[26][8] = {{0b11111111, 0b11100111, 0b11011011, 0b10111101, 0b10111101, 0b10000001, 0b10111101, 0b10111101},
					  			{0b11111111, 0b10000011, 0b10111101, 0b10111101, 0b10000011, 0b10111101, 0b10111101, 0b10000011},
					 			{0b11111111, 0b11100001, 0b11011111, 0b10111111, 0b10111111, 0b10111111, 0b11011111, 0b11100001},
					 			{0b11111111, 0b10000111, 0b10111011, 0b10111101, 0b10111101, 0b10111101, 0b10111011, 0b10000111},
					 			{0b11111111, 0b10000001, 0b10111111, 0b10111111, 0b10000001, 0b10111111, 0b10111111, 0b10000001},
					 			{0b11111111, 0b10000001, 0b10111111, 0b10111111, 0b10000001, 0b10111111, 0b10111111, 0b10111111},
					 			{0b11111111, 0b11100001, 0b11011111, 0b10111111, 0b10110001, 0b10111101, 0b11011101, 0b11100001},
					 			{0b11111111, 0b10111101, 0b10111101, 0b10111101, 0b10000001, 0b10111101, 0b10111101, 0b10111101},
								{0b11111111, 0b10000001, 0b11100111, 0b11100111, 0b11100111, 0b11100111, 0b11100111, 0b10000001},
								{0b11111111, 0b11100001, 0b11111011, 0b11111011, 0b11111011, 0b10111011, 0b11011011, 0b11100111},
								{0b11111111, 0b10111101, 0b10111011, 0b10110111, 0b10001111, 0b10110111, 0b10111011, 0b10111101},
								{0b11111111, 0b10111111, 0b10111111, 0b10111111, 0b10111111, 0b10111111, 0b10111111, 0b10000001},
								{0b11111111, 0b10111101, 0b10011001, 0b10100101, 0b10111101, 0b10111101, 0b10111101, 0b10111101},
								{0b11111111, 0b10111101, 0b10011101, 0b10101101, 0b10110101, 0b10111001, 0b10111101, 0b10111101},
								{0b11111111, 0b11100111, 0b11011011, 0b10111101, 0b10111101, 0b10111101, 0b11011011, 0b11100111},
								{0b11111111, 0b10000011, 0b10111101, 0b10111101, 0b10111101, 0b10000011, 0b10111111, 0b10111111},
								{0b11111111, 0b11100111, 0b11011011, 0b10111101, 0b10111101, 0b11011011, 0b11100111, 0b11111001},
								{0b11111111, 0b10000001, 0b10111101, 0b10111101, 0b10111101, 0b10000011, 0b10110111, 0b10111011},
								{0b11111111, 0b11000001, 0b10111111, 0b10111111, 0b11000011, 0b11111101, 0b11111101, 0b10000011},
								{0b11111111, 0b10000001, 0b11100111, 0b11100111, 0b11100111, 0b11100111, 0b11100111, 0b11100111},
								{0b11111111, 0b10111101, 0b10111101, 0b10111101, 0b10111101, 0b10111101, 0b10111101, 0b11000011},
								{0b11111111, 0b10111101, 0b10111101, 0b10111101, 0b10111101, 0b11011011, 0b11011011, 0b11100111},
								{0b11111111, 0b10111101, 0b10111101, 0b10111101, 0b10100101, 0b10100101, 0b10100101, 0b11011011},
								{0b11111111, 0b10111101, 0b10111101, 0b10111101, 0b11000011, 0b10111101, 0b10111101, 0b10111101},
								{0b11111111, 0b10111101, 0b10111101, 0b10111101, 0b11011011, 0b11100111, 0b11100111, 0b11100111},
								{0b11111111, 0b10000001, 0b11111101, 0b11111011, 0b11110111, 0b11101111, 0b11011111, 0b10000001}};
									
int main(void)
{	
	DDRA = 0xff; // PORTA 신호가 1일시, 전압 입력
	DDRB = 0xff; // PORTB 신호가 1일시, 노란색 LED는 OFF
	DDRC = 0xff; // PORTC 신호가 1일시, 빨간색 LED는 OFF
	PORTB = 0xff;
	
	timer0_setting();
	sei();
	
	/* 원리
	전압 입력인 PORTA를 0~7bit를 빠르게 순환시킨다.
	
	PORTA가 0,1,2,... 가 순환되는 동안,
	PORTB/C 는 그 가로줄에 대응되는 값을 하나씩 일치시킨다.
	*/
	
	
	// 출력할 알파벳(대문자만 가능)
	unsigned char sentence[] = {'M','H','I','V','E','F','O','R','E','V','E','R'};
	
	// 알파벳을 문자 순서로 변환
	unsigned char i;
	for(i=0; i<(sizeof(sentence)/1); i++)
		sentence[i] = sentence[i] - 65; // ASCII 에 따라 대문자를 문자 순서로 변환. A=0, Z=25
	
    while (1) 
    {
		display_slide(sentence, sizeof(sentence)/1);
    }
}

void display_pop_up(unsigned char* order_num, unsigned char length)
{
	unsigned char i, j;
	unsigned char present_num;
	
	for(i=0; i<length; i++)
	{
		present_num = order_num[i];
		while(!(timer_1s))
		{
			for(j=0; j<8; j++)
			{
				PORTA = 0x01<<j;
				PORTC = alphabet[present_num][j];
				
				// 너무 빠른 속도로 순환하면, 전류가 LED에 제대로 공급이 되지 않는다.
				// 하나의 LED에 전류가 공급되려고 하기 전에, 다른 LED를 제어하러 가기 떄문이다.
				// 따라서 1ms 딜레이를 넣었다.
				_delay_ms(1);
			}
		}
		timer_1s = 0;
	}
}

void display_slide(unsigned char* order, unsigned char length)
{
	unsigned char i;

	for(i=0; i<length; i++)
	{
		if(i == 0) // start
			display_slide_part_of_moving(blank, alphabet[order[0]]);
		else // contents
			display_slide_part_of_moving(alphabet[order[i-1]], alphabet[order[i]]);
	}
	display_slide_part_of_moving(alphabet[order[length-1]], blank); // finish
}

void display_slide_part_of_moving(unsigned char* partA, unsigned char* partB)
{
	unsigned char i, row_push;
	
	for(row_push=0; row_push<8; row_push++)
	{
		while(!(timer_150ms))
		{
			for(i=0; i<8; i++)
			{
				PORTA = 0x01 << i;
				PORTC = (partA[i] << row_push) + (partB[i] >> (8-row_push));
				
				_delay_ms(1);
			}
		}
		timer_150ms = 0;
	}
}
