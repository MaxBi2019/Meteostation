#ifndef SD_H_

#define SD_H_

//--------------------------------------------------
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>



//--------------------------------------------------
#define TRUE               1
#define FALSE              0
#define INIT_ATTEMPTS      10
#define MIN_SPACE	       10
#define STABILIZATION_TIME 500
#define EMPTY_PKG          0xFF


#define CS_Port GPIOE
#define CS_Pin  GPIO_PIN_11
#define SELECT_SD()   HAL_GPIO_WritePin(CS_Port, CS_Pin, 0)
#define UNSELECT_SD() HAL_GPIO_WritePin(CS_Port, CS_Pin, 1)
#define LD_ON  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 1); //RED
#define LD_OFF HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 0); //RED
#define SD_SPI hspi5

#define CMD0 (0x40+0) // GO_IDLE_STATE
#define CMD1 (0x40+1) // SEND_OP_COND (MMC)
#define ACMD41 (0xC0+41) // SEND_OP_COND (SDC)
#define CMD8 (0x40+8) // SEND_IF_COND
#define CMD9 (0x40+9) // SEND_CSD
#define CMD16 (0x40+16) // SET_BLOCKLEN
#define CMD17 (0x40+17) // READ_SINGLE_BLOCK
#define CMD24 (0x40+24) // WRITE_BLOCK
#define CMD55 (0x40+55) // APP_CMD
#define CMD58 (0x40+58) // READ_OCR
#define DATA_TOKEN 0xFE

#define SPI_SD_INIT_BEGIN(SPI) sd_spi_info.rate=SPI.Init.BaudRatePrescaler;SPI.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_256;HAL_SPI_Init(&SPI)
#define SPI_SD_INIT_END(SPI)   (SPI).Init.BaudRatePrescaler=sd_spi_info.rate;HAL_SPI_Init(&SPI)

extern SPI_HandleTypeDef SD_SPI;
//--------------------------------------------------

/* Card type flags (CardType) */

#define CT_MMC 0x01 /* MMC ver 3 */
#define CT_SD1 0x02 /* SD ver 1 */
#define CT_SD2 0x04 /* SD ver 2 */
#define CT_SDC (CT_SD1|CT_SD2) /* SD */
#define CT_BLOCK 0x08 /* Block addressing */

//--------------------------------------------------

typedef struct
{
	uint32_t rate;
} spi_info;

spi_info sd_spi_info;

typedef struct sd_info
{
  volatile uint8_t is_compatible : 1;
  volatile uint8_t is_init       : 1;
  volatile char str[512];
} sd_info_ptr;


extern sd_info_ptr sdinfo;
//--------------------------------------------------

//--------------------------------------------------

void SPI_Release();
uint8_t SPI_wait_ready(void);
void SDInit(void);
uint8_t SD_Read_Block (uint8_t *buff, uint32_t lba);
uint8_t SD_Write_Block (uint8_t *buff, uint32_t lba);


#endif /* SD_H_ */
