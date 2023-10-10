// √‚√≥ : https://noel-embedded.tistory.com/624?category=791495
#include <avr/io.h>
#include "clcd.h"
#include "uart.h"

int main(void)
{
	uart0_init();
	i2c_lcd_init();
	
	char str0[16] = "Hello World!";
	char str1[16] = "ATmega128";
	
	printf("Before string transfer....\r\n");
	i2c_lcd_string(0, 0, str0);
	i2c_lcd_string(1, 0, str1);
	printf("string transfer complete....\r\n");	
	
	while(1)
	{
		
	}
}