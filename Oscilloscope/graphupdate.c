/*draws graph with points to renderer when given gather_data* and SDL_Renderer*/
#include "MainHead.h"

void graph_Update(data* data, SDL_Renderer* ren) {
	float j;
	int i;
	float hDiv10 = data->graphHeight / 10;
	float wDiv10 = data->graphWidth / 10;

	SDL_SetRenderDrawColor(ren, 200, 200, 200, 255);
	/*creates darker horizontal line every 1V with 10-bit 0-5V adc*/
	for (j = data->graphHeight; j >= 0; j -= hDiv10) {
		SDL_RenderDrawLine(ren, 0, j, (data->graphWidth), j);
	}
	/*creates darker vertical lines every 50 points*/
	for (i = 0; i <= (data->graphWidth); i += wDiv10) {
		SDL_RenderDrawLine(ren, i, 0, i, data->graphHeight);
	}

	/*draw crosshairs*/
	SDL_SetRenderDrawColor(ren, 150, 150, 150, 255);
	SDL_RenderDrawLine(ren, 0, data->graphHeight / 2, data->graphWidth, data->graphHeight / 2);
	SDL_RenderDrawLine(ren, data->graphWidth / 2, 0, data->graphWidth / 2, data->graphHeight);

	/*set the color of the points*/
	SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
	/*plot data points and connect them*/
	SDL_RenderDrawLines(ren, data->points, data->graphWidth);
}