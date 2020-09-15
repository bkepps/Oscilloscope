// gathers one array of data from a serial port
#include "MainHead.h"
#include <math.h>

#define PI 3.1415926535897932384626433832795028841971693993751058209749

data* data_init() {
	data* grphInfo = malloc(sizeof(data));
	if (grphInfo == NULL)
		return NULL;
	grphInfo->graphHeight = 400;
	grphInfo->graphWidth = 500;
	grphInfo->numOfPoints = grphInfo->graphWidth + 20;
	grphInfo->points = (SDL_Point*)malloc(sizeof(SDL_Point) * grphInfo->numOfPoints);
	grphInfo->valueMax = 1023;
	if (!init_port(grphInfo))
		grphInfo->readSuccess = 1;
	else
		grphInfo->readSuccess = 0;
	grphInfo->Mutex = SDL_CreateMutex();
	grphInfo->resize = 0;
	return grphInfo;
}

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

int data_Gather(data* grphInfo) {
	SDL_LockMutex(grphInfo->Mutex);
	char file_rBuf;
	char dataBuf[10];
	Uint8 end = 0;			//BOOL
	DWORD NumBytesRead;						// Bytes read by ReadFile()
	Uint16 i = 0;
	Uint16 j;
	int math;
	DWORD error;

	for (j = 0; j < grphInfo->numOfPoints; j++) {
		i = 0;
		end = 0;
		/*step through chars and append to data_rBuf until a return char is found. return char is last char in string*/
		do {
			math = ReadFile(grphInfo->port, &file_rBuf, sizeof(char), &NumBytesRead, NULL);		//returns 0 when no chars left to read, will store one more point, we'll ignore it later
			if (!math) {
				error = GetLastError();
				grphInfo->readSuccess = 0;
				SDL_UnlockMutex(grphInfo->Mutex);
				return 1;
			}
			if (file_rBuf == '\n')
				end = 1;
			if (i == 10)				//prevent writing past bounds of dataBuf if no \r is reached -- this seems to occur if port is used too soon after initialization, it reads garbage until actual data is recieved
				break;
			dataBuf[i] = file_rBuf;
			i++;
		} while (!end);
		/*convert data_buf to int. atoi ignores \r at end of string*/
		//data->points[pointCurrent].y = data->graphHeight - (atoi(dataBuf) * ((float) data->graphHeight / (float) data->valueMax));
		//data->points[pointCurrent].x = pointCurrent;
		grphInfo->points[j].y = grphInfo->graphHeight - BitsNBobs_mapTo(atoi(dataBuf), 0, grphInfo->valueMax, 0, grphInfo->graphHeight);
		grphInfo->points[j].x = j;
		if (!NumBytesRead || i == 10)
			j--;
	}
	grphInfo->readSuccess = 1;
	SDL_UnlockMutex(grphInfo->Mutex);
	return 0;
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