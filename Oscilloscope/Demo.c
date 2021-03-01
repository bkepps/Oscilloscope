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
	grphInfo->valueMax = 1024;
	grphInfo->readSuccess = 1;
	grphInfo->Mutex = SDL_CreateMutex();
	grphInfo->resize = 0;
	grphInfo->channelType = 'D';
	return grphInfo;
}

int demo_gather(data* grphInfo) {
	SDL_LockMutex(grphInfo->Mutex);
	for (int i = 0; i < grphInfo->numOfPoints; i++) {
		grphInfo->points[i].x = i;
		grphInfo->points[i].y = grphInfo->graphHeight - BitsNBobs_mapTo(grphInfo->valueMax / 2 * sin(0.01 * PI * i), -grphInfo->valueMax / 2, grphInfo->valueMax / 2, 0, grphInfo->graphHeight);
	}
	grphInfo->readSuccess = 1;
	SDL_UnlockMutex(grphInfo->Mutex);
}