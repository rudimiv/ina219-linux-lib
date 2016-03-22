#include <stdio.h>
#include "ina219.h"

int main(int argc, char *argv[])
{

	//i2c adapter number
	int ina_219_device_i2c_adapter_nr = 1;
	char ina_219_device_filename[INA_219_DEVICE_FILENAME_SIZE];

	snprintf(ina_219_device_filename, INA_219_DEVICE_FILENAME_SIZE, "/dev/i2c-%d", ina_219_device_i2c_adapter_nr);


	ina_219_device *dev = ina_219_device_open(ina_219_device_filename ,INA_219_DEVICE_I2C_ADDR);

	ina_219_device_config(dev, INA_219_DEVICE_BUS_VOLTAGE_RANGE_32 |
						INA_219_DEVICE_GAIN_8 |
						INA_219_DEVICE_BADC_12_BIT_1_AVERAGE |
						INA_219_DEVICE_SADC_12_BIT_1_AVERAGE |
						INA_219_DEVICE_MODE_SHUNT |
						INA_219_DEVICE_MODE_BUS);

	ina_219_device_calibrate(dev, 0.1, 3.2);

	printf("bus: %.10lf\n", ina_219_device_get_bus_voltage(dev));
	printf("shunt: %.10lf\n", ina_219_device_get_shunt_voltage(dev));
	printf("curr: %.10lf\n", ina_219_device_get_current(dev));
	ina_219_device_close(dev);
	return 0;
}