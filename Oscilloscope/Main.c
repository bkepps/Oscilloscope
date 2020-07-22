#include "MainHead.h"

int main(int argc, char** argv) {
	const char* title = "Data Plotter V2";
	Uint32 width = 800;
	Uint32 height = 700;
	Uint8 quit = 0;			//BOOL
	Uint8 status = 0;		//BOOL
	SDL_Event event;
	SDL_Point mousePos;
	Uint8 hasRun = 0;
	int Status;
	void* pointer = NULL;

	//variables to keep track of threads
	SDL_Thread* gatherThread;

	const char* basePath = SDL_GetBasePath();

	/*initialize SDL*/
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* win = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	Uint32* windowID = malloc(sizeof(Uint32));
	*windowID = SDL_GetWindowID(win);

	/*initialize structs and stuff*/
	Textures* textures = init_Textures(basePath, ren);		//load textures
	data* grphInfo = init_data();
	data* grphInfoCPY = malloc(sizeof(data));
	init_dataCopy(grphInfo, grphInfoCPY);
	if (grphInfo == NULL)
		return 64;

	/*initialize gui elements*/
	Slider* timeSlide = init_slider(0, 10, 200, (width - 50), 50);

	/*start first data colection*/
	SDL_UnlockMutex(grphInfo->Mutex);
	gatherThread = SDL_CreateThread(data_Gather, "Gather", (void*)grphInfo);
	SDL_DetachThread(gatherThread);
	
	/*enter window loop*/
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				quit = 1;
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					width = event.window.data1;
					height = event.window.data2;

					/* update all elements who's position is changed by a window resize */
					Slider_UpdatePosition(width - 50, (Uint32)NULL, timeSlide);
					grphInfoCPY->graphHeight = 3 * (height / 4);
					grphInfoCPY->graphHeight = grphInfoCPY->graphHeight - (grphInfoCPY->graphHeight % 10);
					grphInfoCPY->graphWidth = 3 * (width / 4);
					grphInfoCPY->graphWidth = grphInfoCPY->graphWidth - (grphInfoCPY->graphWidth % 10);
					grphInfoCPY->numOfPoints = grphInfoCPY->graphWidth + 20;
					//meke sure realloc doesn't return NULL
					pointer = NULL;
					pointer = realloc(grphInfoCPY->points, sizeof(SDL_Point) * grphInfoCPY->numOfPoints);
					if (pointer != NULL)
						grphInfoCPY->points = pointer;
					else
						return 98;
					grphInfoCPY->resize = 1;
					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					SDL_GetMouseState(&mousePos.x, &mousePos.y);
					/*if the left mouse button is pushed down while over the slider rail or arrow, begin slider move event*/
					if (SDL_PointInRect(&mousePos, &timeSlide->slideRailRectangle) || SDL_PointInRect(&mousePos, &timeSlide->sliderArrowRectangle))
						timeSlide->move = 1;
				}
				break;

			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT) {
					SDL_GetMouseState(&mousePos.x, &mousePos.y);
					/*if a slider motion event is occuring, and the mouse button goes up, end move event, and set final position*/
					if (timeSlide->move) {
						timeSlide->move = 0;
						Slider_MoveWithMouse(mousePos, timeSlide);
					}
				}
				break;

			case SDL_MOUSEMOTION:
				mousePos.y = event.motion.y;
				/*if slider move event is underway, have slider arrow follow the mouse*/
				if (timeSlide->move)
					Slider_MoveWithMouse(mousePos, timeSlide);
				break;
			}

		}
		if (SDL_TryLockMutex(grphInfo->Mutex) == 0) {
			data_copy(grphInfo, grphInfoCPY);
			SDL_UnlockMutex(grphInfo->Mutex);
			gatherThread = SDL_CreateThread(data_Gather, "gather", (void*)grphInfo);
			SDL_DetachThread(gatherThread);
		}
		SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
		SDL_RenderClear(ren);
		/*render any and all GUI elements other than graph*/
		Slider_Render(ren, textures, timeSlide);
		/*render graph*/
		graph_Update(grphInfoCPY, ren);
		SDL_RenderPresent(ren);
	}

		//wait for gather thread to complete before quitting to avoid memory access error

	SDL_Quit();
	CloseHandle(grphInfo->port);//Close the Serial Port 
	return 0;
}