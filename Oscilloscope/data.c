// gathers one array of data from a serial port
#include "MainHead.h"
#include <math.h>


#define PI 3.1415926535897932384626433832795028841971693993751058209749

void* dataCopy_init(data* grphInfo, data* grphInfoCPY) {
	grphInfoCPY->numOfPoints = grphInfo->numOfPoints;
	grphInfoCPY->graphHeight = grphInfo->graphHeight;
	grphInfoCPY->graphWidth = grphInfo->graphWidth;
	grphInfoCPY->points = malloc(sizeof(SDL_Point) * grphInfo->numOfPoints);
	grphInfoCPY->valueMax = grphInfo->valueMax;
	grphInfoCPY->port = NULL;
	grphInfoCPY->Mutex = NULL;
	grphInfoCPY->readSuccess = NULL;
	for (Uint16 i = 0; i < grphInfo->numOfPoints; i++) {
		grphInfoCPY->points[i].y = grphInfo->points[i].y;
		grphInfoCPY->points[i].x = grphInfo->points[i].x;
	}
}

/*
copies points to grphInfoCPY, copies numOfPoints, height, width, and valueMax to grphInfo
*/
void data_copy(data* grphInfo, data* grphInfoCPY) {
	void* pointer = NULL;
	if (grphInfoCPY->resize) {
		pointer = realloc(grphInfo->points, sizeof(SDL_Point) * grphInfoCPY->numOfPoints);
		if (pointer != NULL)
			grphInfo->points = pointer;
		else
			return;
		grphInfoCPY->resize = 0;
	}
	grphInfo->graphHeight = grphInfoCPY->graphHeight;
	grphInfo->graphWidth = grphInfoCPY->graphWidth;
	grphInfoCPY->valueMax = grphInfo->valueMax;
	for (Uint16 i = 0; i < grphInfo->numOfPoints && i < grphInfoCPY->numOfPoints; i++) {
		grphInfoCPY->points[i].y = grphInfo->points[i].y;
		grphInfoCPY->points[i].x = grphInfo->points[i].x;
	}
	grphInfo->numOfPoints = grphInfoCPY->numOfPoints;
}