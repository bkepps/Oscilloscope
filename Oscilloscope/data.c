// gathers one array of data from a serial port
#include "MainHead.h"

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
	for (int i = 0; i < grphInfo->numOfPoints && i < grphInfoCPY->numOfPoints; i++) {
		grphInfoCPY->points[i].y = grphInfo->points[i].y;
		grphInfoCPY->points[i].x = grphInfo->points[i].x;
	}
	grphInfo->numOfPoints = grphInfoCPY->numOfPoints;
}