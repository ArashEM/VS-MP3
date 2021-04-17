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
#include "mountain.c"
#include "owl.c"
#include "main.h"
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
	char message[30];
	
	GUI_SetBkColor(GUI_WHITE);
  GUI_Clear();
  GUI_DrawBitmap(&bmowl, 70, 30);
	GUI_SetColor(GUI_BLACK);
  GUI_SetFont(&GUI_Font16_ASCII);
	GUI_DispStringAt("VS-MP3 player ...", 0, 0);
	sprintf(message,"Build #%s: %s",__DATE__, __TIME__);
	GUI_DispStringAt(message, 120, 0);
}

/*************************** End of file ****************************/
