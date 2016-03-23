#ifndef H_INA_219
#define H_INA_219

#include <linux/i2c-dev.h>
#include <linux/limits.h>
#include <stdint.h>

#define INA_219_DEVICE_FILENAME_SIZE sizeof("/dev/") + NAME_MAX

//Reg map
#define INA_219_DEVICE_CONFIG_REG 0x00
#define INA_219_DEVICE_SHUNT_VOLTAGE_REG 0x01
#define INA_219_DEVICE_BUS_VOLTAGE_REG 0x02
#define INA_219_DEVICE_POWER_REG 0x03
#define INA_219_DEVICE_CURRENT_REG 0x04
#define INA_219_DEVICE_CALIBRATION_REG 0x05

//config reg values (you can unite this values by means of bit operations)
#define INA_219_DEVICE_RESET 0x8000
#define INA_219_DEVICE_BUS_VOLTAGE_RANGE_32 (0b1 << 13)
#define INA_219_DEVICE_BUS_VOLTAGE_RANGE_16 (0b0 << 13)
#define INA_219_DEVICE_GAIN_1 (0b00 << 11)
#define INA_219_DEVICE_GAIN_2 (0b01 << 11) 
#define INA_219_DEVICE_GAIN_4 (0b10 << 11) 
#define INA_219_DEVICE_GAIN_8 (0b11 << 11)
#define INA_219_DEVICE_BADC_12_BIT_1_AVERAGE (0b1000 << 7)
#define INA_219_DEVICE_BADC_12_BIT_2_AVERAGE (0b1001 << 7)
#define INA_219_DEVICE_BADC_12_BIT_4_AVERAGE (0b1010 << 7)
#define INA_219_DEVICE_BADC_12_BIT_32_AVERAGE (0b1101 << 7)
#define INA_219_DEVICE_BADC_12_BIT_128_AVERAGE (0b1111 << 7)
#define INA_219_DEVICE_SADC_12_BIT (0b0011 << 3)
#define INA_219_DEVICE_SADC_12_BIT_1_AVERAGE (0b1000 << 3)
#define INA_219_DEVICE_SADC_12_BIT_2_AVERAGE (0b1001 << 3)
#define INA_219_DEVICE_SADC_12_BIT_4_AVERAGE (0b1010 << 3)
#define INA_219_DEVICE_SADC_12_BIT_32_AVERAGE (0b1101 << 3)
#define INA_219_DEVICE_SADC_12_BIT_128_AVERAGE (0b1111 << 3)
#define INA_219_DEVICE_MODE_SHUNT 0b101
#define INA_219_DEVICE_MODE_BUS 0b110

typedef struct my_ina_219_device{
	int fd;
	unsigned current_LSB;
	unsigned shunt_LSB;
	unsigned bus_LSB;

	uint8_t shunt_on;
	uint8_t bus_on;
} ina_219_device;


extern ina_219_device* ina_219_device_open(const char * ina_219_device_filename, uint8_t i2c_device_addr);
extern int ina_219_device_close(ina_219_device *dev);

extern int32_t ina_219_device_reset(ina_219_device *dev);
extern int32_t ina_219_device_config(ina_219_device *dev, uint16_t conf_p);
extern int32_t ina_219_device_calibrate(ina_219_device *dev, double shunt_resistor_resistance, double max_current);
extern double ina_219_device_get_current(ina_219_device *dev);
extern double ina_219_device_get_bus_voltage(ina_219_device *dev);
extern double ina_219_device_get_shunt_voltage(ina_219_device *dev);

#endif