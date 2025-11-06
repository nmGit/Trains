#include <iostream>
#include <SDL3/SDL.h>

// We will use this renderer to draw into this window every frame.
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;


/* This function runs once at startup. */
bool init()
{
	// Create window
	SDL_SetAppMetadata("Example Renderer Clear", "1.0", "com.example.renderer-clear");

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return false;
	}

	if (!SDL_CreateWindowAndRenderer("examples/renderer/clear", 640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return false;
	}
	SDL_SetRenderLogicalPresentation(renderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX);

	return true;
}

bool processEvent()
{
	SDL_Event event;
	if (SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_QUIT) {
			return false;
		}
	}
	return true;
}

bool render()
{
	const double now = ((double)SDL_GetTicks()) / 1000.0;  /* convert from milliseconds to seconds. */
	/* choose the color for the frame we will draw. The sine wave trick makes it fade between colors smoothly. */
	const float red = (float)(0.5 + 0.5 * SDL_sin(now));
	const float green = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
	const float blue = (float)(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));
	SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);  /* new color, full alpha. */

	/* clear the window to the draw color. */
	SDL_RenderClear(renderer);

	/* put the newly-cleared rendering on the screen. */
	SDL_RenderPresent(renderer);

	return true;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
bool quit(void* appstate, SDL_AppResult result)
{
	/* SDL will clean up the window/renderer for us. */
	return true;
}

int main(int argc, char* argv) {
	std::cout << "Hello, Train Game!" << std::endl;

	init();
	while (processEvent())
	{
		render();
	}

	return 0;
}