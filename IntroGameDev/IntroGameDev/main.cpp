#include <Windows.h>
#include <SDL.h>
#include <SDL_image.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <time.h>
#include <vector>


#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")

#ifdef main
# undef main
#endif

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Surface* screenSurface = NULL;
SDL_Surface* backgroundImage = NULL;
SDL_Surface* ballImage = NULL;

struct Circle
{
	int x, y, r;
};

class LTexture
{
public:
	LTexture();
	~LTexture();

	bool loadFromFile(std::string path);

	void free();
	void Render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL,
		SDL_RendererFlip = SDL_FLIP_NONE);

	int getWidth();
	int getHeight();

private:
	SDL_Texture* mTexture;

	int mWidth;
	int mHeight;

};

LTexture backgroundTexture;
LTexture ballTexture;

int BALL_VELOCITY = 5;

class Ball
{
public:
	static const int BALL_WIDTH = 20;
	static const int BALL_HEIGHT = 20;

	Ball(int x, int y);

	void HandleInput(SDL_Event& e);

	void move(SDL_Rect& square, Circle& circle);

	void Render_Ball();

	Circle& getCollider();

private:
	int mPosX, mPosY;
	int mVelX, mVelY;

	Circle mCollider;

	void shiftColliders();

};

bool checkCollision(Circle& objA, Circle& objB);

bool checkCollision(Circle& objA, SDL_Rect& objB);

double distanceSquared(int x1, int x2, int y1, int y2);

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
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0xFF, 0xFF));

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

void LTexture::Render(int xPos, int yPos, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_Rect renderQuad = {xPos, yPos, mWidth, mHeight};

	if(clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

Ball::Ball(int x, int y)
{
	mPosX = x;
	mPosY = y;

	mCollider.r = BALL_WIDTH / 2;

	mVelX = 0;
	mVelY = 0;

	shiftColliders();
}

void Ball::HandleInput(SDL_Event& e)
{
	if(e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		switch(e.key.keysym.sym)
		{
		case SDLK_UP: mVelY -= BALL_VELOCITY; 
			break;
		case SDLK_DOWN: mVelY += BALL_VELOCITY; 
			break;
		case SDLK_LEFT: mVelX -= BALL_VELOCITY; 
			break;
		case SDLK_RIGHT: mVelX += BALL_VELOCITY; 
			break;
		}
	}

	else if(e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		switch(e.key.keysym.sym)
		{
		case SDLK_UP: mVelY += BALL_VELOCITY; 
			break;
		case SDLK_DOWN: mVelY -= BALL_VELOCITY; 
			break;
		case SDLK_LEFT: mVelX += BALL_VELOCITY; 
			break;
		case SDLK_RIGHT: mVelX -= BALL_VELOCITY; 
			break;
		}
	}
}

void Ball::move(SDL_Rect& square, Circle& circle)
{
	mPosX += mVelX;
	shiftColliders();

	if((mPosX - BALL_WIDTH / 2 < 0) || (mPosX + BALL_WIDTH / 2 > WINDOW_WIDTH) || 
		checkCollision(mCollider, square) || checkCollision(mCollider, circle))
	{
		mPosX -= mVelX;
		shiftColliders();
	}

	mPosY += mVelY;
	shiftColliders();

	if((mPosY - BALL_HEIGHT / 2 < 0) || (mPosY + BALL_HEIGHT / 2 > WINDOW_HEIGHT) ||
		checkCollision(mCollider, square) || checkCollision(mCollider, circle))
	{
		mPosY -= mVelY;
		shiftColliders();
	}
}

void Ball::Render_Ball()
{
	ballTexture.Render(mPosX - mCollider.r, mPosY - mCollider.r);
}

Circle& Ball::getCollider()
{
	return mCollider;
}

void Ball::shiftColliders()
{
	mCollider.x = mPosX;
	mCollider.y = mPosY;
}

bool checkCollision(Circle& objA, Circle& objB)
{
	int tRadiusSquared = objA.r + objB.r;
	tRadiusSquared = tRadiusSquared * tRadiusSquared;

	if(distanceSquared(objA.x, objA.y, objB.x, objB.y) < (tRadiusSquared))
	{
		return true;
	}

	return false;
}

bool checkCollision(Circle& objA, SDL_Rect& objB)
{
	//Closest point in collision box
	int cX, cY;

	//Closest x offset
	if(objA.x < objB.x)
	{
		cX = objB.x;
	}
	else if (objA.x > (objB.x + objB.w))
	{
		cX = objB.x + objB.w;
	}
	else
	{
		cX = objA.x;
	}

	//Closest y offset
	if(objA.y < objB.y)
	{
		cY = objB.y;
	}
	else if (objA.y > (objB.y + objB.h))
	{
		cY = objB.y + objB.h;
	}
	else
	{
		cY = objA.y;
	}

	if(distanceSquared(objA.x, objA.y, cX, cY) < objA.r * objA.r)
	{
		return true;
	}

	return false;

}

double distanceSquared(int x1, int y1, int x2, int y2)
{
	int deltaX = x2 - x1;
	int deltaY = y2 - y1;
	return deltaX * deltaX + deltaY * deltaY;
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
		if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled.");
		}

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

				int imgFlags = IMG_INIT_PNG;
				if(!(IMG_Init(imgFlags) &imgFlags))
				{
					printf("SDL_image couldn ot be initialized. SDL_image Error: %s\n",  IMG_GetError());
					success = false;
				}
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

	IMG_Quit();
	SDL_Quit();
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

			Ball ball (WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
			Ball otherBall(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4);

			SDL_Rect wall;

			wall.x = 500;
			wall.y = 150;
			wall.w = 200;
			wall.h = 20;

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

					ball.HandleInput(event);

					ball.move(wall, otherBall.getCollider());

					SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(renderer);

					SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
					SDL_RenderDrawRect(renderer, &wall);


					ball.Render_Ball();
					otherBall.Render_Ball();
					
				}

				SDL_RenderPresent(renderer);
			}


		}
	}

	Close();

	return 0;
}//end main