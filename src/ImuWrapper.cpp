#include "ImuWrapper.h"

#define MARGIN_OF_ERROR 10

ImuWrapper::ImuWrapper(uint8_t port, pros::Motor** leftSide, pros::Motor** rightSide, int motorCount)
: leftSide(leftSide), rightSide(rightSide), motorCount(motorCount)
{
    imu = new okapi::IMU(port);

    imu->calibrate();
    while (imu->isCalibrating())
        pros::delay(2);
}

void ImuWrapper::TurnLeft(double degrees)
{
    degrees = imu->get() - degrees;

    for (int i = 0; i < motorCount; i++)
    {
        leftSide[i]->move(-50);
        rightSide[i]->move(50);
    }

    while (imu->get() > degrees)
    {
        pros::delay(2);
    }

    for (int i = 0; i < motorCount; i++)
    {
        leftSide[i]->brake();
        rightSide[i]->brake();
    }
}

void ImuWrapper::TurnRight(double degrees)
{
    degrees -= MARGIN_OF_ERROR;
    degrees = imu->get() + degrees;

    for (int i = 0; i < motorCount; i++)
    {
        leftSide[i]->move(50);
        rightSide[i]->move(-50);
    }

    while (imu->get() < degrees)
    {
        pros::delay(2);
    }

    for (int i = 0; i < motorCount; i++)
    {
        leftSide[i]->brake();
        rightSide[i]->brake();
    }
}
