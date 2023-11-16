#pragma once

#include "main.h"
#include "okapi/api.hpp"

class ImuWrapper
{
private:
    okapi::IMU* imu;
    int motorCount;
    pros::Motor** leftSide, **rightSide;
public:
    ImuWrapper(uint8_t port, pros::Motor** leftSide, pros::Motor** rightSide, int motorCount);

    // Turn robot left until it reaches degrees (goes from 0 to +180)
    void TurnLeft(double degrees);
    
    // Turn robot right until it reaches degrees (goes from 0 to +180)
    void TurnRight(double degrees);
};