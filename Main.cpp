#include "Shooter.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "SpawnEnemy.hpp"

int main() {

    Game::Run("Shooter", SCREEN_WIDTH, SCREEN_HEIGHT, []() {
        AddToGame( Player() );
        AddToGame( SpawnEnemy() );
    });
}
