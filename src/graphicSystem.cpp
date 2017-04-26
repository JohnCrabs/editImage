#include <stdio.h>
#include <stdlib.h>

#include "graphicSystem.h"

//Initialize the Systems
void initGraphicSystem()
{
	//initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("ERROR :: SDL FAILED TO INITIALIZED!\n");
		SDL_GetError();
		exit(0);
	}
	
	//initialize TTF
	if(TTF_Init() < 0) {
		printf("ERROR :: CANNOT INITIALIZE TTF!\n%s\n\n", TTF_GetError());
		exit(2);
	}
}

//Create Window
SDL_Window* createWin(const int wid, const int hght, const char* winName, Uint32 flags, int posX, int posY)
{
	SDL_Window* window = NULL;
	window = SDL_CreateWindow(winName, posX, posY, wid, hght, flags);
	
	if(window == NULL) {
		printf("ERROR :: SDL FAILED TO CREATE WINDOW %s!\n", winName);
		SDL_GetError();
		exit(0);
	}
	
	return window;
}

//Create Renderer - Sets Color to Black
SDL_Renderer* createRend(SDL_Window* window, const int index, Uint32 flags, const char* winName)
{
	SDL_Renderer* renderer = NULL;
	renderer = SDL_CreateRenderer(window, index, flags);
	
	if(renderer == NULL) {
		printf("ERROR :: SDL FAILED TO CREATE RENDERER FOR WINDOW %s!\n", winName);
		SDL_GetError();
		exit(0);
	}
	
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	
	return renderer;
}

//Create TEXTURE
SDL_Texture* createTexture(SDL_Renderer* renderer, const char* pathIMG)
{
	SDL_Texture* texture = NULL;
	
	texture = IMG_LoadTexture(renderer, pathIMG);
	
	if(texture == NULL) {
		printf("ERROR :: SDL FAILED TO OPEN IMAGE!");
		SDL_GetError();
		exit(0);
	}
	
	return texture;
}
