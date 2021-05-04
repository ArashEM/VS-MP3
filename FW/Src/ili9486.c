#include "ili9486.h"
#include "spi.h"
#include "gpio.h"
#include <stdint.h>

#define LCD_W 320
#define LCD_H 480
uint16_t	POINT_COLOR = 0x0000;

void LCD_Writ_Bus(uint8_t dat)   
{	
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&dat, 1, 0xFF);
}

void LCD_WR_DATA8(uint8_t da) 
{	
	//OLED_CS_Clr();
  HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET); /*select data */
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); /* select LCD */
	LCD_Writ_Bus(da);  
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); /* deselect LCD */
	//OLED_CS_Set();
}  

void LCD_WR_MDATA8(uint8_t *da, uint16_t num) 
{	
	//OLED_CS_Clr();
  HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET); /*select data */
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); /* select LCD */
	HAL_SPI_Transmit(&hspi2, da, num, 0xFF);  
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); /* deselect LCD */
	//OLED_CS_Set();
}  


void LCD_WR_DATA(uint32_t da)
{//	OLED_CS_Clr();
   
uint8_t temp1,temp2,temp3;
	uint32_t tt1,tt2,tt3;
	tt1=da;
	tt2=da;
	tt3=da;
	temp1=tt1>>16;
	temp2=tt2>>8;
	temp3=tt3;
  HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET); /*select data */
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); /* select LCD */
	LCD_Writ_Bus(temp1);
	LCD_Writ_Bus(temp2);
  LCD_Writ_Bus(temp3);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); /* deselect LCD */
	//OLED_CS_Set();
}	  

void LCD_WR_REG(uint8_t da)	 
{	//	OLED_CS_Clr();
  HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_RESET);	/*select command */
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); /* select LCD */
	LCD_Writ_Bus(da);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); /* deselect LCD */
	//OLED_CS_Set();
}
 void LCD_WR_REG_DATA(uint32_t reg,uint32_t da)
{	//OLED_CS_Clr();
  LCD_WR_REG(reg);
	LCD_WR_DATA(da);
	//OLED_CS_Set();
}

void LCD_RD_REG(uint8_t reg, uint8_t *data)
{
	HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_RESET);	/*select command */
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); /* select LCD */
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&reg, 1, 0xFF);
	HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET); /*select data */
	HAL_SPI_Receive(&hspi2, data, 4, 0xFF);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); /* deselect LCD */
}

void Address_set(uint32_t x1,uint32_t y1,uint32_t x2,uint32_t y2)
{ 
	LCD_WR_REG(0x2a);
   LCD_WR_DATA8(x1>>8);
   LCD_WR_DATA8(x1);
   LCD_WR_DATA8(x2>>8);
   LCD_WR_DATA8(x2);
  
   LCD_WR_REG(0x2b);
   LCD_WR_DATA8(y1>>8);
   LCD_WR_DATA8(y1);
   LCD_WR_DATA8(y2>>8);
   LCD_WR_DATA8(y2);

   LCD_WR_REG(0x2C);					 						 
}

void ili9486_init(void)
{
	/* power on LCD supply */
	HAL_GPIO_WritePin(PWR_EXT_GPIO_Port, PWR_EXT_Pin, GPIO_PIN_RESET);
	
	/* power of Backlight */
	HAL_GPIO_WritePin(BL_PWM_GPIO_Port,BL_PWM_Pin, GPIO_PIN_SET);
	
	/* Wait for LCD to startup */
	HAL_Delay(500);
	
	// reset LCD controller 
	HAL_GPIO_WritePin(LCD_RSTN_GPIO_Port, LCD_RSTN_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(LCD_RSTN_GPIO_Port, LCD_RSTN_Pin, GPIO_PIN_SET);
	
//************* Start Initial Sequence **********// 
LCD_WR_REG(0XF2);
LCD_WR_DATA8(0x18);
LCD_WR_DATA8(0xA3);
LCD_WR_DATA8(0x12);
LCD_WR_DATA8(0x02);
LCD_WR_DATA8(0XB2);
LCD_WR_DATA8(0x12);
LCD_WR_DATA8(0xFF);
LCD_WR_DATA8(0x10);
LCD_WR_DATA8(0x00);
LCD_WR_REG(0XF8);
LCD_WR_DATA8(0x21);
LCD_WR_DATA8(0x04);
LCD_WR_REG(0X13);		//Normal Display Mode ON
LCD_WR_REG(0x36);		//Memory Access Control
LCD_WR_DATA8(0x48);		
LCD_WR_REG(0xB4);		//Display Inversion Control
LCD_WR_DATA8(0x02);
LCD_WR_REG(0xB6);		//Display Function Control
LCD_WR_DATA8(0x02);
LCD_WR_DATA8(0x22);
LCD_WR_DATA8(0x3b);
LCD_WR_REG(0xC1);		//Power Control 2
LCD_WR_DATA8(0x41);
LCD_WR_REG(0xC5);		//VCOM Control 1
LCD_WR_DATA8(0x00);
LCD_WR_DATA8(0x18);
LCD_WR_REG(0x3a);		// Interface Pixel Format
LCD_WR_DATA8(0x66);		
HAL_Delay(150);
LCD_WR_REG(0xE0);		// PGAMCTRL （Positive Gamma Control）
LCD_WR_DATA8(0x0F);
LCD_WR_DATA8(0x1F);
LCD_WR_DATA8(0x1C);
LCD_WR_DATA8(0x0C);
LCD_WR_DATA8(0x0F);
LCD_WR_DATA8(0x08);
LCD_WR_DATA8(0x48);
LCD_WR_DATA8(0x98);
LCD_WR_DATA8(0x37);
LCD_WR_DATA8(0x0A);
LCD_WR_DATA8(0x13);
LCD_WR_DATA8(0x04);
LCD_WR_DATA8(0x11);
LCD_WR_DATA8(0x0D);
LCD_WR_DATA8(0x00);
LCD_WR_REG(0xE1);		// NGAMCTRL（Negative Gamma Control）
LCD_WR_DATA8(0x0F);
LCD_WR_DATA8(0x32);
LCD_WR_DATA8(0x2E);
LCD_WR_DATA8(0x0B);
LCD_WR_DATA8(0x0D);
LCD_WR_DATA8(0x05);
LCD_WR_DATA8(0x47);
LCD_WR_DATA8(0x75);
LCD_WR_DATA8(0x37);
LCD_WR_DATA8(0x06);
LCD_WR_DATA8(0x10);
LCD_WR_DATA8(0x03);
LCD_WR_DATA8(0x24);
LCD_WR_DATA8(0x20);
LCD_WR_DATA8(0x00);
LCD_WR_REG(0x11);			// Sleep OUT
HAL_Delay(120);
LCD_WR_REG(0x29);			// Display ON
} 
	void LCD_Clear(uint16_t Color)
{
	uint16_t i,j;  	
	Address_set(0,0,LCD_W-1,LCD_H-1);
    for(i=0;i<LCD_W;i++)
	 {
	  for (j=0;j<LCD_H;j++)
	   	{
        	LCD_WR_DATA(Color);	 			 
	    }

	  }
}


void LCD_DrawPoint(uint16_t x,uint16_t y)
{
	Address_set(x,y,x,y);
	LCD_WR_DATA(POINT_COLOR); 	    
} 	 

void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{          
	uint16_t i,j; 
	Address_set(xsta,ysta,xend,yend);       
	for(i=ysta;i<=yend;i++)
	{													   	 	
		for(j=xsta;j<=xend;j++)LCD_WR_DATA(color); 	    
	} 					  	    
}  

void LCD_DrawPoint_big(uint16_t x,uint16_t y)
{
	LCD_Fill(x-1,y-1,x+1,y+1,POINT_COLOR);
} 

void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //?????? 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //?????? 
	else if(delta_x==0)incx=0;//??? 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//??? 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //????????? 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//???? 
	{  
		LCD_DrawPoint(uRow,uCol);//?? 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    

void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}


void Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //??????????
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a);             //3           
		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0-a,y0+b);             //1       
		LCD_DrawPoint(x0-b,y0-a);             //7           
		LCD_DrawPoint(x0-a,y0-b);             //2             
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0-b);             //5
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-b,y0+a);             
		a++;
		//??Bresenham????     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		LCD_DrawPoint(x0+a,y0+b);
	}
} 
