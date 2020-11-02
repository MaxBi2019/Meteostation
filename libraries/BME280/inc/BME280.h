/* INCLUDES BEGIN */
#include "stm32f4xx_hal.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
/* INCLUDES END   */



/* CONST BEGIN */
#define TRUE    1
#define MIN_STR 30
#define MAX_STR 100
#define PERIOD  1000
/* CONST END   */



/* REGISTERS FOR TESTING I2C CONNECTION BEGIN  */
// BME280 I2C ADDRES (0x76<<1) HAL take one bit for read/write info
#define BME280_ADDRESS               0xEC

// REGISTER WHERE BME280 ID IS STORED
#define BME280_REG_ID                0xD0

// BME280 ID
#define BME280_ID                    0x60
/* REGISTERS FOR TESTING I2C CONNECTION END    */



/* REGISTERS FOR COLLECTING CONFIGURATION DATA BEGIN */
// BME280 SOFTWARE RESET REGISTER
#define BME280_REG_SOFTRESET         0xE0

// BME280 SOFT RESET VALUE
#define BME280_SOFTRESET_VALUE       0xB6

// Humidity measure control register
#define BME280_REG_CTRL_HUM          0xF2

// BME280 STATUS REGISTER
#define BME280_REGISTER_STATUS       0XF3

// Control register pressure and temperature measure
#define BME280_REG_CTRL_MEAS         0xF4

//Running conversion
#define BME280_STATUS_MEASURING      0X08

//NVM data is being copied
#define BME280_STATUS_NOT_UPDATED    0X01

// Configuration register
#define BME280_REG_CONFIG            0xF5
/* REGISTERS FOR COLLECTING CONFIGURATION DATA END   */



/* CALIBRATION REGISTERS BIGIN */
#define BME280_REGISTER_DIG_T1 	     0x88
#define BME280_REGISTER_DIG_T2 	     0x8A
#define BME280_REGISTER_DIG_T3 	     0x8C
#define BME280_REGISTER_DIG_P1       0x8E
#define BME280_REGISTER_DIG_P2       0x90
#define BME280_REGISTER_DIG_P3       0x92
#define BME280_REGISTER_DIG_P4       0x94
#define BME280_REGISTER_DIG_P5       0x96
#define BME280_REGISTER_DIG_P6       0x98
#define BME280_REGISTER_DIG_P7       0x9A
#define BME280_REGISTER_DIG_P8       0x9C
#define BME280_REGISTER_DIG_P9       0x9E
#define BME280_REGISTER_DIG_H1       0xA1
#define BME280_REGISTER_DIG_H2       0xE1
#define BME280_REGISTER_DIG_H3       0xE3
#define BME280_REGISTER_DIG_H4       0xE4
#define BME280_REGISTER_DIG_H5       0xE5
#define BME280_REGISTER_DIG_H6       0xE7
/* CALIBRATION REGISTERS END   */



/* STAND-BY CONFIGURATION REGISTERS BEGIN */
// MASK FOR TAKING 5, 6, 7-th bit of register BME280_REG_CONFIG
#define BME280_STBY_MASK             0xE0

// 0.5  MS
#define BME280_STBY_0_5              0x00

// 20   MS
#define BME280_STBY_62_5             0x20

// 40   MS
#define BME280_STBY_125              0x40

// 250  MS
#define BME280_STBY_250              0x60

// 500  MS
#define BME280_STBY_500              0x80

// 1000 MS
#define BME280_STBY_1000             0xA0

// 10   MS
#define BME280_STBY_10               0xC0

// 20   MS
#define BME280_STBY_20               0xE0
/* STAND-BY CONFIGURATION REGISTERS END   */



/* MODE REGISTER BEGIN */
// MASK FOR TAKING 0, 1-st bit of register BME280_REG_CTRL_MEAS
#define BME280_MODE_MASK             0x03
#define BME280_MODE_SLEEP            0x00
#define BME280_MODE_FORCED           0x01
#define BME280_MODE_NORMAL           0x03
/* MODE REGISTER END   */



/* FILTERIN BEGIN */
// MASK FOR TAKING 2, 3, 4-th bit of register BME280_REG_CONFIG
#define BME280_FILTER_MASK           0x1C
#define BME280_FILTER_OFF            0x00
#define BME280_FILTER_2              0x04
#define BME280_FILTER_4              0x08
#define BME280_FILTER_8              0x0C
#define BME280_FILTER_16             0x10
/* FILTERIN BEGIN */



/* OVERSAMPLING REGISTERS BEGIN */
// MASK FOR TAKING 5, 6, 7-th bit of register BME280_REG_CTRL_MEAS
#define BME280_OSRS_T_MASK           0xE0
#define BME280_OSRS_T_SKIP           0x00
#define BME280_OSRS_T_x1             0x20
#define BME280_OSRS_T_x2             0x40
#define BME280_OSRS_T_x4             0x60
#define BME280_OSRS_T_x8             0x80
#define BME280_OSRS_T_x16            0xA0

// MASK FOR TAKING 2, 3, 4-th bit of register BME280_REG_CTRL_MEAS
#define BME280_OSRS_P_MASK           0x1C
#define BME280_OSRS_P_SKIP           0x00
#define BME280_OSRS_P_x1             0x04
#define BME280_OSRS_P_x2             0x08
#define BME280_OSRS_P_x4             0x0C
#define BME280_OSRS_P_x8             0x10
#define BME280_OSRS_P_x16            0x14

// MASK FOR TAKING 0, 1, 2-th bit of register BME280_REG_CTRL_HUM
#define BME280_OSRS_H_MASK           0x07
#define BME280_OSRS_H_SKIP           0x00
#define BME280_OSRS_H_x1             0x01
#define BME280_OSRS_H_x2             0x02
#define BME280_OSRS_H_x4             0x03
#define BME280_OSRS_H_x8             0x04
#define BME280_OSRS_H_x16            0x05
/* OVERSAMPLING REGISTERS END   */



/* DATA REGISTERS BEGIN */
#define BME280_REGISTER_PRESSUREDATA 0xF7
#define BME280_REGISTER_TEMPDATA     0xFA
#define BME280_REGISTER_HUMIDDATA    0xFD
/* DATA REGISTERS END   */



/* MACRO LED BEGIN */
#define ERROR_FRQ	                250
#define LED_GPIO_PORT               GPIOD
#define ERROR_PIN                   GPIO_PIN_14
#define SUCCESS_PIN                 GPIO_PIN_12
#define SUCCESS_ON                  HAL_GPIO_WritePin(LED_GPIO_PORT, SUCCESS_PIN, GPIO_PIN_SET)
#define SUCCESS_OFF                 HAL_GPIO_WritePin(LED_GPIO_PORT, SUCCESS_PIN, GPIO_PIN_RESET)
#define BLINK_ERROR                 HAL_GPIO_TogglePin(LED_GPIO_PORT, ERROR_PIN)
/* MACRO LED END   */



/* TYPEDEF BEGIN */
typedef struct
{
  float tmpr_f;
  char  tmpr_s[MIN_STR];

  float hmdt_f;
  char  hmdt_s[MIN_STR];

  float prsr_f;
  char  prsr_s[MIN_STR];
} BME280_DATA;
typedef struct
{
  uint16_t      dig_T1;
  int16_t       dig_T2;
  int16_t       dig_T3;
  uint16_t      dig_P1;
  int16_t       dig_P2;
  int16_t       dig_P3;
  int16_t       dig_P4;
  int16_t       dig_P5;
  int16_t       dig_P6;
  int16_t       dig_P7;
  int16_t       dig_P8;
  int16_t       dig_P9;
  uint8_t       dig_H1;
  int16_t       dig_H2;
  uint8_t       dig_H3;
  int16_t       dig_H4;
  int16_t       dig_H5;
  int8_t        dig_H6;
} BME280_CONF;
/* TYPEDEF END   */



/* EXTERNAL BEGIN */
extern BME280_DATA        bme_280_d;
extern I2C_HandleTypeDef  hi2c1;
/* EXTERNAL END   */



/* FUNCTIONS BEGIN */
void BME280_Init();
void BME280_COLLECT();
/* FUNCTIONS END   */
