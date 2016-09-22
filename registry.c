#include "registry.h"

char* getString( const char* reg, const char* key ) {
	int ret = 0;
	static char string[128];
	ret = sceRegMgrGetKeyStr(reg, key, string, sizeof(string)); 
	return string;
}
