#include "playerCharacter.h"

#include "debugDrawer.h"
#include "enemyManager.h"
#include "gameEngine.h"
#include "objectBase.h"
#include "projectile.h"
#include "projectileManager.h"
#include "stateStack.h"
#include "timerHandler.h"
#include "weaponManager.h"

#include <string>

PlayerCharacter::PlayerCharacter(const float& characterOrientation, const Vector2<float>& characterPosition) :  ObjectBase(ObjectType::Player) {
	_sprite = std::make_shared<Sprite>();
	_sprite->Load(_kingSprite);
	_orientation = characterOrientation;
	_position = characterPosition;
	_collider = std::make_shared<Circle>();
	std::static_pointer_cast<Circle>(_collider)->Init(_position, _sprite->GetHeight() * 0.45f);

	_oldPosition = _position;

	_maxHealth = 1000.f;
	_currentHealth = _maxHealth;
	_healthTextSprite = std::make_shared<TextSprite>();
	_healthTextSprite->Init(fontType, 24, std::to_string(_currentHealth).c_str(), { 255, 255, 255, 255 });
	_healthTextSprite->SetPosition({ windowWidth * 0.05f, windowHeight * 0.9f });

	_dummyTarget = std::make_shared<ObjectBase>(ObjectType::Count);
	_spawnPosition = { windowWidth * 0.5f, windowHeight * 0.5f };
}

PlayerCharacter::~PlayerCharacter() {}

void PlayerCharacter::Init() {
	_regenerationTimer = timerHandler->SpawnTimer(_regenerationCooldown, true, false);

	_weaponComponent = weaponManager->SpawnWeapon(WeaponType::SuperStaff);
	_weaponComponent->SetOwner(shared_from_this(), true);
	_weaponComponent->Init();
}

void PlayerCharacter::Update() {
	UpdateTarget();
	UpdateHealthRegen();
	UpdateInput();
	UpdateMovement();
}

void PlayerCharacter::Render() {
	_sprite->RenderWithOrientation(0, _position, _orientation);
	_weaponComponent->Render();
}

void PlayerCharacter::RenderText() {
	_healthTextSprite->Render();
}

const std::shared_ptr<Collider> PlayerCharacter::GetCollider() const {
	return _collider;
}

void PlayerCharacter::TakeDamage(const int& damageAmount) {
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
	_position = _spawnPosition;
	_orientation = 0.f;
	
	_currentHealth = _maxHealth;
	_healthTextSprite->ChangeText(std::to_string(_currentHealth).c_str(), { 255, 255, 255, 255 });
	_weaponComponent->ResetTimers();
}

void PlayerCharacter::UpdateHealthRegen() {
	if (_currentHealth < _maxHealth) {
		if (_regenerationTimer->GetIsFinished()) {
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

	if (universalFunctions->OutsideBorderX(_position.x, 0.f)) {
		_position.x = _oldPosition.x;
	}
	if (universalFunctions->OutsideBorderY(_position.y, 0.f)) {
		_position.y = _oldPosition.y;
	}
	_collider->SetPosition(_position);
}

void PlayerCharacter::UpdateTarget() {
	_targetPosition = universalFunctions->GetCursorPosition();
	_dummyTarget->SetPosition(_targetPosition);
	_currentTarget = _dummyTarget;

	_direction = (_targetPosition - _position).normalized();
	_orientation = universalFunctions->VectorAsOrientation(_direction);
}

const int PlayerCharacter::GetCurrentHealth() const {
	return _currentHealth;
}