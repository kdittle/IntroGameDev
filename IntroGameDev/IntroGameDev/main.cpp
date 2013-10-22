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

enum KeyPressSurfaces
{
	KEY_PRESS_SURFACE_DEFAULT,
	KEY_PRESS_SURFACE_UP,
	KEY_PRESS_SURFACE_DOWN,
	KEY_PRESS_SURFACE_LEFT,
	KEY_PRESS_SURFACE_RIGHT,
	KEY_PRESS_SURFACE_TOTAL
};

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;
SDL_Surface* imageSurface = NULL;
SDL_Surface* currentSurface = NULL;

SDL_Surface* Key_Pressed_Surfaces[KEY_PRESS_SURFACE_TOTAL];

SDL_Surface* Load_Surface( std::string path);

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

	Key_Pressed_Surfaces[KEY_PRESS_SURFACE_DEFAULT] = Load_Surface("Images/background.png");
	if(imageSurface == NULL)
	{
		printf("Unable to load default image %s! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}

	Key_Pressed_Surfaces[KEY_PRESS_SURFACE_UP] = Load_Surface("Images/up.png");
	if(imageSurface == NULL)
	{
		printf("Unable to load up image %s! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}

	Key_Pressed_Surfaces[KEY_PRESS_SURFACE_DOWN] = Load_Surface("Images/down.png");
	if(imageSurface == NULL)
	{
		printf("Unable to load down image %s! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}

	Key_Pressed_Surfaces[KEY_PRESS_SURFACE_LEFT] = Load_Surface("Images/left.png");
	if(imageSurface == NULL)
	{
		printf("Unable to load left %s! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}

	Key_Pressed_Surfaces[KEY_PRESS_SURFACE_RIGHT] = Load_Surface("Images/right.png");
	if(imageSurface == NULL)
	{
		printf("Unable to load right image %s! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}

	return success;
}

void Close()
{
	for(int i = 0; i < KEY_PRESS_SURFACE_TOTAL; i++)
	{
		SDL_FreeSurface( Key_Pressed_Surfaces[i]);
		Key_Pressed_Surfaces[i] = NULL;
	}

	SDL_DestroyWindow(window);
	window = NULL;

	SDL_Quit();
}

void InputHandler()
{
	bool quit = false;

	SDL_Event event;

	while(!quit)
	{
		while(SDL_PollEvent(&event) != 0)
		{
			if(event.type == SDL_QUIT)
			{
				quit = true;
			}
		}
	}
}

SDL_Surface* Load_Surface(std::string path)
{
	SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
	if(loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		printf(path.c_str());
	}

	return loadedSurface;
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
			bool quit = false;

			SDL_Event event;

			while(!quit)
			{
				while(SDL_PollEvent(&event) != 0)
				{
					if(event.type == SDL_QUIT)
					{
						quit = true;
					}

					if(event.type == SDL_KEYDOWN)
					{
						switch(event.key.keysym.sym)
						{
						case SDLK_UP:
							currentSurface = Key_Pressed_Surfaces[KEY_PRESS_SURFACE_UP];
							break;

						case SDLK_DOWN:
							currentSurface = Key_Pressed_Surfaces[KEY_PRESS_SURFACE_DOWN];
							break;

						case SDLK_LEFT:
							currentSurface = Key_Pressed_Surfaces[KEY_PRESS_SURFACE_LEFT];
							break;

						case SDLK_RIGHT:
							currentSurface = Key_Pressed_Surfaces[KEY_PRESS_SURFACE_RIGHT];
							break;

						default:
							currentSurface = Key_Pressed_Surfaces[KEY_PRESS_SURFACE_DEFAULT];
							break;
						}
					}


					SDL_BlitSurface(imageSurface, NULL, screenSurface, NULL);
					SDL_UpdateWindowSurface(window);
				}
			}


		}
	}

	Close();

	system("PAUSE");

	return 0;
}//end main