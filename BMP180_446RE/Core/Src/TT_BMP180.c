/*
 * TT_BMP180.c
 *
 *  Created on: May 18, 2023
 *      Author: talha
 */
#include "TT_BMP180.h"

// Cal Value
short AC1 = 0;
short AC2 = 0;
short AC3 = 0;
unsigned short AC4 = 0;
unsigned short AC5 = 0;
unsigned short AC6 = 0;
short B1 = 0;
short B2 = 0;
short MB = 0;
short MC = 0;
short MD = 0;
// Calibration Calculate Variables
long UT = 0;
long UP = 0;
long X1 = 0;
long X2 = 0;
long X3 = 0;
long B3 = 0;
long B5 = 0;
unsigned long B4 = 0;
long B6 = 0;
unsigned long B7 = 0;

void TT_BMP180_Init_Read_Cal_Data(void) {
	uint8_t calibDatas[CAL_VALUE_SIZE] = { 0 };

	HAL_I2C_Mem_Read(&hi2c1, BMP_180_READ_ADRESS, CAL_START_ADRESS, 1,
			calibDatas, CAL_VALUE_SIZE, 1000);
	AC1 = ((calibDatas[0] << 8) | calibDatas[1]);
	AC2 = ((calibDatas[2] << 8) | calibDatas[3]);
	AC3 = ((calibDatas[4] << 8) | calibDatas[5]);
	AC4 = ((calibDatas[6] << 8) | calibDatas[7]);
	AC5 = ((calibDatas[8] << 8) | calibDatas[9]);
	AC6 = ((calibDatas[10] << 8) | calibDatas[11]);
	B1 = ((calibDatas[12] << 8) | calibDatas[13]);
	B2 = ((calibDatas[14] << 8) | calibDatas[15]);
	MB = ((calibDatas[16] << 8) | calibDatas[17]);
	MC = ((calibDatas[18] << 8) | calibDatas[19]);
	MD = ((calibDatas[20] << 8) | calibDatas[21]);

}

float TT_Get_Temp_Float(void) {
	uint8_t Temp_read[2] = { 0 };
	uint8_t Temp_write_data[1];
	Temp_write_data[0] = 0x2E;
	float T;

	HAL_I2C_Mem_Write(&hi2c1, BMP_180_WRITE_ADRESS, TEMP_AND_PRES_WRITE_REG, 1,
			Temp_write_data, 1, 10000);
	HAL_Delay(5);
	HAL_I2C_Mem_Read(&hi2c1, BMP_180_READ_ADRESS, READ_TEMP_AND_PRES_REG, 1,
			Temp_read, 2, 1000);
	UT = (Temp_read[0] << 8) | Temp_read[1];
	X1 = (UT - AC6) * AC5 / pow(2, 15);
	X2 = MC * pow(2, 11) / (X1 + MD);
	B5 = X1 + X2;
	T = (B5 + 8) / pow(2, 4);
	return T / 10.0;
}
int TT_Get_Temp_Int(void) {
	return (int) (TT_Get_Temp_Float());
}

float TT_Get_Pressure(int oss) {
	int Wait_time;
	float Press;
	uint8_t pressure_val[3] = { 0 };
	uint8_t pressure_write_data[1] = { 0 };
	pressure_write_data[0] = 0x34 | (oss << 6);
	switch (oss) {
	case (0):
		Wait_time = 5;
		break;
	case (1):
		Wait_time = 8;
		break;
	case (2):
		Wait_time = 14;
		break;
	case (3):
		Wait_time = 26;
		break;
	}
	HAL_I2C_Mem_Write(&hi2c1, BMP_180_WRITE_ADRESS,
	TEMP_AND_PRES_WRITE_REG, 1, pressure_write_data, 1, 1000);
	HAL_Delay(Wait_time);
	HAL_I2C_Mem_Read(&hi2c1, BMP_180_READ_ADRESS, READ_TEMP_AND_PRES_REG, 1,
			pressure_val, 3, 1000);
	UP = (pressure_val[0] << 16 | pressure_val[1] << 8 | pressure_val[0])
			>> (8 - (uint8_t) oss);

	X1 = ((UT - AC6) * (AC5 / (pow(2, 15))));
	X2 = ((MC * (pow(2, 11))) / (X1 + MD));
	B5 = X1 + X2;
	B6 = B5 - 4000;
	X1 = (B2 * (B6 * B6 / (pow(2, 12)))) / (pow(2, 11));
	X2 = AC2 * B6 / (pow(2, 11));
	X3 = X1 + X2;
	B3 = (((AC1 * 4 + X3) << oss) + 2) / 4;
	X1 = AC3 * B6 / pow(2, 13);
	X2 = (B1 * (B6 * B6 / (pow(2, 12)))) / (pow(2, 16));
	X3 = ((X1 + X2) + 2) / pow(2, 2);
	B4 = AC4 * (unsigned long) (X3 + 32768) / (pow(2, 15));
	B7 = ((unsigned long) UP - B3) * (50000 >> oss);
	if (B7 < 0x80000000)
		Press = (B7 * 2) / B4;
	else
		Press = (B7 / B4) * 2;
	X1 = (Press / (pow(2, 8))) * (Press / (pow(2, 8)));
	X1 = (X1 * 3038) / (pow(2, 16));
	X2 = (-7357 * Press) / (pow(2, 16));
	Press = Press + (X1 + X2 + 3791) / (pow(2, 4));
	return Press;
}

float TT_Get_Altitude(int altitude_oss) {
	float P = TT_Get_Pressure(altitude_oss);
	return 44330 * (1 - (pow((P / (float) Po), 1 / 5.255)));
}

