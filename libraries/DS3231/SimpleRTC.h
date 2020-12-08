#include "stm32f4xx_hal.h"
#include <stdio.h>

#ifndef obit
#define obit 			uint8_t
#endif

#ifndef hbit
#define hbit 			uint16_t
#endif

#define TRUE     		1
#define FALSE    		0
#define DS3231_ADDRESS 	0xD0
#define DS3231_MAIN_REG 0x00
#define PACKAGE_SIZE	7
#define IS_12			0x40
#define IS_PM			0x20
#define MOD_24_HOUR		0x3F
#define MOD_12_HOUR		0x1F
#define IS_NEW_CENTURY	0x80
#define IS_OLD_CENTURY	0x00

#define ERROR_FRQ	                250
#define LED_GPIO_PORT               GPIOD
#define ERROR_PIN                   GPIO_PIN_14
#define SUCCESS_PIN                 GPIO_PIN_12
#define SUCCESS_ON                  HAL_GPIO_WritePin(LED_GPIO_PORT, SUCCESS_PIN, GPIO_PIN_SET)
#define SUCCESS_OFF                 HAL_GPIO_WritePin(LED_GPIO_PORT, SUCCESS_PIN, GPIO_PIN_RESET)
#define BLINK_ERROR                 HAL_GPIO_TogglePin(LED_GPIO_PORT, ERROR_PIN)

typedef struct 
{
	obit seconds;
	obit minutes;
	obit hours;
	
	obit day_of_week;
	obit is_12hr_mode;
	obit is_PM;
	
	obit date;
	obit month;
	hbit year;
} DS3231_setter;


typedef struct
{
	obit seconds;
	obit minutes;
	obit hours;
	char time_str[100];

	obit day_of_week;
	obit date;
	obit month;
	hbit year;
	char date_str[100];
} DS3231_getter;


typedef void (*i2c_select) (I2C_HandleTypeDef *hi2c);
typedef void (*procedure)  ();


typedef struct rtc_driver
{
	I2C_HandleTypeDef *hi2c;
	i2c_select innit;
	procedure  get_time;
	procedure  set_time;
	DS3231_setter setter;
	DS3231_getter getter;
} rtc_driver;


extern rtc_driver ds3231_driver;
