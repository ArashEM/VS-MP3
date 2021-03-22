#include "ili9340.h"
#include "spi.h"
#include "gpio.h"

void ili9486_init(void)
{
	//************* Start Initial Sequence **********// 
writecommand(0XF2);
writedata(0x18);
writedata(0xA3);
writedata(0x12);
writedata(0x02);
writedata(0XB2);
writedata(0x12);
writedata(0xFF);
writedata(0x10);
writedata(0x00);
writecommand(0XF8);
writedata(0x21);
writedata(0x04);
writecommand(0X13);

writecommand(0x36);
writedata(0x08);
writecommand(0xB4);
writedata(0x02);
writecommand(0xB6);
writedata(0x02);
writedata(0x22);
writecommand(0xC1);
writedata(0x41);
writecommand(0xC5);
writedata(0x00);
writedata(0x18);
writecommand(0x3a);
writedata(0x66);
HAL_Delay(50);
writecommand(0xE0);
writedata(0x0F);
writedata(0x1F);
writedata(0x1C);
writedata(0x0C);
writedata(0x0F);
writedata(0x08);
writedata(0x48);
writedata(0x98);
writedata(0x37);
writedata(0x0A);
writedata(0x13);
writedata(0x04);
writedata(0x11);
writedata(0x0D);
writedata(0x00);
writecommand(0xE1);
writedata(0x0F);
writedata(0x32);
writedata(0x2E);
writedata(0x0B);
writedata(0x0D);
writedata(0x05);
writedata(0x47);
writedata(0x75);
writedata(0x37);
writedata(0x06);
writedata(0x10);
writedata(0x03);
writedata(0x24);
writedata(0x20);
writedata(0x00);
writecommand(0x11);
HAL_Delay(120);
writecommand(0x29);
writecommand(0x2C);
}
