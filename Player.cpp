#include "Player.hpp"
#include "PlayerBullet.hpp"

void Player::PlayerScript::OnEvent(entt:: registry& reg, entt::entity self, const SDL_Event& event) {
    switch(event.type) {
    case SDL_KEYDOWN:
        switch(event.key.keysym.sym) {
        case SDLK_LEFT:
            reg.get<KeyStateComponent>(self).leftKeyDown = true;
            break;
        case SDLK_RIGHT:
            reg.get<KeyStateComponent>(self).rightKeyDown = true;
            break;
        case SDLK_UP:
            reg.get<KeyStateComponent>(self).upKeyDown = true;
            break;
        case SDLK_DOWN:
            reg.get<KeyStateComponent>(self).downKeyDown = true;
            break;
        case SDLK_SPACE:
            reg.get<KeyStateComponent>(self).fireKeyDown = true;
            break;
    }
    break;
    case SDL_KEYUP:
        switch(event.key.keysym.sym) {
        case SDLK_LEFT:
            reg.get<KeyStateComponent>(self).leftKeyDown = false;
            break;
        case SDLK_RIGHT:
            reg.get<KeyStateComponent>(self).rightKeyDown = false;
            break;
        case SDLK_UP:
            reg.get<KeyStateComponent>(self).upKeyDown = false;
            break;
        case SDLK_DOWN:
            reg.get<KeyStateComponent>(self).downKeyDown = false;
            break;
        case SDLK_SPACE:
            reg.get<KeyStateComponent>(self).fireKeyDown = false;
            break;
        }
    }
}

void Player::PlayerScript::OnUpdate(entt:: registry& reg, entt::entity self, float dt) {
    const auto& keyState = reg.get<KeyStateComponent>(self);
    auto& velocity = reg.get<VelocityComponent>(self);
    auto& position = reg.get<PositionComponent>(self);
    const auto& texture = reg.get<TextureComponent>(self);

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
    auto& fireCooldown = reg.get<FireCooldown>(self);

    if(keyState.fireKeyDown && fireCooldown.timeout <= 0.0f) {
        AddToGame(PlayerBullet(position.x + texture.width, position.y + texture.height / 2.0f));

        fireCooldown.timeout = PLAYER_FIRE_COOLDOWN_TIME;
    }

    fireCooldown.timeout -= dt;

}
