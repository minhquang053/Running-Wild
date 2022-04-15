#include "GameObject.h"
#include <iostream>
#include <stdlib.h>

#define JUMP 0
#define WALK 1
#define IDLE 2
#define DEATH 3
#define ROCK1 4
#define ROCK2 5
#define ROCK3 6
#define ROCK4 7
#define TREE1 8
#define TREE2 9
#define ENEMY 10
#define INVICIBLE 11
#define JUMPINV 12

#define TREE_Y1 420
#define TREE_Y2 447
#define ROCK_Y1 460
#define ROCK_Y2 450
#define ROCK_Y3 480

extern SDL_Renderer* renderer;
extern Mix_Music* gMusic;

SDL_Texture* LoadTexture(const char* filepath)
{
	SDL_Surface* surface = IMG_Load(filepath);
	if (surface == NULL) std::cout << "LoadTexture() ! Error" << std::endl;
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

void LoadMusic()
{
	srand(time(0));

	switch (rand()%6+1) {
	case 1:
		gMusic = Mix_LoadMUS("Assets/OTR.mp3");
		break;
	case 2:
		gMusic = Mix_LoadMUS("Assets/GP.mp3");
		break;
	case 3:
		gMusic = Mix_LoadMUS("Assets/MB.mp3");
		break;
	case 4:
		gMusic = Mix_LoadMUS("Assets/LT.mp3");
		break;
	case 5:
		gMusic = Mix_LoadMUS("Assets/DIMR.mp3");
		break;
	case 6:
		gMusic = Mix_LoadMUS("Assets/RA.mp3");
	default:
		break;
	}
	Mix_PlayMusic(gMusic, 1);
	if (gMusic == NULL) exit(1);
}

GameObject::GameObject(int action, int xpos, int ypos, int scale) // dành cho những vật bất động
{
	SetTexture(action);
	this->scale = scale;
	desRect.x = xpos;
	desRect.y = ypos;
	SDL_QueryTexture(objTexture, NULL, NULL, &srcRect.w, &srcRect.h);
	desRect.w = srcRect.w;
	desRect.h = srcRect.h;
}

GameObject::GameObject(int action, int xpos, int ypos, int totalFrame, int scale)
{
	this->scale = scale;
	desRect.x = xpos;
	desRect.y = ypos;
	SetTexture(action);
}

GameObject::~GameObject()
{
	SDL_DestroyTexture(objTexture);
}

void GameObject::UpdatePosition(int xvel, int yvel)
{
	if (desRect.x + desRect.w < 0) {
		xvel = 1200 + rand()%800 + desRect.w + rand()%500 + 1;
		SetTexture(rand() % 6 + 4);
	}
	srcRect.x += srcRect.w;
	if (srcRect.x >= srcRect.w*totalFrame) srcRect.x = 0;
	desRect.x += xvel;
	desRect.y += yvel;
}

void GameObject::UpdatePosition() // Dành cho việc nhảy lên của player
{
	if (desRect.y <= 170 || landing) {
		landing = true;
		desRect.y += 5;
		if (desRect.y >= 400) {
			desRect.y = 400;
			landed = true;
			landing = false;
			return;
		}
	}
	else {
		desRect.y -= 10;
		landing = false;
	}
}

void GameObject::UpdatePosition(int xvel)
{
	if (desRect.x + desRect.w < 0) {
		xvel = 3000 + desRect.w + rand() % 3000 + 1;
	}
	srcRect.x += srcRect.w;
	if (srcRect.x >= srcRect.w * totalFrame) srcRect.x = 0;
	desRect.x += xvel;
}

void GameObject::Render()
{
	desRect.w = srcRect.w * scale;
	desRect.h = srcRect.h * scale;

	SDL_RenderCopy(renderer, objTexture, &srcRect, &desRect);
}

void GameObject::SetTexture(int action)
{
	totalFrame = 1;
	switch (action)
	{
	case JUMP:
		objTexture = LoadTexture("Assets/Walk.png");
		srcRect.x = srcRect.w * 3;
		break;
	case WALK:
		objTexture = LoadTexture("Assets/Walk4.png");
		totalFrame = 24;
		break;
	case IDLE:
		objTexture = LoadTexture("Assets/Idle2.png");
		totalFrame = 16;
		break;
	case DEATH:
		objTexture = LoadTexture("Assets/Death.png");
		totalFrame = 4;
		break;
	case ROCK1:
		objTexture = LoadTexture("Assets/rocks1_32.png");
		diffBox = 40;
		desRect.y = ROCK_Y1;
		break;
	case ROCK2:
		objTexture = LoadTexture("Assets/rocks1_52.png");
		diffBox = 40;
		desRect.y = ROCK_Y1;
		break;
	case ROCK3:
		objTexture = LoadTexture("Assets/rocks1_62.png");
		diffBox = 40;
		desRect.y = ROCK_Y3;
		break;
	case ROCK4:
		objTexture = LoadTexture("Assets/rocks1_72.png");
		diffBox = 40;
		desRect.y = ROCK_Y2;
		break;
	case TREE1:
		objTexture = LoadTexture("Assets/trees1_12.png");
		diffBox = 40;
		desRect.y = TREE_Y1;
		break;
	case TREE2:
		objTexture = LoadTexture("Assets/trees1_32.png");
		diffBox = 40;
		desRect.y = TREE_Y2;
		break;
	case ENEMY:
		objTexture = LoadTexture("Assets/Enemy3.png");
		totalFrame = 18;
		diffBox = 70;
		break;
	case INVICIBLE:
		objTexture = LoadTexture("Assets/Invicible.png");
		totalFrame = 24;
		break;
	case JUMPINV:
		objTexture = LoadTexture("Assets/JumpInvicible.png");
		srcRect.x = srcRect.w * 3;
		break;
	default:
		break;
	}
	SDL_QueryTexture(objTexture, NULL, NULL, &srcRect.w, &srcRect.h);
	srcRect.w /= totalFrame;
	if (action == 0 || action == 12)
		srcRect.w /= 6;
	desRect.w = srcRect.w;
	desRect.h = srcRect.h;
}

bool GameObject::checkCollision(GameObject*& obj)
{
	if (!check) return false;
	if (
		desRect.x + (srcRect.w)*scale >= obj->desRect.x + obj->diffBox &&
		desRect.y + (srcRect.h)*scale >= obj->desRect.y + obj->diffBox &&
		obj->desRect.x + (obj->srcRect.w)*obj->scale >= desRect.x + obj->diffBox &&
		obj->desRect.y + (obj->srcRect.h)*obj->scale >= desRect.y + obj->diffBox
		)
		return true;
	return false;
}

void GameObject::ManagePower(int onoff)
{
	if (onoff)
	{
		check = false;
		ready = false;
		SetTexture(INVICIBLE);
	}
	else {
		check = true;
		SetTexture(WALK);
	}
}

