#include <Windows.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include "MainHead.h"

data* serial_dataInit() {
	data* grphInfo = malloc(sizeof(data));
	if (grphInfo == NULL)
		return NULL;
	grphInfo->graphHeight = 400;
	grphInfo->graphWidth = 500;
	grphInfo->numOfPoints = grphInfo->graphWidth + 20;
	grphInfo->points = (SDL_Point*)malloc(sizeof(SDL_Point) * grphInfo->numOfPoints);
	grphInfo->valueMax = 1023;
	if (!serial_init(grphInfo))
		grphInfo->readSuccess = 1;
	else
		grphInfo->readSuccess = 0;
	grphInfo->Mutex = SDL_CreateMutex();
	grphInfo->resize = 0;
	grphInfo->channelType = "S";
	return grphInfo;
}

int serial_init(data* grphInfo) {
	/*open serial port*/
	grphInfo->port = CreateFile(L"COM6",                  // Name of the Port to be Opened
		GENERIC_READ,							// Read Access
		0,                            // No Sharing, ports cant be shared
		NULL,                         // No Security
		OPEN_EXISTING,                // Open existing port only
		0,                            // Non Overlapped I/O
		NULL);                        // Null for Comm Devices

	if (grphInfo->port == INVALID_HANDLE_VALUE)
		return 1;
	DCB dcbSerialParams = { 0 };                         // Initializing DCB structure
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	GetCommState(grphInfo->port, &dcbSerialParams);      //retreives  the current settings

	dcbSerialParams.BaudRate = CBR_256000;      // Setting BaudRate = 9600
	dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
	dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
	dcbSerialParams.Parity = NOPARITY;        // Setting Parity = None 

	if (!SetCommState(grphInfo->port, &dcbSerialParams))  //Configuring the port according to settings in DCB 
		return 3;

	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 200;
	timeouts.ReadTotalTimeoutConstant = 1000;
	timeouts.ReadTotalTimeoutMultiplier = 1000;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (!SetCommTimeouts(grphInfo->port, &timeouts))
		return 4;
	return 0;
}

int serial_gather(data* grphInfo) {
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
		grphInfo->points[j].y = grphInfo->graphHeight - BitsNBobs_mapTo(atoi(dataBuf), 0, grphInfo->valueMax, 0, grphInfo->graphHeight);
		grphInfo->points[j].x = j;
		if (!NumBytesRead || i == 10)
			j--;
	}
	grphInfo->readSuccess = 1;
	SDL_UnlockMutex(grphInfo->Mutex);
	return 0;
}