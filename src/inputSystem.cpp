//Include SDL libraries
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "inputSystem.h"

//Input Event
Event_Value inputWindowEvent(SDL_Window* window)
{
	SDL_Event event;
	
	bool click = true;
	
	while(SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT: {
				SDL_DestroyWindow(window);
				exit(0);
				break;
			}
			
			case SDL_MOUSEBUTTONDOWN: {
				switch (event.button.button) {
					case SDL_BUTTON_LEFT: {
						if(click) {
							click = false;
							return L_CLICK;
						}
						break;
					}
					
					default:
						break;
					
				}
				break;
			}
			
			case SDL_MOUSEBUTTONUP: {
				switch (event.button.button) {
					case SDL_BUTTON_LEFT: {
						click = true;
						break;
					}
					case SDL_BUTTON_RIGHT: {
						return ACTION;
						break;
					}
					default:
						break;
					
				}
				break;
			}
			
			case SDL_KEYDOWN: {
				switch (event.key.keysym.sym) {
					/*escape*/
					case SDLK_q:
						exit(0);
					break;
					case SDLK_EQUALS: {
						return ZOOM_IN;
						break;
					}
					case SDLK_MINUS: {
						return ZOOM_OUT;
						break;
					}
					case SDLK_ESCAPE: {
						return COMMAND_LINE;
						break;
					}
					default:
						break;
				}
				break;
			}
			
			default:
				break;
		}
	}
	
	return RETURN;
}