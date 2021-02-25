#include <SDL.h>

#include "MainHead.h"

data* demo_dataInit() {
	data* grphInfo = malloc(sizeof(data));
	if (grphInfo == NULL)
		return NULL;
	grphInfo->graphHeight = 400;
	grphInfo->graphWidth = 500;
	grphInfo->numOfPoints = grphInfo->graphWidth + 20;
	grphInfo->points = (SDL_Point*)malloc(sizeof(SDL_Point) * grphInfo->numOfPoints);
	grphInfo->valueMax = 1023;
	grphInfo->readSuccess = 1;
	grphInfo->Mutex = SDL_CreateMutex();
	grphInfo->resize = 0;
	grphInfo->channelType = "D";
	return grphInfo;
}

int demo_gather(data* grphInfo) {

}