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
	while ((TWSR & 0xF8) != 0x08);  //START 상태(08) 기다림  
}

void I2C_repeated_start()
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	while (!(TWCR & (1 << TWINT))); // TWINT가 1이 될때까지 대기. (ACK가 받아질떄 까지 대기)
	while ((TWSR & 0xF8) != 0x10);  //REPEATED START 상태(10) 기다림
}

// 3-1. 주소 및 데이터 송신하기
void I2C_transmit_address_writeMode(uint8_t address)
{
	TWDR = address;
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	
	while( !(TWCR & (1<< TWINT)) );
	while ((TWSR & 0xF8) != 0x18);  //SLA+W ACK 상태(18) 기다림
}

void I2C_transmit_address_readMode(uint8_t address)
{
	TWDR = address;
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

	while (!(TWCR & (1 << TWINT)));
	while ((TWSR & 0xF8) != 0x40);  //SLA+R ACK 상태(40) 기다림
}

void I2C_transmit_data(uint8_t data) // register address 혹은 data 전송
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

	while (!(TWCR & (1 << TWINT)));
	while ((TWSR & 0xF8) != 0x28);  //DATA ACK 상태(28) 기다림
}

// 3-2. 데이터 수신하기
uint8_t I2C_receive_ACK()
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	
	while( !(TWCR & (1<< TWINT)) );
	while ((TWSR & 0xF8) != 0x50);  //ACK 상태(50) 기다림 
	
	return TWDR;
}

uint8_t I2C_receive_NACK()
{
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	while( !(TWCR & (1<< TWINT)) );
	while ((TWSR & 0xF8) != 0x58);  //NACK 상태(58) 기다림 
	
	return TWDR;
}

// 4. I2C 통신 종료 (스탑 비트 전송)
void I2C_stop()
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}
