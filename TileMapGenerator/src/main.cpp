#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include "Application.h"

int main(int argc, char** argv)
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cout << "Failed to init SDL3!" << std::endl;
	}

	SDL_Window* window = SDL_CreateWindow("TileMapGenerator", 1280, 720, NULL);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

	bool running = true;
	Application* app = new Application(window, renderer, &running);
	while (running)
	{
		app->Update();
		app->Render();
	}

	delete app;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}