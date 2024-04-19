#pragma once
#include <string>

class Setting {
public:
    struct Size {
        int width, height;
    };

private:
    std::string m_title;
    Size m_logicalSize;
    Size m_windowSize;
    float m_secondPerFrame;

public:
    Setting()
        : m_title("Untitled")
        , m_logicalSize({512, 512})
        , m_windowSize({512, 512})
        , m_secondPerFrame(0.01)
    {}

    const std::string& GetTitle() const {
        return m_title;
    }

    Setting& SetTitle(const std::string& title) {
        m_title = title;
        return *this;
    }

    Setting& SetLogicalSize(int width, int height) {
        m_logicalSize = { width, height };
        return *this;
    }

    Size GetLogicalSize() const {
        return m_logicalSize;
    }

    Setting& SetWindowSize(int width, int height) {
        m_windowSize = { width, height };
        return *this;
    }

    Size GetWindowSize() const {
        return m_windowSize;
    }

    float GetSecondPerFrame() const {
        return m_secondPerFrame;
    }

    Setting& SetSecondPerFrame(float secondPerFrame) {
        m_secondPerFrame = secondPerFrame;
        return *this;
    }
};
