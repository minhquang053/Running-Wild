#include <Windows.h>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
#include "GameObject.h"
#include <fstream>
#include <cmath>

#define NUMBER_OF_HINDRANCES 4
#define COOLDOWN 8
#define SKILL 10

const int SCREEN_WIDTH{ 1200 };
const int SCREEN_HEIGHT{ 604 };
const char* WINDOW_TITLE{ "Running Wild" };


SDL_Texture* LoadTexture(const char* filepath);
void LoadMusic();
//
SDL_Renderer* renderer;
SDL_Event event;
//

Mix_Music* gMusic;


// Manage Game Objects
SDL_Texture* background;
SDL_Texture* startButton;
SDL_Texture* gameover;
SDL_Texture* newgame;

GameObject* player;
GameObject* hinds[NUMBER_OF_HINDRANCES];
GameObject* enemy;


int main(int argc, char **argv)
{
	std::string Highscore;
	std::ifstream reader("Highscore.txt");
	getline(reader, Highscore);
	reader.close();

	// Init
	SDL_Window* window;
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) std::cout << "SDL_Init() ! Error" << std::endl;

	window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL) std::cout << "SDL_CreateWindow() ! Error" << std::endl;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) exit(1);
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);


INTRO:
	bool quit = false;

	background = LoadTexture("Assets/spritesheet.png");
	SDL_Rect srcBackground{ 0,0,1920,1080 };

	startButton = LoadTexture("Assets/start.png");
	SDL_Rect desStart{800, 400, 0, 0};
	SDL_QueryTexture(startButton, NULL, NULL, &desStart.w, &desStart.h);
	desStart.w /= 2.5;
	desStart.h /= 2.5;
	desStart.x = 600 - desStart.w / 2 - 30;

	TTF_Init();
	bool start = false;
	TTF_Font* pfont = TTF_OpenFont("Assets/right.ttf", 100);
	SDL_Surface* psurface = TTF_RenderText_Solid(pfont, "Running Wild", { 0,0,0 });
	SDL_Surface* how1Surface = TTF_RenderText_Solid(pfont, "Space/Up Arrow/W/Left Click : Jump", {0,0,0});
	SDL_Surface* how2Surface = TTF_RenderText_Solid(pfont, "K/Right Click : Be invicible for 8 seconds (20 seconds cooldown)", {0,0,0});
	SDL_Texture* how1Texture = SDL_CreateTextureFromSurface(renderer, how1Surface);
	SDL_Texture* how2Texture = SDL_CreateTextureFromSurface(renderer, how2Surface);
	SDL_Texture* ptexture = SDL_CreateTextureFromSurface(renderer, psurface);
	SDL_Rect despRect{ 0,85, psurface->w, psurface->h };
	despRect.x = 600 - despRect.w / 2 - 10;
	SDL_Rect des1How{ 0, 250, how1Surface->w, how1Surface->h };
	des1How.w /= 3;
	des1How.h /= 3;
	des1How.x = 600 - des1How.w / 2;
	SDL_Rect des2How{ 0, 300, how2Surface->w, how2Surface->h };
	des2How.w /= 3;
	des2How.h /= 3;
	des2How.x = 600 - des2How.w / 2;

	SDL_FreeSurface(how2Surface);
	SDL_FreeSurface(psurface);
	SDL_FreeSurface(how1Surface);

	player = new GameObject(2, 50, 400, 4, 3);
	while (!start)
	{
		SDL_RenderCopy(renderer, background, &srcBackground, NULL);
		SDL_RenderCopy(renderer, ptexture, NULL, &despRect);
		SDL_RenderCopy(renderer, startButton, NULL, &desStart);
		SDL_RenderCopy(renderer, how1Texture, NULL, &des1How);
		SDL_RenderCopy(renderer, how2Texture, NULL, &des2How);
		player->Render();
		SDL_RenderPresent(renderer);
		player->UpdatePosition(0, 0);
		SDL_PollEvent(&event);
		switch (event.type) {
		case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT &&
				event.button.x >= desStart.x &&
				event.button.x <= desStart.x + desStart.w &&
				event.button.y >= desStart.y &&
				event.button.y <= desStart.y + desStart.h){
				start = true;
			}
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_SPACE)
				start = true;
			break;
		case SDL_QUIT:
			quit = true;
			start = true;
			break;
		default:
			break;
		}
		SDL_Delay(20);

	}

	delete player;


	Mix_Init(MIX_INIT_MP3);
	Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);

START:
	// Handle frame
	const int FPS = 120;
	const int frameDelay = 1000/FPS;
	Uint32 frameStart;
	int frameTime;

	//


	srand(100);
	int step{};

	player = new GameObject(1, 50, 400, 12, 3);	// action, xpos, ypos, total frames, scale
	enemy = new GameObject(10, 1400, 250, 6, 3);

	int num_in_screen{1}, i; // Để quản lý các chướng ngại vật;
	for (i = 0; i < NUMBER_OF_HINDRANCES; ++i) {
		int ranNum = rand() % 6 + 4;
		hinds[i] = new GameObject(ranNum, 1200 + step + rand()%600, 455, 2, 2);
		step = 700;
	}

	// Make scoreboard
	int score = 0;
	int frameRendered = 0;
	int e = 0;

	int skill = SKILL + 1;
	int cool = COOLDOWN + 1;
	int frameRendered2 = 0;
	int frameRendered3 = 0;
	bool used = false;

	SDL_Color skill_color = { 255, 255, 255 };
	SDL_Color cool_color = { 255, 77, 77 };
	SDL_Color text_color = { 0, 0, 0 };
	TTF_Font* font = TTF_OpenFont("Assets/arcade.ttf", 50);
	SDL_Surface* hsurface = TTF_RenderText_Solid(font, "NEW   HIGHSCORE", text_color);
	SDL_Texture* highScore = SDL_CreateTextureFromSurface(renderer, hsurface);
	SDL_Rect desHigh = { 0, 0, hsurface->w, hsurface->h };
	SDL_Texture* ftexture = NULL;
	SDL_Texture* stexture = NULL;
	SDL_Surface* textSurface = NULL;
	SDL_Texture* ctexture = NULL;

	SDL_Rect desSkillRect;
	SDL_Rect desTextRect;
	SDL_Rect desCoolRect;

RUNNING:
	{
		if (!Mix_PlayingMusic()) LoadMusic();

		while (!quit)
		{
			//Score
			frameRendered++;
			if (frameRendered == 6)
			{
				score++;
				if (score == 2000 || score == 200) num_in_screen++;
				frameRendered = 0;
			}

			if (!player->check) frameRendered2++;
			if (frameRendered2 == 60)
			{
				skill--;
				if (skill == 0) {
					used = true;
					skill = SKILL + 1;
                    player->ManagePower(0);
				}
				textSurface = TTF_RenderText_Solid(font, std::to_string(skill).c_str(), skill_color);
				stexture = SDL_CreateTextureFromSurface(renderer, textSurface);
				desSkillRect = { 17, 390, textSurface->w, textSurface->h };

				frameRendered2 = 0;
			}

			if (used) frameRendered3++;
			if (frameRendered3 == 60)
			{
				cool--;
				if (cool == -2) {
					used = false;
					cool = COOLDOWN + 1;
                    player->ready = true;
				}
				if (cool < 0) {
					textSurface = TTF_RenderText_Solid(font, "Skill is ready !", cool_color);
				}
				else textSurface = TTF_RenderText_Solid(font, std::to_string(cool).c_str(), cool_color);

				ctexture = SDL_CreateTextureFromSurface(renderer, textSurface);
				desCoolRect = { 17, 390, textSurface->w, textSurface->h };

				frameRendered3 = 0;
			}


			if (score > pow(10, e)) e++;
			textSurface = TTF_RenderText_Solid(font, std::to_string(score).c_str(), text_color);
			ftexture = SDL_CreateTextureFromSurface(renderer, textSurface);
			desTextRect = { 1150-e*20, 50, textSurface->w, textSurface->h };
			//


			frameStart = SDL_GetTicks();

			SDL_PollEvent(&event);
			switch (event.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) {
					goto JUMPING;
				}
				else if (event.key.keysym.sym == SDLK_k) {
					if (player->check && player->ready)
					{
						player->ManagePower(1);
					}
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT)
					goto JUMPING;
				else if (event.button.button == SDL_BUTTON_RIGHT)
					if (player->check && player->ready)
					{
						player->ManagePower(1);
					}
				break;
			default:
				break;
			}


			// Render
			//SDL_RenderClear(renderer);

			SDL_RenderCopy(renderer, background, &srcBackground, NULL);
			SDL_RenderCopy(renderer, ftexture, NULL, &desTextRect);
			if (skill <= SKILL && skill >= 0) SDL_RenderCopy(renderer, stexture, NULL, &desSkillRect);
			if (cool <= COOLDOWN && cool >= -2) SDL_RenderCopy(renderer, ctexture, NULL, &desCoolRect);

			for (i = 0; i < num_in_screen; ++i)
				hinds[i]->Render();
			enemy->Render();
			player->Render();


			SDL_RenderPresent(renderer);

			// Check collision
			for (i = 0; i < num_in_screen; ++i)
				if (player->checkCollision(hinds[i])) goto DYING;

			// Update position
				if (srcBackground.x == 1920 * 2) srcBackground.x = 0;
			srcBackground.x += 4;

			for (i = 0; i < num_in_screen; ++i)
				hinds[i]->UpdatePosition(-7, 0);
			enemy->UpdatePosition(-12);
			player->UpdatePosition(0, 0);
			//
			frameTime = SDL_GetTicks() - frameStart;
			if (frameDelay > frameTime) {
				SDL_Delay(frameDelay - frameTime);
			}
		}
	}
	goto ENDGAME;


JUMPING:
	{
		if (!Mix_PlayingMusic()) LoadMusic();

		if (player->check) player->SetTexture(0);
		else player->SetTexture(12);
		while (!player->landed)
		{
			//
			frameRendered++;
			if (frameRendered == 6)
			{
				score++;
				if (score == 200 || score == 2000) num_in_screen++;
				frameRendered = 0;
			}

			if (!player->check) frameRendered2++;
			if (frameRendered2 == 60)
			{
				skill--;
				if (skill == 0) {
					used = true;
					skill = SKILL + 1;
                    player->ManagePower(0);
				}
				textSurface = TTF_RenderText_Solid(font, std::to_string(skill).c_str(), skill_color);
				stexture = SDL_CreateTextureFromSurface(renderer, textSurface);
				desSkillRect = { 17, 390, textSurface->w, textSurface->h };

				frameRendered2 = 0;
			}

			if (used) frameRendered3++;
			if (frameRendered3 == 60)
			{
				cool--;
				if (cool == -2) {
					used = false;
					cool = COOLDOWN + 1;
                    player->ready = true;
				}
				if (cool < 0) {
					textSurface = TTF_RenderText_Solid(font, "Skill is ready !", cool_color);
				}
				else textSurface = TTF_RenderText_Solid(font, std::to_string(cool).c_str(), cool_color);
				ctexture = SDL_CreateTextureFromSurface(renderer, textSurface);
				desCoolRect = { 17, 390, textSurface->w, textSurface->h };

				frameRendered3 = 0;
			}


			if (score > pow(10, e)) e++;
			SDL_Surface* textSurface = TTF_RenderText_Solid(font, std::to_string(score).c_str(), text_color);
			ftexture = SDL_CreateTextureFromSurface(renderer, textSurface);
			SDL_Rect desTextRect{ 1150 - e * 20, 50, textSurface->w, textSurface->h };

			//

			while (SDL_PollEvent(&event))
				switch (event.type) {
				case SDL_QUIT:
					quit = true;
					goto ENDGAME;
					break;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_k && player->check && player->ready) {
						player->ManagePower(1);
						player->SetTexture(12);
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_RIGHT && player->ready) {
						player->ManagePower(1);
						player->SetTexture(12);
					}
					break;
				default:
					break;
				}
			frameStart = SDL_GetTicks();

			SDL_RenderCopy(renderer, background, &srcBackground, NULL);
			SDL_RenderCopy(renderer, ftexture, NULL, &desTextRect);
			if (skill <= SKILL && skill >= 0) SDL_RenderCopy(renderer, stexture, NULL, &desSkillRect);
			if (cool <= COOLDOWN && cool >= -2) SDL_RenderCopy(renderer, ctexture, NULL, &desCoolRect);
			for (i = 0; i < num_in_screen; ++i)
				hinds[i]->Render();
			enemy->Render();
			player->Render();


			SDL_RenderPresent(renderer);

			// Check collision
			for (i = 0; i < num_in_screen; ++i)
				if (player->checkCollision(hinds[i])) goto DYING;
			if (player->checkCollision(enemy)) goto DYING;

			// Update position
			if (srcBackground.x == 1920 * 2) srcBackground.x = 0;
			srcBackground.x += 4;

			for (i = 0; i < num_in_screen; ++i)
				hinds[i]->UpdatePosition(-7, 0);
			enemy->UpdatePosition(-12);
			player->UpdatePosition();
			//
			frameTime = SDL_GetTicks() - frameStart;
			if (frameDelay > frameTime) {
				SDL_Delay(frameDelay - frameTime);
			}
		}
		player->landed = false;
		if (player->check) player->SetTexture(1);
		else player->SetTexture(11);
		goto RUNNING;
	}

DYING:
	{

	if (!Mix_PlayingMusic()) LoadMusic();

	auto timeout = 1000 + SDL_GetTicks();
		player->SetTexture(3); // -> DEATH
		player->UpdatePosition(0, 0);
		while (SDL_GetTicks() < timeout)
		{
			SDL_RenderCopy(renderer, background, &srcBackground, NULL);
			for (i = 0; i < num_in_screen; ++i)
				hinds[i]->Render();
			player->Render();
			SDL_RenderPresent(renderer);
			player->UpdatePosition(0, 0);
			SDL_Delay(250);
		}
	}
ENDGAME:
	{
		//Restart
		desHigh.x = 430;
		desHigh.y = 100;
		gameover = LoadTexture("Assets/gameover.png");
		SDL_Rect desOver = { 0, 170, 0, 0 };
		SDL_QueryTexture(gameover, NULL, NULL, &desOver.w, &desOver.h);
		desOver.w /= 2;
		desOver.h /= 2;
		desOver.x = 600 - desOver.w / 2 + 4;

		newgame = LoadTexture("Assets/newgame.png");
		SDL_Rect desNew = { 0, 400, 0, 0 };
		SDL_QueryTexture(newgame, NULL, NULL, &desNew.w, &desNew.h);
		desNew.w /= 3;
		desNew.h /= 3;
		desNew.x = 600 - desNew.w / 2 + 4;

		if (score > stoi(Highscore))
		{
			std::ofstream writer("Highscore.txt");
			writer << std::to_string(score) << std::endl;
			writer.close();
		}

		while (!quit)
		{
			if (!Mix_PlayingMusic()) LoadMusic();

			SDL_RenderCopy(renderer, background, &srcBackground, NULL);
			SDL_RenderCopy(renderer, gameover, NULL, &desOver);
			SDL_RenderCopy(renderer, newgame, NULL, &desNew);

			if (score > stoi(Highscore))
			{
				SDL_RenderCopy(renderer, highScore, NULL, &desHigh);
			}

			SDL_RenderPresent(renderer);
			SDL_PollEvent(&event);
			switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT &&
					event.button.x >= desNew.x &&
					event.button.x <= desNew.x + desNew.w &&
					event.button.y >= desNew.y &&
					event.button.y <= desNew.y + desNew.h) {
					quit = false;
					goto START;
				}
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_SPACE)
					goto START;
				break;
			case SDL_QUIT:
				quit = true;
				break;
			default:
				break;
			}
		}


		SDL_DestroyTexture(startButton);
		SDL_DestroyTexture(gameover);
		SDL_DestroyTexture(newgame);
		SDL_DestroyTexture(highScore);
		SDL_DestroyTexture(how1Texture);
		SDL_DestroyTexture(how2Texture);
		SDL_DestroyTexture(ptexture);
		SDL_DestroyTexture(ctexture);
		SDL_DestroyTexture(ftexture);
		SDL_DestroyTexture(stexture);



		SDL_FreeSurface(textSurface);
		SDL_FreeSurface(hsurface);

		Mix_FreeMusic(gMusic);

		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);

		Mix_Quit();
		IMG_Quit();
		TTF_Quit();
		SDL_Quit();

		return 0;
	}
}
