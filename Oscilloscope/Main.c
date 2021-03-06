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
	void* pointer = NULL;
	SDL_Color black = { 0, 0, 0 };
	SDL_Surface* surface;

	//variables to keep track of threads
	SDL_Thread* gatherThread;

	const char* basePath = SDL_GetBasePath();

	/*initialize SDL*/
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* win = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	Uint32* windowID = malloc(sizeof(Uint32));
	*windowID = SDL_GetWindowID(win);
	
	/*initialize SDL_TTF*/
	if (TTF_Init())
		return 1232;

	/*initialize structs and stuff*/
	slideTextures* textures = init_Textures(basePath, ren);		//load textures for the sliders

	color* bColor = malloc(sizeof(color));						//background color, used when clearing screen
	bColor->r = 235;
	bColor->g = 235;
	bColor->b = 235;
	bColor->a = 255;

	pointer = BitsNBobs_append(basePath, "Resources\\freefont-20120503\\FreeMonoBold.ttf");		//load font
	TTF_Font* font = TTF_OpenFont(pointer, 14);
	free(pointer);
	if (font == NULL)
		return 34;

	data* grphInfo = demo_dataInit();							//initialize channel 1 (Demo sine wave)
	data* grphInfoCPY = malloc(sizeof(data));
	dataCopy_init(grphInfo, grphInfoCPY);


	/*initialize gui elements*/
	Slider* timeSlide = slider_init(0, 10, 200, (width - 50), 50, font, ren, 11, "time/Div", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10");
	Slider* vertSlide = slider_init(0, 5, 200, (width - 50), 300, font, ren, 6, "volts/Div", "10", "5", "1", "0.5", "0.1");

	/*start first data colection*/
	SDL_UnlockMutex(grphInfo->Mutex);
	gatherThread = SDL_CreateThread(demo_gather, "Gather", (void*)grphInfo);
	SDL_DetachThread(gatherThread);
	
	/*enter window loop*/
	while (!quit) {
		//handle events
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				quit = 1;
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					width = event.window.data1;
					height = event.window.data2;

					/* update all elements who's position is changed by a window resize */
					Slider_UpdatePosition(width - 50, (Uint32)NULL, timeSlide);
					Slider_UpdatePosition(width - 50, (Uint32)NULL, vertSlide);

					grphInfoCPY->graphHeight = 3 * (height / 4);
					grphInfoCPY->graphHeight = grphInfoCPY->graphHeight - (grphInfoCPY->graphHeight % 10);
					grphInfoCPY->graphWidth = 3 * (width / 4);
					grphInfoCPY->graphWidth = grphInfoCPY->graphWidth - (grphInfoCPY->graphWidth % 10);
					grphInfoCPY->numOfPoints = grphInfoCPY->graphWidth + 20;
					//make sure realloc doesn't return NULL
					pointer = NULL;
					pointer = realloc(grphInfoCPY->points, sizeof(SDL_Point) * grphInfoCPY->numOfPoints);
					if (pointer != NULL)
						grphInfoCPY->points = pointer;
					else
						return 98;
					grphInfoCPY->resize = 1;
					break;
				}
				SDL_SetRenderDrawColor(ren, bColor->r, bColor->g, bColor->b, bColor->a);
				SDL_RenderClear(ren);
				/*render any and all GUI elements other than graph*/
				Slider_Render(ren, textures, timeSlide, font);
				Slider_Render(ren, textures, vertSlide, font);
				/*render graph*/
				graph_Update(grphInfoCPY, ren);
				SDL_RenderPresent(ren);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					SDL_GetMouseState(&mousePos.x, &mousePos.y);
					/*if the left mouse button is pushed down while over the slider rail or arrow, begin slider move event*/
					if (SDL_PointInRect(&mousePos, &timeSlide->slideRailRectangle) || SDL_PointInRect(&mousePos, &timeSlide->sliderArrowRectangle))
						timeSlide->move = 1;
					if (SDL_PointInRect(&mousePos, &vertSlide->slideRailRectangle) || SDL_PointInRect(&mousePos, &vertSlide->sliderArrowRectangle))
						vertSlide->move = 1;
				}
				break;
			case SDL_FINGERDOWN:
				mousePos.x = event.tfinger.x;
				mousePos.y = event.tfinger.y;

				if (SDL_PointInRect(&mousePos, &timeSlide->slideRailRectangle) || SDL_PointInRect(&mousePos, &timeSlide->sliderArrowRectangle))
					timeSlide->move = 1;
				if (SDL_PointInRect(&mousePos, &vertSlide->slideRailRectangle) || SDL_PointInRect(&mousePos, &vertSlide->sliderArrowRectangle))
					vertSlide->move = 1;
				break;
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT) {
					SDL_GetMouseState(&mousePos.x, &mousePos.y);
					/*if a slider motion event is occuring, and the mouse button goes up, end move event, and set final position*/
					if (timeSlide->move) {
						timeSlide->move = 0;
						Slider_MoveWithMouse(mousePos, timeSlide);
					}
					if (vertSlide->move) {
						vertSlide->move = 0;
						Slider_MoveWithMouse(mousePos, vertSlide);
					}
				}
				break;
			case SDL_FINGERUP:
				mousePos.x = event.tfinger.x;
				mousePos.y = event.tfinger.y;

				/*if a slider motion event is occuring, and the finger goes up, end move event, and set final position*/
				if (timeSlide->move) {
					timeSlide->move = 0;
					Slider_MoveWithMouse(mousePos, timeSlide);
				}
				if (vertSlide->move) {
					vertSlide->move = 0;
					Slider_MoveWithMouse(mousePos, vertSlide);
				}
				break;
			case SDL_MOUSEMOTION:
				mousePos.y = event.motion.y;
				/*if slider move event is underway, have slider arrow follow the mouse*/
				if (timeSlide->move)
					Slider_MoveWithMouse(mousePos, timeSlide);
				if (vertSlide->move)
					Slider_MoveWithMouse(mousePos, vertSlide);
				break;
			case SDL_FINGERMOTION:
				mousePos.y += event.tfinger.dy;
				/*if slider move event is underway, have slider arrow follow the finger*/
				if (timeSlide->move)
					Slider_MoveWithMouse(mousePos, timeSlide);
				if (vertSlide->move)
					Slider_MoveWithMouse(mousePos, vertSlide);
				break;
			default:
				break;
			}
		}
		//check if data update is complete, if yes handle it and start a new collection
		if (SDL_TryLockMutex(grphInfo->Mutex) == 0) {
			if (!grphInfo->readSuccess)
				return 987;
			else
				data_copy(grphInfo, grphInfoCPY);
			SDL_UnlockMutex(grphInfo->Mutex);
			gatherThread = SDL_CreateThread(demo_gather, "gather", (void*)grphInfo);
			SDL_DetachThread(gatherThread);
		}
		//render stuff
		SDL_SetRenderDrawColor(ren, bColor->r, bColor->g, bColor->b, bColor->a);
		SDL_RenderClear(ren);
		/*render any and all GUI elements other than graph*/
		Slider_Render(ren, textures, timeSlide, font);
		Slider_Render(ren, textures, vertSlide, font);
		/*render graph*/
		graph_Update(grphInfoCPY, ren);
		SDL_RenderPresent(ren);
	}

		//wait for gather thread to complete before quitting to avoid memory access error

	SDL_Quit();
	//CloseHandle(grphInfo->port);//Close the Serial Port 
	return 0;
}
