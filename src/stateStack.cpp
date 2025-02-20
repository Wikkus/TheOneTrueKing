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
#include <string>

Button::Button(const char* buttonText, int height, int width, Vector2<float> position) {
	_position = position;
	
	_boxCollider.Init(position, height, width);
	_cursorCollider.Init({ 0.f, 0.f }, 10.f);
	_text = std::make_shared<TextSprite>();
	
	_text->Init("res/roboto.ttf", 24, buttonText, _textColor);
	_text->SetPosition(_position);
}

bool Button::ClickedOn() {
	_cursorCollider.SetPosition(GetCursorPosition());
	if (GetMouseButtonPressed(SDL_BUTTON_LEFT)) {
		if (collisionCheck->AABBCircleIntersect(_boxCollider, _cursorCollider)) {
			return true;
		}
	}
	return false;
}

void Button::Render() {
	debugDrawer->AddDebugBox(_position, _boxCollider.GetMin(), _boxCollider.GetMax(), _buttonColor);
}

void Button::RenderText() {
	_text->RenderCentered();
}

void Button::SetPosition(Vector2<float> position) {
	_position = position;
	_boxCollider.SetPosition(position);
	_text->SetPosition(_position);
}
GameStateHandler::GameStateHandler() {
	_buttons[ButtonType::MainMenu] = std::make_shared<Button>(_mainMenuText, 64, 128, Vector2<float>(windowWidth * 0.5f, windowHeight * 0.6f));
	_buttons[ButtonType::BossRush] = std::make_shared<Button>(_bossRushText, 64, 128, Vector2<float>(windowWidth * 0.33f, windowHeight * 0.3f));
	_buttons[ButtonType::FormationGame] = std::make_shared<Button>(_formationText, 64, 128, Vector2<float>(windowWidth * 0.33f, windowHeight * 0.3f));
	_buttons[ButtonType::SurvivalGame] = std::make_shared<Button>(_survivalText, 64, 128, Vector2<float>(windowWidth * 0.66f, windowHeight * 0.3f));
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

void GameStateHandler::SetGameMode(GameMode gameMode) {
	_currentGameMode = gameMode;
}

const GameMode GameStateHandler::GetGameMode() const {
	return _currentGameMode;
}

InGameState::InGameState() {
	enemyManager->Reset();
	projectileManager->Reset();
	for (unsigned int i = 0; i < playerCharacters.size(); i++) {
		playerCharacters[i]->Respawn();
	}
}

void InGameState::SetButtonPositions() {}

void InGameState::Update() {
	enemyManager->InsertEnemiesQuadtree();
	projectileManager->InsertProjectilesQuadtree();
	for (unsigned int i = 0; i < playerCharacters.size(); i++) {
		objectBaseQuadTree->Insert(playerCharacters[i], playerCharacters[i]->GetCollider());
		playerCharacters[i]->Update();
	}
	enemyManager->Update();
	obstacleManager->UpdateObstacles();
	projectileManager->Update();
	timerManager->Update();
}

void InGameState::Render() {
	obstacleManager->RenderObstacles();
	enemyManager->Render();
	for (unsigned int i = 0; i < playerCharacters.size(); i++) {
		playerCharacters[i]->Render();
	}
	projectileManager->Render();
}

void InGameState::RenderText() {
	for (unsigned int i = 0; i < playerCharacters.size(); i++) {
		playerCharacters[i]->RenderText();
	}
}

BossRushGameState::BossRushGameState() {
	enemyManager->BossSpawner();
}

void BossRushGameState::SetButtonPositions() {
	InGameState::SetButtonPositions();
}
void BossRushGameState::Update() {
	InGameState::Update();
	enemyManager->UpdateBossRush();
}
void BossRushGameState::Render() {
	InGameState::Render();
	enemyManager->RenderText();
}
void BossRushGameState::RenderText() {
	InGameState::RenderText();
}

FormationGameState::FormationGameState() {}

void FormationGameState::SetButtonPositions() {
	InGameState::SetButtonPositions();
}
void FormationGameState::Update() {
	InGameState::Update();
	enemyManager->UpdateFormation();
}
void FormationGameState::Render() {
	InGameState::Render();
}
void FormationGameState::RenderText() {
	InGameState::RenderText();
}

SurvivalGameState::SurvivalGameState() {}

void SurvivalGameState::SetButtonPositions() {
	InGameState::SetButtonPositions();
}
void SurvivalGameState::Update() {
	InGameState::Update();
	enemyManager->UpdateSurvival();
}
void SurvivalGameState::Render() {
	InGameState::Render();
}
void SurvivalGameState::RenderText() {
	InGameState::RenderText();
}

GameOverState::GameOverState() {
	_waveNumberText = std::make_shared<TextSprite>();
	_waveNumberText->Init("res/roboto.ttf", 24, ("You died on wave: " + std::to_string(enemyManager->GetWaveNumber())).c_str(), { 255, 255, 255, 255});
	_waveNumberText->SetPosition({ windowWidth * 0.2f, windowHeight * 0.35f });
	SetButtonPositions();
}

void GameOverState::SetButtonPositions() {
	_buttons[ButtonType::Restart]->SetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.2f));
	_buttons[ButtonType::MainMenu]->SetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.5f));
	_buttons[ButtonType::Quit]->SetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.8f));
}

void GameOverState::Update() {
	if (_buttons[ButtonType::MainMenu]->ClickedOn()) {
		gameStateHandler->BackToFirstState();

	} else if (_buttons[ButtonType::Quit]->ClickedOn()) {
		runningGame = false;

	} else if (_buttons[ButtonType::Restart]->ClickedOn()) {
		switch (gameStateHandler->GetGameMode()) {
		case GameMode::Formation:
			gameStateHandler->ReplaceCurrentState(std::make_shared<FormationGameState>());
			break;
		case GameMode::Survival:
			gameStateHandler->ReplaceCurrentState(std::make_shared<SurvivalGameState>());
			break;
		default:
			break;
		}
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
	_waveNumberText->RenderCentered();
}

MenuState::MenuState() {
	_titleTextPosition = { windowWidth * 0.5f, windowHeight * 0.125f };

	_titleText = std::make_shared<TextSprite>();
	_titleText->Init("res/roboto.ttf", 48, _gameTitle, _textColor);
	_titleText->SetPosition(_titleTextPosition);
	
	SetButtonPositions();
}

void MenuState::SetButtonPositions() {
	_buttons[ButtonType::BossRush]->SetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.3f));
	_buttons[ButtonType::FormationGame]->SetPosition(Vector2<float>(windowWidth * 0.3f, windowHeight * 0.55f));
	_buttons[ButtonType::SurvivalGame]->SetPosition(Vector2<float>(windowWidth * 0.7f, windowHeight * 0.55f));
	_buttons[ButtonType::Quit]->SetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.8f));
}

void MenuState::Update() {
	if (_buttons[ButtonType::BossRush]->ClickedOn()) {
		gameStateHandler->SetGameMode(GameMode::BossRush);
		gameStateHandler->AddState(std::make_shared<BossRushGameState>());

	} else if (_buttons[ButtonType::FormationGame]->ClickedOn()) {
		gameStateHandler->SetGameMode(GameMode::Formation);
		gameStateHandler->AddState(std::make_shared<FormationGameState>());

	} else if (_buttons[ButtonType::SurvivalGame]->ClickedOn()) {
		gameStateHandler->SetGameMode(GameMode::Survival);
		gameStateHandler->AddState(std::make_shared<SurvivalGameState>());

	} else if (_buttons[ButtonType::Quit]->ClickedOn()) {
		runningGame = false;
	}
}

void MenuState::Render() {
	_buttons[ButtonType::BossRush]->Render();
	_buttons[ButtonType::FormationGame]->Render();
	_buttons[ButtonType::SurvivalGame]->Render();
	_buttons[ButtonType::Quit]->Render();
}

void MenuState::RenderText() {
	_titleText->RenderCentered();
	_buttons[ButtonType::BossRush]->RenderText();
	_buttons[ButtonType::FormationGame]->RenderText();
	_buttons[ButtonType::SurvivalGame]->RenderText();
	_buttons[ButtonType::Quit]->RenderText();
}

PauseState::PauseState() {
	SetButtonPositions();
}

void PauseState::SetButtonPositions() {
	_buttons[ButtonType::Resume]->SetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.2f));
	_buttons[ButtonType::Restart]->SetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.4f));
	_buttons[ButtonType::MainMenu]->SetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.6f));
	_buttons[ButtonType::Quit]->SetPosition(Vector2<float>(windowWidth * 0.5f, windowHeight * 0.8f));
}

void PauseState::Update() {
	if (_buttons[ButtonType::MainMenu]->ClickedOn()) {
		gameStateHandler->BackToFirstState();
	
	} else if (_buttons[ButtonType::Quit]->ClickedOn()) {
		runningGame = false;
	
	} else if (_buttons[ButtonType::Restart]->ClickedOn()) {
		gameStateHandler->BackToFirstState();
		switch (gameStateHandler->GetGameMode()) {
		case GameMode::BossRush:
			gameStateHandler->AddState(std::make_shared<BossRushGameState>());
			break;
		case GameMode::Formation:
			gameStateHandler->AddState(std::make_shared<FormationGameState>());
			break;
		case GameMode::Survival:
			gameStateHandler->AddState(std::make_shared<SurvivalGameState>());
			break;
		default:
			break;
		}

	} else if (_buttons[ButtonType::Resume]->ClickedOn() || GetKeyPressed(SDL_SCANCODE_ESCAPE)) {
		gameStateHandler->RemoveCurrentState();
	}
}

void PauseState::Render() {
	enemyManager->Render();
	for (unsigned int i = 0; i < playerCharacters.size(); i++) {
		playerCharacters[i]->Render();
	}
	projectileManager->Render();

	_buttons[ButtonType::MainMenu]->Render();
	_buttons[ButtonType::Restart]->Render();
	_buttons[ButtonType::Resume]->Render();
	_buttons[ButtonType::Quit]->Render();
}

void PauseState::RenderText() {
	for (unsigned int i = 0; i < playerCharacters.size(); i++) {
		playerCharacters[i]->RenderText();
	}
	_buttons[ButtonType::MainMenu]->RenderText();
	_buttons[ButtonType::Restart]->RenderText();
	_buttons[ButtonType::Resume]->RenderText();
	_buttons[ButtonType::Quit]->RenderText();
}
