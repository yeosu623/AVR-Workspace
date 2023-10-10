#define F_CPU 16000000

// includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>
#include "uart.h"
#include "TWI.h"
#include "MPU6050.h"

FILE OUTPUT	= FDEV_SETUP_STREAM(uartPut, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, uartGet, _FDEV_SETUP_READ);

// variables
unsigned char flag_10ms = 0;

// interrupts
ISR(TIMER0_OVF_vect) // 1ms 주기.
{
	static unsigned char count_1ms = 0;
	
	TCNT0 = -250;
	count_1ms++;
	if(count_1ms > 10-1) // 10ms 주기
	{
		count_1ms = 0;
		flag_10ms = 1;
	}
}

// functions
void timer0_setting()
{
	TCCR0 = ((1<<CS02)|(0<<CS01)|(0<<CS00));
	TCNT0 = -250; //1ms로 설정한다. 1/1600000 * 64 * 250 = 1/1000(s) = 1ms
	TIMSK = (1<<TOIE0);
}

/******************
  MAIN FUNCTIONS
******************/
int main()
{
	stdout = &OUTPUT;
	stdin = &INPUT;
	
	timer0_setting();
	sei();
	uartInit(57600);
	I2C_init();
	MPU6050_init();
	MPU6050_config_gyro(0x00); // full scale range = +-250 degree/s
	MPU6050_config_accel(0x00); // full scale range = 2g
	MPU6050_calibration();
	
	while(1)
	{
		if(flag_10ms == 1)
		{
			flag_10ms = 0;
			MPU6050_print_complementary_angle(0.01, 0.96);
		}
	}
}