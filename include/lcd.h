#pragma once

#include "main.h"
#include <functional>

// PROS LCD wrapper class
// Simplifies creating and managing UI elements on the LCD
class CLcd
{
private:
    bool m_isInitialized = false;

    lv_color_t m_ErrorColor = {0, 0, 255, 255};
    int16_t m_ScreenLine = 0;
public:
    // Initialize LCD
    void Initialize();
    bool IsInitialized() {return m_isInitialized;}

    void AddErrorText(const char* text, ...)
    {
        static char buf[4096];
        va_list argptr;
        va_start(argptr, text);
        vsnprintf(buf, sizeof(buf), text, argptr);
        pros::lcd::set_text_color(m_ErrorColor);
        pros::lcd::set_text(m_ScreenLine++, buf);
        if (m_ScreenLine > 7)
        {
            pros::lcd::clear();
            m_ScreenLine = 0;
        }
    }

    void AddErrorText(const char* text, va_list args)
    {
        static char buf[4096];
        vsnprintf(buf, sizeof(buf), text, args);
        pros::lcd::set_text_color(m_ErrorColor);
        pros::lcd::set_text(m_ScreenLine++, buf);
        if (m_ScreenLine > 7)
        {
            pros::lcd::clear();
            m_ScreenLine = 0;
        }
    }
};

extern CLcd* lcd;