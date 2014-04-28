/*
 * eHutils.cpp
 *
 * Hang Su
 * 2012-08 @ eH
 */
#include "eHutils.h"
#include "string.h"
#include "stdlib.h"

int* parseCSVstr2int(const char* csvstr, int* siz, int offset) {
	/* if size is not given, first find it out*/
	char delimiter = ',';
	const char* ptr;
	if(*siz==-1){
		*siz=0;
		ptr=strchr(csvstr,delimiter);
		while(ptr!=NULL){
			(*siz)++;
			ptr = strchr(ptr+1,delimiter);
		}
		(*siz) += 1;
	}
	
	int* arr = new int[*siz];

	char* endptr;
	arr[0] = strtol(csvstr, &endptr, 10) + offset;
	for(int i=1;i<*siz;i++) {
		arr[i] = strtol(endptr+1, &endptr, 10) + offset;
	}
	return arr;
}

double* parseCSVstr2double(const char* csvstr, int *siz) {	/* if size is not given, first find it out*/
	char delimiter = ',';
	const char* ptr;
	if(*siz==-1){
		*siz=0;
		ptr=strchr(csvstr,delimiter);
		while(ptr!=NULL){
			(*siz)++;
			ptr = strchr(ptr+1,delimiter);
		}
		(*siz) += 1;
	}
	
	double* arr = new double[*siz];
	
	char* endptr;
	arr[0] = strtod(csvstr, &endptr);
	for(int i=1;i<*siz;i++) {
		arr[i] = strtod(endptr+1, &endptr);
	}
	return arr;
}
