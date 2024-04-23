#pragma once
#include "GameEngine/Game.hpp"

using PlayerBulletColitionLayerTag = ColitionLayer1Tag;
using EnemyColitionLayerTag = ColitionLayer2Tag;
using EnemyBulletColitionLayerTag = ColitionLayer3Tag;
using ScorePodLayerTag = ColitionLayer4Tag;

using StarBackgroundLayer = RenderLayer1Tag;
using SpaceShipRenderLayer = RenderLayer4Tag;
using BulletRenderLayer = RenderLayer5Tag;


// Screen dimension constants
constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720 ;

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
constexpr float ENEMY_MIN_SPEED = 150.0f;
constexpr float ENEMY_MAX_SPEED = 200.0f;
constexpr float ENEMY_BULLET_SPEED = 230.0f;
constexpr float ENEMY_BULLET_SPAWN_TIMEOUT_MIN = 1.5f;
constexpr float ENEMY_BULLET_SPAWN_TIMEOUT_MAX = 2.5f;
