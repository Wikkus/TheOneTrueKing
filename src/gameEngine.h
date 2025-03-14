#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_scancode.h>

#include <memory>
#include <random>
#include <unordered_map>
#include <vector>

#define PI 3.14159265358979
#define eulersNumber 2.71828

class Button;
class CollisionCheck;
class DebugDrawer;
class EnemyBase;
class EnemyManager;
class GameStateHandler;
class ImGuiHandler;
class ObjectBase;
class ObstacleManager;
class PlayerCharacter;
class ProjectileManager;
class SearchSortAlgorithms;
class RayCast;
class SteeringBehavior;
class TimerHandler;
class UniversalFunctions;
class WeaponManager;

enum class ButtonType;

template<class T>
class QuadTree;

template<class T>
class Vector2;

extern SDL_Window* window;
extern SDL_Renderer* renderer;

extern std::shared_ptr<CollisionCheck> collisionCheck;
extern std::shared_ptr<EnemyManager> enemyManager;
extern std::shared_ptr<DebugDrawer> debugDrawer;
extern std::shared_ptr<GameStateHandler> gameStateHandler;
extern std::shared_ptr<ImGuiHandler> imGuiHandler;

extern std::shared_ptr<ObstacleManager> obstacleManager;
extern std::vector<std::shared_ptr<PlayerCharacter>> playerCharacters;
extern std::shared_ptr<ProjectileManager> projectileManager;

extern std::shared_ptr<QuadTree<std::shared_ptr<ObjectBase>>> objectBaseQuadTree;
extern std::shared_ptr<QuadTree<std::shared_ptr<ObjectBase>>> obstacleQuadTree;
extern std::shared_ptr<QuadTree<std::shared_ptr<ObjectBase>>> projectileQuadTree;

extern std::shared_ptr<SearchSortAlgorithms> searchSort;
extern std::shared_ptr<RayCast> rayCast;
extern std::shared_ptr<SteeringBehavior> separationBehavior;
extern std::shared_ptr<TimerHandler> timerHandler;
extern std::shared_ptr<UniversalFunctions> universalFunctions;
extern std::shared_ptr<WeaponManager> weaponManager;

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
extern bool GetMouseButton(const Uint8& button);
extern bool GetMouseButtonPressed(const Uint8& button);
extern bool GetMouseButtonReleased(const Uint8& button);

extern KeyState keys[SDL_NUM_SCANCODES];
extern bool GetKey(const SDL_Scancode& key);
extern bool GetKeyPressed(const SDL_Scancode& key);
extern bool GetKeyReleased(const SDL_Scancode& key);

void DrawText(SDL_Texture* textTexture, SDL_Rect* textSrcrect, SDL_Rect textDst);
void ClearText(SDL_Surface* textSurface, SDL_Texture* textTexture);

extern const char* fontType;
extern const char* gameTitle;

extern float deltaTime;
extern int frameNumber;

extern unsigned int lastObjectID;

extern std::random_device randomDevice;
extern std::mt19937 randomEngine;

extern const Vector2<float> deactivatedPosition;
extern Vector2<float> cursorPosition;