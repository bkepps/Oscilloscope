#include "MainHead.h"

slideTextures* init_Textures(const char* basePath, SDL_Renderer* ren) {
	slideTextures* textures = malloc(sizeof(slideTextures));

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

Slider* slider_init(Uint8 initPosition, Uint8 numOfPositions, Uint32 height, Uint32 upperLeftX, Uint32 upperLeftY, TTF_Font* font, SDL_Renderer* ren, int num, ...) {
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

int Slider_MoveWithMouse(SDL_Point mousePos, Slider* slide) {
	int randcalc;

	randcalc = (mousePos.y - slide->slideRailRectangle.y) / slide->spaceBetweenPositions;
	if (randcalc >= 0 && randcalc < slide->numOfPositions) {
		slide->position = randcalc;
	}
	else if (randcalc >= slide->numOfPositions) {
		slide->position = slide->numOfPositions - 1;
	}
	else if (randcalc < 0) {
		slide->position = 0;
	}
	/* set position of arrow on slider */
#pragma warning( suppress : 4244 )
	slide->sliderArrowRectangle.y = 50 + (slide->spaceBetweenPositions * slide->position) - (.5 * slide->sliderArrowRectangle.h);
	return 0;
}

int Slider_Render(SDL_Renderer *ren, slideTextures* textures, Slider* slide, TTF_Font* font) {
	SDL_RenderCopy(ren, textures->sliderRail, NULL, &(slide->slideRailRectangle));
	SDL_RenderCopy(ren, textures->sliderArrow, NULL, &(slide->sliderArrowRectangle));

	//render labels
	for(int i = 0; i <= slide->numOfPositions; i++)
		SDL_RenderCopy(ren, slide->labelTextures[i], NULL, slide->labelRects[i]);

	return 0;
}

Uint8 Slider_UpdatePosition(Uint32 upperLeftX, Uint32 upperLeftY, Slider* slide) {
	if (upperLeftX != (Uint32)NULL) {
		slide->slideRailRectangle.x = upperLeftX;
		slide->sliderArrowRectangle.x = slide->slideRailRectangle.x - (int)(slide->sliderArrowRectangle.w / 2);
	}
	if (upperLeftY != (Uint32)NULL) {
		slide->slideRailRectangle.y = upperLeftY;
		slide->sliderArrowRectangle.y = slide->slideRailRectangle.y + (slide->spaceBetweenPositions * slide->position) - (int)(slide->sliderArrowRectangle.h / 2);
	}	
	//update label's positions
	//title
	slide->labelRects[0]->x = slide->slideRailRectangle.x - ((strlen(slide->labelChars[0]) / 2) * slide->fontHeight / 2) - 1;
	slide->labelRects[0]->y = slide->slideRailRectangle.y - slide->fontHeight - 10;

	//position labels
	for (int i = 1; i <= slide->numOfPositions; i++) {
		slide->labelRects[i]->x = slide->sliderArrowRectangle.x - 4 - (slide->fontHeight / 2) * strlen(slide->labelChars[i]);
		slide->labelRects[i]->y = ((i - 1) * slide->spaceBetweenPositions) - (slide->fontHeight / 2) + slide->slideRailRectangle.y;
	}
	
	return 0;
}