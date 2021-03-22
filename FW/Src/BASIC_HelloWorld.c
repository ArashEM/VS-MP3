/*********************************************************************
*                SEGGER MICROCONTROLLER SYSTEME GmbH                 *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2004  SEGGER Microcontroller Systeme GmbH        *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

***** emWin - Graphical user interface for embedded applications *****
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : BASIC_HelloWorld.c
Purpose     : Simple demo drawing "Hello world"
----------------------------------------------------------------------
*/

#include "GUI.h"
#include <stdio.h>
#include "moon.c"
/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       MainTask
*/

void show_banner(void) {
	int xPos, yPos;
	char message[30];
	
	
  GUI_Clear();
	/* Draw the fixed background for all the animations */
  GUI_DrawBitmap(&bmmoon, 0, 0);
	xPos = LCD_GetXSize() / 2;
	yPos = LCD_GetYSize() / 5;
  GUI_SetFont(&GUI_Font24_ASCII);
	GUI_DispStringHCenterAt("VS-MP3 player ...", xPos, yPos);
	
	sprintf(message,"Build #%s: %s",__DATE__, __TIME__);
	yPos = LCD_GetYSize() / 3;
	GUI_DispStringHCenterAt(message, xPos, yPos);
}

/*************************** End of file ****************************/
