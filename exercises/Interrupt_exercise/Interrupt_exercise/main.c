// 0번 외부인터럽트 사용해보기

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile int sw;

ISR(INT0_vect)
{
	sw=1;
}

void external_interrupt_setting() //인터럽트 설정
{
	EIMSK=(1<<INT0); //0번 외부 인터럽트를 사용하겠다.
	EICRA=((1<<ISC01)|(1<ISC00)); //0번 외부 인터럽트는 입력 신호가 rising edge일 때 발생한다.
}

int main(void)
{
    DDRD = 0x00;
	DDRA = 0xff;
	
	external_interrupt_setting();
	sei(); //인터럽트 실행
	
    while (1) 
    {
		if(sw==1)
		{
			PORTA = 0xff;
			_delay_ms(1000);
			PORTA = 0x00;
			sw=0;
		}
    }
}

