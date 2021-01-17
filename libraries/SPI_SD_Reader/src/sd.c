#include "sd.h"
#include "terminal.h"


//--------------------------------------------------
sd_info_ptr sdinfo =
{
	.is_compatible    = FALSE,
	.is_init          = FALSE
};


static void
Error()
{
	LD_ON;
	while (1);
}


static void
Stabilize(void)
{
  HAL_Delay(STABILIZATION_TIME);
}


uint8_t
SPI_IO(uint8_t Byte)
{
    uint8_t r_byte = 0;
    if(HAL_SPI_TransmitReceive(&SD_SPI,(uint8_t*) &Byte,(uint8_t*) &r_byte,1,0x1000)!=HAL_OK)
    {
	    Error();
    }
    return r_byte;
}


void
SPI_SendByte(uint8_t bt)
{
	  SPI_IO(bt);
}


uint8_t 
SPI_ReceiveByte(void)
{
  uint8_t bt = SPI_IO(0xFF);
  return bt;
}


void 
SPI_Release(void)
{
	SPI_IO(EMPTY_PKG);
}


static uint8_t SD_cmd (uint8_t cmd, uint32_t arg)

{
	uint8_t n, res;
	// ACMD<n> is the command sequense of CMD55-CMD<n>
	if (cmd & 0x80)
	{
		cmd &= 0x7F;
		res = SD_cmd(CMD55, 0);
		if (res > 1) return res;
	}
	UNSELECT_SD();
	SPI_ReceiveByte();
	SELECT_SD();
	SPI_ReceiveByte();
	
	SPI_SendByte(cmd); // Start + Command index
	SPI_SendByte((uint8_t)(arg >> 24)); // Argument[31..24]
	SPI_SendByte((uint8_t)(arg >> 16)); // Argument[23..16]
	SPI_SendByte((uint8_t)(arg >> 8)); // Argument[15..8]
	SPI_SendByte((uint8_t)arg); // Argument[7..0]
	
	n = 0x01; // Dummy CRC + Stop
	if (cmd == CMD0) {n = 0x95;} // Valid CRC for CMD0(0)
	if (cmd == CMD8) {n = 0x87;} // Valid CRC for CMD8(0x1AA)
	SPI_SendByte(n);
	
	n = 10; // Wait for a valid response in timeout of 10 attempts
	do
	{
	    res = SPI_ReceiveByte();
	} 
	while ((res & 0x80) && --n);
	return res;

}

uint8_t SD_Read_Block (uint8_t *buff, uint32_t lba)

{
  uint8_t result;
  uint16_t cnt;
  result=SD_cmd (CMD17, lba); //CMD17 даташит стр 50 и 96
  if (result!=0x00)
	  return 5; //Выйти, если результат не 0x00
  SPI_Release();
  cnt=0;
  do
  { //Ждем начала блока
      result=SPI_ReceiveByte();
      cnt++;
  }
  while ((result!=DATA_TOKEN) && (cnt<0xFFFF));
  if (cnt>=0xFFFF)
	  return 5;
  for (cnt=0;cnt<512;cnt++)
	  buff[cnt]=SPI_ReceiveByte(); //получаем байты блока из шины в буфер
  SPI_Release(); //Пропускаем контрольную сумму
  SPI_Release();
  return 0;
}


uint8_t SD_Write_Block (uint8_t *buff, uint32_t lba)
{
    uint8_t result;
    uint16_t cnt;
    result=SD_cmd(CMD24,lba); //CMD24 даташит стр 51 и 97-98
    if (result!=0x00)
    {
	    return 6; //Выйти, если результат не 0x00
    }
    SPI_Release();
    SPI_SendByte (0xFE); //Начало буфера
    for (cnt=0;cnt<512;cnt++)
	    SPI_SendByte(buff[cnt]); //Данные
    SPI_Release(); //Пропустим котрольную сумму
    SPI_Release();
    result=SPI_ReceiveByte();
    if ((result&0x05)!=0x05)
    {
  	    return 6; //Выйти, если результат не 0x00
    }
    cnt=0;
    do
    { //Ждем окончания состояния BUSY
	    result=SPI_ReceiveByte();
	    cnt++;
    }
    while ( (result!=EMPTY_PKG)&&(cnt<0xFFFF) );
    if (cnt>=0xFFFF)
    {
  	    return 6; //Выйти, если результат не 0x00
    }
    return 0;
}


void
SDInit(void)
{
  Stabilize();

  int16_t tmr;
  uint8_t ocr[4];
  uint8_t i;
  print(str_buf);

  for (uint8_t num = 0; !sdinfo.is_init && num < INIT_ATTEMPTS; ++num)
  {
	  sprintf(str_buf, "Init-%u\r\n", num+1);
	  print(str_buf);

	  SPI_SD_INIT_BEGIN(SD_SPI);
	  UNSELECT_SD();
	  for(i=0; i<16; i++)
	      SPI_Release();

	  SPI_SD_INIT_END(SD_SPI);
	  SELECT_SD();
	  SPI_Release();
	  if (SD_cmd(CMD0, 0) == 1)
		  sdinfo.is_init = TRUE;
  }

  if (!sdinfo.is_init)
	  return;

  SPI_Release();
  if (SD_cmd(CMD8, 0x1AA) != 1)
	  return;

  for (i = 0; i < 4; i++)
	  ocr[i] = SPI_ReceiveByte();

  if (ocr[2] != 0x01 || ocr[3] != 0xAA)
	  return;

  for (tmr = 12000; tmr && SD_cmd(ACMD41, 1UL << 30); tmr--);

  if (tmr && SD_cmd(CMD58, 0) != 0)
	  return;

  for (i = 0; i < 4; i++)
	  ocr[i] = SPI_ReceiveByte();

  if (!(ocr[0] & 0x40))
	  return;

  sdinfo.is_compatible = TRUE;
  print("Card is successfully initialized\r\n");
}



uint8_t SPI_wait_ready(void)
{
  uint8_t res;
  uint16_t cnt;
  cnt=0;
  do { //Ждем окончания состояния BUSY
    res=SPI_ReceiveByte();
    cnt++;
  } while ( (res!=EMPTY_PKG)&&(cnt<0xFFFF) );
  if (cnt>=0xFFFF) return 1;
  return res;
}



