# Introduction

This folder contains SCH/PCB files for VS-MP3 project
- `mian`: includes Altium project for main board.
- `dtr`: includes Altium project for LCD/Touch board.

# HW information 

This section includes all information you need about HW platform.  
Like pin assignment, crystal frequency and ...

## General 
- Used micro-controller: `STM32F103RCT6`
  - `HSE` crystal frequency: **8 Mhz**
  - `LSE` crystal frequency: **32.768 Khz**
- LCD controller: `ili9340` / `ili9486`
- Touch screen controller: `xpt2046`
- `VS1063` crystal frequency: **12.288 Mhz**


## Pin assignment

Pin names are according to `STM32F103xC, STM32F103xD, STM32F103xE` datasheet, **Table 5**
- `Pin name`: came from micro-controller datasheet
- `Alias in Code`: In order to access a pin via `STM32 HAL GPIO API` you need `PORT` and `PIN`
  - `PORT` is the name in `Alias in Code` column appended to `_GPIO_Port`. (e.g. if Alias is `LED`, it's **port** name will be `LED_GPIO_Port`)
  - `PIN` is name in `Alias in Code` column appended to `_Pin`. (e.g. if Alias is `LED`, it's **pin** name will be `LED_Pin`)
  - As full example, we can toggle LED via `HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);`
- `Peripheral`: which peripheral is used via this pin 
- `Direction`: Pin direction and driver/input type
  - `OUT/PP`: push-pull output driver
  - `IN/PP`: pulled-up input
  - `IN/A`: analog input
  - `OUT/A`: analog output
- `Description`: Some note about this pin (Active high/low, ...)

| Pin name  | Alias in Code | Peripheral 		| Direction | Description 											|
| --------- | --------------| ------------------| ----------| ------------------------------------------------------|
| `PC13`    | `CHG`			| `GPIO`			| `IN/PP`	| `CHG` pin of `MAX1555`   								|
| `PC0`     | `ADC_BAT`		| `ADC1_IN10`		| `IN/A`	| Analog battery voltage equal to `0.28 x V-BAT` 		|
| `PC1`     | `KEY1`		| `GPIO`			| `IN/PP`	| Push button 1  										|
| `PC2`		| `KEY2`		| `GPIO`			| `IN/PP`	| Push button 2  										|
| `PC3`		| `KEY3`		| `GPIO`			| `IN/PP`	| Push button 3  										|
| `PA0`		| `KEY4`		| `GPIO`			| `IN/PP`	| Push button 4  										|
| `PA1`		| `LED`			| `GPIO`			| `OUT/PP`	| Push button 4  										|
| `PA4`		| `VDAC`		| `DAC_OUT1`		| `OUT/A`	| Analog output (`HDR-PIN12`)							|
| `PC4`		| `xCS`			| `GPIO`			| `OUT/PP`	| `xCS` pin of `VS1063a`								|
| `PC5`		| `xDCS`		| `GPIO`			| `OUT/PP`	| `xDCS` pin of `VS1063a`								|
| `PB0`		| `DREQ`		| `GPIO/EXTI0_IRQn`	| `IN`		| `DREQ` pin of `VS1063a`								|
| `PB1`		| `xRST`		| `GPIO`			| `OUT/PP`	| `DREQ` pin of `VS1063a`								|
| `PB10`	| `LCD_DCX`		| `GPIO`			| `OUT/PP`	| `DCX` pin of `ili9340`(`HDR-PIN15`)					|
| `PB11`	| `TP_CS`		| `GPIO/SPI2-NCS1`	| `OUT/PP`	| `CS` pin of `xpt2046` (`HDR-PIN26`)					|
| `PB12`	| `LCD_CS`		| `GPIO/SPI2-NCS0`	| `OUT/PP`	| `CS` pin of `ili9340` (`HDR-PIN24`)					|
| `PC6`		| `LCD_RSTN`	| `GPIO`			| `OUT/PP`	| `RSTN` pin of `ili9340` (`HDR-PIN13`)					|
| `PA8`		| `BL_PWM`		| `GPIO`			| `OUT/PP`	| Backlight PWM for LCD (`HDR-PIN7`)					|
| `PC7`		| `TP_IRQ`		| `GPIO/EXTI7_IRQn`	| `IN`		| `IRQ` pin of `xpt2046` (`HDR-PIN11`)					|
| `PA13`	| `SWDIO`		| `SYSTEM`			| `N.A.`	| `SWDIO` pin of `STM32F103RCT6`						|
| `PA14`	| `SWCLK`		| `SYSTEM`			| `N.A.`	| `SWCLK` pin of `STM32F103RCT6`						|
| `PA15`	| `PWR_EXT`		| `GPIO`			| `OUT/PP`	| Enable `3v3` supply in `HDR-PIN1`/`HDR-PIN17`			|
| `PB3`		| `SD_CD`		| `GPIO`			| `IN/PP`	| SD card detect pin 									|
| `PB4`		| `REG_PG`		| `GPIO`			| `IN/PP`	| `PG` pin of `TPS62110QRSARQ1`							|
| `PB5`		| `REG_LBO`		| `GPIO`			| `IN/PP`	| `LBO` pin of `TPS62110QRSARQ1` (`LBI = 0.35 x V-BAT`)	|
| `PB8`		| `REG_SYNC`	| `GPIO`			| `OUT/PP`	| `SYNC` pin of `TPS62110QRSARQ1` (Optional connection)	|
| `PB9`		| `VBUS`		| `GPIO`			| `IN`		| Detect power on USB connector (`VBUS = 0.63 x V-USB`)	|




sd