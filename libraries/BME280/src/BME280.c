/* INCLUDES BEGIN */
#include "BME280.h"
#include "usb_device.h"
/* INCLUDES END   */



/* GLOBAL VAR BEGIN */
BME280_DATA     bme_280_d;
BME280_CONF     coeffs;
uint32_t		bme_280_raw;
char            bme_280_buffer[MAX_STR];
static int32_t  t_fine;
/* GLOBAL VAR END   */



/* STATIC PROTOTIPE BEGIN */
static void Error();
static void Check1();
static void Check2();
static void RRegU1B(uint8_t Reg, uint8_t  *Dest);
static void RRegS1B(uint8_t Reg, int8_t   *Dest);
static void RRegU2B(uint8_t Reg, uint16_t *Dest);
static void RRegS2B(uint8_t Reg, int16_t  *Dest);
static void RRegU3B(uint8_t Reg, uint32_t *Dest);
static void WRegU1B(uint8_t Reg, uint8_t *Val);
static void BE2B(uint16_t *Val);
static void BE3B(uint32_t *Val);
static void Reset();
static void RStatus(uint8_t *Dst);
static void RCofs();
static void SStnb(uint8_t stnb);
static void SMode(uint8_t mode);
static void Filter(uint8_t filter);
static void OverST(uint8_t overs);
static void OverSP(uint8_t overs);
static void OverSH(uint8_t overs);
static void Clbrt(uint8_t filter, uint8_t overst, uint8_t oversp, uint8_t oversh);
float RTemperature();
float RPressure();
float RHumidity();
/* STATIC PROTOTIPE END   */



/* STATIC DEFINITION BEGIN */
/* ---------------
 * FUNCTION: ERROR
 * ---------------
 * IF error occurred, then starts
 * infinite loop and blink with red
 * LED
 */
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

/* ----------------------- */

/* -----------------
 * FUNCTION: RRegU1B
 * -----------------
 * Read 1 unsigned Byte from
 * BME280 REG into DEST
 * Rise Error: if error occurred
 *             while reading
 */
static void
RRegU1B(uint8_t Reg,
		uint8_t *Dest)
{
  *Dest = 0;
  HAL_StatusTypeDef status = HAL_OK;
  status = HAL_I2C_Mem_Read(&hi2c1,
   						    BME280_ADDRESS,
							Reg,
							I2C_MEMADD_SIZE_8BIT,
							Dest,
							1,
							0x10000);
  if (status != HAL_OK)
  	  Error();
}

/* ----------------------- */

/* -----------------
 * FUNCTION: RRegS1B
 * -----------------
 * Read 1 signed Byte from
 * BME280 REG into DEST
 * Rise Error: if error occurred
 *             while reading
 */
static void
RRegS1B(uint8_t Reg,
		int8_t  *Dest)
{
  *Dest = 0;
  HAL_StatusTypeDef status = HAL_OK;
  status = HAL_I2C_Mem_Read(&hi2c1,
   						    BME280_ADDRESS,
							Reg,
							I2C_MEMADD_SIZE_8BIT,
							(uint8_t*)Dest,
							1,
							0x10000);
  if (status != HAL_OK)
  	  Error();
}

/* ----------------------- */

/* -----------------
 * FUNCTION: RRegU2B
 * -----------------
 * Read 2 unsigned Bytes from
 * BME280 REG into DEST
 * Rise Error: if error occurred
 *             while reading
 */
static void
RRegU2B(uint8_t  Reg,
		uint16_t *Dest)
{
  *Dest = 0;
  HAL_StatusTypeDef status = HAL_OK;
  status = HAL_I2C_Mem_Read(&hi2c1,
   						    BME280_ADDRESS,
							Reg,
							I2C_MEMADD_SIZE_8BIT,
							(uint8_t*)Dest,
							2,
							0x10000);
  if (status != HAL_OK)
  	  Error();
}

/* ----------------------- */

/* -----------------
 * FUNCTION: RRegS2B
 * -----------------
 * Read 2 signed Bytes from
 * BME280 REG into DEST
 * Rise Error: if error occurred
 *             while reading
 */
static void
RRegS2B(uint8_t  Reg,
		int16_t *Dest)
{
  *Dest = 0;
  HAL_StatusTypeDef status = HAL_OK;
  status = HAL_I2C_Mem_Read(&hi2c1,
   						    BME280_ADDRESS,
							Reg,
							I2C_MEMADD_SIZE_8BIT,
							(uint8_t*)Dest,
							2,
							0x10000);
  if (status != HAL_OK)
  	  Error();
}

/* ----------------------- */

/* -----------------
 * FUNCTION: RRegU3B
 * -----------------
 * Read 3 unsigned Bytes from
 * BME280 REG into DEST
 * Rise Error: if error occurred
 *             while reading
 */
static void
RRegU3B(uint8_t  Reg,
		uint32_t *Dest)
{
  *Dest = 0;
  HAL_StatusTypeDef status = HAL_OK;
  status = HAL_I2C_Mem_Read(&hi2c1,
   						    BME280_ADDRESS,
							Reg,
							I2C_MEMADD_SIZE_8BIT,
							(uint8_t*)Dest,
							3,
							0x10000);
  *Dest &= 0xFFFFFF;
  if (status != HAL_OK)
  	  Error();
}

/* ----------------------- */

/* -----------------
 * FUNCTION: RRegS3B
 * -----------------
 * Read 3 signed Bytes from
 * BME280 REG into DEST
 * Rise Error: if error occurred
 *             while reading
 */
static void
RRegS3B(uint8_t  Reg,
		int32_t  *Dest)
{
  *Dest = 0;
  HAL_StatusTypeDef status = HAL_OK;
  status = HAL_I2C_Mem_Read(&hi2c1,
   						    BME280_ADDRESS,
							Reg,
							I2C_MEMADD_SIZE_8BIT,
							(uint8_t*)Dest,
							3,
							0x10000);
  *Dest &= 0xFFFFFF;
  if (status != HAL_OK)
  	  Error();
}

/* ----------------------- */

/* -----------------
 * FUNCTION: WRegU1B
 * -----------------
 * Read 1 Byte from buffer VAL
 * into BME280 register REG
 * Rise Error: if error occurred
 *             while writing
 */
static void
WRegU1B(uint8_t Reg,
        uint8_t *Val)
{
  HAL_StatusTypeDef status = HAL_OK;
  status = HAL_I2C_Mem_Write(&hi2c1,
   						    BME280_ADDRESS,
							Reg,
							I2C_MEMADD_SIZE_8BIT,
							Val,
							1,
							0x10000);
  if (status != HAL_OK)
  	  Error();
}

/* ----------------------- */

/* --------------
 * FUNCTION: BE2B
 * --------------
 * Reverse 2 Bytes of VAL in Big Endian style
 */
static void
BE2B(uint16_t *Val)
{
  *Val = (*Val >> 8) | (*Val << 8);
}

/* ----------------------- */

/* --------------
 * FUNCTION: BE3B
 * --------------
 * Reverse 3 Bytes of VAL in Big Endian style
 */
static void
BE3B(uint32_t *Val)
{
  *Val = ((*Val>>16) & 0x000000FF) | (*Val & 0x0000FF00) | ((*Val<<16) & 0x00FF0000);
}

/* ----------------------- */

/* -------------------
 * FUNCTION: RRegBEU2B
 * -------------------
 * Read 2 unsigned Bytes from REG (reversing them)
 * into DEST
 * Rise Error: if error occurred
 *             while reading
 */
static void
RRegBEU2B(uint8_t  Reg,
		  uint16_t *Dest)
{
  RRegU2B(Reg, Dest);
  BE2B(Dest);
}

/* ----------------------- */

/* -------------------
 * FUNCTION: RRegBEU3B
 * -------------------
 * Read 3 unsigned Bytes from REG (reversing them)
 * into DEST
 * Rise Error: if error occurred
 *             while reading
 */
static void
RRegBEU3B(uint8_t   Reg,
		  uint32_t  *Dest)
{
  RRegU3B(Reg, Dest);
  BE3B(Dest);
}

/* ----------------------- */

/* -------------------
 * FUNCTION: RRegBES2B
 * -------------------
 * Read 2 signed Bytes from REG (reversing them)
 * into DEST
 * Rise Error: if error occurred
 *             while reading
 */
static void
RRegBES2B(uint8_t  Reg,
		  int16_t  *Dest)
{
  RRegS2B(Reg, Dest);
  BE2B(Dest);
}

/* ----------------------- */

/* -------------------
 * FUNCTION: RRegBES3B
 * -------------------
 * Read 3 signed Bytes from REG (reversing them)
 * into DEST
 * Rise Error: if error occurred
 *             while reading
 */
static void
RRegBES3B(uint8_t   Reg,
		  int32_t   *Dest)
{
  RRegS3B(Reg, Dest);
  BE3B(Dest);
}

/* ----------------------- */

/* -----------------
 * FUNCTION: RStatus
 * -----------------
 * Read BME280 status into DST
 */
static void
RStatus(uint8_t *Dst)
{
  RRegU1B(BME280_REGISTER_STATUS, Dst);
}

/* ----------------------- */

/* ----------------
 * FUNCTION: Check1
 * ----------------
 * Check if STM32F411 can communicate
 * with BME280 through I2C
 * Rise Error: if not
 */
static void
Check1()
{
  uint8_t sensor_id = 0;
  RRegU1B(BME280_REG_ID, &sensor_id);
  if (sensor_id != BME280_ID)
	  Error();
}

/* ----------------------- */

/* ----------------
 * FUNCTION: Check2
 * ----------------
 * Check if configuration data
 * was successfully written
 * Rise Error: if not
 */
static void
Check2()
{
  uint8_t overs_tp = 0;
  uint8_t overs_h  = 0;
  RRegU1B(BME280_REG_CTRL_MEAS, &overs_tp);
  RRegU1B(BME280_REG_CTRL_HUM,  &overs_h);
  if (!(overs_tp & BME280_OSRS_T_MASK))
	  Error();
  if (!(overs_tp & BME280_OSRS_P_MASK))
	  Error();
  if (!(overs_h  & BME280_OSRS_H_MASK))
	  Error();
}

/* ----------------------- */

/* ---------------
 * FUNCTION: Reset
 * ---------------
 * Reset BME280 and make it
 * ready to be configurated
 */
static void
Reset()
{
	uint8_t reset_val = BME280_SOFTRESET_VALUE;
	uint8_t status = BME280_STATUS_NOT_UPDATED;
	WRegU1B(BME280_REG_SOFTRESET, &reset_val);
	while (status & BME280_STATUS_NOT_UPDATED)
		RStatus(&status);
}

/* ----------------------- */

/* ---------------
 * FUNCTION: SStnb
 * ---------------
 * Set Stand_by mode
 */
static void
SStnb(uint8_t stnb)
{
  uint8_t reg = 0;
  RRegU1B(BME280_REG_CONFIG, &reg);
  reg &= ~BME280_STBY_MASK;
  reg |= stnb & BME280_STBY_MASK;
  WRegU1B(BME280_REG_CONFIG, &reg);
}

/* ----------------------- */

/* ---------------
 * FUNCTION: SStnb
 * ---------------
 * Set Working Mode
 */
static void
SMode(uint8_t mode)
{
  uint8_t reg = 0;
  RRegU1B(BME280_REG_CTRL_MEAS, &reg);
  reg &= ~BME280_MODE_MASK;
  reg |= mode & BME280_MODE_MASK;
  WRegU1B(BME280_REG_CTRL_MEAS, &reg);
}

/* ----------------------- */

/* ----------------
 * FUNCTION: Filter
 * ----------------
 * Set Filtering
 */
static void
Filter(uint8_t filter)
{
  uint8_t reg = 0;
  RRegU1B(BME280_REG_CONFIG, &reg);
  reg &= ~BME280_FILTER_MASK;
  reg |= filter & BME280_FILTER_MASK;
  WRegU1B(BME280_REG_CONFIG, &reg);
}

/* ----------------------- */

/* ----------------
 * FUNCTION: OverST
 * ----------------
 * Set Over-sampling for Temperature
 */
static void
OverST(uint8_t overs)
{
  uint8_t reg = 0;
  RRegU1B(BME280_REG_CTRL_MEAS, &reg);
  reg = reg & ~BME280_OSRS_T_MASK;
  reg |= overs & BME280_OSRS_T_MASK;
  WRegU1B(BME280_REG_CTRL_MEAS, &reg);
}

/* ----------------------- */

/* ----------------
 * FUNCTION: OverSP
 * ----------------
 * Set Over-sampling for Pressure
 */
static void
OverSP(uint8_t overs)
{
  uint8_t reg = 0;
  RRegU1B(BME280_REG_CTRL_MEAS, &reg);
  reg &= ~BME280_OSRS_P_MASK;
  reg |= overs & BME280_OSRS_P_MASK;
  WRegU1B(BME280_REG_CTRL_MEAS, &reg);
}

/* ----------------------- */

/* ----------------
 * FUNCTION: OverSH
 * ----------------
 * Set Over-sampling for Humidity
 */
static void
OverSH(uint8_t overs)
{
  uint8_t reg = 0;
  RRegU1B(BME280_REG_CTRL_HUM, &reg);
  reg &= ~BME280_OSRS_H_MASK;
  reg |= overs & BME280_OSRS_H_MASK;
  WRegU1B(BME280_REG_CTRL_HUM, &reg);
  RRegU1B(BME280_REG_CTRL_MEAS, &reg);
  WRegU1B(BME280_REG_CTRL_MEAS, &reg);
}
/* ----------------------- */

/* ---------------
 * FUNCTION: Clbrt
 * ---------------
 * Calibrate BME280
 */
static void
Clbrt(uint8_t filter,
	  uint8_t overst,
	  uint8_t oversp,
	  uint8_t oversh)
{
	Filter(filter);
	OverST(overst);
	OverSP(oversp);
	OverSH(oversh);
}

/* ----------------------- */

/* ---------------
 * FUNCTION: RCofs
 * ---------------
 * Read coefficients into COEFFS
 */
static void
RCofs()
{
  uint8_t part_1 = 0;
  uint8_t part_2 = 0;

  RRegU2B(BME280_REGISTER_DIG_T1, &coeffs.dig_T1);
  RRegS2B(BME280_REGISTER_DIG_T2, &coeffs.dig_T2);
  RRegS2B(BME280_REGISTER_DIG_T3, &coeffs.dig_T3);

  RRegU2B(BME280_REGISTER_DIG_P1, &coeffs.dig_P1);
  RRegS2B(BME280_REGISTER_DIG_P2, &coeffs.dig_P2);
  RRegS2B(BME280_REGISTER_DIG_P3, &coeffs.dig_P3);
  RRegS2B(BME280_REGISTER_DIG_P4, &coeffs.dig_P4);
  RRegS2B(BME280_REGISTER_DIG_P5, &coeffs.dig_P5);
  RRegS2B(BME280_REGISTER_DIG_P6, &coeffs.dig_P6);
  RRegS2B(BME280_REGISTER_DIG_P7, &coeffs.dig_P7);
  RRegS2B(BME280_REGISTER_DIG_P8, &coeffs.dig_P8);
  RRegS2B(BME280_REGISTER_DIG_P9, &coeffs.dig_P9);

  RRegU1B(BME280_REGISTER_DIG_H1, &coeffs.dig_H1);
  RRegS2B(BME280_REGISTER_DIG_H2, &coeffs.dig_H2);
  RRegU1B(BME280_REGISTER_DIG_H3, &coeffs.dig_H3);

  RRegU1B(BME280_REGISTER_DIG_H4,   &part_1);
  RRegU1B(BME280_REGISTER_DIG_H4+1, &part_2);
  coeffs.dig_H4 = (part_1 << 4) | (part_2 & 0xF);

  RRegU1B(BME280_REGISTER_DIG_H5+1, &part_1);
  RRegU1B(BME280_REGISTER_DIG_H5,   &part_2);
  coeffs.dig_H5 = (part_1 << 4) | (part_2 >> 4);

  RRegS1B(BME280_REGISTER_DIG_H6, &coeffs.dig_H6);
}

/* ----------------------- */

/* ----------------------
 * FUNCTION: RTemperature
 * ----------------------
 * Read Temperature
 */
float RTemperature()
{
  int32_t t_fine;
  float temper_float = 0.0f;
  int32_t adc_T, var1, var2;
  RRegBES3B(BME280_REGISTER_TEMPDATA, &adc_T);
  adc_T >>= 4;
  var1 = ((((adc_T>>3) - ((int32_t)coeffs.dig_T1 <<1))) *
		 ((int32_t)coeffs.dig_T2)) >> 11;
  var2 = (((((adc_T>>4) - ((int32_t)coeffs.dig_T1)) *
		 ((adc_T>>4) - ((int32_t)coeffs.dig_T1))) >> 12) *
		 ((int32_t)coeffs.dig_T3)) >> 14;
  t_fine = var1 + var2;
  temper_float = ((t_fine * 5 + 128) >> 8);
  temper_float /= 100.0f;
  return temper_float;
}

/* ----------------------- */

/* -------------------
 * FUNCTION: RPressure
 * -------------------
 * Read Pressure
 */
float RPressure()
{
  float press_float = 0.0f;
  int32_t press_raw;
  int64_t var1, var2, p;
  /* We need to have temperature measurement */
  /* In order to calculate pressure          */
  RRegBES3B(BME280_REGISTER_PRESSUREDATA,&press_raw);
  press_raw >>= 4;
  var1 = ((int64_t) t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)coeffs.dig_P6;
  var2 = var2 + ((var1 * (int64_t)coeffs.dig_P5) << 17);
  var2 = var2 + ((int64_t)coeffs.dig_P4 << 35);
  var1 = ((var1 * var1 * (int64_t)coeffs.dig_P3) >> 8) + ((var1 * (int64_t)coeffs.dig_P2) << 12);
  var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)coeffs.dig_P1) >> 33;
  if (var1 == 0)
	  /* do NOT even try to DIVIDE by ZERO */
      return 0;
  p = 1048576 - press_raw;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)coeffs.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)coeffs.dig_P8) * p) >> 19;
  p = ((p + var1 + var2) >> 8) + ((int64_t)coeffs.dig_P7 << 4);
  p = ((p >> 8) * 1000) + (((p & 0xff) * 390625) / 100000);
  press_float = p / 100.0f;
  return press_float;
}

/* ----------------------- */

/* -------------------
 * FUNCTION: RHumidity
 * -------------------
 * Read Humidity
 */
float RHumidity()
{
  float hum_float = 0.0f;
  int32_t v_x1_u32r;
  int16_t adc_H;
  RRegBES2B(BME280_REGISTER_HUMIDDATA, &adc_H);
  v_x1_u32r = t_fine - (int32_t) 76800;
  v_x1_u32r = (((((adc_H << 14) - (((int32_t)coeffs.dig_H4) << 20) - (((int32_t)coeffs.dig_H5) * v_x1_u32r)) +
		      ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)coeffs.dig_H6)) >> 10) *
		      (((v_x1_u32r * ((int32_t)coeffs.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
		      ((int32_t)2097152)) * ((int32_t)coeffs.dig_H2) + 8192) >> 14));
  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)coeffs.dig_H1)) >> 4));
  v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
  v_x1_u32r = (v_x1_u32r > 419430400? 419430400: v_x1_u32r);
  hum_float = (int32_t)(v_x1_u32r>>12) / 1024.0f;
  return hum_float;
}
/* STATIC DEFINITION END   */



/* FUNCTION DEFINITION BEGIN */
/* ---------------------
 * FUNCTION: BME280_Init
 * ---------------------
 * Initialize and calibrate
 * BME280
 * If all is OK, turn Green LED ON
 *
 */
void
BME280_Init(void)
{
  Check1();
  Reset();
  RCofs();
  SStnb(BME280_STBY_1000);
  Clbrt(BME280_FILTER_4,
		BME280_OSRS_T_x4,
		BME280_OSRS_P_x2,
		BME280_OSRS_H_x1);
  SMode(BME280_MODE_NORMAL);
  Check2();
  SUCCESS_ON;
}

/* ----------------------- */

/* ------------------------
 * FUNCTION: BME280_COLLECT
 * ------------------------
 * Read BME280 measurements into BME-280_D
 */
void
BME280_COLLECT()
{
  float temperature = RTemperature();
  bme_280_d.tmpr_f  = temperature;
  sprintf(bme_280_d.tmpr_s, "<> Temperature: %.3f *C\r\n", temperature);

  float humidity    = RHumidity();
  bme_280_d.hmdt_f  = humidity;
  sprintf(bme_280_d.hmdt_s, "<> Humidity:    %.3f mmHg\r\n", humidity);

  float pressure    = RPressure() * 0.000750061683f;
  bme_280_d.prsr_f  = pressure;
  sprintf(bme_280_d.prsr_s, "<> Pressure:    %.3f %%\r\n", pressure);
}
/* FUNCTION DEFINITION END   */
