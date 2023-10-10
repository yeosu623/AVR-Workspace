// 이 코드는 Atemga128 데이터시트에 있는 예제 코드에 기반하여 작성되었습니다.
#define F_CPU 16000000 // Clock Speed
#define BAUD 9600 // Baud Rate. 통신 속도.
#define MYUBRR F_CPU/16/BAUD-1 // UBRR 계산. Asynchronous Normal Mode(U2X = 0)

#include <avr/io.h>
#include <util/delay.h>

void USART_init(unsigned int ubrr)
{
	/* set baud rate */
	UBRR0H = (unsigned char) (ubrr>>8);
	UBRR0L = (unsigned char) ubrr;
	
	/* enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	
	/* Set frame format : 8 data, 2 stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

/* Sending Frames with 5 to 8 data bit */
void USART_Transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	// UCSR0A 레지스터 안에 있는 UDRE0 비트는, 버퍼가 비어있는지를 확인한다. 비어있으면 1, 안비어있으면 0.
	while (!(UCSR0A & (1<<UDRE0)));
		
	/* Put data into buffer, sends the data */
	// Block diagram 를 보면, UBR 데이터 레지스터에 값을 넣기만 하면 자동으로 ATmega에 전송해주는걸 알 수 있다.
	UDR0 = data; // UDR0에 데이터를 넣으면 Transmit
}

/* Receiving Frames with 5 to 8 Data Bits */
unsigned char USART_Receive(void)
{
	/* Wait for data to be received */
	// UCSR0A 레지스터 안에 있는 RXC0 비트는, Receive 가 완료되었는지를 확인한다. 완료됬으면 1, 안됬으면 0.
	while (!(UCSR0A & (1<<RXC0)));
		
	/* Get and return received data from buffer */
	// Block diagram 를 보면, UBR 데이터 레지스터에 값이 들어오면 자동으로 ATmega에 전송해주는걸 알 수 있다.
	return UDR0; // UDR0의 데이터를 반환하면 Receive
}

/* Sending String type data */
void USART_Transmit_String(unsigned char *data, unsigned short length, char lineBreak)
{	
	unsigned short i;
	for(i=0; i<length; i++)
	{
		while (!(UCSR0A & (1<<UDRE0))); // Wait for empty transmit buffer
		UDR0 = data[i];
	}
	while (!(UCSR0A & (1<<UDRE0))); // Wait for empty transmit buffer
	if(lineBreak == 1) UDR0 = 13; // if true, Press Enter Key.
}

/* Receiving String type data */
/* Divided by Enter key on terminal. */
/* To use, you should send Array address of index 0. */
unsigned short USART_Receive_String(unsigned char *arr)
{
	unsigned short i = 0;
	
	for(i=0; i<65535; i++)
	{
		arr[i] = USART_Receive();
		if(arr[i] == 13) // Enter 라면
		{
			arr[i] = '\0'; // Enter 대신에 공백을 넣고
			return i; // length 를 출력한다.
		}
	}
}

int main(void)
{
	USART_init(MYUBRR);
	unsigned char a[100];
	unsigned short length;

    while (1) 
    {
		USART_Transmit_String("test", 4, 1);
		_delay_ms(500);
    }
}

