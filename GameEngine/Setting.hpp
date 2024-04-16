#pragma once
#include <string>

class Setting {
    std::string m_title;
    int m_screenWidth;
    int m_screenHeight;
    float m_secondPerFrame;

public:
    Setting()
        : m_title("Untitled")
        , m_screenWidth(512)
        , m_screenHeight(512)
        , m_secondPerFrame(0.01)
    {}

    const std::string& GetTitle() const {
        return m_title;
    }

    Setting& SetTitle(const std::string& title) {
        m_title = title;
        return *this;
    }

    int GetScreenWidth() const {
        return m_screenWidth;
    }

    Setting& SetScreenWidth(int screenWidth) {
        m_screenWidth = screenWidth;
        return *this;
    }

    int GetScreenHeight() const {
        return m_screenHeight;
    }

    Setting& SetScreenHeight(int screenHeight) {
        m_screenHeight = screenHeight;
        return *this;
    }

    float GetSecondPerFrame() const {
        return m_secondPerFrame;
    }

    Setting& SetSecondPerFrame(float secondPerFrame) {
        m_secondPerFrame = secondPerFrame;
        return *this;
    }
};
