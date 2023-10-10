#define F_CPU 16000000

// includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>
#include "uart.h"
#include "SPI.h"
#include "TWI.h"
#include "MPU6050.h"
#include "SSD1331Z.h"
#include "sample_palettes.h"

FILE OUTPUT	= FDEV_SETUP_STREAM(uartPut, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, uartGet, _FDEV_SETUP_READ);

// variables
unsigned char flag_10ms = 0;
extern float complementary_x_angle;
extern float complementary_y_angle;
extern float complementary_z_angle;

// interrupts
ISR(TIMER0_OVF_vect) // 1ms 주기.
{
	static unsigned char count_1ms = 0;
	
	TCNT0 = 6;
	count_1ms++;
	if(count_1ms > 9) // 10ms 주기
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
	SPI_MasterInit();
	I2C_init();
	MPU6050_init();
	MPU6050_config_gyro(0x08); // full scale range = +-500 degree/s
	MPU6050_config_accel(0x08); // full scale range = 4g
	MPU6050_calibration();
	SSD1331Z_init();
	
	while(1)
	{
		if(flag_10ms == 1)
		{
			flag_10ms = 0;
			MPU6050_print_complementary_angle(0.01, 0.96);
			
			Cmd_MovingSquare(complementary_x_angle, complementary_y_angle, 3, Color.blue);
			//Cmd_RotatingLine(45, 45, complementary_z_angle, Color.pink);
		}
	}
}