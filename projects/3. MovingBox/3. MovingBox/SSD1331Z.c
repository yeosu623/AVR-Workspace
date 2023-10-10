// Include files
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include "SSD1331Z.h"

// constants
#define PI 3.141592

// masks
#define colorCmask 0xF800 // 1111 1000 0000 0000
#define colorBmask 0x07E0 // 0000 0111 1110 0000
#define colorAmask 0x001F // 0000 0000 0001 1111

// Commands
#define COMMAND_MODE 0b11011111; // 사용하는 핀에 따라 달라질 수 있음. (현재 핀 : B5)
#define DATA_INPUT_MDOE 0b00100000; // 사용하는 핀에 따라 달라질 수 있음. (현재 핀 : B5)

#define SET_COLUMN_ADDRESS 0x15
#define SET_ROW_ADDRESS 0x75
#define REMAP_AND_COLOR_DEPTH_SETTING 0xA0
#define DISPLAY_OFF 0xAE
#define DISPLAY_ON 0xAF
#define SCROLLING_SETUP 0x27
#define DEACTIVATE_SCROLLING 0x2E
#define ACTIVATE_SCROLLING 0x2F
#define POWER_SAVE_MODE 0xB0
#define DRAW_LINE 0x21
#define DRAWING_RECTANGLE 0x22
#define CLEAR_WINDOW 0x25
#define FILL_ENABLE_DISABLE 0x26
	
// functions
void SSD1331Z_init()
{
	DDRB |= ((1<<PINB4) | (1<<PINB5) | (1<<PINB6));
	// PB4 = Reset        // Reset = 0, Running = 1
	// PB5 = Data/Command // Command = 0, Data = 1
	// PB6 = Chip Select. // Select = 0, Don't Select = 1
	PORTB |= ((1<<PINB4) | (1<<PINB5) | (0<<PINB6));
	
	Setting_TurnOnDisplay();
	Cmd_RemapAndColorDepthSetting_Default();
	Cmd_ClearWindow();
	_delay_ms(10);
}

void Send_Cmd(unsigned char cData)
{
	// simple SPI transmit.
	SPDR = cData;
	while(!(SPSR & (1<<SPIF)));
}

void Setting_CommandMode()
{
	PORTB = PORTB & COMMAND_MODE;
}

void Setting_DataMode()
{
	PORTB = PORTB | DATA_INPUT_MDOE;
}

void Setting_TurnOnDisplay()
{
	Setting_CommandMode();
	Send_Cmd(DISPLAY_ON);
	Setting_DataMode();
}

void Setting_TurnOffDisplay()
{
	Setting_CommandMode();
	Send_Cmd(DISPLAY_OFF);
	Setting_DataMode();
}

void Cmd_SetColumnAddress(uint8_t ColStart, uint8_t ColEnd)
{
	Setting_CommandMode();
	Send_Cmd(SET_COLUMN_ADDRESS);
	Send_Cmd(ColStart);
	Send_Cmd(ColEnd);
	Setting_DataMode();
}

void Cmd_SetRowAddress(uint8_t RowStart, uint8_t RowEnd)
{
	Setting_CommandMode();
	Send_Cmd(SET_ROW_ADDRESS);
	Send_Cmd(RowStart);
	Send_Cmd(RowEnd);
	Setting_DataMode();
}

void Cmd_RemapAndColorDepthSetting(uint8_t colorFormat)
{
	/* colorFormat cmd list
	0 = 256 color format
	1 = 65k color format (default)
	2 = 65k color format2 (see datasheet)
	*/
	uint8_t cmd = 0b01100000; // default cmd
	cmd |= (colorFormat << 6);
	Setting_CommandMode();
	Send_Cmd(REMAP_AND_COLOR_DEPTH_SETTING);
	Setting_DataMode();
}

void Cmd_RemapAndColorDepthSetting_Default()
{
	Setting_CommandMode();
	Send_Cmd(REMAP_AND_COLOR_DEPTH_SETTING);
	Send_Cmd(0b01100000);
	Setting_DataMode();
}

void Cmd_DrawPixel(uint8_t x, uint8_t y, unsigned short sData)
{
	// fill 1 pixel with received color on desired coordination.
	
	// set coordination
	Cmd_SetColumnAddress(x, x);
	Cmd_SetRowAddress(y, y);
	
	// Send data
	SPDR = sData >> 8;
	while(!(SPSR & (1<<SPIF)));
	SPDR = sData;
	while(!(SPSR & (1<<SPIF)));
	
	// reset to default setting
	Cmd_SetColumnAddress(0, 95);
	Cmd_SetRowAddress(0, 63);
}

void Cmd_FillArea(uint8_t ColStart, uint8_t RowStart, uint8_t ColEnd, uint8_t RowEnd, unsigned short sData)
{
	// fill Area with received color on desired area.
	
	// set coordination
	Cmd_SetColumnAddress(ColStart, ColEnd);
	Cmd_SetRowAddress(RowStart, RowEnd);
	
	// Send data
	uint16_t area_pixels = (ColEnd-ColStart+1) * (RowEnd-RowStart+1);
	
	for(int i=0; i<area_pixels; i++)
	{
		SPDR = sData >> 8;
		while(!(SPSR & (1<<SPIF)));
		SPDR = sData;
		while(!(SPSR & (1<<SPIF)));
	}
	
	// reset to default setting
	Cmd_SetColumnAddress(0, 95);
	Cmd_SetRowAddress(0, 63);
}

void Cmd_Fill(unsigned short sData)
{
	// fill whole screen with single color.
	unsigned short i;
	unsigned short pixel_area = 96 * 64;
	unsigned char cData1 = sData >> 8;
	unsigned char cData2 = sData;
	for(i=0; i<pixel_area; i++)
	{
		SPDR = cData1;
		while(!(SPSR & (1<<SPIF)));
		SPDR = cData2;
		while(!(SPSR & (1<<SPIF)));
	}
}

void Cmd_ClearWindow()
{
	Setting_CommandMode();
	Send_Cmd(CLEAR_WINDOW);
	Send_Cmd(0x00); // Column Address of Start
	Send_Cmd(0x00); // Raw Address of Start
	Send_Cmd(0x5F); // Column Address of End
	Send_Cmd(0x3F); // Raw Address of End
	Setting_DataMode();
}

void Cmd_ClearArea(uint8_t ColStart, uint8_t RowStart, uint8_t ColEnd, uint8_t RowEnd)
{
	Setting_CommandMode();
	Send_Cmd(CLEAR_WINDOW);
	Send_Cmd(ColStart);
	Send_Cmd(RowStart);
	Send_Cmd(ColEnd);
	Send_Cmd(RowEnd);
	Setting_DataMode();
}

void Cmd_ScrollingSetup()
{
	Setting_CommandMode();
	Send_Cmd(SCROLLING_SETUP);
	Send_Cmd(0x02); // 1프레임당 스크롤이 수평으로 움직이는 속도.
	Send_Cmd(0x00); // ???
	Send_Cmd(0x01); // ???
	Send_Cmd(0x01); // 1프레임당 스크롤이 수직으로 움직이는 속도.
	Send_Cmd(0x00); // Set time interval. 0~3 level.
	Setting_DataMode();
}

void Cmd_DeactivateScrolling()
{
	Setting_CommandMode();
	Send_Cmd(DEACTIVATE_SCROLLING);
	Setting_DataMode();
}

void Cmd_ActivateScrolling()
{
	Setting_CommandMode();
	Send_Cmd(ACTIVATE_SCROLLING);
	Setting_DataMode();
}

void Cmd_PowerSaveMode(uint8_t swc)
{
	// 1 = Enable power save, 0 = Disable it
	
	Setting_CommandMode();
	Send_Cmd(POWER_SAVE_MODE);
	if(swc == 1) // Enable Power save mode
		Send_Cmd(0x1A);
	else // Disable Power save mode
		Send_Cmd(0x0B);
	Setting_DataMode();
}

void Cmd_DrawLine(uint8_t ColStart, uint8_t RowStart, uint8_t ColEnd, uint8_t RowEnd, unsigned short sData)
{
	uint8_t colorC = (sData & colorCmask) >> 10;
	uint8_t colorB = (sData & colorBmask) >> 5;
	uint8_t colorA = (sData & colorAmask) << 1;
	
	Setting_CommandMode();
	Send_Cmd(DRAW_LINE);
	Send_Cmd(ColStart);
	Send_Cmd(RowStart);
	Send_Cmd(ColEnd);
	Send_Cmd(RowEnd);
	Send_Cmd(colorC);
	Send_Cmd(colorB);
	Send_Cmd(colorA);
	Setting_DataMode();
}

void Cmd_DrawingRectangle_NoFill(uint8_t ColStart, uint8_t RowStart, uint8_t ColEnd, uint8_t RowEnd, unsigned short LineSData)
{
	// Ask to Fill Rectangle in previous
	Cmd_FillEnableDisable(0); // disable
	
	// Drawing Rectangle
	uint8_t LineColorC = (LineSData & colorCmask) >> 10;
	uint8_t LineColorB = (LineSData & colorBmask) >> 5;
	uint8_t LineColorA = (LineSData & colorAmask) << 1;
	
	Setting_CommandMode();
	Send_Cmd(DRAWING_RECTANGLE);
	Send_Cmd(ColStart);
	Send_Cmd(RowStart);
	Send_Cmd(ColEnd);
	Send_Cmd(RowEnd);
	Send_Cmd(LineColorC);
	Send_Cmd(LineColorB);
	Send_Cmd(LineColorA);
	Send_Cmd(0);
	Send_Cmd(0);
	Send_Cmd(0);
	Setting_DataMode();
}

void Cmd_DrawingRectangle_WithFill(uint8_t ColStart, uint8_t RowStart, uint8_t ColEnd, uint8_t RowEnd, unsigned short LineSData, unsigned short FillSData)
{
	// Ask to Fill Rectangle in previous
	Cmd_FillEnableDisable(1); // Enable
	
	// Drawing Rectangle
	uint8_t LineColorC = (LineSData & colorCmask) >> 10;
	uint8_t LineColorB = (LineSData & colorBmask) >> 5;
	uint8_t LineColorA = (LineSData & colorAmask) << 1;
	uint8_t FillColorC = (FillSData & colorCmask) >> 10;
	uint8_t FillColorB = (FillSData & colorBmask) >> 5;
	uint8_t FillColorA = (FillSData & colorAmask) << 1;
	
	Setting_CommandMode();
	Send_Cmd(DRAWING_RECTANGLE);
	Send_Cmd(ColStart);
	Send_Cmd(RowStart);
	Send_Cmd(ColEnd);
	Send_Cmd(RowEnd);
	Send_Cmd(LineColorC);
	Send_Cmd(LineColorB);
	Send_Cmd(LineColorA);
	Send_Cmd(FillColorC);
	Send_Cmd(FillColorB);
	Send_Cmd(FillColorA);
	Setting_DataMode();
}

void Cmd_FillEnableDisable(uint8_t FillEnable)
{
	// FillEnable = 1 : enable fill mode in drawing rectangle.
	// FillEnable = 0 : disable it.
	
	uint8_t cmd = 0x00; // default cmd
	cmd |= (FillEnable << 0);
	
	Setting_CommandMode();
	Send_Cmd(FILL_ENABLE_DISABLE);
	Send_Cmd(cmd);
	Setting_DataMode();
}

void Cmd_DrawingTriangle(uint8_t pointAX, uint8_t pointAY, uint8_t pointBX, uint8_t pointBY, uint8_t pointCX, uint8_t pointCY, unsigned short sData)
{
	Cmd_DrawLine(pointAX, pointAY, pointBX, pointBY, sData);
	Cmd_DrawLine(pointBX, pointBY, pointCX, pointCY, sData);
	Cmd_DrawLine(pointCX, pointCY, pointAX, pointAY, sData);
}

long rounding(float f)
{
	return (long)(f + 0.5);
}

void Cmd_DrawingCircle(uint8_t pointX, uint8_t pointY, uint8_t radius, unsigned short sData)
{
	short i, j;
	short gap_x, gap_y;
	short gap_distance;
	for(i=0; i<64; i++)
	{
		for(j=0; j<96; j++)
		{
			 gap_x = pointX - j;
			 gap_y = pointY - i;
			 gap_distance = rounding(sqrt(gap_x*gap_x + gap_y*gap_y));
			 if(gap_distance == radius) Cmd_DrawPixel(j, i, sData);
		}
	}
}

void Cmd_RotatingLine(uint8_t centerX, uint8_t centerY, float yaw_angle, unsigned short sData)
{	
	// 기울어진 각도에 따라 선을 그려주는 함수입니다.
	// 변화하는 각도를 주기적으로 넣어주면 멋진 애니메이션 효과를 볼 수 있습니다.
	
	Cmd_ClearWindow(); // 먼저, 화면을 초기화한다.
	_delay_ms(1); // 화면 초기화 후의 여유 시간.

	float slope = tan(yaw_angle * PI / 180); // 각도를 기울기로 변환 (예 : 45도 -> 1)
	if((rounding(yaw_angle) % 90 == 0) && (rounding(yaw_angle) % 180 != 0)) slope = 100; // tan(90)을 계산할 때 기울기가 무제한이 되는것을 방지.
	
	// 지금부터, 선의 양 끝의 두 점을 구한다.
	// 먼저, x를 1씩 증가시키면서 y를 기울기의 배율만큼 증가시킨다.
	// 그러면 x가 오른쪽 경계를 넘어가거나 y가 화면을 벗어나게 된다. 그때가 선의 끝부분이 된다.
	short edgeX1 = centerX;
	float edgeY1_f = centerY;
	long edgeY1;
	
	while(1)
	{
		if(edgeX1 >= 95)
		{
			edgeX1 = 95;
			break;
		}

		edgeY1_f += slope;
		edgeY1 = rounding(edgeY1_f);
		if(edgeY1 >= 63)
		{
			edgeY1 = 63;
			break;
		}
		else if(edgeY1 <= 0)
		{
			edgeY1 = 0;
			break;
		}
		
		// 마지막에 X1을 증가시킨다.
		edgeX1++;
	}
	
	// 그 다음, x를 1씩 감소시키면서 y를 기울기의 배율만큼 증가시킨다.
	// 그러면 x가 왼쪽 경계를 넘어가거나 y가 화면을 벗어나게 된다. 그때가 선의 끝부분이 된다.
	short edgeX2 = centerX;
	float edgeY2_f = centerY;
	long edgeY2;
	
	while(1)
	{
		if(edgeX2 <= 0)
		{
			edgeX2 = 0;
			break;
		}

		edgeY2_f -= slope;
		edgeY2 = rounding(edgeY2_f);
		if(edgeY2 >= 63)
		{
			edgeY2 = 63;
			break;
		}
		else if(edgeY2 <= 0)
		{
			edgeY2 = 0;
			break;
		}
		
		// 마지막에 X2를 감소시킨다.
		edgeX2--;
	}
	
	Cmd_DrawLine(edgeX1, edgeY1, edgeX2, edgeY2, sData); // 마지막으로 선을 그린다.
}

float coordX = 10; // initial value for MovingSquare() function.
float coordY = 10; // initial value for MovingSquare() function.
float speed_alpha = 2; // 가중치
void Cmd_MovingSquare(float x_angle, float y_angle, uint8_t size, unsigned short sData)
{
	// roll, pitch 각도에 따라서 상자를 움직이는 애니메이션을 표시하는 함수입니다.
	// main 함수에서 "초당 100회"의 주기를 가지는 반복문을 활용하여 함수를 활성화 할 수 있습니다. (roll, pitch 값을 계속 넣어주어야 하기 때문에.)
	
	// 화면 초기화
	Cmd_ClearArea((char)coordX-size, (char)coordY-size, (char)coordX+size, (char)coordY+size);
	
	// 상자 좌표 계산
	coordX += (x_angle / 100.0 * speed_alpha);
	if(coordX > 95-size) coordX = 95-size;
	else if(coordX < size) coordX = size;
	
	coordY += (-y_angle / 100.0 * speed_alpha);
	if(coordY > 63-size) coordY = 63-size;
	else if(coordY < size) coordY = size;
	
	// 움직인 상자 표시
	Cmd_DrawingRectangle_NoFill((char)coordX-size, (char)coordY-size, (char)coordX+size, (char)coordY+size, sData);
}