#ifndef _VS10XX_h_
#define _VS10XX_h_

#include <inttypes.h>
#include <stm32f1xx_hal.h>

/* Control pins */
#define	VS_xCS					GPIO_PIN_4
#define	VS_xCS_PORT			GPIOC

#define VS_xDCS 				GPIO_PIN_5
#define VS_xDCS_PORT 		GPIOC

#define VS_DREQ 				GPIO_PIN_0
#define VS_DREQ_PORT 		GPIOB


#define VS_xRST 				GPIO_PIN_1
#define VS_xRST_PORT 		GPIOB

#define vs_select_control()     HAL_GPIO_WritePin(VS_xCS_PORT, VS_xCS, GPIO_PIN_RESET)
#define vs_deselect_control()   HAL_GPIO_WritePin(VS_xCS_PORT, VS_xCS, GPIO_PIN_SET)
#define vs_select_data()        HAL_GPIO_WritePin(VS_xDCS_PORT, VS_xDCS, GPIO_PIN_RESET)
#define vs_deselect_data()      HAL_GPIO_WritePin(VS_xDCS_PORT, VS_xDCS, GPIO_PIN_SET)

// should be followed by 200ms delays for the cap to charge/discharge
#define vs_assert_xreset()      HAL_GPIO_WritePin(VS_xRST_PORT, VS_xRST, GPIO_PIN_RESET)
#define vs_deassert_xreset()    HAL_GPIO_WritePin(VS_xRST_PORT, VS_xRST, GPIO_PIN_SET)

#define VS_BUFF_SZ      32      // how much data to send in one batch to VS1063

// VS10xx SCI read and write commands
#define VS_WRITE_COMMAND    0x02
#define VS_READ_COMMAND     0x03

// VS10xx SCI Registers
#define SCI_MODE            0x0
#define SCI_STATUS          0x1
#define SCI_BASS            0x2
#define SCI_CLOCKF          0x3
#define SCI_DECODE_TIME     0x4
#define SCI_AUDATA          0x5
#define SCI_WRAM            0x6
#define SCI_WRAMADDR        0x7
#define SCI_HDAT0           0x8
#define SCI_HDAT1           0x9
#define SCI_AIADDR          0xa
#define SCI_VOL             0xb
#define SCI_AICTRL0         0xc
#define SCI_AICTRL1         0xd
#define SCI_AICTRL2         0xe
#define SCI_AICTRL3         0xf

// SCI_MODE
#define SM_DIFF		        0x0001
#define SM_LAYER12	        0x0002
#define SM_RESET	        0x0004
#define SM_CANCEL	        0x0008
#define SM_PDOWN	        0x0010  // ? undocumented
#define SM_TESTS	        0x0020
#define SM_STREAM	        0x0040  // ? undocumented
#define SM_PLUSV	        0x0080  // ? undocumented
#define SM_DACT		        0x0100
#define SM_SDIORD	        0x0200
#define SM_SDISHARE	        0x0400
#define SM_SDINEW	        0x0800
#define SM_ENCODE           0x1000
//#define SM_UNKNOWN        0x2000
#define SM_LINE1            0x4000
#define SM_CLK_RANGE        0x8000

// SCI_STATUS
#define SS_REFERENCE_SEL    0x0001
#define SS_AD_CLOCK         0x0002
#define SS_APDOWN1          0x0004
#define SS_APDOWN2          0x0008
#define SS_VER1             0x0010
#define SS_VER2             0x0020
#define SS_VER3             0x0040
#define SS_VER4             0x0080
//#define SS_UNKNOWN        0x0100
//#define SS_UNKNOWN        0x0200
#define SS_VCM_DISABLE      0x0400
#define SS_VCM_OVERLOAD     0x0800
#define SS_SWING1           0x1000
#define SS_SWING2           0x2000
#define SS_SWING3           0x4000
#define SS_DO_NOT_JUMP      0x8000

// parametric_x addresses translated to WRAMADDR
#define endFillByte         0xc0c6
#define earSpeakerLevel     0xc0de
#define ogg_gain_offset     0xc0ea

#define chipID_L						0xC0C0
#define chipID_H						0xC0C1
#define Version							0xC0C2


uint16_t vs_read_register(SPI_HandleTypeDef *hspi, const uint8_t address);
void vs_write_register(SPI_HandleTypeDef *hspi, const uint8_t address, const uint16_t value);
void vs_write_register_hl(SPI_HandleTypeDef *hspi, const uint8_t address, const uint8_t highbyte, const uint8_t lowbyte);

void vs_write_data(SPI_HandleTypeDef *hspi, uint8_t *buff, uint16_t size);
void vs_sine_test(SPI_HandleTypeDef *hspi, uint8_t index);

uint16_t vs_read_wramaddr(SPI_HandleTypeDef *hspi, const uint16_t address);
void vs_write_wramaddr(SPI_HandleTypeDef *hspi, const uint16_t address, const uint16_t value);

uint16_t vs_cancel_play(SPI_HandleTypeDef *hspi);
uint16_t vs_end_play(SPI_HandleTypeDef *hspi);

void vs_wait(void);
void vs_setup(SPI_HandleTypeDef *hspi);
void vs_setup_i2s(void);
void vs_soft_reset(SPI_HandleTypeDef *hspi);
void vs_set_volume(SPI_HandleTypeDef *hspi, const uint8_t leftchannel, const uint8_t rightchannel);
void vs_fill(SPI_HandleTypeDef *hspi, const uint16_t len);
void vs_ear_speaker(SPI_HandleTypeDef *hspi, const uint8_t level);

#endif /* ifdef _VS10XX_h_ */
