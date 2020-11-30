#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <Windows.h>
#include <stdarg.h>

#include "slider.h"

/*
struct that stores one set of points and related data
*/

typedef struct {
	Uint32 numOfPoints;		//number of points to be collected
	Uint16 graphHeight;
	Uint16 graphWidth;
	SDL_Point* points;		//data points ready to be graphed
	Uint16 valueMax;		//largest possible value from ADC
	HANDLE port;
	SDL_mutex* Mutex;
	Uint8 resize;			//BOOL, true if points* needs to be resized
	Uint8 readSuccess;		//BOOL, true if port accessed successfully
} data;



/*
* struct with all elements in the info bar
*/
typedef struct {
	Uint32 width;
	Uint32 height;
}infoBar;

//BitsNBobs.c
const char* BitsNBobs_append(const char* orgin, char* toAppend);

int BitsNBobs_mapTo(int x, int inMin, int inMax, int outMin, int outMax);

//data.c
data* data_init();

void* dataCopy_init(data* grphInfo, data* grphInfoCPY);

 int data_Gather(data* grphInfo);

 /*
copies points to grphInfoCPY, copies numOfPoints, height, width, and valueMax to grphInfo
*/
 void data_copy(data* grphInfo, data* grphInfoCPY);

//graph.c
 void graph_Update(data* grphInfo, SDL_Renderer* ren);

//init.c

 int init_port(data* grphInfo);

 //render.c
 int renderInfobar(Uint32 width, Uint32 height);


