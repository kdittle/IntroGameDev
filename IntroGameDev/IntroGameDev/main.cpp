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

class Timer
{
private:
	Uint32 mstartTicks;
	Uint32 mpausedTicks;

	bool mpaused;
	bool started;

public:
	Timer();

	void start();
	void stop();
	void pause();
	void unpause();

	Uint32 get_ticks();

	bool is_started();
	bool is_paused();
};

struct Circle
{
	int x, y, r;
};

struct vector2f
{
	float x;
	float y;

	vector2f()
	{
		x = 0;
		y = 0;
	}

	vector2f(float x_, float y_)
	{
		x = x_;
		y = y_;
	}

	void add(vector2f v)
	{
		y = y + v.y;
		x = x + v.x;
	}

	void subtract(vector2f v)
	{
		y = y - v.y;
		x = x - v.x;
	}

	void negate()
	{
		x = x * -1;
		y = y * -1;
	}

	void negate_x()
	{
		x = x * -1;
	}

	void negate_y()
	{
		y = y * -1;
	}

	void divide(float n)
	{
		x = x / n;
		y = y / n;
	}

	float mag()
	{
		return std::sqrt(x*x + y*y);
	}

	void normalize()
	{
		float m = mag();
		if( m != 0)
		{
			divide(m);
		}
	}
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

	void VRender(vector2f location, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL,
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
	Ball(vector2f location, vector2f velocity);

	void HandleInput(SDL_Event& e);

	void move(SDL_Rect& square, Circle& circle);
	void v_move();//SDL_Rect& square, Circle& circle);

	void Render_Ball();
	void V_Rend_Ball();

	Circle& getCollider();
	Circle& v_getCollider();

private:
	int mPosX, mPosY;
	vector2f mPosition;

	int mVelX, mVelY;
	vector2f mVelocity;

	Circle mCollider;
	Circle v_mCollider;

	void shiftColliders();
	void v_shiftColliders();

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

void LTexture::VRender(vector2f location, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_Rect renderQuad = {location.x, location.y, mWidth, mHeight};

	if(clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopyEx(renderer, mTexture, clip, &renderQuad, angle, center, flip);
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

Ball::Ball(vector2f location, vector2f velocity)
{
	mPosition = location;
	mVelocity = velocity;

	v_mCollider.r = BALL_WIDTH / 2;

	//mVelocity = vector2f(0, 0);

	v_shiftColliders();
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

void Ball::v_move()//SDL_Rect& square, Circle& circle)
{
	Timer timer;
	float timeKeeper = 0;
	float deltaTime = 0;
	float preTime = 0;

	timeKeeper = timer.get_ticks();
	deltaTime = (timeKeeper - preTime) / 1000;
	preTime = timeKeeper;


	mPosition.add(mVelocity);
	v_shiftColliders();

	std::cout << mPosition.x << ", " << mPosition.y << std::endl;

	if((mPosition.x  < 0) || (mPosition.x + BALL_WIDTH) > WINDOW_WIDTH)
	{
		mVelocity.negate_x();
		v_shiftColliders();
	}

	if((mPosition.y < 0) || (mPosition.y + BALL_WIDTH) > WINDOW_HEIGHT)
	{
		mVelocity.negate_y();
		v_shiftColliders();
	}

	//if(checkCollision(v_mCollider, square) || checkCollision(v_mCollider, circle))
	//{
	//	mVelocity.negate_x();
	//	mPosition.add(mVelocity);
	//	v_shiftColliders();
	//}
}

void Ball::Render_Ball()
{
	ballTexture.Render(mPosX - mCollider.r, mPosY - mCollider.r);
}

void Ball::V_Rend_Ball()
{
	ballTexture.VRender(mPosition);
}

Circle& Ball::getCollider()
{
	return mCollider;
}

Circle& Ball::v_getCollider()
{
	return v_mCollider;
}

void Ball::shiftColliders()
{
	mCollider.x = mPosX;
	mCollider.y = mPosY;
}

void Ball::v_shiftColliders()
{
	v_mCollider.x = mPosition.x;
	v_mCollider.y = mPosition.y;
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

bool InputHandler(bool run)
{
	SDL_Event event;

	run = true;

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
	}

	return run;
}

Timer::Timer()
{
	mstartTicks = 0;
	mpausedTicks = 0;
	mpaused = false;
	started = false;
}

void Timer::start()
{
	started = true;
	mpaused = false;

}
Uint32 Timer::get_ticks()
{
	Uint32 time = 0;

	if(started == true)
	{
		if(mpaused)
		{
			time = mpausedTicks;
		}
		else
		{
			time = SDL_GetTicks() - mstartTicks;
		}
	}

	return time;;
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

			vector2f location = vector2f (WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
			vector2f velocity = vector2f(.25, .25);

			Ball ball(location, velocity);
			Ball otherBall(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4);

			SDL_Rect wall;

			wall.x = 500;
			wall.y = 150;
			wall.w = 200;
			wall.h = 20;

			while(InputHandler(run))
			{

				ball.v_move();//wall, otherBall.getCollider());

				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(renderer);

				/*SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
				SDL_RenderDrawRect(renderer, &wall);*/


				ball.V_Rend_Ball();
				//otherBall.Render_Ball();

				SDL_RenderPresent(renderer);


			}


		}
	}

	Close();

	return 0;
}//end main