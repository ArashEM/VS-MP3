#include <stm32f1xx_hal.h>
#include "vs10xx.h"
#include <string.h>

// read the 16-bit value of a VS10xx register
uint16_t vs_read_register(SPI_HandleTypeDef *hspi, const uint8_t address)
{
    uint8_t		buff[2]		= 	{VS_READ_COMMAND, address};
		uint16_t	res;
		
    vs_deselect_data();
    vs_select_control();
    vs_wait();
    HAL_SPI_Transmit(hspi, buff, sizeof(buff), 0xFFFF);
    HAL_SPI_Receive(hspi, buff, sizeof(buff), 0xFFFF);
    vs_deselect_control();
    vs_wait();
		
		res = (buff[0] << 8) | buff[1];
		
    return res;
}

// write VS10xx register
void vs_write_register_hl(SPI_HandleTypeDef *hspi, const uint8_t address, const uint8_t highbyte, const uint8_t lowbyte)
{
		uint8_t	buff[4] = {VS_WRITE_COMMAND, address, highbyte, lowbyte};
    vs_deselect_data();
    vs_select_control();
    vs_wait();
    HAL_Delay(2);
    HAL_SPI_Transmit(hspi, buff, sizeof(buff), 0xFFFF);
    vs_deselect_control();
    vs_wait();
}


// write VS10xx 16-bit SCI registers
void vs_write_register(SPI_HandleTypeDef *hspi, const uint8_t address, const uint16_t value)
{
    uint8_t highbyte	= (value & 0xff00) >> 8;
		uint8_t	lowbyte		= value & 0x00ff;
	
    vs_write_register_hl(hspi, address, highbyte, lowbyte);
}

void vs_write_wramaddr(SPI_HandleTypeDef *hspi, const uint16_t address, const uint16_t value)
{
    vs_write_register(hspi, SCI_WRAMADDR, address);
    vs_write_register(hspi, SCI_WRAM, value);
}

// read data rams
uint16_t vs_read_wramaddr(SPI_HandleTypeDef *hspi, const uint16_t address)
{
    uint16_t rv = 0;
    vs_write_register(hspi, SCI_WRAMADDR, address);
    rv = vs_read_register(hspi, SCI_WRAM);
    return rv;
}

void vs_write_data(SPI_HandleTypeDef *hspi, uint8_t *buff, uint16_t size)
{
		vs_deselect_control();
		vs_select_data();
		vs_wait();
		HAL_SPI_Transmit(hspi, buff, size, 0xFFFF);
		vs_wait();
		vs_deselect_data();
}

// sine test
void vs_sine_test(SPI_HandleTypeDef *hspi, uint8_t index)
{
	uint8_t		sdi_test[] = {0x53, 0xEF, 0x6E, 0xA8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	vs_write_register(hspi, SCI_MODE, 0x4820);
	vs_write_data(hspi, sdi_test, sizeof(sdi_test));
}

// set VS10xx volume attenuation    0x00 lound - 0xfe silent
void vs_set_volume(SPI_HandleTypeDef *hspi, const uint8_t leftchannel, const uint8_t rightchannel)
{
    // volume = dB/0.5
		vs_write_register_hl(hspi, SCI_VOL, leftchannel, rightchannel);
}

// wait for VS_DREQ to get HIGH before sending new data to SPI
void vs_wait(void)
{
    while(HAL_GPIO_ReadPin(VS_DREQ_PORT, VS_DREQ) != GPIO_PIN_SET){ };
}

// set up pins
void vs_setup(SPI_HandleTypeDef *hspi)
{
    vs_deassert_xreset();
    HAL_Delay(400);
    vs_wait();
		/**	
		*		Setup clock multiplier
		*		XTALI 	= 12.288 Mhz
		*		CLKI  	= XTALI×4:0		->	SC_MUL = 5
		*		MAX CLK	= 67.6Mhz			->	SC_ADD = 2
		*/
		vs_write_register(hspi, SCI_CLOCKF, 0xB000);
		HAL_Delay(5);
}


// level    0 - disabled - suited for listening through loudspeakers
//          1 - suited for listening to normal musical scores with headphones
//          2 - less subtle than 1
//          3 - for old and 'dry' recordings
void vs_ear_speaker(SPI_HandleTypeDef *hspi, const uint8_t level)
{
    const uint16_t ear_speaker_level[4] = { 0, 0x2ee0, 0x9470, 0xc350 };

    vs_write_wramaddr(hspi, earSpeakerLevel, ear_speaker_level[level%4]);
}

void vs_fill(SPI_HandleTypeDef *hspi, const uint16_t len)
{
		uint8_t buff[VS_BUFF_SZ];
    uint8_t fill;
    uint16_t i = 0;

    fill = vs_read_wramaddr(hspi, endFillByte);
    memset(buff, fill, VS_BUFF_SZ);

    vs_select_data();
    for (i = 0; i < (len / VS_BUFF_SZ); i++) {
        vs_wait();
				HAL_SPI_Transmit(hspi, buff, VS_BUFF_SZ - 1, 0xFFFFFF);
    }
    vs_wait();
    HAL_SPI_Transmit(hspi, buff, (len % VS_BUFF_SZ) - 1, 0xFFFFFF);
    vs_deselect_data();
}

// returns 0 on success
// returns the old codec id on failure
uint16_t vs_end_play(SPI_HandleTypeDef *hspi)
{
		uint8_t i=0;
    uint16_t rv;

    if (vs_read_register(hspi, SCI_HDAT1) == 0x664c) {
        vs_fill(hspi, 12288);
    } else {
        vs_fill(hspi, 2052);
    }
    vs_write_register(hspi, SCI_MODE, SM_CANCEL);
    vs_fill(hspi, 32);
    while (vs_read_register(hspi, SCI_MODE) & SM_CANCEL) {
        vs_fill(hspi, 32);
        i++;
        if (i == 64) {
            vs_soft_reset(hspi);
            break;
        }
    }
    rv = vs_read_register(hspi, SCI_HDAT1);
    return rv;
}


void vs_soft_reset(SPI_HandleTypeDef *hspi)
{
    vs_write_register(hspi, SCI_MODE, SM_SDINEW | SM_RESET);
    HAL_Delay(2);
	
    /**	
		*		Setup clock multiplier
		*		XTALI 	= 12.288 Mhz
		*		CLKI  	= XTALI×4:0		->	SC_MUL = 5
		*		MAX CLK	= 67.6Mhz			->	SC_ADD = 2
		*/
		vs_write_register(hspi, SCI_CLOCKF, 0xB000);
		HAL_Delay(5);
}

uint16_t vs_cancel_play(SPI_HandleTypeDef *hspi)
{
    uint16_t rv;

    rv = vs_read_register(hspi, SCI_HDAT1);
    vs_end_play(hspi);
    if (rv == 0x664c) {
        vs_fill(hspi, 12288);
    } else {
        vs_fill(hspi, 2052);
    }
    rv = vs_read_register(hspi, SCI_HDAT1);
    return rv;
}