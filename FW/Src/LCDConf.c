/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.44 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : Display driver configuration file
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @file    LCDConf_stm3210c_eval.c
  * @author  MCD Application Team
  * @brief   Driver for STM3210C-EVAL RevC board LCD
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics. 
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license SLA0044,
  * the "License"; You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *                      http://www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "GUI.h"
#include "GUIDRV_FlexColor.h"
#include "main.h"
#include "ili9340.h"
#include "ili9486.h"

/*********************************************************************
*
*       Layer configuration (to be modified)
*
**********************************************************************
*/

//
// Physical display size
//
#if   defined ( LCD_ILI9340 )
	#define XSIZE_PHYS  240
	#define YSIZE_PHYS  320
#elif defined ( LCD_ILI9486 ) 
  #define XSIZE_PHYS  320
	#define YSIZE_PHYS  480
#else
  #error No LCD controller is defined
#endif 

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VXSIZE_PHYS
#define VXSIZE_PHYS XSIZE_PHYS
#endif
#ifndef   VYSIZE_PHYS
#define VYSIZE_PHYS YSIZE_PHYS
#endif
#ifndef   XSIZE_PHYS
#error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
#error Physical Y size of display is not defined!
#endif
#ifndef   GUICC_565
#error Color conversion not defined!
#endif
#ifndef   GUIDRV_FLEXCOLOR
#error No display driver defined!
#endif

/*********************************************************************
*
*       Defines, sfrs
*
**********************************************************************
*/
//
// COG interface register addr.
//

typedef struct
{
  __IO uint16_t REG;
  __IO uint16_t RAM;

} LCD_CONTROLLER_TypeDef;

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/
static void LCD_LL_Init(void);


/********************************************************************
*
*       LcdWriteReg
*
* Function description:
*   Sets display register
*/
static void LcdWriteReg(U8 Data)
{
#if   defined ( LCD_ILI9340 )
	writecommand((uint8_t)Data);
#elif defined ( LCD_ILI9486 )
  LCD_WR_REG((U8) Data);
#else
  #error No LCD controller is defined
#endif 	
}

/********************************************************************
*
*       LcdWriteData
*
* Function description:
*   Writes a value to a display register
*/
static void LcdWriteData(U8 Data)
{
#if   defined ( LCD_ILI9340 )
	writedata((uint8_t)Data);
#elif defined ( LCD_ILI9486 )
  LCD_WR_DATA8((U8) Data);
#else
  #error No LCD controller is defined
#endif 	
}

/********************************************************************
*
*       LcdWriteDataMultiple
*
* Function description:
*   Writes multiple values to a display register.
*/
static void LcdWriteDataMultiple(U8 *pData, int NumItems)
{
 #if   defined ( LCD_ILI9340 )
	writeMdata((uint8_t *) pData, NumItems);
#elif defined ( LCD_ILI9486 )
  LCD_WR_MDATA8((U8 *) pData, NumItems);
#else
  #error No LCD controller is defined
#endif 	  
	  
}

/********************************************************************
*
*       LcdReadDataMultiple
*
* Function description:
*   Reads multiple values from a display register.
*/
static void LcdReadDataMultiple(U8 *pData, int NumItems)
{
  while (NumItems--)
  {
    //*pData++ = LCD_IO_ReadData();
    while (1);
  }
}

//static U8 LcdReadData(void)
//{
//	return 0;
//}

/*********************************************************************
*
*       Public functions
*
**********************************************************************
*/

/**
  * @brief  Initializes the LCD.
  * @param  None
  * @retval LCD state
  */
static void LCD_LL_Init(void)
{	
#if   defined ( LCD_ILI9340 )
	ili9340_init();
#elif defined ( LCD_ILI9486 )
  ili9486_init();
#else
  #error No LCD controller is defined
#endif 
}

/*********************************************************************
*
*       LCD_X_Config
*
* Function description:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/
void LCD_X_Config(void)
{
  GUI_DEVICE *pDevice;
  CONFIG_FLEXCOLOR Config = {0};
  GUI_PORT_API PortAPI = {0};
  //
  // Set display driver and color conversion
  //
  pDevice = GUI_DEVICE_CreateAndLink(GUIDRV_FLEXCOLOR, GUICC_565, 0, 0);
  //
  // Display driver configuration, required for Lin-driver
  //
  LCD_SetSizeEx(0, XSIZE_PHYS , YSIZE_PHYS);
  LCD_SetVSizeEx(0, VXSIZE_PHYS, VYSIZE_PHYS);
  //
  // Orientation
  //
#if   defined ( LCD_ILI9340 )
	Config.Orientation =  GUI_SWAP_XY ;
#elif defined ( LCD_ILI9486 )
  Config.Orientation =  GUI_SWAP_XY ;
#else
  #error No LCD controller is defined
#endif 
    


  GUIDRV_FlexColor_Config(pDevice, &Config);
  //
  // Set controller and operation mode
  //
  PortAPI.pfWrite8_A0  = LcdWriteReg;
  PortAPI.pfWrite8_A1  = LcdWriteData;
  PortAPI.pfWriteM8_A1 = LcdWriteDataMultiple;
	//PortAPI.pfRead8_A1  =  LcdReadData;
  PortAPI.pfReadM8_A1  = LcdReadDataMultiple;

  // Find the current LCD and initialize GUIDRV
  //if (ili9320_drv.ReadID() == ILI9320_ID)
  //{
  //  GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66708, GUIDRV_FLEXCOLOR_M16C0B16);
  //}
  //else
  //{
   // GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66709, GUIDRV_FLEXCOLOR_M16C0B16);
	 GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66709, GUIDRV_FLEXCOLOR_M16C0B8);
  //}
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Function description:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void *pData)
{
  int r;
  (void) LayerIndex;
  (void) pData;

  switch (Cmd)
  {
    case LCD_X_INITCONTROLLER:
    {

      LCD_LL_Init();

      return 0;
    }
    default:
      r = -1;
  }
  return r;
}


/*************************** End of file ****************************/

