#include "ili9340.h"
#include "spi.h"
#include "gpio.h"

uint32_t	addr_col, addr_row;

void writecommand(uint8_t c)
{
  HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_RESET);	/*select command */
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); /* select LCD */
  HAL_SPI_Transmit(&hspi2, &c, 1, 0xFF);
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); /* deselect LCD */
  HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET); /*select data */
}

void writedata(uint8_t c)
{
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); /* select LCD */
  HAL_SPI_Transmit(&hspi2, &c, 1, 0xFF);
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); /* deselect LCD */
}

void writeMdata(uint8_t *c, uint32_t num)
{
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); /* select LCD */
  HAL_SPI_Transmit(&hspi2, c, num, 0xFF);
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); /* deselect LCD */
}

void ili9340_init(void)
{
	/* power on LCD supply */
	HAL_GPIO_WritePin(PWR_EXT_GPIO_Port, PWR_EXT_Pin, GPIO_PIN_RESET);
	/* Wait for LCD to startup */
	HAL_Delay(500);
	// reset LCD controller 
	HAL_GPIO_WritePin(LCD_RSTN_GPIO_Port, LCD_RSTN_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LCD_RSTN_GPIO_Port, LCD_RSTN_Pin, GPIO_PIN_SET);
	
	writecommand(0x01); // Software reset
	HAL_Delay(5);
	
	writecommand(0xEF);
  writedata(0x03);
  writedata(0x80);
  writedata(0x02);

  writecommand(0xCF);
  writedata(0x00);
  writedata(0XC1);
  writedata(0X30);

  writecommand(0xED);
  writedata(0x64);
  writedata(0x03);
  writedata(0X12);
  writedata(0X81);

  writecommand(0xE8);
  writedata(0x85);
  writedata(0x00);
  writedata(0x78);

  writecommand(0xCB);
  writedata(0x39);
  writedata(0x2C);
  writedata(0x00);
  writedata(0x34);
  writedata(0x02);

  writecommand(0xF7);
  writedata(0x20);

  writecommand(0xEA);
  writedata(0x00);
  writedata(0x00);

  writecommand(ILI9341_PWCTR1);    //Power control
  writedata(0x23);   //VRH[5:0]

  writecommand(ILI9341_PWCTR2);    //Power control
  writedata(0x10);   //SAP[2:0];BT[3:0]

  writecommand(ILI9341_VMCTR1);    //VCM control
  writedata(0x3e);
  writedata(0x28);

  writecommand(ILI9341_VMCTR2);    //VCM control2
  writedata(0x86);  //--

  writecommand(ILI9341_MADCTL);    // Memory Access Control
  writedata(0x48);

  writecommand(ILI9341_PIXFMT);
  writedata(0x55);

  writecommand(ILI9341_FRMCTR1);
  writedata(0x00);
  writedata(0x13); // 0x18 79Hz, 0x1B default 70Hz, 0x13 100Hz

  writecommand(ILI9341_DFUNCTR);    // Display Function Control
  writedata(0x08);
  writedata(0x82);
  writedata(0x27);

  writecommand(0xF2);    						// 3Gamma Function Disable
  writedata(0x00);

  writecommand(ILI9341_GAMMASET);    //Gamma curve selected
  writedata(0x01);

  writecommand(ILI9341_GMCTRP1);    //Set Gamma
  writedata(0x0F);
  writedata(0x31);
  writedata(0x2B);
  writedata(0x0C);
  writedata(0x0E);
  writedata(0x08);
  writedata(0x4E);
  writedata(0xF1);
  writedata(0x37);
  writedata(0x07);
  writedata(0x10);
  writedata(0x03);
  writedata(0x0E);
  writedata(0x09);
  writedata(0x00);

  writecommand(ILI9341_GMCTRN1);    //Set Gamma
  writedata(0x00);
  writedata(0x0E);
  writedata(0x14);
  writedata(0x03);
  writedata(0x11);
  writedata(0x07);
  writedata(0x31);
  writedata(0xC1);
  writedata(0x48);
  writedata(0x08);
  writedata(0x0F);
  writedata(0x0C);
  writedata(0x31);
  writedata(0x36);
  writedata(0x0F);

  writecommand(ILI9341_SLPOUT);    //Exit Sleep
  HAL_Delay(120);
  writecommand(ILI9341_DISPON);    //Display on
} 

void tft_drawPixel(uint32_t x, uint32_t y, uint32_t color)
{
	uint16_t	temp;
	uint8_t		cmd;
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); 		/* select LCD */
  // No need to send x if it has not changed (speeds things up)
  if (addr_col != x) {
    HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_RESET);	/*select command */
		cmd = ILI9341_CASET;
    HAL_SPI_Transmit(&hspi2, &cmd , sizeof(cmd) , 0xff);
		HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET); 		/*select data */
		
		temp = (x >> 8) | (x << 8);
		HAL_SPI_Transmit(&hspi2, (uint8_t *) &temp, sizeof(temp), 0xff);
		HAL_SPI_Transmit(&hspi2, (uint8_t *) &temp, sizeof(temp), 0xff);
    
		addr_col = x;
  }

  // No need to send y if it has not changed (speeds things up)
  if (addr_row != y) {

		HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_RESET);	/*select command */
		cmd = ILI9341_PASET;
    HAL_SPI_Transmit(&hspi2, &cmd , sizeof(cmd) , 0xff);
		HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET); 		/*select data */;

		temp = (y >> 8) | (y << 8);
		HAL_SPI_Transmit(&hspi2, (uint8_t *) &temp, sizeof(temp), 0xff);
		HAL_SPI_Transmit(&hspi2, (uint8_t *) &temp, sizeof(temp), 0xff);
		
    addr_row = y;
  }

		HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_RESET);	/*select command */
		cmd = ILI9341_RAMWR;
    HAL_SPI_Transmit(&hspi2, &cmd , sizeof(cmd) , 0xff);
		HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET); 		/*select data */;

		temp = (color >> 8) | (color << 8);
		HAL_SPI_Transmit(&hspi2, (uint8_t *) &temp, sizeof(temp), 0xff);
	
		HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); 		/* deselect LCD */
}

void tft_drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color)
{
  int32_t  x  = 0;
  int32_t  dx = 1;
  int32_t  dy = r+r;
  int32_t  p  = -(r>>1);

  // These are ordered to minimise coordinate changes in x or y
  // tft_drawPixel can then send fewer bounding box commands
  tft_drawPixel(x0 + r, y0, color);
  tft_drawPixel(x0 - r, y0, color);
  tft_drawPixel(x0, y0 - r, color);
  tft_drawPixel(x0, y0 + r, color);

  while(x<r){

    if(p>=0) {
      dy-=2;
      p-=dy;
      r--;
    }

    dx+=2;
    p+=dx;

    x++;

    // These are ordered to minimise coordinate changes in x or y
    // tft_drawPixel can then send fewer bounding box commands
    tft_drawPixel(x0 + x, y0 + r, color);
    tft_drawPixel(x0 - x, y0 + r, color);
    tft_drawPixel(x0 - x, y0 - r, color);
    tft_drawPixel(x0 + x, y0 - r, color);

    tft_drawPixel(x0 + r, y0 + x, color);
    tft_drawPixel(x0 - r, y0 + x, color);
    tft_drawPixel(x0 - r, y0 - x, color);
    tft_drawPixel(x0 + r, y0 - x, color);
    }
}
