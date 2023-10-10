// 출처 : https://kyuhyuk.kr/article/avr/2022/08/28/AVR-ATmega328P-UART

/*************************
UART0 채널을 사용합니다.
*************************/
#define F_CPU 16000000

#include <avr/io.h>
#include "uart.h"

void uartInit(uint16_t ubrr)
{
	const unsigned int baudRate = (F_CPU / 16 / ubrr) - 1;
	// UBRR에 전송 속도(Buadrate)를 설정
	UBRR0L = (uint8_t)(baudRate & 0xFF);
	UBRR0H = (uint8_t)(baudRate >> 8);

	// UART 송신기와 수신기 활성화
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
	uartPuts("\n\r");
	uartPuts("[INFO] UART initialized!\n\r");
}

void uartPut(unsigned char data)
{
	// 전송 버퍼가 비어 있을 때까지 기다립니다
	while (!(UCSR0A & (1 << UDRE0)))
	;

	// 데이터를 UDR(전송 레지스터)에 입력
	UDR0 = data;
}

void uartPuts(char *s)
{
	// 문자열의 끝을 알리는 NULL이 나올때까지 단일 문자를 전송합니다
	while (*s > 0)
	uartPut(*s++);
}

void uartPutHex8(uint8_t val)
{
	// 입력 값에서 상위 및 하위 니블 추출
	uint8_t upperNibble = (val & 0xF0) >> 4;
	uint8_t lowerNibble = val & 0x0F;

	// 니블을 ASCII 16진수로 변환
	upperNibble += upperNibble > 9 ? 'A' - 10 : '0';
	lowerNibble += lowerNibble > 9 ? 'A' - 10 : '0';

	// 문자를 출력
	uartPut(upperNibble);
	uartPut(lowerNibble);
}

void uartPutHex16(uint16_t val)
{
	// 상위 8비트를 전송
	uartPutHex8((uint8_t)(val >> 8));

	// 하위 8비트를 전송
	uartPutHex8((uint8_t)(val & 0x00FF));
}

void uartPutU8(uint8_t val)
{
	uint8_t dig1 = '0', dig2 = '0';

	// 100 단위의 값을 계산합니다
	while (val >= 100)
	{
		val -= 100;
		dig1++;
	}

	// 10 단위의 값을 계산합니다
	while (val >= 10)
	{
		val -= 10;
		dig2++;
	}

	// 첫 번째 숫자를 출력 (0이면 출력하지 않습니다)
	if (dig1 != '0')
	uartPut(dig1);

	// 두 번째 숫자를 출력 (0이면 출력하지 않습니다)
	if ((dig1 != '0') || (dig2 != '0'))
	uartPut(dig2);

	// 마지막 숫자를 출력
	uartPut(val + '0');
}

void uartPutS8(int8_t val)
{
	// 값이 음수인지 확인합니다
	if (val & 0x80)
	{
		// 음수 기호를 출력합니다
		uartPut('-');

		// 부호 없는 값을 가져옵니다
		val = ~(val - 1);
	}

	// 부호 없는 값을 출력합니다
	uartPutU8((uint8_t)val);
}

void uartPutU16(uint16_t val)
{
	uint8_t dig1 = '0', dig2 = '0', dig3 = '0', dig4 = '0';

	// 10000 단위의 값을 계산합니다
	while (val >= 10000)
	{
		val -= 10000;
		dig1++;
	}

	// 1000 단위의 값을 계산합니다
	while (val >= 1000)
	{
		val -= 1000;
		dig2++;
	}

	// 100 단위의 값을 계산합니다
	while (val >= 100)
	{
		val -= 100;
		dig3++;
	}

	// 10 단위의 값을 계산합니다
	while (val >= 10)
	{
		val -= 10;
		dig4++;
	}

	// 이전 값이 출력되었는지 저장하는 변수입니다
	uint8_t prevPrinted = 0;

	// 첫 번째 숫자를 출력 (0이면 출력하지 않습니다)
	if (dig1 != '0')
	{
		uartPut(dig1);
		prevPrinted = 1;
	}

	// 두 번째 숫자를 출력 (0이면 출력하지 않습니다)
	if (prevPrinted || (dig2 != '0'))
	{
		uartPut(dig2);
		prevPrinted = 1;
	}

	// 세 번째 숫자를 출력 (0이면 출력하지 않습니다)
	if (prevPrinted || (dig3 != '0'))
	{
		uartPut(dig3);
		prevPrinted = 1;
	}

	// 네 번째 숫자를 출력 (0이면 출력하지 않습니다)
	if (prevPrinted || (dig4 != '0'))
	{
		uartPut(dig4);
		prevPrinted = 1;
	}

	// 마지막 숫자를 출력
	uartPut(val + '0');
}

void uartPutS16(int16_t val)
{
	// check for negative number
	if (val & 0x8000)
	{
		// print minus sign
		uartPut('-');

		// convert to unsigned magnitude
		val = ~(val - 1);
	}

	// print unsigned magnitude
	uartPutU16((uint16_t)val);
}

unsigned char uartGet(void)
{
	// 데이터 수신을 기다립니다
	while (!(UCSR0A & (1 << RXC0)))
	;

	// 데이터를 반환합니다
	return UDR0;
}

void uartGetLine(char *buf, uint8_t n)
{
	uint8_t bufIdx = 0;
	char c;

	// 수신된 문자가 캐리지 리턴(\r)이 나올때까지 수신합니다
	do
	{
		// 단일 문자를 수신합니다
		c = uartGet();

		// 수신한 단일 문자를 출력합니다
		uartPut(c);

		// 버퍼에 단일 문자를 저장합니다
		buf[bufIdx++] = c;
	} while ((bufIdx < n) && (c != '\r'));

	// 문자열 버퍼의 마지막에 NULL을 입력해서 문자열의 끝을 지정합니다
	buf[bufIdx] = 0;
}