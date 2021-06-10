/*****************************************************************************
  * @file    debug.h
  * @author  Arash Golgol
  * @brief   debug utilities for mp3 player
*****************************************************************************/
#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include "stm32f1xx_hal_gpio.h"

/*debug macro */
#if defined( DEBUG ) 
	#define debug_print(fmt, args...) 	printf("DEBUG: %s:%d:%s(): " fmt, \
    __FILE__, __LINE__, __func__, ##args)
#else
	#define debug_print(fmt, args...) /* Don't do anything in release builds */
#endif

/**
 *	@brief 	PIN is turned on during execution of FUNC
 */
#define MEASURE_EXEC_TIME(FUNC, PORT, PIN) 			\
do {																						\
	HAL_GPIO_WritePin(PORT,PIN, GPIO_PIN_SET);		\
	FUNC; 																				\
	HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_RESET);	\
} while(0)


#endif
