#include <avr/io.h>
#include <avr/interrupt.h>

#include "GPIOtoPWM.h"
#include "GPIOtoPWMvariables.h"

ISR(TIMER0_OVF_vect) // 100us마다 인터럽트 발생.
{
	TCNT0 = -25;
	
	// Make PWM here!
	uint8_t PINx;
	for(PINx=0; PINx<8; PINx++)
	{
		/* PORTA */
		if(freqA[PINx] != 0)
		{
			// 1. Duty cycles 이전
			if(countA[PINx] < dutyCycleA[PINx])
			{
				if(inverseA[PINx]) PORTA &= (~(1<<PINx)); // 0으로 시작
				else PORTA |= (1<<PINx); // 1로 시작
			}
			// 2. Duty cycles 이후
			else
			{
				if(inverseA[PINx]) PORTA |= (1<<PINx); // 1로 만듬
				else PORTA &= (~(1<<PINx)); // 0으로 만듬.
			}
			// 3. 초기화
			if(countA[PINx] == freqA[PINx])
			{
				countA[PINx] = 0;
				if(inverseA[PINx]) PORTA &= (~(1<<PINx)); // 0으로 초기화
				else PORTA |= (1<<PINx); // 1로 초기화
			}
			countA[PINx]++;
		}
		
		/* PORTB */
		if(freqB[PINx] != 0)
		{
			// 1. Duty cycles 이전
			if(countB[PINx] < dutyCycleB[PINx])
			{
				if(inverseB[PINx]) PORTB &= (~(1<<PINx)); // 0으로 시작
				else PORTB |= (1<<PINx); // 1로 시작
			}
			// 2. Duty cycles 이후
			else
			{
				if(inverseB[PINx]) PORTB |= (1<<PINx); // 1로 만듬
				else PORTB &= (~(1<<PINx)); // 0으로 만듬.
			}
			// 3. 초기화
			if(countB[PINx] == freqB[PINx])
			{
				countB[PINx] = 0;
				if(inverseB[PINx]) PORTB &= (~(1<<PINx)); // 0으로 초기화
				else PORTB |= (1<<PINx); // 1로 초기화
			}
			countB[PINx]++;
		}
		
		/* PORTC */
		if(freqC[PINx] != 0)
		{
			// 1. Duty cycles 이전
			if(countC[PINx] < dutyCycleC[PINx])
			{
				if(inverseC[PINx]) PORTC &= (~(1<<PINx)); // 0으로 시작
				else PORTC |= (1<<PINx); // 1로 시작
			}
			// 2. Duty cycles 이후
			else
			{
				if(inverseC[PINx]) PORTC |= (1<<PINx); // 1로 만듬
				else PORTC &= (~(1<<PINx)); // 0으로 만듬.
			}
			// 3. 초기화
			if(countC[PINx] == freqC[PINx])
			{
				countC[PINx] = 0;
				if(inverseC[PINx]) PORTC &= (~(1<<PINx)); // 0으로 초기화
				else PORTC |= (1<<PINx); // 1로 초기화
			}
			countC[PINx]++;
		}
		
		/* PORTD */
		if(freqD[PINx] != 0)
		{
			// 1. Duty cycles 이전
			if(countD[PINx] < dutyCycleD[PINx])
			{
				if(inverseD[PINx]) PORTD &= (~(1<<PINx)); // 0으로 시작
				else PORTD |= (1<<PINx); // 1로 시작
			}
			// 2. Duty cycles 이후
			else
			{
				if(inverseD[PINx]) PORTD |= (1<<PINx); // 1로 만듬
				else PORTD &= (~(1<<PINx)); // 0으로 만듬.
			}
			// 3. 초기화
			if(countD[PINx] == freqD[PINx])
			{
				countD[PINx] = 0;
				if(inverseD[PINx]) PORTD &= (~(1<<PINx)); // 0으로 초기화
				else PORTD |= (1<<PINx); // 1로 초기화
			}
			countD[PINx]++;
		}
		
		/* PORTE */
		if(freqE[PINx] != 0)
		{
			// 1. Duty cycles 이전
			if(countE[PINx] < dutyCycleE[PINx])
			{
				if(inverseE[PINx]) PORTE &= (~(1<<PINx)); // 0으로 시작
				else PORTE |= (1<<PINx); // 1로 시작
			}
			// 2. Duty cycles 이후
			else
			{
				if(inverseE[PINx]) PORTE |= (1<<PINx); // 1로 만듬
				else PORTE &= (~(1<<PINx)); // 0으로 만듬.
			}
			// 3. 초기화
			if(countE[PINx] == freqE[PINx])
			{
				countE[PINx] = 0;
				if(inverseE[PINx]) PORTE &= (~(1<<PINx)); // 0으로 초기화
				else PORTE |= (1<<PINx); // 1로 초기화
			}
			countE[PINx]++;
		}
		
		/* PORTF */
		if(freqF[PINx] != 0)
		{
			// 1. Duty cycles 이전
			if(countF[PINx] < dutyCycleF[PINx])
			{
				if(inverseF[PINx]) PORTF &= (~(1<<PINx)); // 0으로 시작
				else PORTF |= (1<<PINx); // 1로 시작
			}
			// 2. Duty cycles 이후
			else
			{
				if(inverseF[PINx]) PORTF |= (1<<PINx); // 1로 만듬
				else PORTF &= (~(1<<PINx)); // 0으로 만듬.
			}
			// 3. 초기화
			if(countF[PINx] == freqF[PINx])
			{
				countF[PINx] = 0;
				if(inverseF[PINx]) PORTF &= (~(1<<PINx)); // 0으로 초기화
				else PORTF |= (1<<PINx); // 1로 초기화
			}
			countF[PINx]++;
		}
	}
}

void timer0_setting()
{
	TCCR0 = ((1<<CS02)|(0<<CS01)|(0<<CS00)); // 64 prescaler
	TCNT0 = -25; //100us로 설정한다. 1/16000000 * 64 * 25 = 100us
	TIMSK = (1<<TOIE0);
}

void GPIOtoPWMInit()
{
	timer0_setting();
	sei();
}

void GPIOtoPWM(uint8_t PORTx, uint8_t PINx, uint8_t duty, uint8_t inverse, uint16_t freq)
{
	// 100us마다 인터럽트가 걸리는 타이머0을 사용하여 PWM 신호를 만들어 본다.
	
	// PORTx : you can write to use A ~ F.
	// PINx : you can use 0 ~ 7.
	// duty : you can use 0 ~ 100 duty cycle.
	// inverse : 0 = 0-> 1 PWM signal, 1 = 1 -> 0 PWM signal
	// freq : you can use 0 ~ 100 freq.
	
	// 주의 : 주파수가 100이 넘어가면 PWM 생성 신호에 버그가 생길 수 있음.
	
	switch(PORTx)
	{
		case 'A':
			DDRA = DDRA | (1<<PINx);
			freqA[PINx] = 10000 / freq; // 초당 인터럽트 / freq.
			inverseA[PINx] = inverse;
			dutyCycleA[PINx] = freqA[PINx] * duty / 100;
			break;
		case 'B':
			DDRB = DDRB | (1<<PINx);
			freqB[PINx] = 10000 / freq; // 초당 인터럽트 / freq.
			inverseB[PINx] = inverse;
			dutyCycleB[PINx] = freqB[PINx] * duty / 100;
			break;
		case 'C':
			DDRC = DDRC | (1<<PINx);
			freqC[PINx] = 10000 / freq; // 초당 인터럽트 / freq.
			inverseC[PINx] = inverse;
			dutyCycleC[PINx] = freqC[PINx] * duty / 100;
			break;
		case 'D':
			DDRD = DDRD | (1<<PINx);
			freqD[PINx] = 10000 / freq; // 초당 인터럽트 / freq.
			inverseD[PINx] = inverse;
			dutyCycleD[PINx] = freqD[PINx] * duty / 100;
			break;
		case 'E':
			DDRE = DDRE | (1<<PINx);
			freqE[PINx] = 10000 / freq; // 초당 인터럽트 / freq.
			inverseE[PINx] = inverse;
			dutyCycleE[PINx] = freqE[PINx] * duty / 100;
			break;
		case 'F':
			DDRF = DDRF | (1<<PINx);
			freqF[PINx] = 10000 / freq; // 초당 인터럽트 / freq.
			inverseF[PINx] = inverse;
			dutyCycleF[PINx] = freqF[PINx] * duty / 100;
			break;
	}
}

void SET_FREQ(uint8_t PORTx, uint8_t PINx, uint16_t freq)
{
	switch(PORTx)
	{
		case 'A' : freqA[PINx] = 10000 / freq; break;
		case 'B' : freqB[PINx] = 10000 / freq; break;
		case 'C' : freqC[PINx] = 10000 / freq; break;
		case 'D' : freqD[PINx] = 10000 / freq; break;
		case 'E' : freqE[PINx] = 10000 / freq; break;
		case 'F' : freqF[PINx] = 10000 / freq; break;
	}
}

void SET_DUTY(uint8_t PORTx, uint8_t PINx, uint8_t duty)
{
	switch(PORTx)
	{
		case 'A' : dutyCycleA[PINx] = freqA[PINx] * duty / 100; break;
		case 'B' : dutyCycleB[PINx] = freqB[PINx] * duty / 100; break;
		case 'C' : dutyCycleC[PINx] = freqC[PINx] * duty / 100; break;
		case 'D' : dutyCycleD[PINx] = freqD[PINx] * duty / 100; break;
		case 'E' : dutyCycleE[PINx] = freqE[PINx] * duty / 100; break;
		case 'F' : dutyCycleF[PINx] = freqF[PINx] * duty / 100; break;
	}
}