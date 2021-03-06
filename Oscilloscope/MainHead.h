#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <Windows.h>
#include <stdarg.h>

#include "slider.h"
#include "BitsNBobs.h"

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986

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
	char channelType;		//S = serial
} data;

typedef struct {
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
} color;



/*
* struct with all elements in the info bar
*/
typedef struct {
	Uint32 width;
	Uint32 height;
}infoBar;

//data.c

void* dataCopy_init(data* grphInfo, data* grphInfoCPY);

	//copies points to grphInfoCPY, copies numOfPoints, height, width, and valueMax to grphInfo
 void data_copy(data* grphInfo, data* grphInfoCPY);

//Demo.c
 data* demo_dataInit();
 int demo_gather(data* grphinfo);					//1 volt/div sine wave

//graph.c

 void graph_Update(data* grphInfo, SDL_Renderer* ren);
 int graph_updateScale(data* grphInfo, Slider* slider);				//update scale of graph from slider struct

 //render.c

 int renderInfobar(Uint32 width, Uint32 height);

 //Serial.c

 data* serial_dataInit();
 int serial_init(data* grphInfo);
 int serial_gather(data* grphInfo);