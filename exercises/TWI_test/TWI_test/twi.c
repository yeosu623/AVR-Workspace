// 출처 : https://blog.naver.com/PostView.naver?blogId=jae7438&logNo=220985088746&redirect=Dlog&widgetTypeCall=true&directAccess=false

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include "twi.h"

void TWI_Init(void)
{
	// Slave 모드로 설정할 때는 TWBR를 주석처리해준다.
	// Prescaler = 1 일 때, 	TWBR = 12면 400KHz, TWBR = 72면 100KHz 신호를 만든다.
	
	TWBR = 12;  // TWI SCL 주기.
	TWSR = (0<<TWPS1)|(0<<TWPS0); // Prescaler = 1
}

void TWI_Master_Write(unsigned char Data, unsigned char Addr)
{
	TWCR = ((1<< TWINT) | (1 << TWSTA) | (1 << TWEN)); //TWI 시작
	while (!(TWCR & (1<< TWINT)));                    //TWINT 클리어될 동안
	TWDR = Addr << 1;
	TWCR = ((1 << TWINT) | (1 <<TWEN));
	while (!(TWCR & (1<< TWINT)));                    //TWINT 클리어될 동안
	TWDR = Data;
	TWCR = ((1 << TWINT) | (1 <<TWEN));
	while (!(TWCR & (1<< TWINT)));// == 0x00) || ((TWSR & 0xf8) !=0x28));
	TWCR = ((1<< TWINT) | (1 << TWSTO) | (1 << TWEN)); //정지
}

unsigned char TWI_Master_Read(unsigned char Addr)
{
	unsigned char Data;
	
	TWCR = ((1<< TWINT) | (1 << TWSTA) | (1 << TWEN)); // TWI 시작
	while (!(TWCR & (1<< TWINT)));                   // TWINT 클리어될 동안
	TWDR = Addr << 1;                                // 슬레이브 주소
	TWCR = ((1<< TWINT) | (1 << TWEN) | (1 << TWEA));
	while (!(TWCR & (1<< TWINT)));                   // TWINT 클리어될 동안
	TWCR = ((1<< TWINT) | (1 << TWEN));
	while (!(TWCR&(1<< TWINT)));                      // TWINT 클리어될 동안
	Data =TWDR;                                  // 데이터 읽기
	TWCR = ((1<< TWINT) | (1 << TWSTO) | (1 << TWEN)); // 정지
	return Data;                                  // 읽은 데이터 반환
}

void TWI_Slave_Write(unsigned char Data, unsigned char Add)
{
	TWAR = Add << 1;                             //마스터 주소
	TWCR = ((1<< TWEA) | (1 << TWEN));
	while (!(TWCR & (1<< TWINT)));              // TWINT 클리어될 동안
	TWDR = Data;                            // 데이터 쓰기
	TWCR = ((1<< TWINT) | (1 << TWEA) | (1 << TWEN));
	while (!(TWCR & (1<< TWINT)));              // TWINT 클리어될 동안
	TWCR = ((1<< TWINT) | (1 << TWEA) | (1 << TWEN));
}

unsigned char TWI_Slave_Read(unsigned char Add)
{
	unsigned char Data;
	
	TWAR = Add << 1;             //슬레이브주소
	TWCR = ((1<< TWEA) | (1 << TWEN));
	while (!(TWCR & (1<< TWINT)));// || ((TWSR&0xf8)!=0x60));  // TWINT 클리어될 동안
	TWCR = ((1<< TWINT) | (1 << TWEA) | (1 << TWEN));
	while (!(TWCR&(1<< TWINT)));//||((TWSR&0xf8)!=0x80));    // TWINT 클리어될 동안
	Data =TWDR;               //데이터 읽기
	TWCR = ((1<< TWINT) | (1 << TWEA) | (1 << TWEN));
	while (!(TWCR&(1<< TWINT)));//||((TWSR&0xf8)!=0xa0));    // TWINT 클리어될 동안
	return Data;                // 읽은 데이터 반환
}