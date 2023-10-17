#pragma once

#include <stdarg.h>
#include "lcd.h"

// Override the functions in this class to create a competition
class Competition
{
public:
    // Called on robot startup, do your initialization here
    virtual void Initialize() = 0;
    // Called on autonomous start
    // Do your autonomous stuff in this function
    virtual void DoAutonomous() = 0;
    // Called when the players take control
    // Use this to map buttons and joysticks to the robots
    virtual void DoOpControl() = 0; 

    // Call on a fatal error
    // Does not return
    inline void Error(const char* fmt, ...)
    {
        va_list argptr;
        va_start(argptr, fmt);
        printf("ERROR: ");
        vprintf(fmt, argptr);
        printf("\n");
        printf("Exiting...\n");

        if (lcd->IsInitialized())
            lcd->AddErrorText(fmt, argptr);

        while (1)
        {
            pros::delay(2);
        }
    }
};

// IMPORTANT: Set this to your competition in `your_competition.cpp`! 
// This is how the code knows what competition to use
// DO NOT USE MULTIPLE COMPETITIONS AT A TIME, THEY WILL OVERWRITE THIS VARIABLE
extern Competition* curCompetition;