#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_scancode.h>

#include <random>
#include <unordered_map>
#include <vector>

#define PI 3.14159265358979
#define eulersNumber 2.71828

class Button;
class DebugDrawer;
class EnemyManager;
class GameStateHandler;
class ImGuiHandler;
class ObjectBase;
class ObstacleManager;
class PlayerCharacter;
class ProjectileManager;
class RayCast;
class SteeringBehavior;
class TimerManager;

enum class ButtonType;

template<class T>
class QuadTree;

extern SDL_Window* window;
extern SDL_Renderer* renderer;

extern std::shared_ptr<EnemyManager> enemyManager;
extern std::shared_ptr<DebugDrawer> debugDrawer;
extern std::shared_ptr<GameStateHandler> gameStateHandler;
extern std::shared_ptr<ImGuiHandler> imGuiHandler;
extern std::shared_ptr<ObstacleManager> obstacleManager;
extern std::shared_ptr<PlayerCharacter> playerCharacter;
extern std::shared_ptr<ProjectileManager> projectileManager;
extern std::shared_ptr<QuadTree<std::shared_ptr<ObjectBase>>> objectBaseQuadTree;
extern std::shared_ptr<RayCast> rayCast;
extern std::shared_ptr<SteeringBehavior> separationBehavior;
extern std::shared_ptr<TimerManager> timerManager;


extern std::unordered_map<ButtonType, std::shared_ptr<Button>> _buttons;
extern bool runningGame;

extern float windowHeight;
extern float windowWidth;

struct KeyState {
	bool state = false;
	int changeFrame = 0;
};

struct MouseButtonState {
	bool state = false;
	int changeFrame = 0;
};

extern MouseButtonState mouseButtons[6];
extern bool GetMouseButton(Uint8 button);
extern bool GetMouseButtonPressed(Uint8 button);
extern bool GetMouseButtonReleased(Uint8 button);

extern KeyState keys[SDL_NUM_SCANCODES];
extern bool GetKey(SDL_Scancode key);
extern bool GetKeyPressed(SDL_Scancode key);
extern bool GetKeyReleased(SDL_Scancode key);

void DrawText(SDL_Texture* textTexture, SDL_Rect* textSrcrect, SDL_Rect textDst);
void ClearText(SDL_Surface* textSurface, SDL_Texture* textTexture);


extern float deltaTime;
extern int frameNumber;

extern std::random_device randomDevice;
extern std::mt19937 randomEngine;