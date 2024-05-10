#pragma once
#include "collision.h"
#include "SDL2/SDL_pixels.h"

#include <array>
#include <memory>
#include <stack>
#include <unordered_map>
#include "vector"

class TextSprite;

enum class GameMode {
	Formation,
	Survival,
	Count
};

enum class ButtonType {
	MainMenu,
	FormationGame,
	SurvivalGame,
	Quit,
	Restart,
	Resume,
	Count
};

class Button {
public:
	Button(const char* buttonText, int height, int width, Vector2<float> position);
	~Button() {}

	bool ClickedOn();
	void Render();
	void RenderText();
	void SetTargetPosition(Vector2<float> position);

private:
	SDL_Color _buttonColor = { 120,81,169, 255 };
	SDL_Color _textColor = { 212, 175, 55, 255 };

	AABB _boxCollider;
	std::shared_ptr<TextSprite> _text;
	Vector2<float> _position;
};

class State {
public:
	State() {}
	~State() {}

	virtual void SetButtonPositions() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void RenderText() = 0;
};

class GameStateHandler {
public:
	GameStateHandler();
	~GameStateHandler();

	void AddState(std::shared_ptr<State> state);
	void BackToFirstState();
	void ReplaceCurrentState(std::shared_ptr<State> state);
	void RemoveCurrentState();

	void UpdateState();
	void RenderState();
	void RenderStateText();

	void SetGameMode(GameMode gameMode);

	const GameMode GetGameMode() const;

private:
	const char* _mainMenuText = "Main Menu";
	const char* _formationText = "Formation";
	const char* _survivalText = "Survival";
	const char* _quitText = "Quit";
	const char* _restartText = "Restart";
	const char* _resumeText = "Resume";
	std::vector<std::shared_ptr<State>> _states;
	GameMode _currentGameMode = GameMode::Count;

};

class GameOverState : public State {
public:
	GameOverState();
	~GameOverState() {}

	void SetButtonPositions() override;
	void Update() override;
	void Render() override;
	void RenderText() override;

private:
	std::shared_ptr<TextSprite> _waveNumberText;

};

class InGameState : public State {
public:
	InGameState();
	~InGameState() {}

	void SetButtonPositions() override;
	void Update() override;
	void Render() override;
	void RenderText() override;
};
class SurvivalGameState : public InGameState {
public:
	SurvivalGameState();
	~SurvivalGameState() {}

	void SetButtonPositions() override;
	void Update() override;
	void Render() override;
	void RenderText() override;

};
class FormationGameState : public InGameState {
public:
	FormationGameState();
	~FormationGameState() {}

	void SetButtonPositions() override;
	void Update() override;
	void Render() override;
	void RenderText() override;

};
class MenuState : public State {
public:
	MenuState();
	~MenuState() {}

	void SetButtonPositions() override;
	void Update() override;
	void Render() override;
	void RenderText() override;

private:
	const char* _gameTitle = "TheOneTrueKing";
	SDL_Color _textColor = { 212, 175, 55, 255 };
	std::shared_ptr<TextSprite> _titleText;
	Vector2<float> _titleTextPosition;
};
class PauseState : public State {
public:
	PauseState();
	~PauseState() {}

	void SetButtonPositions() override;
	void Update() override;
	void Render() override;
	void RenderText() override;
};
