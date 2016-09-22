#include <psp2/appmgr.h>
#include <psp2/apputil.h>

#include <stdio.h> 
#include <string.h>

/** id.dat **/
char buff[255];
char mid[50];  //unknown
char dig[50];  //unknown
char did[50];  //PSID
char aid[50];  //DRM Account name - or "NP/account_id" in registry
char oid[255]; //username
char svr[50];  //firmware

void readIDDAT();
void convert_dat(char* buff);