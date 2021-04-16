#ifndef _ILI9486_H_
#define _ILI9486_H_

#include <stdint.h>

void LCD_WR_DATA8(uint8_t da);
void LCD_WR_MDATA8(uint8_t *da, uint16_t num); 
void LCD_WR_DATA(uint32_t da);
void LCD_WR_REG(uint8_t da);
void LCD_WR_REG_DATA(uint32_t reg,uint32_t da);
void ili9486_init(void);
void LCD_Clear(uint16_t Color);
void LCD_RD_REG(uint8_t reg, uint8_t *data);

#endif  // _ILI9486_H_
