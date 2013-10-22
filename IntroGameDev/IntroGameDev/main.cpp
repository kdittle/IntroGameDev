#include <Windows.h>
#include <SDL.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <iostream>
#include <stdio.h>
#include <string>


#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")

#ifdef main
# undef main
#endif

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;
SDL_Surface* backgroundImage = NULL;

//Creates window and surface.
bool Initialize()
{
	bool success = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize. SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		 window = SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

		if(window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			screenSurface = SDL_GetWindowSurface(window);
		}
	}

	return success;
}

bool Load_Image()
{
	bool success = true;

	backgroundImage = SDL_LoadBMP("background.bmp");
	if(backgroundImage == NULL)
	{
		printf("Unable to load default image %s! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}

	return success;
}

void Close()
{
	SDL_FreeSurface(backgroundImage);
	backgroundImage= NULL;

	SDL_DestroyWindow(window);
	window = NULL;

	SDL_Quit();
}

void InputHandler()
{
	bool run = true;

	SDL_Event event;

	while(run)
	{
		while(SDL_PollEvent(&event) != 0)
		{
			if(event.type == SDL_QUIT)
			{
				run = false;
			}
		}
	}
}

int main()
{//being main

	if(!Initialize())
	{
		printf("Failed to initialisze. \n");
	}
	else
	{
		if(!Load_Image())
		{
			printf("Unable to load image. \n");
		}
		else
		{
			bool run = true;

			SDL_Event event;

			while(run)
			{
				while(SDL_PollEvent(&event) != 0)
				{
					if(event.type == SDL_QUIT)
					{
						run = false;
					}

					if(event.type == SDL_KEYDOWN)
					{
						switch(event.key.keysym.sym)
						{
						case SDLK_ESCAPE:
							run = false;
							break;
						}
					}


					SDL_BlitSurface(backgroundImage, NULL, screenSurface, NULL);
					SDL_UpdateWindowSurface(window);
				}
			}


		}
	}

	Close();

	return 0;
}//end main