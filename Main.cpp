#include "Shooter.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "SpawnEnemy.hpp"

int main() {
    auto setting = Setting()
        .SetTitle("Shooter")
        .SetScreenWidth(SCREEN_WIDTH)
        .SetScreenHeight(SCREEN_HEIGHT)
        .SetSecondPerFrame(SECOND_PER_FRAME);

    Game::Run(setting, []() {
        AddToGame( Player() );
        AddToGame( SpawnEnemy() );
    });
}
