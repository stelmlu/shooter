#include "GameEngine/GameEngine.hpp"
#include "Player.hpp"
#include "PlayerBullet.hpp"

void Player::PlayerScript::OnEvent(GameObject& self, const SDL_Event& event) {
    switch(event.type) {
    case SDL_KEYDOWN:
        switch(event.key.keysym.sym) {
        case SDLK_LEFT:
            self.GetComponent<KeyStateComponent>().leftKeyDown = true;
            break;
        case SDLK_RIGHT:
            self.GetComponent<KeyStateComponent>().rightKeyDown = true;
            break;
        case SDLK_UP:
            self.GetComponent<KeyStateComponent>().upKeyDown = true;
            break;
        case SDLK_DOWN:
            self.GetComponent<KeyStateComponent>().downKeyDown = true;
            break;
        case SDLK_SPACE:
            self.GetComponent<KeyStateComponent>().fireKeyDown = true;
            break;
    }
    break;
    case SDL_KEYUP:
        switch(event.key.keysym.sym) {
        case SDLK_LEFT:
            self.GetComponent<KeyStateComponent>().leftKeyDown = false;
            break;
        case SDLK_RIGHT:
            self.GetComponent<KeyStateComponent>().rightKeyDown = false;
            break;
        case SDLK_UP:
            self.GetComponent<KeyStateComponent>().upKeyDown = false;
            break;
        case SDLK_DOWN:
            self.GetComponent<KeyStateComponent>().downKeyDown = false;
            break;
        case SDLK_SPACE:
            self.GetComponent<KeyStateComponent>().fireKeyDown = false;
            break;
        }
    }
}

void Player::PlayerScript::OnUpdate(GameObject& self, float dt) {
    const auto& keyState = self.GetComponent<KeyStateComponent>();
    auto& velocity = self.GetComponent<VelocityComponent>();
    auto& position = self.GetComponent<PositionComponent>();
    const auto& texture = self.GetComponent<TextureComponent>();

    // Apply acceleration if key is pressed.
    if(keyState.leftKeyDown) {
        velocity.dx -= PLAYER_ACCELERATION * dt;
        velocity.dx = std::max(velocity.dx, -PLAYER_MAX_SPEED);
    }
    if(keyState.rightKeyDown) {
        velocity.dx += PLAYER_ACCELERATION * dt;
        velocity.dx = std::min(velocity.dx, PLAYER_MAX_SPEED);
    }
    if(keyState.upKeyDown) {
        velocity.dy -= PLAYER_ACCELERATION * dt;
        velocity.dy = std::max(velocity.dy, -PLAYER_MAX_SPEED);
    }
    if(keyState.downKeyDown) {
        velocity.dy += PLAYER_ACCELERATION * dt;
        velocity.dy = std::min(velocity.dy, PLAYER_MAX_SPEED);
    }
    if(!keyState.leftKeyDown && !keyState.rightKeyDown) {
        if(velocity.dx < 0.0f) {
            velocity.dx += PLAYER_DEACCELERATION * dt;
            velocity.dx = std::min(velocity.dx, 0.0f);
        }
        else if(velocity.dx > 0.0f) {
            velocity.dx -= PLAYER_DEACCELERATION * dt;
            velocity.dx = std::max(velocity.dx, 0.0f);
        }
    }
    if(!keyState.upKeyDown && !keyState.downKeyDown) {
        if(velocity.dy < 0.0f) {
            velocity.dy += PLAYER_DEACCELERATION * dt;
            velocity.dy = std::min(velocity.dy, 0.0f);
        }
        else if(velocity.dy > 0.0f) {
            velocity.dy -= PLAYER_DEACCELERATION * dt;
            velocity.dy = std::max(velocity.dy, 0.0f);
        }
    }

    // Prevent the player from leavning the screen.
    float nextX = position.x + velocity.dx * dt;
    float nextY = position.y + velocity.dy * dt;

    if (nextX < 0 || (nextX + texture.width) > SCREEN_WIDTH) {
        velocity.dx = 0;
    }

    if (nextY < 0 || (nextY + texture.height) > SCREEN_HEIGHT) {
        velocity.dy = 0;
    }

    // Handle user shooting
    auto& fireCooldown = self.GetComponent<FireCooldown>();

    if(keyState.fireKeyDown && fireCooldown.timeout <= 0.0f) {
        AddToGame(PlayerBullet(position.x + texture.width, position.y + texture.height / 2.0f));
        fireCooldown.timeout = PLAYER_FIRE_COOLDOWN_TIME;
    }

    fireCooldown.timeout -= dt;
}
