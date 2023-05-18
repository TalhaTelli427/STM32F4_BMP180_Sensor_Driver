/*
 * TT_BMP180.h
 *
 *  Created on: May 18, 2023
 *      Author: talha
 */

#ifndef INC_TT_BMP180_H_
#define INC_TT_BMP180_H_

#include "stm32f4xx_hal.h"
#include <math.h>

extern I2C_HandleTypeDef hi2c1;

#define BMP_180_READ_ADRESS 0xEF
#define BMP_180_WRITE_ADRESS 0xEE
#define CAL_VALUE_SIZE 22
#define CAL_START_ADRESS 0xAA
#define TEMP_AND_PRES_WRITE_REG 0xF4
#define READ_TEMP_AND_PRES_REG 0xF6
#define Po 101325

void TT_BMP180_Init_Read_Cal_Data(void);

float T_Get_Temp_Float(void);
int TT_Get_Temp_Int(void);
float TT_Get_Pressure(int oss);
float TT_Get_Altitude(int altitude_oss);






#endif /* INC_TT_BMP180_H_ */
