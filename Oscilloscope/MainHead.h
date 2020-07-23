#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <Windows.h>
#include <math.h>
#include <stdarg.h>

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
} data;

/*
struct with all key elements of a slide selector
*/
typedef struct {
	Uint8 position;
	Uint8 numOfPositions;
	Uint16 spaceBetweenPositions;
	SDL_Rect slideRailRectangle;			//
	SDL_Rect sliderArrowRectangle;			//
	char** labelChars;						//Array of strings for labels, first entry is label for slider, then one string per possible position
	SDL_Texture** labelTextures;			//Array of surfaces with labels, same format as labelChars
	SDL_Rect** labelRects;					//store rects label textures get rendered to
	Uint8 move;								//BOOL
	Uint8 fontHeight;
} Slider;

/*
contains all textures required
*/
typedef struct {
	SDL_Texture* sliderArrow;
	SDL_Texture* sliderRail;
} Textures;

//BitsNBobs.c
const char* BitsNBobs_append(const char* orgin, const char* toAppend);

int BitsNBobs_mapTo(int x, int inMin, int inMax, int outMin, int outMax);

//data.c
 int data_Gather(data* grphInfo);

 /*
copies points to grphInfoCPY, copies numOfPoints, height, width, and valueMax to grphInfo
*/
 void data_copy(data* grphInfo, data* grphInfoCPY);

 //int data_Process(data_raw* rawData, data_processed* processedData);

//graph.c
 void graph_Update(data* grphInfo, SDL_Renderer* ren);

//init.c
 data* init_data();

 void* init_dataCopy(data* grphInfo, data* grphInfoCPY);

 //initPosition = position slider starts on, numOfPositions = total # of positions, height is # of pixels from top to bottom of slide rail, upperLeftX & upperLeftY are x and y coords of upper left corner of slide rail, font is font used for labels, ren is an SDL_renderer*, num is the number of labels including the title, after that do one string per label with the first one being the title
 Slider* init_slider(Uint8 initPosition, Uint8 numOfPositions, Uint32 height, Uint32 upperLeftX, Uint32 upperLeftY, TTF_Font* font, SDL_Renderer* ren, int num,...);

 Textures* init_Textures(const char* basePath, SDL_Renderer* ren);

 int init_port(data* grphInfo);

 //Slider.c
 /*call to move slider, not necessarilly with mouse, it just needs a point*/
 int Slider_MoveWithMouse(SDL_Point, Slider*);

 int Slider_Render(SDL_Renderer*, Textures*, Slider*, TTF_Font*);

/*update position of slider on screen, for x or y give NULL if no update is desired. x & y correspond to upper left corner of slide rail*/
 Uint8 Slider_UpdatePosition(Uint32 upperLeftX, Uint32 upperLeftY, Slider* slide);
