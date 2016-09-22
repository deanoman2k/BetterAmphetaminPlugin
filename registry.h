#include <stdio.h> 
#include <string.h>

int sceRegMgrGetKeyStr(const char* reg, const char* key, char* str, const int buf_size);
char* getString( const char* reg, const char* key );