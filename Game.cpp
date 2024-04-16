#include <SDL.h>
#include <SDL_image.h>
#include "Game.hpp"
#include "GameObject.hpp"

SDL_Window *Game::m_window = nullptr;
SDL_Renderer *Game::m_renderer = nullptr;
std::unordered_map<std::string, SDL_Texture *> Game::m_textureCache = {};
std::random_device Game::m_randomDevice;
std::mt19937 Game::m_radomGenerator(Game::m_randomDevice());
std::map<std::pair<float, float>, std::uniform_real_distribution<float>> Game::m_randomDistributions;


void Game::onScriptComponentConstructed(entt::registry &reg, entt::entity entity)
{
    auto gameObject = GameObject(entity);
    reg.get<ScriptComponent>(entity).OnConstructed(gameObject);
}

void Game::onScriptComponentDestroyed(entt::registry &reg, entt::entity entity)
{
    auto gameObject = GameObject(entity);
    reg.get<ScriptComponent>(entity).OnDestroyed(gameObject);
}

void Game::invokeCallOnEvent(entt::registry &reg, const SDL_Event &event)
{
    auto view = reg.view<ScriptComponent>();
    view.each([&reg, &event](entt::entity entity, auto &script) {
        auto gameObject = GameObject(entity);
        script.OnEvent(gameObject, event);
    });
}

void Game::invokeCallOnUpdate(entt::registry &reg)
{
    auto view = reg.view<ScriptComponent>();
    view.each([&reg](entt::entity entity, auto &script) {
        auto gameObject = GameObject(entity);
        script.OnUpdate(gameObject, SECOND_PER_UPDATE);
    });
}

void Game::invokeMovement(entt::registry &reg)
{
    auto view = reg.view<PositionComponent, const VelocityComponent>();
    view.each([](auto &pos, const auto &vel) {
        pos.x += vel.dx * SECOND_PER_UPDATE;
        pos.y += vel.dy * SECOND_PER_UPDATE;
    });
}

void Game::invokeDrawTexture(entt::registry &reg, float interpolation)
{
    auto view = reg.view<const PositionComponent, const TextureComponent>();
    view.each([&reg, interpolation](entt::entity entity, const auto &pos, const auto &tex) {
        if(reg.any_of<VelocityComponent>(entity)) {
            auto& vel = reg.get<VelocityComponent>(entity);
            SDL_Rect dst = {
                static_cast<int>(pos.x + vel.dx * SECOND_PER_UPDATE * interpolation + 0.5f),
                static_cast<int>(pos.y + vel.dy * SECOND_PER_UPDATE * interpolation + 0.5f),
                tex.width, tex.height
            };
            SDL_RenderCopy(m_renderer, tex.texture, NULL, &dst);
        }
        else {
            SDL_Rect dst = {
                static_cast<int>(pos.x + 0.5f), static_cast<int>(pos.y + 0.5f),
                tex.width, tex.height
            };
            SDL_RenderCopy(m_renderer, tex.texture, NULL, &dst);
        }
    });
}

TextureComponent Game::LoadTexture(const std::string& path) {
    // Check if the texture is already loaded
    auto it = m_textureCache.find(path);
    if (it != m_textureCache.end()) {
        SDL_Texture* existingTexture = it->second;
        int w, h;
        SDL_QueryTexture(existingTexture, NULL, NULL, &w, &h);
        return TextureComponent{existingTexture, w, h};
    }

    // Create texture from surface pixels
    SDL_Texture* texture = IMG_LoadTexture(m_renderer, path.c_str());
    if (!texture) {
        SDL_Log("Unable to create texture from %s: %s", path.c_str(), SDL_GetError());
        return {};
    }

    // Add the texture to our texture cache
    m_textureCache[path] = texture;

    // Get texture width and height
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);

    return TextureComponent{texture, w, h};
}

void Game::Run(const std::string& title, int width, int height, const std::function<void(void)>& onSetup) {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            return;
        }

        // Create SDL window
        m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
        if (!m_window) {
            std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return;
        }

        // Create SDL renderer with hardware acceleration
        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!m_renderer) {
            std::cerr << "Failed to create SDL renderer: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(m_window);
            SDL_Quit();
            return;
        }

        // For ScriptComponent call the Constructor and Destructor class on construct and destruct.
        entt::registry& reg = Registry::Get();
        reg.on_construct<ScriptComponent>().connect<&onScriptComponentConstructed>();
        reg.on_destroy<ScriptComponent>().connect<&onScriptComponentDestroyed>();

        onSetup();

        // Enter loop
        bool quit = false;
        SDL_Event event;
        float ms_per_update = SECOND_PER_UPDATE * 1000.0f;
        float previous = static_cast<float>(SDL_GetTicks64());
        float lag = 0.0f;

        while (!quit) {
            float current = static_cast<float>(SDL_GetTicks64());
            float elapsed = current - previous;
            previous = current;
            lag += elapsed;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                }
                else {
                    invokeCallOnEvent(reg, event);
                }
            }
            while(lag >= ms_per_update) {
                lag -= ms_per_update;
                invokeCallOnUpdate(reg);
                invokeMovement(reg);
            }

            SDL_RenderClear(m_renderer);
            invokeDrawTexture(reg, lag / ms_per_update);
            SDL_RenderPresent(m_renderer);
            SDL_Delay(16); // Cap the frame rate to approximately 60 fps
        }
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }