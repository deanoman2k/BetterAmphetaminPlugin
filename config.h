#ifndef CONFIG_H
#define CONFIG_H

#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DATA_PATH "ux0:/data/amphetamin"
#define CONFIG_PATH "ux0:/data/amphetamin/config.txt"

typedef struct CONFIG {
	int FREQ_ACCURATE;
	int SHOW_ADVANCED;
	int AUTO_OC;
	int AUTO_UC;
	int AUTO_UC_EXT;
	int DEFAULT_PROFILE;
	int TEMP_IN_FAHRENHEIT;
	int PSN_INFO;
	int PSTV_FEATURES;
	int C1_RED;
	int C1_GREEN;
	int C1_BLUE;
	int C2_RED;
	int C2_GREEN;
	int C2_BLUE;
	int C3_RED;
	int C3_GREEN;
	int C3_BLUE;
	int C4_RED;
	int C4_GREEN;
	int C4_BLUE;
} CONFIG;

int loadConfig(CONFIG *config);

#endif
