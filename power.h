#include <psp2/power.h>

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>	//atoi

/** Battery **/
int scePowerIsBatteryExist();
int scePowerGetBatteryTemp();
int scePowerGetBatteryVolt();
int scePowerGetBatterySOH();

/** Processor Clock **/
int scePowerGetGpuXbarClockFrequency();
int scePowerSetGpuXbarClockFrequency(int freq);

int getClockFrequency();
void setClockFrequency();
void setProfile();


int getBatteryRemCapacity();
int getBatteryCapacity();
int getBatteryLifeTime();
char* getBatteryPercentage();
char* getBatteryVoltage();
char* getBatteryTempInCelsius();
char* getBatteryTempInFahrenheit();
const int getBatteryStatus();
