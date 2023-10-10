#define I2C_SCL PD0
#define I2C_SDA PD1

#include <avr/io.h>
#include "TWI.h"

// 1. I2C 설정하기
void I2C_init()
{
	DDRD |= (1 << I2C_SCL);
	DDRD |= (1 << I2C_SDA);
	
	TWBR = 32; // I2C 클록 주파수 설정 : 200KHz
}

// 2. TWI 시작하기 (시작 비트 전송)
void I2C_start()
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	
	while( !(TWCR & (1 << TWINT)) ); // TWINT가 1이 될때까지 대기. (ACK가 받아질떄 까지 대기)
}

// 3-1. 주소 및 데이터 송신하기
void I2C_transmit(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	
	while( !(TWCR & (1<< TWINT)) );
}

// 3-2. 데이터 수신하기
uint8_t I2C_receive_ACK()
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	
	while( !(TWCR & (1<< TWINT)) );
	
	return TWDR;
}

uint8_t I2C_receive_NACK()
{
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	while( !(TWCR & (1<< TWINT)) );
	
	return TWDR;
}

// 4. I2C 통신 종료 (스탑 비트 전송)
void I2C_stop()
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}
