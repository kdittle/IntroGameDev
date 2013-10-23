#include <Windows.h>
#include <SDL.h>
#include <SDL_image.h>
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

class LTexture
{
public:
	LTexture();
	~LTexture();

	bool loadFromFile(std::string path);

	void free();
	void Render(int x, int y);

	int getWidth();
	int getHeight();

private:
	SDL_Texture* mTexture;

	int mWidth;
	int mHeight;

};

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Surface* screenSurface = NULL;
SDL_Surface* backgroundImage = NULL;
SDL_Surface* ballImage = NULL;

LTexture backgroundTexture;
LTexture ballTexture;

LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	free();

	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if(loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), SDL_GetError());
	}
	else
	{
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if(newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL_Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		SDL_FreeSurface(loadedSurface);
	}

	mTexture = newTexture;

	return mTexture != NULL;

}

void LTexture::free()
{
	if(mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::Render(int xPos, int yPos)
{
	SDL_Rect renderQuad = {xPos, yPos, mWidth, mHeight};
	SDL_RenderCopy(renderer, mTexture, NULL, &renderQuad);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

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
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

			if(renderer == NULL)
			{
				printf("Renderer could not be created. SDL_Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}
		}
	}

	return success;
}

bool Load_Image()
{
	bool success = true;


	if(!backgroundTexture.loadFromFile("background.png"))
	{
		printf("Unable to load background image %s! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}

	if(!ballTexture.loadFromFile("ball.png"))
	{
		printf("Unable to load ball image %s! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}

	return success;
}

void Close()
{
	ballTexture.free();
	backgroundTexture.free();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

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

					SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(renderer);

					backgroundTexture.Render(0,0);
					ballTexture.Render(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

					SDL_RenderPresent(renderer);
				}
			}


		}
	}

	Close();

	system("PAUSE");

	return 0;
}//end main