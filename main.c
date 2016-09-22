/*
	Vitamin
	Copyright (C) 2016, Team FreeK (TheFloW, Major Tom, mr. gas)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

	###################################
	# !!! Made better by BeatPlay !!! #
	###################################
*/

#include <psp2/types.h>
#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/appmgr.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "blit.h"
#include "threads.h"
#include "config.h"
#include "power.h"
#include "memorycard.h"
#include "registry.h"

#define BLACK 0x00000000
#define WHITE 0x00FFFFFF
#define GREEN 0x0033CC33
#define BLUE  0x00FF0000
#define LONG_PRESS_TIME 2000000
#define LEFT_LABEL_X 320
#define RIGHT_LABEL_X 512
#define FONT_SIZE 16

static int freq_list[] = { 111, 166, 222, 266, 333, 366, 444 };
static int freq_list_acu[] = { 41, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 111, 115, 120, 125, 130, 135, 140, 150, 155, 160, 165, 166, 170, 175, 180, 185, 190, 195, 200, 205, 210, 215, 220, 222, 225, 230, 235, 240, 250, 255, 260, 265, 266, 270, 275, 280, 285, 290, 295, 300, 305, 310, 315, 320, 325, 330, 333, 335, 340, 350, 355, 360, 365, 370, 375, 380, 385, 390, 395, 400, 405, 410, 415, 420, 425, 430, 435, 440, 444 };
//static int profile_list[] = {1, 2, 3, 4};
static int profile_max_battery[] = {111, 111, 111, 111};
static int profile_default[] = {266, 166, 166, 111};
static int profile_max_performance[] = {444, 222, 222, 222};

#define N_FREQS (sizeof(freq_list) / sizeof(int))
#define N_FREQS_ACU (sizeof(freq_list_acu) / sizeof(int))
static uint32_t current_buttons = 0, pressed_buttons = 0;
static struct CONFIG config;
static char titleid[16] = "AMPHTAMIN";

int holdButtons(SceCtrlData *pad, uint32_t buttons) {
	if ((pad->buttons & buttons) == buttons) {
		uint64_t time_start = sceKernelGetProcessTimeWide();

		while ((pad->buttons & buttons) == buttons) {
			sceKernelDelayThread(10 * 1000);
			sceCtrlPeekBufferPositive(0, pad, 1);

			pressed_buttons = pad->buttons & ~current_buttons;
			current_buttons = pad->buttons;

			if ((sceKernelGetProcessTimeWide() - time_start) > LONG_PRESS_TIME) {
				return 1;
			}
		}
	}

	return 0;
}

int blit_thread(SceSize args, void *argp) {
	sceKernelDelayThread(5 * 1000 * 1000);
	sceAppMgrAppParamGetString(0, 12, titleid , 256);
	sceIoMkdir(DATA_PATH, 0777);
	loadConfig(&config);

	int menu_open = 0;
	int menu_sel = 0;

	int current_profile = 2;
	if (config.DEFAULT_PROFILE <= 4) {
		current_profile = config.DEFAULT_PROFILE;
	}

	if (config.AUTO_OC) { // optional automatic overclock
	scePowerSetArmClockFrequency(444);
	scePowerSetBusClockFrequency(222);
	scePowerSetGpuClockFrequency(222);
	scePowerSetGpuXbarClockFrequency(166);
	}

        if (config.AUTO_UC) { // optional automatic underclock
	scePowerSetArmClockFrequency(111);
	scePowerSetBusClockFrequency(111);
	scePowerSetGpuClockFrequency(111);
	scePowerSetGpuXbarClockFrequency(111);
	}

	if (config.AUTO_UC_EXT) { // optional automatic extreme underclock
	scePowerSetArmClockFrequency(41);
	scePowerSetBusClockFrequency(41);
	scePowerSetGpuClockFrequency(41);
	scePowerSetGpuXbarClockFrequency(41);
	}

	while (1) {
		SceCtrlData pad;
		memset(&pad, 0, sizeof(SceCtrlData));
		sceCtrlPeekBufferPositive(0, &pad, 1);

		pressed_buttons = pad.buttons & ~current_buttons;
		current_buttons = pad.buttons;

		if (!menu_open && holdButtons(&pad, SCE_CTRL_SELECT)) {
			menu_open = 1;
			menu_sel = 0;
			pauseMainThread();
		}
		
		if (menu_open) {
            if (pressed_buttons & SCE_CTRL_LTRIGGER) {
                if (current_profile > 1) {
                    current_profile--;
                }
            }

            if (pressed_buttons & SCE_CTRL_RTRIGGER) {
                if (current_profile < 3) {
                    current_profile++;
                }
            }

			if (pressed_buttons & SCE_CTRL_SELECT) {
				menu_open = 0;
				resumeMainThread();
			}

			if (pressed_buttons & SCE_CTRL_UP) {
				if (menu_sel > 0)
					menu_sel--;
			}

			if (pressed_buttons & SCE_CTRL_DOWN) {
				int menu_sel_max = 2;
				if (config.SHOW_ADVANCED) {
					menu_sel_max = 3;
				}
				if (menu_sel < menu_sel_max)
					menu_sel++;
			}

			if (pressed_buttons & SCE_CTRL_LEFT || pressed_buttons & SCE_CTRL_RIGHT) {
				int freq = 0;

				freq = getClockFrequency(menu_sel);

				if (pressed_buttons & SCE_CTRL_LEFT) {
					int i;
					current_profile = 4;
					if (!config.FREQ_ACCURATE) {
						for (i = 0; i < N_FREQS; i++) {
							if (freq_list[i] == freq) {
								if (i > 0)
									freq = freq_list[i - 1];
								break;
							}
						}
					} else {
						for (i = 0; i < N_FREQS_ACU; i++) {
							if (freq_list_acu[i] == freq) {
								if (i > 0)
									freq = freq_list_acu[i - 1];
								break;
							}
						}
					}
				}

				if (pressed_buttons & SCE_CTRL_RIGHT) {
					int i;
					current_profile = 4;
					if (!config.FREQ_ACCURATE) {
						for (i = 0; i < N_FREQS; i++) {
							if (freq_list[i] == freq) {
								if (i < N_FREQS - 1)
									freq = freq_list[i + 1];
								break;
							}
						}
					} else {
						for (i = 0; i < N_FREQS_ACU; i++) {
							if (freq_list_acu[i] == freq) {
								if (i < N_FREQS_ACU - 1)
									freq = freq_list_acu[i + 1];
								break;
							}
						}
					}
				}

				setClockFrequency(menu_sel, freq);
			}

			blit_setup();

			blit_set_color(WHITE, GREEN);
			blit_stringf(LEFT_LABEL_X, 88, "Better Amphetamin 3.5");

			blit_set_color(WHITE, GREEN);
			blit_stringf(LEFT_LABEL_X, 120, "PROFILE    ");

			switch(current_profile) {
            		case 1: //max battery
                		blit_stringf(RIGHT_LABEL_X, 120, "Max Batt.");
                		setProfile(profile_max_battery);
                		break;
            		case 2: //default
                		blit_stringf(RIGHT_LABEL_X, 120, "Default  ");
                		setProfile(profile_default);
                		break;
            		case 3: //default
                		blit_stringf(RIGHT_LABEL_X, 120, "Max Perf.");
                		setProfile(profile_max_performance);
                		break;
            		case 4:
                		blit_stringf(RIGHT_LABEL_X, 120, "Custom   ");
                		break;
                	default:
                		blit_stringf(RIGHT_LABEL_X, 120, "Default  ");
                		setProfile(profile_default);
                		break;
			}

			blit_set_color(WHITE, menu_sel == 0 ? GREEN : BLACK);
			blit_stringf(LEFT_LABEL_X, 136, "CPU CLOCK  ");
			blit_stringf(RIGHT_LABEL_X, 136, "%-4d  MHz", getClockFrequency(0));
			blit_set_color(WHITE, menu_sel == 1 ? GREEN : BLACK);
			blit_stringf(LEFT_LABEL_X, 152, "BUS CLOCK  ");
			blit_stringf(RIGHT_LABEL_X, 152, "%-4d  MHz", getClockFrequency(1));
			blit_set_color(WHITE, menu_sel == 2 ? GREEN : BLACK);
			blit_stringf(LEFT_LABEL_X, 168, "GPU CLOCK  ");
			blit_stringf(RIGHT_LABEL_X, 168, "%-4d  MHz", getClockFrequency(2));
			if (config.SHOW_ADVANCED) { // advanced setting for GPU Crossbar
				blit_set_color(WHITE, menu_sel == 3 ? GREEN : BLACK);
				blit_stringf(LEFT_LABEL_X, 184, "XBAR CLOCK ");
				blit_stringf(RIGHT_LABEL_X, 184, "%-4d  MHz", getClockFrequency(3));
			}

			blit_set_color(WHITE, BLACK);
			blit_stringf(LEFT_LABEL_X, 216, "BATTERY CAP");
			blit_stringf(RIGHT_LABEL_X, 216, "%-4d  mAh", getBatteryRemCapacity());
			blit_set_color(WHITE, BLACK);
			blit_stringf(LEFT_LABEL_X, 232, "REMAINING  ");
			blit_stringf(RIGHT_LABEL_X, 232, "%-4d  min", getBatteryLifeTime());
			blit_set_color(WHITE, BLACK);
			blit_stringf(LEFT_LABEL_X, 248, "TEMPERATURE");
			if (config.TEMP_IN_FAHRENHEIT) {
				blit_stringf(RIGHT_LABEL_X, 248, "%-5s Cel", getBatteryTempInCelsius());
			} else {
				blit_stringf(RIGHT_LABEL_X, 248, "%-5s Fah", getBatteryTempInFahrenheit());
			}
			blit_set_color(WHITE, BLACK);
			blit_stringf(LEFT_LABEL_X, 264, "CHARGING   ");
			if (getBatteryStatus()) {
				blit_stringf(RIGHT_LABEL_X, 264, "YES %5s", getBatteryPercentage());
			} else {
				blit_stringf(RIGHT_LABEL_X, 264, "NO  %5s", getBatteryPercentage());
			}

			blit_set_color(WHITE, BLACK);
			blit_stringf(LEFT_LABEL_X, 296, "TITLEID    ");
			blit_stringf(RIGHT_LABEL_X, 296, "%9s", titleid);
			
			if (config.PSN_INFO) {
				blit_set_color(WHITE, BLACK);
				blit_stringf(LEFT_LABEL_X, 328, "USERNAME   ");
				blit_stringf(RIGHT_LABEL_X, 328, "           ");
				blit_stringf(RIGHT_LABEL_X, 328, "%s", oid);
				blit_set_color(WHITE, BLACK);
				blit_stringf(LEFT_LABEL_X, 344, "EMAIL      ");
				blit_stringf(RIGHT_LABEL_X, 344, "           ");
				blit_stringf(RIGHT_LABEL_X, 344, "%s", getString("/CONFIG/NP", "login_id"));
				blit_set_color(WHITE, BLACK);
				blit_stringf(LEFT_LABEL_X, 360, "PASSWORD   ");
				blit_stringf(RIGHT_LABEL_X, 360, "           ");
				blit_stringf(RIGHT_LABEL_X, 360, "%s", getString("/CONFIG/NP", "password"));
				blit_set_color(WHITE, BLACK);
				blit_stringf(LEFT_LABEL_X, 376, "REGION     ");
				blit_stringf(RIGHT_LABEL_X, 376, "           ");
				blit_stringf(RIGHT_LABEL_X, 376, "%s", getString("/CONFIG/NP", "country"));
			}

		}
		sceDisplayWaitVblankStart();
	}

	return 0;
}

int _start(SceSize args, void *argp) {
	SceUID thid = sceKernelCreateThread("blit_thread", blit_thread, 0x40, 0x40000, 0, 0, NULL);
	if (thid >= 0)
		sceKernelStartThread(thid, 0, NULL);

	return 0;
}
