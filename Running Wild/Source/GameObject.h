#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include <iostream>
#include <time.h>
#include "SDL_mixer.h"

class GameObject
{
private:
	int totalFrame;
	int diffBox = 0;

	SDL_Rect srcRect = { 0,0,0,0 };
	SDL_Rect desRect = { 0,0,0,0 };

	SDL_Texture* objTexture = NULL;
public:
	int scale = 1;
	bool landing = false;
	bool landed = false;
	bool check = true;
	bool ready = true;

	GameObject(int action, int xpos, int ypos, int scale);
	GameObject(int action, int xpos, int ypos, int totalFrame, int scale);
	~GameObject();

	bool checkCollision(GameObject*& obj);
	void UpdatePosition(int xvel, int yvel);
	void UpdatePosition();
	void UpdatePosition(int xvel);
	void Render();
	void SetTexture(int action);
	void ManagePower(int onoff);
};


