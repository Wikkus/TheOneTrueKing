#include "stateStack.h"

#include "dataStructuresAndMethods.h"
#include "enemyManager.h"
#include "gameEngine.h"
#include "objectBase.h"
#include "obstacleManager.h"
#include "playerCharacter.h"
#include "projectileManager.h"
#include "quadTree.h"
#include "textSprite.h"
#include "timerManager.h"

#include <vector>

Button::Button(const char* buttonText, int height, int width, Vector2<float> position) {
	_position = position;
	
	_boxCollider = AABB::makeFromPositionSize(position, height, width);
	_text = std::make_shared<TextSprite>();
	
	_text->Init("res/roboto.ttf", 24, buttonText, _textColor);
	_text->SetTargetPosition(_position);
}

bool Button::ClickedOn() {
	Vector2<float> cursorPosition = GetCursorPosition();
	Circle circle;
	circle.position = cursorPosition;
	circle.radius = 10.f;
	if (GetMouseButtonPressed(SDL_BUTTON_LEFT)) {
		if (AABBCircleIntersect(_boxCollider, circle)) {
			return true;
		}
	}
	return false;
}

void Button::Render() {
	debugDrawer->AddDebugBox(_position, _boxCollider.min, _boxCollider.max, _buttonColor);
}

void Button::RenderText() {
	_text->RenderCentered();
}

void Button::SetTargetPosition(Vector2<float> position) {
	_position = position;
	_boxCollider = AABB::makeFromPositionSize(position, _boxCollider.height, _boxCollider.width);
	_text->SetTargetPosition(_position);
}
GameStateHandler::GameStateHandler() {
	_buttons[ButtonType::MainMenu] = std::make_shared<Button>(_mainMenuText, 64, 128, Vector2<float>(windowWidth * 0.5f, windowHeight * 0.6f));
	_buttons[ButtonType::Play] = std::make_shared<Button>(_playText, 64, 128, Vector2<float>(windowWidth * 0.5f, windowHeight * 0.3f));
	_buttons[ButtonType::Quit] = std::make_shared<Button>(_quitText, 64, 128, Vector2<float>(windowWidth * 0.5f, windowHeight * 0.8f));
	_buttons[ButtonType::Restart] = std::make_shared<Button>(_restartText, 64, 128, Vector2<float>(windowWidth * 0.5f, windowHeight * 0.4f));
	_buttons[ButtonType::Resume] = std::make_shared<Button>(_resumeText, 64, 128, Vector2<float>(windowWidth * 0.5f, windowHeight * 0.2f));
}

GameStateHandler::~GameStateHandler() {}

//Add a new state which will be the state that is called in update
void GameStateHandler::AddState(std::shared_ptr<State> state) {
	_states.emplace_back(state);
}
//Removes all the states except the first one which is MainMenu state
void GameStateHandler::BackToFirstState() {
	while (_states.size() > 1) {
		_states.pop_back();
	}
	_states.back()->SetButtonPositions();
}
//Replaces removes the current state and add a new state
void GameStateHandler::ReplaceCurrentState(std::shared_ptr<State> state) {
	_states.pop_back();
	_states.emplace_back(state);
}
/*Removes the current state and starts updating the former state
and sets the positions of the necessary buttons for said state*/
void GameStateHandler::RemoveCurrentState() {
	_states.pop_back();
	_states.back()->SetButtonPositions();
}
//Updates the state at the back of the vector
void GameStateHandler::UpdateState() {
	_states.back()->Update();
}

void GameStateHandler::RenderState() {
	_states.back()->Render();
}

void GameStateHandler::RenderStateText() {
	_states.back()->RenderText();
}

InGameState::InGameState() {
	playerCharacter->Respawn();
}

void InGameState::SetButtonPositions() {}

void InGameState::Update() {
	//objectBaseQuadTree->Insert(playerCharacter, playerCharacter->GetCircleCollider());
	
	enemyManager->UpdateQuadTree();
	projectileManager->UpdateQuadTree();

	enemyManager->Update();
	obstacleManager->UpdateObstacles();
	projectileManager->Update();
	playerCharacter->Update();
	timerManager->Update();


}

void InGameState::Render() {
	enemyManager->Render();
	obstacleManager->RenderObstacles();
	playerCharacter->Render();
	projectileManager->Render();
}

void InGameState::RenderText() {
	playerCharacter->RenderText();
}

SurvivalGameState::SurvivalGameState() {}

void SurvivalGameState::SetButtonPositions() {
	InGameState::SetButtonPositions();
}

void SurvivalGameState::Update() {
	enemyManager->UpdateSurvival();
	InGameState::Update();
}

void SurvivalGameState::Render() {
	InGameState::Render();
}

void SurvivalGameState::RenderText() {
	InGameState::RenderText();
}

TacticalGameState::TacticalGameState() {}

void TacticalGameState::SetButtonPositions() {
	InGameState::SetButtonPositions();
}

void TacticalGameState::Update() {
	InGameState::Update();
	enemyManager->UpdateTactical();

}

void TacticalGameState::Render() {
	InGameState::Render();
}

void TacticalGameState::RenderText() {}

GameOverState::GameOverState() {
	SetButtonPositions();
}

void GameOverState::SetButtonPositions() {
	_buttons[ButtonType::Restart]->SetTargetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.2f));
	_buttons[ButtonType::MainMenu]->SetTargetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.5f));
	_buttons[ButtonType::Quit]->SetTargetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.8f));
}

void GameOverState::Update() {
	if (_buttons[ButtonType::MainMenu]->ClickedOn()) {
		gameStateHandler->BackToFirstState();

	} else if (_buttons[ButtonType::Quit]->ClickedOn()) {
		runningGame = false;

	} else if (_buttons[ButtonType::Restart]->ClickedOn()) {
		gameStateHandler->ReplaceCurrentState(std::make_shared<InGameState>());
	}
}

void GameOverState::Render() {
	_buttons[ButtonType::MainMenu]->Render();
	_buttons[ButtonType::Quit]->Render();
	_buttons[ButtonType::Restart]->Render();
}

void GameOverState::RenderText() {
	_buttons[ButtonType::MainMenu]->RenderText();
	_buttons[ButtonType::Quit]->RenderText();
	_buttons[ButtonType::Restart]->RenderText();
}

MenuState::MenuState() {
	_titleTextPosition = { windowWidth * 0.5f, windowHeight * 0.125f };

	_titleText = std::make_shared<TextSprite>();
	_titleText->Init("res/roboto.ttf", 48, _gameTitle, _textColor);
	_titleText->SetTargetPosition(_titleTextPosition);
	
	SetButtonPositions();
}

void MenuState::SetButtonPositions() {
	_buttons[ButtonType::Play]->SetTargetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.3f));
	_buttons[ButtonType::Quit]->SetTargetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.7f));
}

void MenuState::Update() {
	if (_buttons[ButtonType::Play]->ClickedOn()) {
		gameStateHandler->AddState(std::make_shared<SurvivalGameState>());

	} else if (_buttons[ButtonType::Quit]->ClickedOn()) {
		runningGame = false;
	}
}

void MenuState::Render() {
	_buttons[ButtonType::Play]->Render();
	_buttons[ButtonType::Quit]->Render();
}

void MenuState::RenderText() {
	_titleText->RenderCentered();
	_buttons[ButtonType::Play]->RenderText();
	_buttons[ButtonType::Quit]->RenderText();
}

PauseState::PauseState() {
	SetButtonPositions();
}

void PauseState::SetButtonPositions() {
	_buttons[ButtonType::Resume]->SetTargetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.2f));
	_buttons[ButtonType::Restart]->SetTargetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.4f));
	_buttons[ButtonType::MainMenu]->SetTargetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.6f));
	_buttons[ButtonType::Quit]->SetTargetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.8f));
}

void PauseState::Update() {
	if (_buttons[ButtonType::MainMenu]->ClickedOn()) {
		gameStateHandler->BackToFirstState();
	
	} else if (_buttons[ButtonType::Quit]->ClickedOn()) {
		runningGame = false;
	
	} else if (_buttons[ButtonType::Restart]->ClickedOn()) {
		gameStateHandler->BackToFirstState();
		gameStateHandler->AddState(std::make_shared<InGameState>());

	} else if (_buttons[ButtonType::Resume]->ClickedOn() || GetKeyPressed(SDL_SCANCODE_ESCAPE)) {
		gameStateHandler->RemoveCurrentState();
	}
	
}

void PauseState::Render() {
	enemyManager->Render();
	playerCharacter->Render();
	projectileManager->Render();

	_buttons[ButtonType::MainMenu]->Render();
	_buttons[ButtonType::Restart]->Render();
	_buttons[ButtonType::Resume]->Render();
	_buttons[ButtonType::Quit]->Render();
}

void PauseState::RenderText() {
	playerCharacter->RenderText();

	_buttons[ButtonType::MainMenu]->RenderText();
	_buttons[ButtonType::Restart]->RenderText();
	_buttons[ButtonType::Resume]->RenderText();
	_buttons[ButtonType::Quit]->RenderText();
}

