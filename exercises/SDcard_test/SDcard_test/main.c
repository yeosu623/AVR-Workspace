/* 이 코드는 https://kyuhyuk.kr/article/avr/2022/08/28/AVR-ATmega328P-SDCard-1 에서 도움을 받아 작성되었습니다. */
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

// Hardware define
#define DDR_SPI			DDRB
#define PORT_SPI		PORTB
#define CS				PINB0
#define MOSI			PINB2
#define MISO			PINB3
#define SCK				PINB1

#define CS_ENABLE()		PORT_SPI &= ~(1 << CS)
#define CS_DISABLE()	PORT_SPI |= (1<<CS)

// Software define : CMD
#define CMD0			0
#define CMD0_ARG		0x00000000
#define CMD0_CRC		0x94 //10010100b
#define CMD8			8
#define CMD8_ARG		0x000001AA
/* 
	CND8_CRC = (01000011 << 1)
	End Bit 자리를 만들어줘야 하기 때문에 << 1을 합니다.
	End Bit을 1로 설정하는 부분은 sdCommand()의
	spiTransfer(crc | 0x01); 에서 처리합니다.
*/
#define CMD8_CRC		0x86
#define CMD58			58
#define CMD58_ARG		0x00000000
#define CMD58_CRC		0x00
#define CMD55			55
#define CMD55_ARG		0x00000000
#define CMD55_CRC		0x00
#define ACMD41			41
#define ACMD41_ARG		0x40000000
#define ACMD41_CRC		0x00
#define CMD17			17
#define CMD17_CRC		0x00
#define CMD24			24
#define CMD24_ARG		0x00
#define CMD24_CRC		0x00

#define SD_READY		0
#define SD_SUCCESS		0
#define SD_ERROR		1

#define SD_MAX_READ_ATTEMPTS	1563
#define SD_MAX_WRITE_ATTEMPTS   3907
#define SD_BLOCK_LEN            512
#define SD_START_TOKEN          0xFE
#define SD_ERROR_TOKEN          0x00

// Software define : macro functions
#define PARAM_ERROR(X)      X & 0b01000000
#define ADDR_ERROR(X)       X & 0b00100000
#define ERASE_SEQ_ERROR(X)  X & 0b00010000
#define CRC_ERROR(X)        X & 0b00001000
#define ILLEGAL_CMD(X)      X & 0b00000100
#define ERASE_RESET(X)      X & 0b00000010
#define IN_IDLE(X)          X & 0b00000001

#define CMD_VER(X)          ((X >> 4) & 0xF0)
#define VOL_ACC(X)          (X & 0x1F)

#define VOLTAGE_ACC_27_33   0b00000001
#define VOLTAGE_ACC_LOW     0b00000010
#define VOLTAGE_ACC_RES1    0b00000100
#define VOLTAGE_ACC_RES2    0b00001000

#define POWER_UP_STATUS(X)  X & 0x40
#define CCS_VAL(X)          X & 0x40
#define VDD_2728(X)         X & 0b10000000
#define VDD_2829(X)         X & 0b00000001
#define VDD_2930(X)         X & 0b00000010
#define VDD_3031(X)         X & 0b00000100
#define VDD_3132(X)         X & 0b00001000
#define VDD_3233(X)         X & 0b00010000
#define VDD_3334(X)         X & 0b00100000
#define VDD_3435(X)         X & 0b01000000
#define VDD_3536(X)         X & 0b10000000

#define SD_R1_NO_ERROR(X)   X < 0x02

#define SD_TOKEN_OOR(X)     X & 0b00001000
#define SD_TOKEN_CECC(X)    X & 0b00000100
#define SD_TOKEN_CC(X)      X & 0b00000010
#define SD_TOKEN_ERROR(X)   X & 0b00000001

/*************************
SPI 초기화
*************************/
void spiInit()
{
	// CS와 MOSI, SCK를 출력으로 설정합니다.
	DDR_SPI |= (1<<CS) | (1<<MOSI) | (1 << SCK);
	
	// MISO를 Pull-up 레지스터로 활성화 합니다.
	DDR_SPI |= (1<< MISO);
	
	// SPI를 활성화하고 Master로 CLock은 Fosc/128로 설정합니다.
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR1) | (1<<SPR0);
}

uint8_t spiTransfer(uint8_t data)
{
	// SPI로 전송할 데이터를 SPDR(SPI Data Register)에 저장
	SPDR = data;
	
	/*
	SPIF(SPI Interrupt Flag)가 Set될 때까지 기다립ㄴ디ㅏ.
	SPIF가 Set이 되었다면 SPI로 데이터가 전송이 완료된 것입니다.
	*/
	while(!(SPSR & (1<<SPIF)));
	
	// SPDR를 반환합니다.
	return SPDR;
}

/*************************
Power Up Sequence
*************************/
void sdPowerUpSeq()
{
	// SDCard CS Assert
	CS_ENABLE();
	
	// SDCard에 전원이 공급되는데 최소로 필요한 1msec의 Delay 실행
	_delay_ms(1);
	
	/* 동기화 하기 위해 80개의 클럭사이클을 보냅니다.
	   SPI는 직렬 통신이기 때문에 각 클럭 사이클당 1비트의 데이터가 전송됩니다.
	   0xFF(11111111b)는 8비트이기 때문에 10번을 보내면
	   80개의 클럭 사이클을 보내는 것과 같습니다.
	*/
	for(uint8_t i=0; i<10; i++)
		spiTransfer(0xFF);
		
	// SDCard CS Deassert
	CS_DISABLE();
	spiTransfer(0xFF);
}

/*************************
SDCard Command 전송
*************************/
void sdCommand(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	// SDCard에 Command 를 전송합니다.
	// Bit 47 : Start bit 이고 '0'을 보내줍니다.
	// Bit 46 : Transmission bit 이고 '1'을 보내줍니다.
	// 따라서 0x40(01000000b)을 OR 연산해줍니다.
	spiTransfer(cmd | 0x40);
	
	// Argument 를 전송합니다.
	spiTransfer((uint8_t)(arg >> 24));
	spiTransfer((uint8_t)(arg >> 16));
	spiTransfer((uint8_t)(arg >> 8));
	spiTransfer((uint8_t)(arg)); 
	
	// CRC7(Cyclic Redundancy Check)을 전송합니다.
	// 이건 명령 내용이 SDcard에서 올바르게 수신되었는지 확인하는 용도로 사용합니다.
	// SPI모드에서는 몇 가지 명령만 올바른 CRC를 필요로 하고,
	// 필요하지 않은 경우에는 모두 0으로 설정합니다.
	
	// Bit 0 : End bit 이고 '1'을 보내줍니다.
	// 따라서 0x01(00000001b)와 OR연산 해줍니다.
	spiTransfer(crc | 0x01);
}

/*************************
SDCard 초기화 과정
*************************/
/*************************
1단계. CMD0 으로 SDcard Software Reset하기
여기서 SDcard를 SPI모드로 바꿔줄 수 있다.
*************************/

// R1 Response 를 받습니다.
uint8_t sdReadRes1()
{
	uint8_t index = 0, res1;
	
	// 실제 데이터가 수신될 때 까지 폴링을 유지합니다.
	while((res1 = spiTransfer(0xff)) == 0xff)
	{
		index++;
		// 8바이트 동안 수신된 데이터가 없으면 멈춥니다.
		if(index > 8) break;
	}
	
	return res1;
}

// CMD0 커멘드를 보내줍니다!!
uint8_t sdGoIdleState()
{
	// SDCard CS Assert
	spiTransfer(0xff);
	CS_ENABLE();
	spiTransfer(0xff);
	
	// CMD0를 전송합니다.
	sdCommand(CMD0, CMD0_ARG, CMD0_CRC);
	
	// Response를 읽습니다.
	uint8_t res1 = sdReadRes1();
	
	// SDCard CS Deassert
	spiTransfer(0xff);
	CS_DISABLE();
	spiTransfer(0xff);
	
	return res1;
}

/*************************
2단계. CMD8을 보내줍니다.
SDcard의 버전이 1.0인지 2.0 이상인지 확인합니다.
그리고 인식에 문제가 없는지 확인합니다.
*************************/
// R3 혹은 R7 Response 를 받습니다.
void sdReadRes3Res7(uint8_t *res)
{
	// R1 Response 를 읽습니다.
	res[0] = sdReadRes1();
	
	/*
		R1 Response에 Error가 존재하면
		더 이상 진행하지 않고 반환합니다.
	*/
	if(res[0] > 1) return;
	
	/*
		R1 Response에 Error가 없다면
		남은 바이트를 읽습니다.
	*/
	res[1] = spiTransfer(0xff);
	res[2] = spiTransfer(0xff);
	res[3] = spiTransfer(0xff);
	res[4] = spiTransfer(0xff);
}

// CMD8 을 전송하고 응답을 받습니다.
void sdSendIfCond(uint8_t *res)
{
	// SDCard CS Assert
	spiTransfer(0xff);
	CS_ENABLE();
	spiTransfer(0xff);
	
	// CMD8 전송
	sdCommand(CMD8, CMD8_ARG, CMD8_CRC);
	
	// CMD8에 대한 Response를 읽습니다.
	sdReadRes3Res7(res);
	
	// SDCard CS Deassert
	spiTransfer(0xff);
	CS_DISABLE();
	spiTransfer(0xff);
}

/*************************
3단계. CMD58을 보내줍니다.
OCR(Operation Conditions Register)를 확인합니다.
*************************/
// CMD58을 보내줍니다.
void sdReadOcr(uint8_t *res)
{
	// SDCard CS Assert
	spiTransfer(0xff);
	CS_ENABLE();
	spiTransfer(0xff);
	
	// CMD58 전송
	sdCommand(CMD58, CMD58_ARG, CMD58_CRC);
	
	// CMD8에 대한 Response를 읽습니다.
	sdReadRes3Res7(res);
	
	// SDCard CS Deassert
	spiTransfer(0xff);
	CS_DISABLE();
	spiTransfer(0xff);
}

/*************************
4단계. ACMD41을 보내줍니다.
ACMD41(Send Operating Condition)은 SDCard의 초기화 과정을 시작합니다.
주의할 점은, ACMD41과 같이 A로 시작하는 커맨드는 사용하기 전에
CMD55(APP_CMD)를 보내서 다음에 사용하려는 커맨드가 ACMD 라는 것을 SDCard에게 알려야 합니다.
*************************/
// CMD55 
uint8_t sdSendApp()
{
	// SDCard CS Assert
	spiTransfer(0xFF);
	CS_ENABLE();
	spiTransfer(0xFF);

	// CMD55 전송
	sdCommand(CMD55, CMD55_ARG, CMD55_CRC);

	// Response를 읽습니다
	uint8_t r1 = sdReadRes1();

	// SDCard CS Deassert
	spiTransfer(0xFF);
	CS_DISABLE();
	spiTransfer(0xFF);

	return r1;
}

// ACMD41
// ACMD는 사용하기 전에 CMD55를 먼저 보내야 하는것에 유의한다!!
uint8_t sdSendOpCond()
{
	// SDCard CS Assert
	spiTransfer(0xFF);
	CS_ENABLE();
	spiTransfer(0xFF);

	// ACMD41 전송
	sdCommand(ACMD41, ACMD41_ARG, ACMD41_CRC);

	// Response를 읽습니다
	uint8_t r1 = sdReadRes1();

	// SDCard CS Deassert
	spiTransfer(0xFF);
	CS_DISABLE();
	spiTransfer(0xFF);

	return r1;
}

/*************************
5단계. SDcard 초기화
앞의 1~4 단계를 함수로 묶어서 사용한다!
*************************/
uint8_t sdInit()
{
    uint8_t res[5], cmdAttempts = 0;

    sdPowerUpSeq();

    /*
      CMD0을 보내서 SDCard를 IDLE로 만듭니다
      최대 10번 Retry 합니다
    */
    while((res[0] = sdGoIdleState()) != 0x01)
    {
        cmdAttempts++;
        if(cmdAttempts > 10) return SD_ERROR;
    }

    // CMD8을 보냅니다
    sdSendIfCond(res);
    if(res[0] != 0x01)
    {
        return SD_ERROR;
    }

    // Echo Pattern을 확인합니다
    if(res[4] != 0xAA)
    {
        return SD_ERROR;
    }

    // ACMD41을 통해 SDCard 초기화를 시도합니다
    cmdAttempts = 0;
    do
    {
        if(cmdAttempts > 100) return SD_ERROR;

        // ACMD41을 보내기전에 CMD55를 보냅니다
        res[0] = sdSendApp();

        // Response에 Error가 없다면 ACMD41을 보냅니다
        if(res[0] < 2)
        {
            res[0] = sdSendOpCond();
        }

        // 10ms동안 기다립니다
        _delay_ms(10);

        cmdAttempts++;
    }
    while(res[0] != SD_READY);

    // OCR을 읽습니다
    sdReadOcr(res);

    // SDCard가 Ready 상태인지 확인합니다
    if(!(res[1] & 0x80)) return SD_ERROR;

    return SD_SUCCESS;
}

/*************************
SDCard에서 데이터를 읽기
// 본 예시는 블록 길이가 항상 512바이트로 설정되어 있는 경우로 고려합니다.
// SD_BLOCK_LEN 매크로 변수의 값을 바꾸어서 설정할 수 있습니다.
*************************/
// CMD16 : SDCard의 읽기 쓰기 작업은 설정된 길이의 블록으로 수행됩니다.
// CMD16으로 블록 길이를 설정할 수 있습니다.

// CMD17 : 단일 블록 읽기 명령입니다.
/*
	token = 0xFE // 읽기 성공
	token = 0x0X // 데이터 에러
	token = 0xFF // 타임아웃
*/
uint8_t sdReadSingleBlock(uint32_t addr, uint8_t *buf, uint8_t *token)
{
	uint8_t res1, read;
	uint16_t readAttempts;
	
	// Token을 초기화합니다.
	*token = 0xFF;
	
	// SDCard CS Assert
	spiTransfer(0xFF);
	CS_ENABLE();
	spiTransfer(0xFF);
	
	// CMD17 전송
	sdCommand(CMD17, addr, CMD17_CRC);
	
	// Response를 읽습니다.
	res1 = sdReadRes1();
	
	// SDCard에서 Response를 받은 경우
	if(res1 != 0xFF)
	{
		// Response Token을 기다립니다. (Timeout은 100ms 입니다.)
		readAttempts = 0;
		while(++readAttempts != SD_MAX_READ_ATTEMPTS)
			if((read = spiTransfer(0xFF)) != 0xFF) break;
			
		// Response Token이 0xFE(시작 토큰)인 경우
		if(read == 0xFE)
		{
			// 블록을 읽고, buf 배열에 저장합니다.
			for(uint16_t i=0; i<SD_BLOCK_LEN; i++) *buf++ = spiTransfer(0xFF);
			
			// 16비트의 CRC를 읽습니다.
			spiTransfer(0xFF);
			spiTransfer(0xFF);
		}
		
		// Token을 SDCard의 Response로 설정합니다.
		*token = read;
	}
	
	// SDCard CS Deassert
	spiTransfer(0xFF);
	CS_DISABLE();
	spiTransfer(0xFF);
	
	return res1;
}

// CMD24 : 단일 블록만큼 SDCard에 작성하는 명령입니다.
/*
  512바이트 단일 블록을 기록합니다
  token = 0x00 // Busy 타임아웃
  token = 0x05 // 데이터 수락
  token = 0xFF // 응답 타임아웃
*/
uint8_t sdWriteSingleBlock(uint32_t addr, uint8_t *buf, uint8_t *token)
{
    uint8_t res1;
    uint8_t readAttempts, read;

    // Token을 초기화 합니다
    *token = 0xFF;

    // SDCard CS Assert
    spiTransfer(0xFF);
    CS_ENABLE();
    spiTransfer(0xFF);

    // CMD24 전송
    sdCommand(CMD24, addr, CMD24_CRC);

    // Response를 읽습니다
    res1 = sdReadRes1();

    // Response에 오류가 없는 경우
    if(res1 == SD_READY)
    {
        // Start Token을 전송합니다
        spiTransfer(SD_START_TOKEN);

        // Buffer의 내용을 전송합니다
        for(uint16_t i = 0; i < SD_BLOCK_LEN; i++) spiTransfer(buf[i]);

        // Response를 기다립니다 (Timeout = 250ms)
        readAttempts = 0;
        while(++readAttempts != SD_MAX_WRITE_ATTEMPTS)
            if((read = spiTransfer(0xFF)) != 0xFF) { *token = 0xFF; break; }

        // 데이터가 수락되면 실행합니다
        if((read & 0x1F) == 0x05)
        {
            // Token을 '데이터 수락'으로 설정
            *token = 0x05;

            // 쓰기 작업이 끝날때까지 기다립니다 (Timeout = 250ms)
            readAttempts = 0;
            while(spiTransfer(0xFF) == 0x00)
                if(++readAttempts == SD_MAX_WRITE_ATTEMPTS) { *token = 0x00; break; }
        }
    }

    // SDCard CS Deassert
    spiTransfer(0xFF);
    CS_DISABLE();
    spiTransfer(0xFF);

    return res1;
}



/*************************
UART로 출력하기
*************************/
void sdPrintR1(uint8_t res)
{
	if(res & 0b10000000) {
		uartPuts("\tError: MSB = 1\n");
		return;
	}
	if(res == 0) {
		uartPuts("\tCard Ready\n");
		return;
	}
	if(PARAM_ERROR(res))
	uartPuts("\tParameter Error\n");
	if(ADDR_ERROR(res))
	uartPuts("\tAddress Error\n");
	if(ERASE_SEQ_ERROR(res))
	uartPuts("\tErase Sequence Error\n");
	if(CRC_ERROR(res))
	uartPuts("\tCRC Error\n");
	if(ILLEGAL_CMD(res))
	uartPuts("\tIllegal Command\n");
	if(ERASE_RESET(res))
	uartPuts("\tErase Reset Error\n");
	if(IN_IDLE(res))
	uartPuts("\tIn Idle State\n");
}

void sdPrintR7(uint8_t *res)
{
	sdPrintR1(res[0]);

	if(res[0] > 1) return;

	uartPuts("\tCommand Version: ");
	uartPutHex8(CMD_VER(res[1]));
	uartPuts("\n");

	uartPuts("\tVoltage Accepted: ");
	if(VOL_ACC(res[3]) == VOLTAGE_ACC_27_33)
	uartPuts("2.7-3.6V\n");
	else if(VOL_ACC(res[3]) == VOLTAGE_ACC_LOW)
	uartPuts("LOW VOLTAGE\n");
	else if(VOL_ACC(res[3]) == VOLTAGE_ACC_RES1)
	uartPuts("RESERVED\n");
	else if(VOL_ACC(res[3]) == VOLTAGE_ACC_RES2)
	uartPuts("RESERVED\n");
	else
	uartPuts("NOT DEFINED\n");

	uartPuts("\tEcho: ");
	uartPutHex8(res[4]);
	uartPuts("\n");
}

void sdPrintR3(uint8_t *res)
{
	sdPrintR1(res[0]);

	if(res[0] > 1) return;

	uartPuts("\tCard Power Up Status: ");
	if(POWER_UP_STATUS(res[1]))
	{
		uartPuts("READY\n");
		uartPuts("\tCCS Status: ");
		if(CCS_VAL(res[1])){ uartPuts("1\n"); }
		else uartPuts("0\n");
	}
	else
	{
		uartPuts("BUSY\n");
	}

	uartPuts("\tVDD Window: ");
	if(VDD_2728(res[3])) uartPuts("2.7-2.8, ");
	if(VDD_2829(res[2])) uartPuts("2.8-2.9, ");
	if(VDD_2930(res[2])) uartPuts("2.9-3.0, ");
	if(VDD_3031(res[2])) uartPuts("3.0-3.1, ");
	if(VDD_3132(res[2])) uartPuts("3.1-3.2, ");
	if(VDD_3233(res[2])) uartPuts("3.2-3.3, ");
	if(VDD_3334(res[2])) uartPuts("3.3-3.4, ");
	if(VDD_3435(res[2])) uartPuts("3.4-3.5, ");
	if(VDD_3536(res[2])) uartPuts("3.5-3.6");
	uartPuts("\n");
}

void sdPrintDataErrToken(uint8_t token)
{
	if(SD_TOKEN_OOR(token))
	uartPuts("\tData out of range\n");
	if(SD_TOKEN_CECC(token))
	uartPuts("\tCard ECC failed\n");
	if(SD_TOKEN_CC(token))
	uartPuts("\tCC Error\n");
	if(SD_TOKEN_ERROR(token))
	uartPuts("\tError\n");
}

/*************************
main 함수
*************************/
#define F_CPU 16000000UL
#include <util/delay.h>

int main(void)
{
	// Response를 담을 배열을 선언합니다
	uint8_t res[5], sdBufRead[512], sdBufWrite[512], token;
	uint32_t addr = 0x00000200;

	// UART를 초기화 합니다
	const unsigned int baudRate = (F_CPU / 16 / 9600) - 1;
	uartInit(baudRate);

	// SPI를 초기화 합니다
	spiInit();
	
	// SDCard에 안정적인 공급을 위하여 100ms 기다립니다.
	_delay_ms(100);

	// SDCard를 초기화 합니다
	if (sdInit() != SD_SUCCESS)
	{
		uartPuts("Error initializaing SD CARD\n");
		while (1)
		;
	}
	else
	{
		uartPuts("SD Card initialized\n");
		
		/* SDcard Read */
		// Sector 0을 읽습니다
		res[0] = sdReadSingleBlock(addr, sdBufRead, &token);

		// Response를 출력합니다
		if (SD_R1_NO_ERROR(res[0]) && (token == 0xFE))
		{
			for (uint16_t i = 0; i < 512; i++)
			{
				if (i % 32 == 0)
				uartPuts("\n");
				uartPutHex8(sdBufRead[i]);
			}
			uartPuts("\n");
		}
		else
		{
			uartPuts("Error reading sector\n");
		}
		
		
		/* SDcard Write */
		// 0x55로 버퍼를 채웁니다.
		for(uint16_t i=0; i<512; i++) sdBufWrite[i] = 0x55;
		
		// 주소 0x100에 0x55를 기록합니다.
		res[0] = sdWriteSingleBlock(addr, sdBufWrite, &token);
		
		
		uartPuts("\n\nAFTER WRITING TO SDCARD on 0x01 value...\n\n");
		/* SDcard Read */
		// Sector 0을 읽습니다
		res[0] = sdReadSingleBlock(addr, sdBufRead, &token);

		// Response를 출력합니다
		if (SD_R1_NO_ERROR(res[0]) && (token == 0xFE))
		{
			for (uint16_t i = 0; i < 512; i++)
			{
				if (i % 32 == 0)
				uartPuts("\n");
				uartPutHex8(sdBufRead[i]);
			}
			uartPuts("\n");
		}
		else
		{
			uartPuts("Error reading sector\n");
		}

		// 오류 토큰을 수신했을때
		if(!(token & 0xF0))
		{
			uartPuts("Error token:\n");
			sdPrintDataErrToken(token);
		}
		else if(token == 0xFF)
		{
			uartPuts("Timeout\n");
		}
	}
	
	while(1)
		;
}