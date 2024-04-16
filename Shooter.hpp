#pragma once
#include "Component/RedTag.hpp"
#include "Component/BlueTag.hpp"
#include "Component/GreenTag.hpp"


using PlayerTag = RedTag;
using PlayerBulletTag = BlueTag;
using EnemyTag = GreenTag;

// Screen dimension constants
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

// Update rate
constexpr float SECOND_PER_FRAME = 0.01;

// Player
constexpr float PLAYER_MAX_SPEED = 600.0f;
constexpr float PLAYER_BULLET_SPEED = 800.0f;
constexpr float PLAYER_FIRE_COOLDOWN_TIME = 0.15f;
constexpr float PLAYER_ACCELERATION = 3000.0f;
constexpr float PLAYER_DEACCELERATION = 3000.0f;

// Enemy
constexpr float ENEMY_SPAWN_TIMEOUT_MIN = 1.2f;
constexpr float ENEMY_SPAWN_TIMEOUT_MAX = 2.0f;
constexpr float ENEMY_MIN_SPEED = 200.0f;
constexpr float ENEMY_MAX_SPEED = 400.0f;
