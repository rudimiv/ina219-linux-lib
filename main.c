#include <stdio.h>
#include <unistd.h>
#include "ina219.h"
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define KEYS_PARSE_STRING "d:k:pcvtah"
#define KEY_ON 1
#define KEY_OFF 0


typedef uint8_t key_flag;

void ina219_tool_print_help()
{
	printf("simple utilit for communication with one or more ina219 devices\n");
	printf("ina219-tool [-v] [-c] [-p] [-a] [-k {koeff}] {-d {filename}} \
{device_addr_1 [device_addr_2 ... [device_addr_n]]}\n\n");
	printf("-v print voltage\n");
	printf("-c print current\n");
	printf("-p print power\n");
	printf("-a print voltage, current and power\n");
	printf("-k VALUE multiply current on the VALUE\n");
	printf("-d FILENAME device file for access to i2c bus\n");
	printf("device_addr_1 [device_addr_2 ... [device_addr_n]] ina219 device \
i2c addresses. Usually 0x40, 0x41, 0x42, 0x43\n");
}
int main(int argc, char *argv[])
{
	int argc_p;
	int opt;
	unsigned i2c_device_number;
	key_flag time_flag, voltage_flag, current_flag, power_flag;

	time_flag = voltage_flag = current_flag = power_flag = KEY_OFF;

	int ina_219_device_i2c_adapter_nr = 0;
	char ina_219_device_filename[INA_219_DEVICE_FILENAME_SIZE];
	double current_device_koeff = 1.0;
	//snprintf(ina_219_device_filename, INA_219_DEVICE_FILENAME_SIZE, "/dev/i2c-%d", ina_219_device_i2c_adapter_nr);snprintf(ina_219_device_filename, INA_219_DEVICE_FILENAME_SIZE, "/dev/i2c-%d", ina_219_device_i2c_adapter_nr);


	while ((opt = getopt(argc, argv, KEYS_PARSE_STRING)) != -1) {
		switch (opt) {
			case 'h':
				ina219_tool_print_help();
				return 0;
				break;
			case 'd':
				strncpy(ina_219_device_filename, optarg, INA_219_DEVICE_FILENAME_SIZE);
				break;
			case 'k':
				sscanf(optarg, "%lf", &current_device_koeff);
				break;
			case 't':
				time_flag = KEY_ON;
				break;
			case 'a':
				voltage_flag = KEY_ON;
				current_flag = KEY_ON;
				power_flag = KEY_ON;
				break;
			case 'v':
				voltage_flag = KEY_ON;
				break;
			case 'c':
				current_flag = KEY_ON;
				break;
			case 'p':
				power_flag = KEY_ON;
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
		
		if (!dev) {
			printf("file error\n");

			return 0;
		}


		ina_219_device_config(dev, INA_219_DEVICE_BUS_VOLTAGE_RANGE_32 |
							INA_219_DEVICE_GAIN_8 |
							INA_219_DEVICE_BADC_12_BIT_1_AVERAGE |
							INA_219_DEVICE_SADC_12_BIT |
							INA_219_DEVICE_MODE_SHUNT |
							INA_219_DEVICE_MODE_BUS);

		ina_219_device_calibrate(dev, 0.1, 1.0);

		printf("%x: ", i2c_device_number);

		double voltage = ina_219_device_get_bus_voltage(dev);
		double current = ina_219_device_get_current(dev);
		if (voltage_flag == KEY_ON) {
			printf("%.03lf V ", voltage);
		}
		if (current_flag == KEY_ON) {
			printf("%.03lf A ", current_device_koeff * current);
		}

		if (power_flag == KEY_ON) {
			printf("%.03lf W ", current_device_koeff * current * voltage);
		}
		ina_219_device_close(dev);
		argc_p++;
		printf("\n");
	}
}