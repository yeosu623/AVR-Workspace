#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	/* Be Careful about YOU MUST SET "SS" PIN TO START SPI COMMUNICATION!!!!!!!!!! */
	DDRB = (1<<PINB2) | (1<<PINB1) | (1<<PINB0);
	/* Enable SPI, Master, set clock rate F_osc/16 */
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
}

void SPI_MasterTransmit(unsigned char cData)
{
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
}

int main(void)
{
	SPI_MasterInit();

	while (1)
	{
		SPI_MasterTransmit('M');
	}
}

