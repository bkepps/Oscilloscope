#pragma once
#include <SDL.h>
/*
struct with all key elements of a slide selector
*/
typedef struct {
	Uint8 position;
	Uint8 numberOfPositions;
	Uint16 spaceBetweenPositions;
	SDL_Rect slideRailRectangle;			//
	SDL_Rect sliderArrowRectangle;			//
	Uint8 move;								//BOOL
} Slider;

/*call to move slider, not necessarilly with mouse, it just needs a point*/
int Slider_MoveWithMouse(SDL_Point, Slider*);

int Slider_Render(SDL_Renderer*, SDL_Texture*, SDL_Texture*, Slider*);