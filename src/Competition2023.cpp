#include "Competition.h"
#include "main.h"
#include "lcd.h"
#include "pros/vision.hpp"

// Competition class for 2023
// Use this as an example, do not include in your final project
// Make sure you only have one competition file in your project

class Competition2023 : public Competition
{
private:
    pros::vision_signature_s_t g_triangle;
    pros::Vision* vision;

    pros::Motor* left[3], *right[3];
    pros::Motor* catapult;
    pros::ADIDigitalIn* limitSwitch;
public:
    virtual void Initialize();
    virtual void DoAutonomous();
    virtual void DoOpControl();
};

// Define your competition like this
Competition2023 comp_2023;
Competition* curCompetition = &comp_2023;

#define VISION_SYSTEM_PORT 11

void Competition2023::Initialize()
{
    // Motor and button initialization goes here
    lcd->Initialize();
    lcd->AddErrorText("Test text: 0x%x", 0xAAAAAAAA);

    vision = new pros::Vision(VISION_SYSTEM_PORT);
    if (errno != PROS_SUCCESS)
    {
        Error("Cannot configure vision system on port %d: %s\n", VISION_SYSTEM_PORT, strerror(errno));
    }

    left[0] = new pros::Motor(1);
    left[1] = new pros::Motor(2);
    left[2] = new pros::Motor(3);
    left[2]->set_reversed(true);
    right[0] = new pros::Motor(4);
    right[0]->set_reversed(true);
    right[1] = new pros::Motor(5);
    right[1]->set_reversed(true);
    right[2] = new pros::Motor(6);
    catapult = new pros::Motor(14, MOTOR_GEARSET_36, true);
    catapult->set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);
    catapult->move_absolute(0.0, 100);

    limitSwitch = new pros::ADIDigitalIn('E');
}

void Competition2023::DoAutonomous()
{
    while (true)
    {
        pros::delay(2);
    }
}

bool catapultIsMoving = false;

void Competition2023::DoOpControl()
{
    pros::Controller* controller = new pros::Controller(CONTROLLER_MASTER);
    while (true)
    {
        int fwd =  controller->get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int turn = controller->get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

        int left_ = fwd - turn;
        int right_ = fwd + turn;

        for (int i = 0; i < 3; i++)
            left[i]->move(left_);
        for (int i = 0; i < 3; i++)
            right[i]->move(right_);

        if (catapultIsMoving && limitSwitch->get_value())
        {
            catapult->brake();
            catapultIsMoving = false;
        }

        if (controller->get_digital(DIGITAL_A) && !catapultIsMoving)
        {
            catapult->move_velocity(100);
            catapultIsMoving = true;
        }

        pros::delay(2);
    }
}
