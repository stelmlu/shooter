#include "Settings.hpp"
#include "Game.hpp"

#include "Player.hpp"

struct Pos { int x, y; };

int main() {
    Game::Run("Shooter", SCREEN_WIDTH, SCREEN_HEIGHT, []() {
        AddToGame(Player());
    });
}
