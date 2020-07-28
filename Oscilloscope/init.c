#include "MainHead.h"

data* init_data() {
	data* grphInfo = malloc(sizeof(data));
	if (grphInfo == NULL)
		return NULL;
	grphInfo->graphHeight = 400;
	grphInfo->graphWidth = 500;
	grphInfo->numOfPoints = grphInfo->graphWidth + 20;
	grphInfo->points = (SDL_Point*) malloc(sizeof(SDL_Point) * grphInfo->numOfPoints);
	grphInfo->valueMax = 1023;
	if (!init_port(grphInfo))
		grphInfo->readSuccess = 1;
	else
		grphInfo->readSuccess = 0;
	grphInfo->Mutex = SDL_CreateMutex();
	grphInfo->resize = 0;
	return grphInfo;
}

void* init_dataCopy(data* grphInfo, data* grphInfoCPY) {
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

int init_port(data* grphInfo) {
	/*open serial port*/
	grphInfo->port = CreateFile(L"COM4",                  // Name of the Port to be Opened
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

Slider* init_slider(Uint8 initPosition, Uint8 numOfPositions, Uint32 height, Uint32 upperLeftX, Uint32 upperLeftY, TTF_Font* font, SDL_Renderer* ren, int num, ...) {
	va_list valist;
	va_start(valist, num);
	SDL_Surface* surface;
	SDL_Color black = { 0, 0, 0 };

	Slider* slider = malloc(sizeof(Slider));
	slider->move = 0;
	slider->position = initPosition;
	slider->numOfPositions = numOfPositions;
	slider->spaceBetweenPositions = height / (slider->numOfPositions - 1);

	//slider->slideRailRectangle.x = upperLeftX;
	//slider->slideRailRectangle.y = upperLeftY;
	slider->slideRailRectangle.h = height;
	slider->slideRailRectangle.w = 8;

	slider->sliderArrowRectangle.h = 15;
	slider->sliderArrowRectangle.w = 20;
	//slider->sliderArrowRectangle.x = slider->slideRailRectangle.x - (int)(slider->sliderArrowRectangle.w / 2);
	//slider->sliderArrowRectangle.y = slider->slideRailRectangle.y + (slider->spaceBetweenPositions * slider->position) - (int)(slider->sliderArrowRectangle.h / 2);
	
	slider->labelChars = malloc(sizeof(char*) * slider->numOfPositions);
	slider->labelTextures = malloc(sizeof(SDL_Texture*) * slider->numOfPositions);
	slider->labelRects = malloc(sizeof(SDL_Rect*) * slider->numOfPositions);

	slider->fontHeight = TTF_FontHeight(font);

	//create textures containing text generated from string arguments and initialize rectangles to render them to
	for (int i = 0; i <= slider->numOfPositions; i++) {
		slider->labelChars[i] = va_arg(valist, char*);
		if (!(surface = TTF_RenderUTF8_Blended(font, slider->labelChars[i], black)))
			return NULL;
		slider->labelTextures[i] = SDL_CreateTextureFromSurface(ren, surface);
		slider->labelRects[i] = malloc(sizeof(SDL_Rect));
		slider->labelRects[i]->w = strlen(slider->labelChars[i]) * ((slider->fontHeight / 2) + 1);
		slider->labelRects[i]->h = slider->fontHeight;
	}
	va_end(valist);

	//set position of slider
	Slider_UpdatePosition(upperLeftX, upperLeftY, slider);
	return slider;
}

Textures* init_Textures(const char* basePath, SDL_Renderer *ren) {
	Textures* textures = malloc(sizeof(Textures));

	char* returnedFilePath = BitsNBobs_append(basePath, "Resources\\SliderArrow.bmp");			//load arrow for sliders to texture
	SDL_Surface* sliderArrowsurf = SDL_LoadBMP(returnedFilePath);
	free(returnedFilePath);
	textures->sliderArrow = SDL_CreateTextureFromSurface(ren, sliderArrowsurf);
	SDL_FreeSurface(sliderArrowsurf);

	returnedFilePath = BitsNBobs_append(basePath, "Resources\\SliderRail.bmp");				//load rail arrow slides on
	SDL_Surface* sliderRailsurf = SDL_LoadBMP(returnedFilePath);
	free((void*)returnedFilePath);
	textures->sliderRail = SDL_CreateTextureFromSurface(ren, sliderRailsurf);
	SDL_FreeSurface(sliderRailsurf);

	return textures;
}