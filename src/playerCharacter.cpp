#include "playerCharacter.h"

#include "dataStructuresAndMethods.h"
#include "debugDrawer.h"
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
	_circleCollider = std::make_shared<Circle>();
	_circleCollider->Init(_position, 16.f);
	_oldPosition = _position;

	_currentHealth = _maxHealth;
	
	_healthTextSprite = std::make_shared<TextSprite>();
	_healthTextSprite->SetPosition(Vector2<float>(windowWidth * 0.05f, windowHeight * 0.9f));
}

PlayerCharacter::~PlayerCharacter() {}

void PlayerCharacter::Init() {
	_healthTextSprite->Init("res/roboto.ttf", 24, std::to_string(_currentHealth).c_str(), { 255, 255, 255, 255 });
	_attackTimer = timerManager->CreateTimer(_attackCooldown);
	_regenerationTimer = timerManager->CreateTimer(_regenerationTime);
}

void PlayerCharacter::Update() {
	UpdateHealthRegen();
	UpdateInput();
	UpdateMovement();
	UpdateTarget();
}

void PlayerCharacter::Render() {
	_sprite->RenderWithOrientation(_position, _orientation);
}

void PlayerCharacter::RenderText() {
	_healthTextSprite->Render();
}

const std::shared_ptr<Collider> PlayerCharacter::GetCollider() const {
	return _circleCollider;
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

void PlayerCharacter::FireProjectile() {
	_multiShotAngle = -PI * 0.0625f;
	for (unsigned int i = 0; i < _multiShotAmount; i++) {
		_multiShotDirection = RotateDirection(_multiShotAngle, _direction);
		projectileManager->SpawnProjectile(ProjectileType::PlayerProjectile, projectileManager->GetPlayerProjectileSprite(), 
			_orientation + _multiShotAngle, _attackDamage, _projectileSpeed, _multiShotDirection, _position);
		_multiShotAngle += PI * 0.0625f;
	}
}

void PlayerCharacter::Respawn() {
	_position = Vector2<float>(windowWidth * 0.5f, windowHeight * 0.5f);
	_orientation = 0.f;
	_attackTimer->ResetTimer();

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
		if (_attackTimer->GetTimerFinished()) {
			FireProjectile();
			_attackTimer->ResetTimer();
		}
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
	_circleCollider->SetPosition(_position);
}

void PlayerCharacter::UpdateTarget() {
	_direction = (GetCursorPosition() - _position).normalized();
	_orientation = VectorAsOrientation(_direction);
}

const int PlayerCharacter::GetCurrentHealth() const {
	return _currentHealth;
}