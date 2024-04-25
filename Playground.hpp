#pragma once
#include "Shooter.hpp"
#include "GameEngine/GameEngine.hpp"
#include "Player.hpp"
#include "SpawnEnemy.hpp"
#include "SpawnStar.hpp"
#include "ScoreLabel.hpp"
#include "Menu.hpp"

class Playground {
public:
    void operator()() {
        AddToGame( Player() );
        AddToGame( SpawnEnemy() );
        AddToGame( SpawnStar() );
        AddToGame( ScoreLabel() );
    }
};
