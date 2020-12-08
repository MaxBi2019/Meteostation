#include "SimpleRTC.h"

static char *days_of_week[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

static void DS3231_init(I2C_HandleTypeDef *hi2c);
static void DS3231_set_time();
static void DS3231_get_time();
static uint8_t dec2bcd(uint8_t d);
static uint8_t bcd2dec(uint8_t b);
static void Error();

rtc_driver ds3231_driver = 
{
	.innit    = DS3231_init,
	.get_time = DS3231_get_time,
	.set_time = DS3231_set_time
};


static void
Error()
{
  SUCCESS_OFF;
  while(TRUE)
	{
	  BLINK_ERROR;
	  HAL_Delay(ERROR_FRQ);
	}
}



static uint8_t
dec2bcd(uint8_t decimal)
{
  return ((decimal/10 * 16) + (decimal % 10));
}


static uint8_t
bcd2dec(uint8_t bcd)
{
  return ((bcd/16 * 10) + (bcd % 16));
}


static void 
DS3231_init(I2C_HandleTypeDef *hi2c)
{
  ds3231_driver.hi2c = hi2c;
}


static void 
DS3231_get_time()
{

	HAL_StatusTypeDef status = HAL_OK;
	obit buffer[PACKAGE_SIZE];
	obit hour_byte;
	char *mode;

	HAL_I2C_Mem_Read(ds3231_driver.hi2c, DS3231_ADDRESS, DS3231_MAIN_REG, 1, &buffer, PACKAGE_SIZE, 10);
	if (status != HAL_OK)
	  	  Error();

	hour_byte = buffer[2];

	if(hour_byte & IS_12)
	{
		if(hour_byte & IS_PM)
		{
			mode = "PM";
		}
		else
		{
			mode = "AM";
		}
		ds3231_driver.getter.hours = bcd2dec(hour_byte & MOD_12_HOUR);
	}
	else
	{
		mode = "AM";
		ds3231_driver.getter.hours = bcd2dec(hour_byte & MOD_24_HOUR);
	}

	ds3231_driver.getter.seconds     = bcd2dec(buffer[0]);
	ds3231_driver.getter.minutes     = bcd2dec(buffer[1]);
	ds3231_driver.getter.day_of_week = bcd2dec(buffer[3]);
	ds3231_driver.getter.date        = bcd2dec(buffer[4]);
	ds3231_driver.getter.month       = bcd2dec(buffer[5] & 0x1F);
	ds3231_driver.getter.year        = ((buffer[5] & IS_NEW_CENTURY) == 1) ? (2100 + bcd2dec(buffer[6])) : (2000 + bcd2dec(buffer[6]));

	sprintf(ds3231_driver.getter.time_str, "%s%u:%s%u:%s%u %s\n", (ds3231_driver.getter.hours   < 10)? "0": "", ds3231_driver.getter.hours,
														    	  (ds3231_driver.getter.minutes < 10)? "0": "", ds3231_driver.getter.minutes,
																  (ds3231_driver.getter.seconds < 10)? "0": "", ds3231_driver.getter.seconds,
																   mode);
	sprintf(ds3231_driver.getter.date_str, "%s %s%u:%s%u:%u\n", days_of_week[ds3231_driver.getter.day_of_week],
																(ds3231_driver.getter.date  < 10)? "0": "", ds3231_driver.getter.date,
																(ds3231_driver.getter.month < 10)? "0": "", ds3231_driver.getter.month,
																ds3231_driver.getter.year);

}


static void 
DS3231_set_time()
{
	HAL_StatusTypeDef status = HAL_OK;
	obit buffer[7];
	obit century = 0, year = ds3231_driver.setter.year;
	obit hour;
	if(year >= 2100)
	{
		century = IS_NEW_CENTURY;
		year -= 2100;
	}
	else
	{
		century = IS_OLD_CENTURY;
		year -= 2000;
	}
	if(ds3231_driver.setter.is_12hr_mode)
	{
		if(ds3231_driver.setter.is_PM)
		{
			hour = dec2bcd(ds3231_driver.setter.hours & MOD_12_HOUR);
			hour |= IS_PM | IS_12 ;
		}
		else
		{
			hour = dec2bcd(ds3231_driver.setter.hours & MOD_12_HOUR);
			hour |= IS_12;
			hour &= ~IS_PM;
		}
	}
	else
	{
		hour = dec2bcd(ds3231_driver.setter.hours & MOD_24_HOUR);
		hour &= ~IS_12;
	}
	buffer[0]=dec2bcd(ds3231_driver.setter.seconds);
	buffer[1]=dec2bcd(ds3231_driver.setter.minutes);
	buffer[2]=hour;
	buffer[3]=dec2bcd(ds3231_driver.setter.day_of_week);
	buffer[4]=dec2bcd(ds3231_driver.setter.date);
	buffer[5]=(dec2bcd(ds3231_driver.setter.month) | century);
	buffer[6]=dec2bcd(year);

	status = HAL_I2C_Mem_Write(ds3231_driver.hi2c, DS3231_ADDRESS, DS3231_MAIN_REG, 1, &buffer, PACKAGE_SIZE, 10);
	if (status != HAL_OK)
		  	  Error();
}
