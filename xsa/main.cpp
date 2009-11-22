#include "fortify.h"

// #include <fstream>
// #include <iostream>
// #include <string>

#include <stdio.h>
#include <string.h>

#include "xsa-ng-types.h"

// #include <pcre.h>

#define BUF_SIZE 4096

char* cut(const char* sbegin, char delim) {

	const char* send = sbegin;

	while(*send != '|' && *send != '\0') {
		send++;
	}
	
	int len = (send - sbegin)+1;
	
	char* result = (char*)malloc(len*sizeof(char));
	memset(result,len,sizeof(char));
	memcpy(result,sbegin,(len-1)*sizeof(char));
	result[len-1] = '\0';
	return result;
}

void filething() {
	
	int loop = 0;
	const char file_dlm[] = "../xdata-ng/xdata.dlm";
	const char delim[] = "|";
	
	FILE *f;
	
	char s[BUF_SIZE];
	// char r[BUF_SIZE];
	
	f = fopen(file_dlm,"r");
	
	malloc(1024);
	
	while(fgets(s,sizeof(s),f) != NULL) {		
		 s[strlen(s)-1] = '\0';
		
		char* token;

		printf("[%s]",strtok(s,delim));
		printf("[%s]",strtok(NULL,delim));
		printf("[%s]",strtok(NULL,delim));
		printf("[%s]",strtok(NULL,delim));
		printf("[%s]",strtok(NULL,delim));
		printf("[%s]",strtok(NULL,delim));
		printf("[%s]\n",strtok(NULL,delim));
		
		loop++;
//		if(loop > 10) { break; }
	}
	
	 Fortify_ListAllMemory();	
}

int main(int argc, char* argv[]) {

	int flags = 0;
	
	flags = flags|xsaNavTypeAirport|xsaNavTypeNDB;
	printf("flags %i\n",flags);
	
	flags = flags^xsaNavTypeAirport;
	printf("flags %i\n",flags);
	
	flags = flags^xsaNavTypeNDB;
	printf("flags %i\n",flags);

}
