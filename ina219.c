#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ina219.h"


//physics constants
//shunt voltage precision = 10 mkV
#define INA_219_DEVICE_SHUNT_VOLTAGE_LSB 1e+05
//bus voltage precision = 4 mV
#define INA_219_DEVICE_BUS_VOLTAGE_LSB 250

static int32_t ina_219_device_read_reg(ina_219_device *dev, uint8_t reg);
static int32_t ina_219_device_write_reg(ina_219_device *dev, uint8_t refg, uint16_t data);

ina_219_device* ina_219_device_open(const char * ina_219_device_filename, uint8_t i2c_device_addr)
{
	ina_219_device *res = (ina_219_device*) malloc(sizeof(ina_219_device));
	//open file-descriptor
	if ((res->fd = open(ina_219_device_filename, O_RDWR)) < 0)
		goto open_i2c_error;

	//set device address
	if (ioctl(res->fd, I2C_SLAVE, i2c_device_addr) < 0)
		goto address_device_error;

	res->shunt_on = 0;
	res->bus_on = 0;

	return res;

address_device_error:
	printf("init communication with device error\n");
open_i2c_error:
	printf("open device error\n");

	free(res);
	exit(1);
}

int ina_219_device_close(ina_219_device *dev)
{
	close(dev->fd);
	free(dev);
}
/*
	shunt_resistor_resistance in Om
	max_current in A
*/
double ina_219_device_get_current(ina_219_device *dev)
{
	int32_t res = ina_219_device_read_reg(dev, INA_219_DEVICE_CURRENT_REG);

	if (res < 0) {
		printf("read current error\n");
		exit(1);
	}

	//не забываем отбросить старшие разряды
	int16_t res_p = (int16_t)res;

	return (double)res_p / dev->current_LSB;
}

double ina_219_device_get_bus_voltage(ina_219_device *dev)
{
	int32_t res;
	int16_t res_p;

	if (!dev->bus_on) {
		printf("warning: bus mode is off\n");
		return 0.0;
	}

	do {
		res = ina_219_device_read_reg(dev, INA_219_DEVICE_BUS_VOLTAGE_REG);

		if (res < 0) {
			printf("read bus voltage error\n");
			exit(1);
		}

		//не забываем отбросить старшие разряды
		res_p = (int16_t)res;
	} while (!(res_p & 0b10u));

	if (res_p & 0b01u) {
		printf("Math Overflow error\n");
		return 0;
	}

	res_p >>= 3;
	return (double)res_p / dev->bus_LSB;
}

double ina_219_device_get_shunt_voltage(ina_219_device *dev)
{
	if (!dev->shunt_on) {
		printf("warning: shunt mode is off\n");
		return 0.0;
	}

	int32_t res = ina_219_device_read_reg(dev, INA_219_DEVICE_SHUNT_VOLTAGE_REG);

	if (res < 0) {
		printf("read shunt voltage error\n");
		exit(1);
	}

	//не забываем отбросить старшие разряды
	int16_t res_p = (int16_t)res;

	return (double)res_p / dev->shunt_LSB;
}

int32_t ina_219_device_calibrate(ina_219_device *dev, double shunt_resistor_resistance, double max_current)
{
	double current_LSB = max_current / 32768;

	unsigned degree = 1;

	while ((int)current_LSB < 1 ) {
		current_LSB *= 10;
		degree *= 10;
	}

	degree = degree >= 10 ? degree / 10 : 1;

	dev->current_LSB = degree;
	dev->bus_LSB = INA_219_DEVICE_BUS_VOLTAGE_LSB;
	dev->shunt_LSB = INA_219_DEVICE_SHUNT_VOLTAGE_LSB;

	current_LSB = 1 / (double) degree;

	double calibration = 0.04096 / (shunt_resistor_resistance * current_LSB);
	uint16_t cal_reg_value = (uint16_t) trunc(calibration);

	return ina_219_device_write_reg(dev, INA_219_DEVICE_CALIBRATION_REG, cal_reg_value);
}

int32_t ina_219_device_config(ina_219_device *dev, uint16_t conf_p)
{
	if (~(conf_p | ~INA_219_DEVICE_MODE_SHUNT))
		dev->shunt_on = 0;
	else
		dev->shunt_on = 1;

	if (~(conf_p | ~INA_219_DEVICE_MODE_BUS))
		dev->bus_on = 0;
	else
		dev->bus_on = 1;

	return ina_219_device_write_reg(dev, INA_219_DEVICE_CONFIG_REG, conf_p);
}

int32_t ina_219_device_reset(ina_219_device *dev)
{
	return ina_219_device_write_reg(dev, INA_219_DEVICE_CONFIG_REG, INA_219_DEVICE_RESET);
}

//read word and swap it
int32_t ina_219_device_read_reg(ina_219_device *dev, uint8_t reg)
{
	int32_t res;

	res = i2c_smbus_read_word_data(dev->fd, reg);

	if (res < 0)
		return -1;

	//swap bytes
	res = ((res << 8) | (res >> 8)) & 0xffff;

	return res;
}

//swap data-word and write it
int32_t ina_219_device_write_reg(ina_219_device *dev, uint8_t reg, uint16_t data)
{
	int32_t res;

	//swap bytes
	data = (data << 8) | (data >> 8);

	res = i2c_smbus_write_word_data(dev->fd, reg, data);

	return res;
}
