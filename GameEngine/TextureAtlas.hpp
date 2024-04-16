#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

class TextureAtlas {
private:
    struct Skyline {
        int x, y, width;
    };

    SDL_Renderer* m_renderer;
    SDL_Texture* m_atlasTexture;
    int m_atlasWidth;
    int m_atlasHeight;
    std::vector<Skyline> m_skylines;
    const int m_padding = 2;

public:
    TextureAtlas(SDL_Renderer* renderer, int width, int height)
        : m_renderer(renderer), m_atlasWidth(width), m_atlasHeight(height) {
        m_atlasTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
        SDL_SetTextureBlendMode(m_atlasTexture, SDL_BLENDMODE_BLEND);
        m_skylines.push_back({m_padding, m_padding, width - 2 * m_padding}); // Initialize with a single large skyline
    }

    ~TextureAtlas() {
        if (m_atlasTexture) {
            SDL_DestroyTexture(m_atlasTexture);
        }
    }

    SDL_Texture* GetAtlasTexture() {
        return m_atlasTexture;
    }

    SDL_Rect AddImage(const std::string& imagePath) {
        SDL_Surface* surface = IMG_Load(imagePath.c_str());
        if (!surface) {
            // Handle loading error
            return SDL_Rect{0, 0, 0, 0};
        }

        int imageWidth = surface->w + m_padding;
        int imageHeight = surface->h + m_padding;
        int bestIndex;
        int bestY = std::numeric_limits<int>::max();
        int bestX = 0;

        // Find the best position for the new image
        for (size_t i = 0; i < m_skylines.size(); ++i) {
            int y = findPositionForNewNode(i, imageWidth, imageHeight, bestX);
            if (y + imageHeight < bestY) {
                bestY = y;
                bestIndex = i;
            }
        }

        // If bestY + imageHeight > atlasHeight, resize the atlas
        if (bestY + imageHeight > m_atlasHeight) {
            resize(m_atlasWidth, bestY + imageHeight);
        }

        // Place image at best position
        addSkylineLevel(bestIndex, bestX, bestY, imageWidth, imageHeight);
        SDL_Rect rect = {bestX, bestY, surface->w, surface->h};
        SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
        SDL_FreeSurface(surface);

        // Render the texture to the atlas
        SDL_SetRenderTarget(m_renderer, m_atlasTexture);
        SDL_RenderCopy(m_renderer, texture, NULL, &rect);
        SDL_SetRenderTarget(m_renderer, NULL);
        SDL_DestroyTexture(texture);

        return rect;
    }

private:
    void resize(int newWidth, int newHeight) {
        SDL_Texture* newAtlas = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, newWidth, newHeight);
        SDL_SetTextureBlendMode(newAtlas, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(m_renderer, newAtlas);
        SDL_RenderCopy(m_renderer, m_atlasTexture, NULL, NULL); // Copy old atlas to new
        SDL_SetRenderTarget(m_renderer, NULL);
        SDL_DestroyTexture(m_atlasTexture);
        m_atlasTexture = newAtlas;
        m_atlasWidth = newWidth;
        m_atlasHeight = newHeight;
    }

    int findPositionForNewNode(int index, int width, int height, int& bestX) {
        int x = m_skylines[index].x;
        int y = m_skylines[index].y;
        if (x + width > m_atlasWidth) return std::numeric_limits<int>::max();
        int i = index;
        int maxWidth = m_skylines[i].width;
        while (maxWidth < width) {
            y = std::max(y, m_skylines[i].y);
            i++;
            if (i == m_skylines.size()) break;
            maxWidth += m_skylines[i].width;
        }
        bestX = x;
        return y;
    }

    void addSkylineLevel(int index, int x, int y, int width, int height) {
        Skyline newNode = {x, y + height, width};
        m_skylines.insert(m_skylines.begin() + index, newNode);

        // Remove parts of the skyline that are covered by the new node
        for (size_t i = index + 1; i < m_skylines.size(); ++i) {
            if (m_skylines[i].x < m_skylines[i - 1].x + m_skylines[i - 1].width) {
                int shrink = m_skylines[i - 1].x + m_skylines[i - 1].width - m_skylines[i].x;
                m_skylines[i].x += shrink;
                m_skylines[i].width -= shrink;
                if (m_skylines[i].width <= 0) {
                    m_skylines.erase(m_skylines.begin() + i);
                    i--;
                } else break;
            } else break;
        }

        // Merge skylines
        mergeSkylines();
    }

    void mergeSkylines() {
        for (size_t i = 1; i < m_skylines.size(); ++i) {
            if (m_skylines[i - 1].y == m_skylines[i].y) {
                m_skylines[i - 1].width += m_skylines[i].width;
                m_skylines.erase(m_skylines.begin() + i);
                i--;
            }
        }
    }
};
