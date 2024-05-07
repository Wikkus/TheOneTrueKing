#pragma once
#include "collision.h"
#include "SDL2/SDL_pixels.h"

#include <array>
#include <memory>
#include <stack>
#include <unordered_map>
#include "vector"

class TextSprite;


enum class ButtonType {
	MainMenu,
	Play,
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

private:
	const char* _mainMenuText = "Main Menu";
	const char* _playText = "Play";
	const char* _quitText = "Quit";
	const char* _restartText = "Restart";
	const char* _resumeText = "Resume";
	std::vector<std::shared_ptr<State>> _states;

};

class GameOverState : public State {
public:
	GameOverState();
	~GameOverState() {}

	void SetButtonPositions() override;
	void Update() override;
	void Render() override;
	void RenderText() override;
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
class TacticalGameState : public InGameState {
public:
	TacticalGameState();
	~TacticalGameState() {}

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
