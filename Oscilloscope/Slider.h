#pragma once
#include <SDL.h>
#include <SDL_ttf.h>

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
} slideTextures;

//Slider.c

slideTextures* init_Textures(const char* basePath, SDL_Renderer* ren);

 //initPosition = position slider starts on, numOfPositions = total # of positions, height is # of pixels from top to bottom of slide rail, upperLeftX & upperLeftY are x and y coords of upper left corner of slide rail, font is font used for labels, ren is an SDL_renderer*, num is the number of labels including the title, after that do one string per label with the first one being the title
Slider * slider_init(Uint8 initPosition, Uint8 numOfPositions, Uint32 height, Uint32 upperLeftX, Uint32 upperLeftY, TTF_Font * font, SDL_Renderer * ren, int num, ...);

/*call to move slider, not necessarilly with mouse, it just needs a point*/
int Slider_MoveWithMouse(SDL_Point, Slider*);

int Slider_Render(SDL_Renderer*, slideTextures*, Slider*, TTF_Font*);

/*update position of slider on screen, for x or y give NULL if no update is desired. x & y correspond to upper left corner of slide rail*/
Uint8 Slider_UpdatePosition(Uint32 upperLeftX, Uint32 upperLeftY, Slider* slide);