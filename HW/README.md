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
- LCD controller: `ili9341` / `ili9486`
- Touch screen controller: `xpt2046`
- `VS1063a` crystal frequency: **12.288 Mhz**


## µC Pin assignment

Pin names are according to `STM32F103xC` datasheet, **Table 5**
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
  - `IN/AF`: input alternative function
  - `OUT/AF`: output alternative function
- `Description`: Some note about this pin (Active high/low, ...)

| Pin name  	| Alias in Code | Peripheral 		| Direction | Description 											|
| --------------| --------------| ------------------| ----------| ------------------------------------------------------|
| `PC13`    	| `CHG`			| `GPIO`			| `IN/PP`	| `CHG` pin of `MAX1555`   								|
| `PC0`     	| `ADC_BAT`		| `ADC1_IN10`		| `IN/A`	| Analog battery voltage equal to `0.28 x V-BAT` 		|
| `PC1`     	| `KEY1`		| `GPIO`			| `IN/PP`	| Push button 1 (`S3`) 									|
| `PC2`			| `KEY2`		| `GPIO`			| `IN/PP`	| Push button 2 (`S4`)									|
| `PC3`			| `KEY3`		| `GPIO`			| `IN/PP`	| Push button 3 (`S5`)									|
| `PA0`			| `KEY4`		| `GPIO`			| `IN/PP`	| Push button 4 (`S6`)									|
| `PA1`			| `LED`			| `GPIO`			| `OUT/PP`	| Green LED (`D6`)  									|
| `PA4`			| `VDAC`		| `DAC_OUT1`		| `OUT/A`	| Analog output (`HDR-PIN12`)							|
| `PA15`		| `PWR_EXT`		| `GPIO`			| `OUT/PP`	| Enable `3v3_EXT` supply in `HDR-PIN1`/`HDR-PIN17`		|
| `PB4`			| `REG_PG`		| `GPIO`			| `IN/PP`	| `PG` pin of `TPS62110QRSARQ1`							|
| `PB5`			| `REG_LBO`		| `GPIO`			| `IN/PP`	| `LBO` pin of `TPS62110QRSARQ1` (`LBI = 0.35 x V-BAT`)	|
| `PB8`			| `REG_SYNC`	| `GPIO`			| `OUT/PP`	| `SYNC` pin of `TPS62110QRSARQ1` (Optional connection)	|
| `PB9`			| `VBUS`		| `GPIO`			| `IN`		| Detect power on USB connector (`VBUS = 0.63 x V-USB`)	|
| `PC4`			| `xCS`			| `GPIO`			| `OUT/PP`	| `xCS` pin of `VS1063a`								|
| `PC5`			| `xDCS`		| `GPIO`			| `OUT/PP`	| `xDCS` pin of `VS1063a`								|
| `PB0`			| `DREQ`		| `GPIO/EXTI0_IRQn`	| `IN`		| `DREQ` pin of `VS1063a`								|
| `PB1`			| `xRST`		| `GPIO`			| `OUT/PP`	| `DREQ` pin of `VS1063a`								|
| `PA5`			| `SPI1_SCK`	| `SPI1/SCK`		| `OUT/AF`	| `SCLK` pin of `VS1063a`								|
| `PA6`			| `SPI1_MISO`	| `SPI1/MISO`		| `IN/AF`	| `SO` pin of `VS1063a`									|
| `PA7`			| `SPI1_MOSI`	| `SPI1/MOSI`		| `OUT/AF`	| `SI` pin of `VS1063a`									|
| `PB10`		| `LCD_DCX`		| `GPIO`			| `OUT/PP`	| `DCX` pin of `ili9341`(`HDR-PIN15`)					|
| `PB12`		| `LCD_CS`		| `GPIO/SPI2-NCS0`	| `OUT/PP`	| `CS` pin of `ili9341` (`HDR-PIN24`)					|
| `PC6`			| `LCD_RSTN`	| `GPIO`			| `OUT/PP`	| `RSTN` pin of `ili9341` (`HDR-PIN13`)					|
| `PB11`		| `TP_CS`		| `GPIO/SPI2-NCS1`	| `OUT/PP`	| `CS` pin of `xpt2046` (`HDR-PIN26`)					|
| `PC7`			| `TP_IRQ`		| `GPIO/EXTI7_IRQn`	| `IN`		| `IRQ` pin of `xpt2046` (`HDR-PIN11`)					|
| `PA8`			| `BL_PWM`		| `GPIO`			| `OUT/PP`	| Backlight PWM for LCD (`HDR-PIN7`)					|
| `PA13`		| `SWDIO`		| `SYSTEM`			| `N.A.`	| `SWDIO` pin of `STM32F103RCT6`						|
| `PA14`		| `SWCLK`		| `SYSTEM`			| `N.A.`	| `SWCLK` pin of `STM32F103RCT6`						|
| `PB3`			| `SD_CD`		| `GPIO`			| `IN/PP`	| SD card detect pin 									|
| `PC8`			| `SDIO_D0`		| `SDIO/D0`			| `AF`		| `D0` pin of SD card (SDIO works on in `1bit` mode)	|
| `PC9`			| `SDIO_D1`		| `SDIO/D1`			| `AF`		| `D1` pin of SD card	(**Not working yet**)			|
| `PC10`		| `SDIO_D2`		| `SDIO/D2`			| `AF`		| `D2` pin of SD card	(**Not working yet**)			|
| `PC11`		| `SDIO_D3`		| `SDIO/D3`			| `AF`		| `D3` pin of SD card	(**Not working yet**)			|
| `PC12`		| `SDIO_CK`		| `SDIO/CK`			| `OUT/AF`	| `CLK` pin of SD card									|
| `PD2`			| `SDIO_CMD`	| `SDIO/CMD`		| `OUT/AF`	| `CMD` pin of SD card									|
| `PB13`		| `SPI2_SCK`	| `SPI2/SCK`		| `OUT/AF`	| `SPI/CLK` for `ili9341`/`xpt2046` (`HDR-PIN23`)		|
| `PB15`		| `SPI2_MOSI`	| `SPI2/MOSI`		| `OUT/AF`	| `SPI/MOSI` for `ili9341`/`xpt2046` (`HDR-PIN19`)		|
| `PB14`		| `SPI2_MISO`	| `SPI2/MISO`		| `IN/AF`	| `SPI/MISO` for `ili9341`/`xpt2046` (`HDR-PIN21`)		|
| `PB6`			| `I2C1_SCL`	| `I2C1/SCL`		| `OUT/AF`	| `I2C/SCL` with 4.7k pull-up (`HDR-PIN5`)				|
| `PB7`			| `I2C1_SDA`	| `I2C1/SDA`		| `AF`		| `I2C/SDA` with 4.7k pull-up (`HDR-PIN3`)				|
| `BOOT0`		| `N.A.`		| `SYSTEM`			| `N.A.`	| pulled down to `GND` via 10K							|
| `PB2 (BOOT1)`	| `N.A.`		| `SYSTEM`			| `N.A.`	| pulled down to `GND` via 10K							|
| `PA2`			| `USART2_TX`	| `USART2/TX`		| `OUT/AF`	| optional connection for `RX` pin of `VS1063a`			|
| `PA3`			| `USART2_RX`	| `USART2/RX`		| `IN/AF`	| optional connection for `TX` pin of `VS1063a`			|
| `PA9`			| `USART1_TX`	| `USART1/TX`		| `OUT/AF`	| USART/Tx interface (`HDR-PIN8`)						|
| `PA10`		| `USART1_RX`	| `USART1/TX`		| `IN/AF`	| USART/Rx interface (`HDR-PIN10`)						|

## Header pin assignment

Here is table of `VS1063a` UART pin assignment (**P1**)

| Pin Num.  | Description   |
| --------- | ------------- |
|	**1**	| `GND`			|
|	**2**	| `VS-RX`		|
|	**3**	| `VS-TX`		|


Here is table of VS-MP3 main board header pin (**P2**)


| Description  	| Pin		| Pin		| Description   	|
|---------------|-----------|-----------|-------------------|
| `3v3_EXT`		| **1**		|	**2**	| `5V0` (USB only)	|
| `I2C1_SDA`	| **3**		|	**4**	| `5V0` (USB only)	|
| `I2C1_SCL`	| **5**		|	**6**	| `GND`				|
| `BL_PWM` 		| **7**		|	**8**	| `USART1_TX`		|
| `GND`			| **9**		|	**10**	| `USART1_RX`		|
| `TP_IRQ`		| **11**	|	**12**	| `VDAC`			|
| `LCD_RSTN`	| **13**	|	**14**	| `GND`				|
| `LCD_DCX`		| **15**	|	**16**	| `N.C.`			|
| `3v3_EXT`		| **17**	|	**18**	| `N.C.`			|
| `SPI2_MOSI`	| **19**	|	**20**	| `GND`				|
| `SPI2_MISO`	| **21**	|	**22**	| `N.C.`			|
| `SPI2_SCK`	| **23**	|	**24**	| `LCD_CS`			|
| `GND`			| **25**	|	**26**	| `TP_CS`			|
