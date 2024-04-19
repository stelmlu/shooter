#include "Shooter.hpp"
#include "GameEngine/GameEngine.hpp"
#include "Player.hpp"
#include "SpawnEnemy.hpp"

int main() {
    auto setting = Setting()
        .SetTitle("Shooter")
        .SetLogicalSize(SCREEN_WIDTH, SCREEN_HEIGHT)
        .SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT)
        .SetSecondPerFrame(SECOND_PER_FRAME);

    Game::Run(setting, []() {

        auto a = Game::LoadTexture("gfx/player.png");
        auto b = Game::LoadTexture("gfx/enemy.png");

        AddToGame( Player() );
        AddToGame( SpawnEnemy() );
    });
}
