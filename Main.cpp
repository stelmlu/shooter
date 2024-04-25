#include "Shooter.hpp"
#include "GameEngine/GameEngine.hpp"
#include "GameEngine/Game.hpp"
#include "Menu.hpp"

int main() {
    auto setting = Setting()
        .SetTitle("Shooter")
        .SetLogicalSize(SCREEN_WIDTH, SCREEN_HEIGHT)
        .SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT)
        .SetSecondPerFrame(SECOND_PER_FRAME);

    Game::Run(setting, []() {
        // Preload the images
        Game::LoadTexture("gfx/title.png");
        Game::LoadTexture("gfx/enemy.png");
        Game::LoadTexture("gfx/enemybullet.png");
        Game::LoadTexture("gfx/explosion.png");
        Game::LoadTexture("gfx/num0.png");
        Game::LoadTexture("gfx/num1.png");
        Game::LoadTexture("gfx/num2.png");
        Game::LoadTexture("gfx/num3.png");
        Game::LoadTexture("gfx/num4.png");
        Game::LoadTexture("gfx/num5.png");
        Game::LoadTexture("gfx/num6.png");
        Game::LoadTexture("gfx/num7.png");
        Game::LoadTexture("gfx/num8.png");
        Game::LoadTexture("gfx/num9.png");
        Game::LoadTexture("gfx/player.png");
        Game::LoadTexture("gfx/playerbullet.png");
        Game::LoadTexture("gfx/points.png");
        Game::LoadTexture("gfx/star1.png");
        Game::LoadTexture("gfx/star2.png");
        Game::LoadTexture("gfx/star3.png");
        Game::LoadTexture("gfx/star4.png");
        Game::LoadTexture("gfx/star5.png");
        Game::LoadTexture("gfx/textscore.png");
        Game::LoadTexture("gfx/textinfo.png");
        Game::LoadTexture("gfx/textgameover.png");

        // Show the menu
        AddToGame( Menu() );
    });
}
