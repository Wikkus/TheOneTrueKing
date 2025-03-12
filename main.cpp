#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <windows.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_sdl.h"
#include "ImGui/imgui_impl_sdl.h"

#include "src/bossBoar.h"
#include "src/collision.h"
#include "src/debugDrawer.h"
#include "src/enemyBase.h"
#include "src/enemyManager.h"
#include "src/gameEngine.h"
#include "src/imGuiManager.h"
#include "src/obstacleManager.h"
#include "src/playerCharacter.h"
#include "src/projectileManager.h"
#include "src/quadTree.h"
#include "src/searchSortAlgorithms.h"
#include "src/rayCast.h"
#include "src/sprite.h"
#include "src/stateStack.h"
#include "src/steeringBehavior.h"
#include "src/timerHandler.h"
#include "src/textSprite.h"
#include "src/universalFunctions.h"
#include "src/vector2.h"
#include "src/WeaponManager.h"

int main(int argc, char* argv[]) {
	HWND windowHandle = GetConsoleWindow();
	ShowWindow(windowHandle, SW_HIDE);

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	IMG_Init(1);

	window = SDL_CreateWindow(gameTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	collisionCheck = std::make_shared<CollisionCheck>();
	enemyManager = std::make_shared<EnemyManager>();
	gameStateHandler = std::make_shared<GameStateHandler>();
	debugDrawer = std::make_shared<DebugDrawer>();
	imGuiHandler = std::make_shared<ImGuiHandler>();
	obstacleManager = std::make_shared<ObstacleManager>();
	projectileManager = std::make_shared<ProjectileManager>();
	playerCharacters.emplace_back(std::make_shared<PlayerCharacter>(0.f, Vector2<float>(windowWidth * 0.5f, windowHeight * 0.5f)));
	searchSort = std::make_shared<SearchSortAlgorithms>();
	rayCast = std::make_shared<RayCast>();

	timerHandler = std::make_shared<TimerHandler>();
	universalFunctions = std::make_shared<UniversalFunctions>();
	weaponManager = std::make_shared<WeaponManager>();

	QuadTreeNode quadTreeNode;
	quadTreeNode.rectangle = std::make_shared<AABB>();
	quadTreeNode.rectangle->Init(Vector2(windowWidth * 0.5f, windowHeight * 0.5f), windowHeight, windowWidth);
	objectBaseQuadTree = std::make_shared<QuadTree<std::shared_ptr<ObjectBase>>>(quadTreeNode, 100);
	obstacleQuadTree = std::make_shared<QuadTree<std::shared_ptr<ObjectBase>>>(quadTreeNode, 25);
	projectileQuadTree = std::make_shared<QuadTree<std::shared_ptr<ObjectBase>>>(quadTreeNode, 200);

	//Init here
	cursorPosition = universalFunctions->GetCursorPosition();
	enemyManager->Init();
	for (unsigned int i = 0; i < playerCharacters.size(); i++) {
		playerCharacters[i]->Init();
	}
	imGuiHandler->Init();
	projectileManager->Init();
	obstacleManager->Init();
	weaponManager->Init();

	gameStateHandler->AddState(std::make_shared<MenuState>());

	Uint64 previous_ticks = SDL_GetPerformanceCounter();
	runningGame = true;

	SDL_Event eventType;
	int scanCode = 0;
	Uint64 ticks;
	Uint64 delta_ticks;

	while (runningGame) {
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();

		frameNumber++;
		ticks = SDL_GetPerformanceCounter();
		delta_ticks = ticks - previous_ticks;
		previous_ticks = ticks;
		deltaTime = (float)delta_ticks / (float)SDL_GetPerformanceFrequency();

		while (SDL_PollEvent(&eventType)) {
			ImGui_ImplSDL2_ProcessEvent(&eventType);
			switch (eventType.type) {
				case SDL_QUIT: {
					runningGame = false;
					break;
				}
				case SDL_KEYDOWN: {
					scanCode = eventType.key.keysym.scancode;
					if (eventType.key.repeat) {
						break;
					}
					keys[scanCode].changeFrame = frameNumber;
					keys[scanCode].state = true;
					break;
				}
				case SDL_KEYUP: {
					scanCode = eventType.key.keysym.scancode;
					keys[scanCode].changeFrame = frameNumber;
					keys[scanCode].state = false;
					break;
				}
				case SDL_MOUSEBUTTONDOWN: {
					if (eventType.key.repeat) {
						break;
					}
					mouseButtons[eventType.button.button].changeFrame = frameNumber;
					mouseButtons[eventType.button.button].state = true;
					break;
				}
				case SDL_MOUSEBUTTONUP: {
					mouseButtons[eventType.button.button].changeFrame = frameNumber;
					mouseButtons[eventType.button.button].state = false;
				}
			}
		}

		//Update here
		cursorPosition = universalFunctions->GetCursorPosition();
		gameStateHandler->UpdateState();

		SDL_SetRenderDrawColor(renderer, 75, 75, 75, 255);
		SDL_RenderClear(renderer);

		//Render images here
		gameStateHandler->RenderState();

		debugDrawer->DrawRectangles();
		debugDrawer->DrawCircles();
		debugDrawer->DrawLines();

		//Render text here
		gameStateHandler->RenderStateText();

		//objectBaseQuadTree->Render();
		objectBaseQuadTree->Clear();
		projectileQuadTree->Clear();
		
		imGuiHandler->Render();

		SDL_RenderPresent(renderer);
		SDL_Delay(16);
	}
	imGuiHandler->ShutDown();
	SDL_DestroyWindow(window);
	SDL_Quit();
	std::exit(0);
	
	return 0;
}