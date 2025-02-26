#include "gameEngine.h"

#include "universalFunctions.h"
#include "debugDrawer.h"
#include "enemyBase.h"
#include "enemyManager.h"
#include "imGuiManager.h"
#include "obstacleManager.h"
#include "playerCharacter.h"
#include "projectileManager.h"
#include "quadTree.h"
#include "searchSortAlgorithms.h"
#include "rayCast.h"
#include "stateStack.h"
#include "steeringBehavior.h"
#include "timerManager.h"
#include "weaponManager.h"
#include "vector2.h"

SDL_Window* window;
SDL_Renderer* renderer;

std::shared_ptr<CollisionCheck> collisionCheck;
std::shared_ptr<RayCast> rayCast;

std::shared_ptr<DebugDrawer> debugDrawer;
std::shared_ptr<GameStateHandler> gameStateHandler;
std::shared_ptr<ImGuiHandler> imGuiHandler;
std::shared_ptr<QuadTree<std::shared_ptr<ObjectBase>>> objectBaseQuadTree;
std::shared_ptr<SearchSortAlgorithms> searchSort;
std::shared_ptr<UniversalFunctions> universalFunctions;

std::vector<std::shared_ptr<PlayerCharacter>> playerCharacters;
std::shared_ptr<EnemyManager> enemyManager;
std::shared_ptr<ObstacleManager> obstacleManager;
std::shared_ptr<ProjectileManager> projectileManager;
std::shared_ptr<TimerManager> timerManager;
std::shared_ptr<WeaponManager> weaponManager;

std::unordered_map<ButtonType, std::shared_ptr<Button>> _buttons;

bool runningGame = false;

float windowHeight = 600.f;
float windowWidth = 800.f;

MouseButtonState mouseButtons[6];

KeyState keys[SDL_NUM_SCANCODES];
bool GetMouseButton(const Uint8& button) {
	return mouseButtons[button].state;
}

bool GetMouseButtonPressed(const Uint8& button) {
	return mouseButtons[button].state && mouseButtons[button].changeFrame == frameNumber;
}

bool GetMouseButtonReleased(const Uint8& button) {
	return !mouseButtons[button].state && mouseButtons[button].changeFrame == frameNumber;
}

bool GetKey(const SDL_Scancode& key) {
	return keys[key].state;
}

bool GetKeyPressed(const SDL_Scancode& key) {
	return keys[key].state && keys[key].changeFrame == frameNumber;
}

bool GetKeyReleased(const SDL_Scancode& key) {
	return !keys[key].state && keys[key].changeFrame == frameNumber;
}

void DrawText(SDL_Texture* textTexture, SDL_Rect* textSrcrect, SDL_Rect textDst) {
	SDL_RenderCopy(renderer, textTexture, textSrcrect, &textDst);
}

void ClearText(SDL_Surface* textSurface, SDL_Texture* textTexture) {
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);
}

const char* fontType = "res/roboto.ttf";
const char* gameTitle = "TheOneTrueKing";

float deltaTime = 0.f;
int frameNumber = 0;

unsigned int lastObjectID = 0;

std::random_device randomDevice;
std::mt19937 randomEngine(randomDevice());

const Vector2<float>& deactivatedPosition = { -10000.f, -10000.f };