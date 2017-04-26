//Include SDL libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

//Initialize the Systems
extern void initGraphicSystem();

//Create Window
extern SDL_Window* createWin(const int wid, const int hght, const char* winName, Uint32 flags, int posX = SDL_WINDOWPOS_CENTERED, int posY = SDL_WINDOWPOS_CENTERED);

//Create Renderer
extern SDL_Renderer* createRend(SDL_Window* window,const int index, Uint32 flags, const char* winName);

//Create Texture
extern SDL_Texture* createTexture(SDL_Renderer* renderer, const char* pathIMG);