#ifndef SSD1331Z_H_
#define SSD1331Z_H_

void SSD1331Z_init();
void Setting_TurnOnDisplay();
void Setting_TurnOffDisplay();
void Cmd_SetColumnAddress(uint8_t ColStart, uint8_t ColEnd);
void Cmd_SetRowAddress(uint8_t RowStart, uint8_t RowEnd);
void Cmd_RemapAndColorDepthSetting(uint8_t colorFormat);
void Cmd_RemapAndColorDepthSetting_Default();
void Cmd_DrawPixel(uint8_t x, uint8_t y, unsigned short sData);
void Cmd_Fill(unsigned short sData);
void Cmd_FillArea(uint8_t ColStart, uint8_t RowStart, uint8_t ColEnd, uint8_t RowEnd, unsigned short sData);
void Cmd_ClearWindow();
void Cmd_ClearArea(uint8_t ColStart, uint8_t RowStart, uint8_t ColEnd, uint8_t RowEnd);
void Cmd_ScrollingSetup();
void Cmd_DeactivateScrolling();
void Cmd_ActivateScrolling();
void Cmd_PowerSaveMode(uint8_t swc);
void Cmd_DrawLine(uint8_t ColStart, uint8_t RowStart, uint8_t ColEnd, uint8_t RowEnd, unsigned short sData);
void Cmd_DrawingRectangle_NoFill(uint8_t ColStart, uint8_t RowStart, uint8_t ColEnd, uint8_t RowEnd, unsigned short LineSData);
void Cmd_DrawingRectangle_WithFill(uint8_t ColStart, uint8_t RowStart, uint8_t ColEnd, uint8_t RowEnd, unsigned short LineSData, unsigned short FillSData);
void Cmd_DrawingTriangle(uint8_t pointAX, uint8_t pointAY, uint8_t pointBX, uint8_t pointBY, uint8_t pointCX, uint8_t pointCY, unsigned short sData);
long rounding(float f);
void Cmd_DrawingCircle(uint8_t pointX, uint8_t pointY, uint8_t radius, unsigned short sData);
void Cmd_RotatingLine(uint8_t centerX, uint8_t centerY, float yaw_angle, unsigned short sData);
void Cmd_MovingSquare(float roll_angle, float pitch_angle, uint8_t size, unsigned short sData);

#endif /* SSD1331Z_H_ */