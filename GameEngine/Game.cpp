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

static void invokeCallOnEvent(entt::registry &reg, const SDL_Event &event)
{
    auto view = reg.view<ScriptComponent>();
    view.each([&reg, &event](entt::entity entity, auto &script) {
        auto gameObject = GameObject(reg, entity);
        script.OnEvent(gameObject, event);
    });
}

static void invokeCallOnUpdate(entt::registry &reg, float secondPerFrame)
{
    auto view = reg.view<ScriptComponent>();
    view.each([&reg, secondPerFrame](entt::entity entity, auto &script) {
        auto gameObject = GameObject(reg, entity);
        script.OnUpdate(gameObject, secondPerFrame);
    });
}

static void invokeMovement(entt::registry &reg, float secondPerFrame)
{
    auto view = reg.view<PositionComponent, const VelocityComponent>();
    view.each([secondPerFrame](auto &pos, const auto &vel) {
        pos.x += vel.dx * secondPerFrame;
        pos.y += vel.dy * secondPerFrame;
    });
}

template<typename RenderLayerTag>
static void invokeDrawTexture(entt::registry &reg, SDL_Renderer* renderer, TextureAtlas* atlas, float secondPerFrame, float interpolation)
{
    auto view = reg.view<RenderLayerTag, const PositionComponent, const TextureComponent>();
    view.each([&reg, secondPerFrame, &renderer, &atlas, interpolation](entt::entity entity, const auto &pos, const auto &tex) {
        if(reg.any_of<AddBlenderComponent>(entity)) return;
        if(reg.any_of<VelocityComponent>(entity)) {
            auto& vel = reg.get<VelocityComponent>(entity);
            SDL_Rect dst = {
                static_cast<int>(pos.x + vel.dx * secondPerFrame * interpolation + 0.5f),
                static_cast<int>(pos.y + vel.dy * secondPerFrame * interpolation + 0.5f),
                tex.rect.w, tex.rect.h
            };
            SDL_RenderCopy(renderer, atlas->GetAtlasTexture(), &tex.rect, &dst);
        }
        else {
            SDL_Rect dst = {
                static_cast<int>(pos.x + 0.5f), static_cast<int>(pos.y + 0.5f),
                tex.rect.w, tex.rect.h
            };
            SDL_RenderCopy(renderer, atlas->GetAtlasTexture(), &tex.rect, &dst);
        }
    });
}

template<typename RenderLayerTag>
static void invokeDrawAddBlendTexture(entt::registry &reg, SDL_Renderer* renderer, TextureAtlas* atlas, float secondPerFrame, float interpolation)
{
    auto view = reg.view<RenderLayerTag, const PositionComponent, const TextureComponent, AddBlenderComponent>();
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
    SDL_SetTextureBlendMode(atlas->GetAtlasTexture(), SDL_BLENDMODE_ADD);
    
    view.each([&reg, secondPerFrame, &renderer, &atlas, interpolation](entt::entity entity, const auto &pos, const auto &tex, auto& addBlender) {
        SDL_SetTextureColorMod(atlas->GetAtlasTexture(), addBlender.r, addBlender.g, addBlender.b);
        SDL_SetTextureAlphaMod(atlas->GetAtlasTexture(), addBlender.a);

        if(reg.any_of<VelocityComponent>(entity)) {
            auto& vel = reg.get<VelocityComponent>(entity);
            SDL_Rect dst = {
                static_cast<int>(pos.x + vel.dx * secondPerFrame * interpolation + 0.5f),
                static_cast<int>(pos.y + vel.dy * secondPerFrame * interpolation + 0.5f),
                tex.rect.w, tex.rect.h
            };
            SDL_RenderCopy(renderer, atlas->GetAtlasTexture(), &tex.rect, &dst);
        }
        else {
            SDL_Rect dst = {
                static_cast<int>(pos.x + 0.5f), static_cast<int>(pos.y + 0.5f),
                tex.rect.w, tex.rect.h
            };
            SDL_RenderCopy(renderer, atlas->GetAtlasTexture(), &tex.rect, &dst);
        }
    });

    SDL_SetTextureColorMod(atlas->GetAtlasTexture(), 255, 255, 255);
    SDL_SetTextureAlphaMod(atlas->GetAtlasTexture(), 255);
    SDL_SetTextureBlendMode(atlas->GetAtlasTexture(), SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

template<typename ColitionLayerTag>
static void invokeOnColition(entt::registry& reg, SDL_Renderer* renderer, float dt) {
    auto view = reg.view<ColitionLayerTag, PositionComponent, TextureComponent, AABBComponent, VelocityComponent, ScriptComponent>();
    auto otherView = reg.view<ColitionLayerTag, PositionComponent, TextureComponent, AABBComponent, VelocityComponent>();
    for(entt::entity self : view) {
        const auto& pos = reg.get<PositionComponent>(self);
        const auto& tex = reg.get<TextureComponent>(self);
        const auto& vel = reg.get<VelocityComponent>(self);
        const auto& aabb = reg.get<AABBComponent>(self);

        float ax1 = pos.x + vel.dx*dt + aabb.left;
        float ay1 = pos.y + vel.dy*dt + aabb.top;
        float ax2 = pos.x + tex.rect.w + vel.dx*dt + aabb.right;
        float ay2 = pos.y + tex.rect.h + vel.dy*dt + aabb.bottom;

        auto& script = reg.get<ScriptComponent>(self);

        if(aabb.draw == true) {
            auto rect = SDL_FRect{ ax1, ay1, ax2 - ax1, ay2  - ay1 };
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRectF(renderer, &rect);
        }

        for(entt::entity other : otherView) {
            if(self == other) continue;
            const auto& otherPos = reg.get<PositionComponent>(other);
            const auto& otherTex = reg.get<TextureComponent>(other);
            const auto& otherVel = reg.get<VelocityComponent>(other);
            const auto& otherAabb = reg.get<AABBComponent>(other);

            float bx1 = otherPos.x + otherVel.dx*dt + otherAabb.left;
            float by1 = otherPos.y + otherVel.dy*dt + otherAabb.top;
            float bx2 = otherPos.x + otherTex.rect.w + otherVel.dx*dt + otherAabb.right;
            float by2 = otherPos.y + otherTex.rect.h + otherVel.dy*dt + otherAabb.bottom;

            if((ax1 <= bx2 && ax2 >= bx1) && (ay1 <= by2 && ay2 >= by1)) {
                GameObject go = GameObject(reg, self);
                GameObject otherGo = GameObject(reg, other);
                script.OnCollision(go, otherGo);                
            }
        }
    }
}

static void setupRenderer(SDL_Renderer* renderer,  const Setting& setting) {
    const auto logicalSize = setting.GetLogicalSize();
    const auto windowSize = setting.GetWindowSize();
    // Calculate the aspect ratio of the logical size
    float aspectRatio = static_cast<float>(logicalSize.width) / static_cast<float>(logicalSize.height);

    // Calculate the maximum possible size maintaining the aspect ratio
    int renderWidth = windowSize.width;
    int renderHeight = static_cast<int>(renderWidth / aspectRatio);

    if (renderHeight > windowSize.height) {
        renderHeight = windowSize.height;
        renderWidth = static_cast<int>(renderHeight * aspectRatio);
    }

    // Set the logical size and the drawing area (viewport)
    SDL_RenderSetLogicalSize(renderer, logicalSize.width, logicalSize.height);
}

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

void Game::Run(Setting& setting, const std::function<void(void)>& onSetup)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return;
    }

    const auto windowSize = setting.GetWindowSize();

    // Set the scaling quality to the highest
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    // Create SDL window
    m_window = SDL_CreateWindow(setting.GetTitle().c_str(),
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        windowSize.width, windowSize.height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
            else if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                setting.SetWindowSize(event.window.data1, event.window.data2);
                setupRenderer(m_renderer, setting);
            }
            else {
                invokeCallOnEvent(reg, event);
            }
        }

        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255); // Black
        SDL_RenderClear(m_renderer);

        while(lag >= ms_per_update) {
            lag -= ms_per_update;
            invokeCallOnUpdate(reg, m_secondPerFrame);
            invokeOnColition<ColitionLayer1Tag>(reg, m_renderer, m_secondPerFrame);
            invokeOnColition<ColitionLayer2Tag>(reg, m_renderer, m_secondPerFrame);
            invokeOnColition<ColitionLayer3Tag>(reg, m_renderer, m_secondPerFrame);
            invokeOnColition<ColitionLayer4Tag>(reg, m_renderer, m_secondPerFrame);
            invokeOnColition<ColitionLayer5Tag>(reg, m_renderer, m_secondPerFrame);
            invokeOnColition<ColitionLayer6Tag>(reg, m_renderer, m_secondPerFrame);
            invokeOnColition<ColitionLayer7Tag>(reg, m_renderer, m_secondPerFrame);
            invokeOnColition<ColitionLayer8Tag>(reg, m_renderer, m_secondPerFrame);
            invokeMovement(reg, m_secondPerFrame);
        }
        invokeDrawTexture<RenderLayer1Tag>(reg, m_renderer, m_atlas, m_secondPerFrame, lag / ms_per_update);
        invokeDrawTexture<RenderLayer2Tag>(reg, m_renderer, m_atlas, m_secondPerFrame, lag / ms_per_update);
        invokeDrawTexture<RenderLayer3Tag>(reg, m_renderer, m_atlas, m_secondPerFrame, lag / ms_per_update);
        invokeDrawTexture<RenderLayer4Tag>(reg, m_renderer, m_atlas, m_secondPerFrame, lag / ms_per_update);
        invokeDrawTexture<RenderLayer5Tag>(reg, m_renderer, m_atlas, m_secondPerFrame, lag / ms_per_update);
        invokeDrawTexture<RenderLayer6Tag>(reg, m_renderer, m_atlas, m_secondPerFrame, lag / ms_per_update);
        invokeDrawTexture<RenderLayer7Tag>(reg, m_renderer, m_atlas, m_secondPerFrame, lag / ms_per_update);
        invokeDrawTexture<RenderLayer8Tag>(reg, m_renderer, m_atlas, m_secondPerFrame, lag / ms_per_update);

        invokeDrawAddBlendTexture<RenderLayer1Tag>(reg, m_renderer, m_atlas, m_secondPerFrame, lag / ms_per_update);
        invokeDrawAddBlendTexture<RenderLayer2Tag>(reg, m_renderer, m_atlas, m_secondPerFrame, lag / ms_per_update);
        invokeDrawAddBlendTexture<RenderLayer3Tag>(reg, m_renderer, m_atlas, m_secondPerFrame, lag / ms_per_update);
        invokeDrawAddBlendTexture<RenderLayer4Tag>(reg, m_renderer, m_atlas, m_secondPerFrame, lag / ms_per_update);
        invokeDrawAddBlendTexture<RenderLayer5Tag>(reg, m_renderer, m_atlas, m_secondPerFrame, lag / ms_per_update);
        invokeDrawAddBlendTexture<RenderLayer6Tag>(reg, m_renderer, m_atlas, m_secondPerFrame, lag / ms_per_update);
        invokeDrawAddBlendTexture<RenderLayer7Tag>(reg, m_renderer, m_atlas, m_secondPerFrame, lag / ms_per_update);
        invokeDrawAddBlendTexture<RenderLayer8Tag>(reg, m_renderer, m_atlas, m_secondPerFrame, lag / ms_per_update);

        SDL_RenderPresent(m_renderer);
    }

    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}
