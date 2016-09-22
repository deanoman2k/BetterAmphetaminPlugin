#include "memorycard.h"

/** read in ux0:id.dat when available **/
void readIDDAT() {	
	FILE* f1 = fopen("ux0:id.dat", "r");
	
	if (f1 == NULL){
		if (f1 != NULL) fclose(f1);
	} else {
		while (fscanf(f1, "%s", buff) == 1) {
			convert_dat(buff);
		}	
		fclose(f1);
	}	
}

void convert_dat(char* buff){

	char mid_string[] = "MID=";
	char dig_string[] = "DIG=";
	char did_string[] = "DID=";
	char aid_string[] = "AID=";
	char oid_string[] = "OID=";
	char svr_string[] = "SVR=";
	
	char delimiter[] = "=";
	char *ptr;
	
	//printf("buffer: %s\n", buff);
	
	if(strstr(buff, mid_string)) {	
		//printf("needle found!");
		ptr = strtok(buff, delimiter);
		//printf("prefix %s\n", ptr);
		ptr = strtok(NULL, delimiter);	//next part..
		//printf("mid found %s\n", ptr);
		strncpy(mid, ptr, 50);
		//printf("Mid is %s\n", ptr);
		
	} else if(strstr(buff, dig_string)) {
		ptr = strtok(buff, delimiter);
		ptr = strtok(NULL, delimiter);	
		strncpy(dig, ptr, 50);		
		
	} else if(strstr(buff, did_string)) {
		ptr = strtok(buff, delimiter);
		ptr = strtok(NULL, delimiter);	
		strncpy(did, ptr, 50);		
		
	} else if(strstr(buff, aid_string)) {
		ptr = strtok(buff, delimiter);
		ptr = strtok(NULL, delimiter);	
		strncpy(aid, ptr, 50);		
		
	} else if(strstr(buff, oid_string)) {
		ptr = strtok(buff, delimiter);
		ptr = strtok(NULL, delimiter);	
		strncpy(oid, ptr, 50);		
		
	} else if(strstr(buff, svr_string)) {
		ptr = strtok(buff, delimiter);
		ptr = strtok(NULL, delimiter);	
		strncpy(svr, ptr, 50);		
		
	}
}