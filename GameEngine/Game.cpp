#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "Game.hpp"

SDL_Window *Game::m_window = nullptr;
SDL_Renderer *Game::m_renderer = nullptr;
std::unordered_map<std::string, SDL_Texture *> Game::m_textureCache = {};
std::unordered_map<std::string, entt::entity> Game::m_searchableMap = {};
float Game::m_secondPerFrame = 0.01;

std::random_device Game::m_randomDevice;
std::mt19937 Game::m_radomGenerator(Game::m_randomDevice());
std::map<std::pair<float, float>, std::uniform_real_distribution<float>> Game::m_randomDistributions;

std::unordered_map<std::string, SDL_Rect> Game::m_textureRectCache;
TextureAtlas* Game::m_atlas;


void Game::onScriptComponentConstructed(entt::registry &reg, entt::entity entity)
{
    auto gameObject = GameObject(reg, entity);
    reg.get<ScriptComponent>(entity).OnConstructed(gameObject);
}

void Game::onScriptComponentDestroyed(entt::registry &reg, entt::entity entity)
{ 
    auto gameObject = GameObject(reg, entity);
    reg.get<ScriptComponent>(entity).OnDestroyed(gameObject);
}

void Game::onSearchableComponentConstructed(entt::registry& reg, entt::entity entity)
{
    auto& searchableComponent = reg.get<SearchableComponent>(entity);
    m_searchableMap.emplace(std::make_pair(searchableComponent.name, entity));
}

void Game::onSearchableComponentDestroyed(entt::registry& reg, entt::entity entity)
{
    auto& searchableComponent = reg.get<SearchableComponent>(entity);
    auto findResult = m_searchableMap.find(searchableComponent.name);
    if(findResult != m_searchableMap.end()) {
        m_searchableMap.erase(findResult);
    }
}

void Game::invokeCallOnEvent(entt::registry &reg, const SDL_Event &event)
{
    auto view = reg.view<ScriptComponent>();
    view.each([&reg, &event](entt::entity entity, auto &script) {
        auto gameObject = GameObject(reg, entity);
        script.OnEvent(gameObject, event);
    });
}

void Game::invokeCallOnUpdate(entt::registry &reg)
{
    auto view = reg.view<ScriptComponent>();
    view.each([&reg](entt::entity entity, auto &script) {
        auto gameObject = GameObject(reg, entity);
        script.OnUpdate(gameObject, m_secondPerFrame);
    });
}

void Game::invokeMovement(entt::registry &reg)
{
    auto view = reg.view<PositionComponent, const VelocityComponent>();
    view.each([](auto &pos, const auto &vel) {
        pos.x += vel.dx * m_secondPerFrame;
        pos.y += vel.dy * m_secondPerFrame;
    });
}

void Game::invokeDrawTexture(entt::registry &reg, float interpolation)
{
    auto view = reg.view<const PositionComponent, const TextureComponent>();
    view.each([&reg, interpolation](entt::entity entity, const auto &pos, const auto &tex) {
        if(reg.any_of<VelocityComponent>(entity)) {
            auto& vel = reg.get<VelocityComponent>(entity);
            SDL_Rect dst = {
                static_cast<int>(pos.x + vel.dx * m_secondPerFrame * interpolation + 0.5f),
                static_cast<int>(pos.y + vel.dy * m_secondPerFrame * interpolation + 0.5f),
                tex.rect.w, tex.rect.h
            };
            SDL_RenderCopy(m_renderer, m_atlas->GetAtlasTexture(), &tex.rect, &dst);
        }
        else {
            SDL_Rect dst = {
                static_cast<int>(pos.x + 0.5f), static_cast<int>(pos.y + 0.5f),
                tex.rect.w, tex.rect.h
            };
            SDL_RenderCopy(m_renderer, m_atlas->GetAtlasTexture(), &tex.rect, &dst);
        }
    });
}

void Game::Run(const Setting& setting, const std::function<void(void)>& onSetup)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return;
    }

    // Create SDL window
    m_window = SDL_CreateWindow(setting.GetTitle().c_str(),
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        setting.GetScreenWidth(), setting.GetScreenHeight(),
        SDL_WINDOW_SHOWN);
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

    // Create the texture atlas
    m_atlas = new TextureAtlas(m_renderer, 1024, 1024);

    // For ScriptComponent and SearchableComponent call the Constructor and Destructor class on construct and destruct.
    entt::registry& reg = Registry::Get();
    reg.on_construct<ScriptComponent>().connect<&onScriptComponentConstructed>();
    reg.on_destroy<ScriptComponent>().connect<&onScriptComponentDestroyed>();
    reg.on_construct<SearchableComponent>().connect<&onSearchableComponentConstructed>();
    reg.on_destroy<SearchableComponent>().connect<&onSearchableComponentDestroyed>();
    
    // Let the user set up it things
    m_secondPerFrame = setting.GetSecondPerFrame();
    onSetup();

    // Enter the main loop
    bool quit = false;
    SDL_Event event;
    float ms_per_update = setting.GetSecondPerFrame() * 1000.0f;
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
            invokeOnCollision<RedTag>(reg);
            invokeOnCollision<BlueTag>(reg);
            invokeOnCollision<GreenTag>(reg);
            invokeOnCollision<YellowTag>(reg);
            invokeMovement(reg);
        }

        SDL_RenderClear(m_renderer);
        invokeDrawTexture(reg, lag / ms_per_update);

        SDL_RenderPresent(m_renderer);
    }

    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}
