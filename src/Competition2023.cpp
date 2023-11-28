#include "Competition.h"
#include "main.h"
#include "lcd.h"
#include "pros/vision.hpp"
#include "ImuWrapper.h"
#include "lemlib/api.hpp"
#include "lemlib/logger/stdout.hpp"
#include "pros/misc.h"

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
    pros::Motor* intake;
    pros::ADIDigitalIn* limitSwitch;
    pros::ADIDigitalOut* climbController, *leftWingController;
    pros::Imu* imu;
    pros::MotorGroup* leftGroup, *rightGroup;
    lemlib::Drivetrain* drivetrain;
    lemlib::ControllerSettings* linearController, *angularController;
    lemlib::Chassis* chassis;
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

    vision = new pros::Vision(VISION_SYSTEM_PORT);
    if (errno != PROS_SUCCESS)
    {
        printf("Cannot configure vision system on port %d: %s\n", VISION_SYSTEM_PORT, strerror(errno));
    }

    left[0] = new pros::Motor(18);
    left[1] = new pros::Motor(19, true);
    left[2] = new pros::Motor(20, true);
    right[0] = new pros::Motor(11);
    right[1] = new pros::Motor(12, true);
    right[2] = new pros::Motor(13);
    catapult = new pros::Motor(15, MOTOR_GEARSET_36, true);
    catapult->set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_COAST);
    catapult->move_absolute(0.0, 100);
    intake = new pros::Motor(16);

    leftGroup = new pros::MotorGroup({*left[0], *left[1], *left[2]});
    rightGroup = new pros::MotorGroup({*right[0], *right[1], *right[2]});

    drivetrain = new lemlib::Drivetrain(leftGroup, rightGroup, 10, lemlib::Omniwheel::NEW_325, 360, 2);

    if (errno != PROS_SUCCESS)
    {
        printf("ERROR Initializing one or more motors!\n");
    }

    limitSwitch = new pros::ADIDigitalIn('E');
    climbController = new pros::ADIDigitalOut('G');
    leftWingController = new pros::ADIDigitalOut('H');

    imu = new pros::Imu(17);
    
    linearController = new lemlib::ControllerSettings(10, 30, 1, 100, 3, 500, 20);
    angularController = new lemlib::ControllerSettings(2, 10, 1, 100, 3, 500, 20);

    chassis = new lemlib::Chassis(*drivetrain, *linearController, *angularController, lemlib::OdomSensors(nullptr, nullptr, nullptr, nullptr, imu));
    chassis->calibrate();

    printf("Init done\n");
}

void screen() {
    // loop forever
    while (true) {
    }
}

static bool leftSide = false;

void Competition2023::DoAutonomous()
{
    chassis->setPose(0, 0, 0);
    chassis->moveToOld(0, -10, 0.0f, INT32_MAX);

#if 1
    if (leftSide)
    {
    }
    else
    {

    }
#else
    catapult->move(95);
#endif
}

bool catapultIsMoving = false;
// When X is pressed, toggle fling mode
// This is where we continously lower and fling the catapult until fling mode is disabled
bool isFlinging = false;

void Competition2023::DoOpControl()
{
    pros::Controller* controller = new pros::Controller(CONTROLLER_MASTER);
    while (true)
    {
        int fwd =  controller->get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int turn = controller->get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

        int left_ = fwd + turn;
        int right_ = fwd - turn;

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

        if (controller->get_digital(DIGITAL_X))
        {
            isFlinging = !isFlinging;
            if (isFlinging)
                catapult->move(95);
            else
                catapult->brake();
        }

        if (controller->get_digital(DIGITAL_B))
        {
            isFlinging = false;
            catapultIsMoving = false;
            catapult->brake();
        }

        if (controller->get_digital(DIGITAL_Y))
        {
            catapult->move(95);
            pros::delay(400);
            catapult->brake();
        }

        if (controller->get_digital(DIGITAL_UP))
        {
            climbController->set_value(HIGH);
        }
        else if (controller->get_digital(DIGITAL_DOWN))
        {
            climbController->set_value(LOW);
        }
        
        if (controller->get_digital(DIGITAL_RIGHT))
        {
            leftWingController->set_value(HIGH);
        }
        else if (controller->get_digital(DIGITAL_LEFT))
        {
            leftWingController->set_value(LOW);
        }

        if (controller->get_digital(DIGITAL_L2))
        {
            intake->set_reversed(false);
            intake->move_voltage(12000);
        }
        else if (controller->get_digital(DIGITAL_L1))
        {
            intake->set_reversed(true);
            intake->move_voltage(12000);
        }
        else
        {
            intake->brake();
        }

        pros::delay(2);
    }
}
