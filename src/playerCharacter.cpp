#include "playerCharacter.h"

#include "dataStructuresAndMethods.h"
#include "debugDrawer.h"
#include "enemyManager.h"
#include "gameEngine.h"
#include "objectBase.h"
#include "projectile.h"
#include "projectileManager.h"
#include "stateStack.h"
#include "timerManager.h"

#include <string>

PlayerCharacter::PlayerCharacter(float characterOrientation, unsigned int objectID, Vector2<float> characterPosition) : 
	ObjectBase(objectID, ObjectType::Player) {
	_sprite = std::make_shared<Sprite>();
	_sprite->Load(_kingSprite);
	_orientation = characterOrientation;
	_position = characterPosition;
	_collider = std::make_shared<Circle>();
	std::static_pointer_cast<Circle>(_collider)->Init(_position, _sprite->h * 0.45f);

	_oldPosition = _position;

	_maxHealth = 1000.f;
	_currentHealth = _maxHealth;
	_healthTextSprite = std::make_shared<TextSprite>();
	_healthTextSprite->SetPosition(Vector2<float>(windowWidth * 0.05f, windowHeight * 0.9f));

	_dummyTarget = std::make_shared<ObjectBase>(INT_MAX, ObjectType::Count);
}

PlayerCharacter::~PlayerCharacter() {}

void PlayerCharacter::Init() {
	_healthTextSprite->Init("res/roboto.ttf", 24, std::to_string(_currentHealth).c_str(), { 255, 255, 255, 255 });
	_regenerationTimer = timerManager->CreateTimer(_regenerationCooldown);

	_weaponComponent = enemyManager->AccessWeapon(WeaponType::SuperStaff);
	_weaponComponent->Init(shared_from_this());
}

void PlayerCharacter::Update() {
	UpdateTarget();
	UpdateHealthRegen();
	UpdateInput();
	UpdateMovement();
}

void PlayerCharacter::Render() {
	_sprite->RenderWithOrientation(_position, _orientation);
}

void PlayerCharacter::RenderText() {
	_healthTextSprite->Render();
}

const std::shared_ptr<Collider> PlayerCharacter::GetCollider() const {
	return _collider;
}

void PlayerCharacter::TakeDamage(unsigned int damageAmount) {
	_currentHealth -= damageAmount;	
	if (_currentHealth <= 0) {
		_currentHealth = 0;
		ExecuteDeath();
	}
	_healthTextSprite->ChangeText(std::to_string(_currentHealth).c_str(), { 255, 255, 255, 255 });
}

void PlayerCharacter::ExecuteDeath() {
	gameStateHandler->ReplaceCurrentState(std::make_shared<GameOverState>());
}

void PlayerCharacter::Respawn() {
	_position = Vector2<float>(windowWidth * 0.5f, windowHeight * 0.5f);
	_orientation = 0.f;

	_currentHealth = _maxHealth;
	_healthTextSprite->ChangeText(std::to_string(_currentHealth).c_str(), { 255, 255, 255, 255 });
}

void PlayerCharacter::UpdateHealthRegen() {
	if (_currentHealth < _maxHealth) {
		if (_regenerationTimer->GetTimerFinished()) {
			_currentHealth += 1;
			if (_currentHealth > _maxHealth) {
				_currentHealth = _maxHealth;
			}
			_healthTextSprite->ChangeText(std::to_string(_currentHealth).c_str(), { 255, 255, 255, 255 });
			_regenerationTimer->ResetTimer();
		}
	}
}

void PlayerCharacter::UpdateInput() {
	if (GetMouseButton(SDL_BUTTON_LEFT)) {
		_weaponComponent->Attack();
	}
	if (GetKeyPressed(SDL_SCANCODE_ESCAPE)) {
		gameStateHandler->AddState(std::make_shared<PauseState>());
	}
}

void PlayerCharacter::UpdateMovement() { 
	_oldPosition = _position;
	_velocity = { 0.f, 0.f };

	if (GetKey(SDL_SCANCODE_W)) {
		_velocity.y -= _movementSpeed;

	} else if (GetKey(SDL_SCANCODE_S)) {
		_velocity.y += _movementSpeed;
	}	
	
	if (GetKey(SDL_SCANCODE_A)) {
		_velocity.x -= _movementSpeed;

	} else if (GetKey(SDL_SCANCODE_D)) {
		_velocity.x += _movementSpeed;
	}
	_position += _velocity * deltaTime;

	if (OutOfBorderX(_position.x, 0.f)) {
		_position.x = _oldPosition.x;
	}
	if (OutOfBorderY(_position.y, 0.f)) {
		_position.y = _oldPosition.y;
	}
	_collider->SetPosition(_position);
}

void PlayerCharacter::UpdateTarget() {
	_targetPosition = GetCursorPosition();
	_dummyTarget->SetPosition(_targetPosition);
	_currentTarget = _dummyTarget;

	_direction = (_targetPosition - _position).normalized();
	_orientation = VectorAsOrientation(_direction);
}

const int PlayerCharacter::GetCurrentHealth() const {
	return _currentHealth;
}