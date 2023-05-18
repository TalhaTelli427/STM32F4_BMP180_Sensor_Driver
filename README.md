# STM32F4_BMP180_Sensor_Driver
I wrote a driver for the BMP180 using the HAL library for the STM32F446RE card. Thanks to this driver,temperature, pressure and altitude data can be obtained from the BMP 180 module.
First of all, we read the calibration data in the EEPROM so that we can measure temperature and pressure over the BMP180.
Then we calculate the temperature and pressure data by using the read calibration data. In order to take an altitude measurement, we obtain it by using the pressure data. 
Details can be found in the datasheet of the BMP 180.
