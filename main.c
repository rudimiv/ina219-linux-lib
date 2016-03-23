#include <stdio.h>
#include <unistd.h>
#include "ina219.h"
#include <time.h>
#include <stdint.h>
#include <stdlib.h>

#define DEVICE_I2C_ADDR_1 0x40
#define DEVICE_I2C_ADDR_2 0x41

#define KEYS_PARSE_STRING "cvta"
#define KEY_ON 1
#define KEY_OFF 0


typedef uint8_t key_flag;

int main(int argc, char *argv[])
{
	int argc_p;
	int opt;
	unsigned i2c_device_number;
	key_flag time_flag, voltage_flag, current_flag;

	time_flag = voltage_flag = current_flag = KEY_OFF;

	int ina_219_device_i2c_adapter_nr = 0;
	char ina_219_device_filename[INA_219_DEVICE_FILENAME_SIZE];

	snprintf(ina_219_device_filename, INA_219_DEVICE_FILENAME_SIZE, "/dev/i2c-%d", ina_219_device_i2c_adapter_nr);


	while ((opt = getopt(argc, argv, KEYS_PARSE_STRING)) != -1) {
		switch (opt) {
			case 't':
				time_flag = KEY_ON;
				break;
			case 'a':
				voltage_flag = KEY_ON;
				current_flag = KEY_ON;
				break;
			case 'v':
				voltage_flag = KEY_ON;
				break;
			case 'c':
				current_flag = KEY_ON;
				break;
			case '?':
				printf("invalid parametr\n");
				return 0;
				break;
		};
	}

	if (time_flag) {
		time_t t = time(NULL);
  		struct tm* aTm = localtime(&t);
  		printf("%04d/%02d/%02d %02d:%02d:%02d\t", 
  			aTm->tm_year+1900, 
  			aTm->tm_mon+1,
  			aTm->tm_mday, 
  			aTm->tm_hour,
  			aTm->tm_min, 
  			aTm->tm_sec);
	}

	argc_p = optind;

	while (argc != argc_p) {
		sscanf(argv[argc_p], "%x", &i2c_device_number);
		ina_219_device *dev = ina_219_device_open(ina_219_device_filename, i2c_device_number);
	
		ina_219_device_config(dev, INA_219_DEVICE_BUS_VOLTAGE_RANGE_32 |
							INA_219_DEVICE_GAIN_8 |
							INA_219_DEVICE_BADC_12_BIT_1_AVERAGE |
							INA_219_DEVICE_SADC_12_BIT |
							INA_219_DEVICE_MODE_SHUNT |
							INA_219_DEVICE_MODE_BUS);

		ina_219_device_calibrate(dev, 0.1, 1.0);

		printf("0x%x: ", i2c_device_number);
		if (voltage_flag == KEY_ON) {
			printf("%.05lfv ", ina_219_device_get_bus_voltage(dev));
		}
		if (current_flag == KEY_ON) {
			printf("%.05lfa ", ina_219_device_get_current(dev));
		}

		ina_219_device_close(dev);
		argc_p++;
	}

	printf("\n");
}

/*int main(int argc, char *argv[])
{

	//i2c adapter number
	int ina_219_device_i2c_adapter_nr = 0;
	char ina_219_device_filename[INA_219_DEVICE_FILENAME_SIZE];

	snprintf(ina_219_device_filename, INA_219_DEVICE_FILENAME_SIZE, "/dev/i2c-%d", ina_219_device_i2c_adapter_nr);


	ina_219_device *dev_yellow = ina_219_device_open(ina_219_device_filename ,DEVICE_I2C_ADDR_2);
	ina_219_device *dev_red = ina_219_device_open(ina_219_device_filename ,DEVICE_I2C_ADDR_1);

	ina_219_device_config(dev_yellow, INA_219_DEVICE_BUS_VOLTAGE_RANGE_32 |
						INA_219_DEVICE_GAIN_8 |
						INA_219_DEVICE_BADC_12_BIT_1_AVERAGE |
						INA_219_DEVICE_SADC_12_BIT |
						INA_219_DEVICE_MODE_SHUNT |
						INA_219_DEVICE_MODE_BUS);

	ina_219_device_config(dev_red, INA_219_DEVICE_BUS_VOLTAGE_RANGE_32 |
						INA_219_DEVICE_GAIN_8 |
						INA_219_DEVICE_BADC_12_BIT_1_AVERAGE |
						INA_219_DEVICE_SADC_12_BIT |
						INA_219_DEVICE_MODE_SHUNT |
						INA_219_DEVICE_MODE_BUS);

	ina_219_device_calibrate(dev_yellow, 0.1, 1.0);
	ina_219_device_calibrate(dev_red, 0.1, 1.0);

	printf("yellow: \n");
	print_information(dev_yellow);
	printf("\n\nred: \n");
	print_information(dev_red);

	ina_219_device_close(dev_yellow);
	ina_219_device_close(dev_red);
	return 0;
}*/