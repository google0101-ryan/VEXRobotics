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

lemlib::Chassis* chassis;

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
public:
    lemlib::Drivetrain* drivetrain;
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
    intake = new pros::Motor(16, false);

    leftGroup = new pros::MotorGroup({*left[0], *left[1], *left[2]});
    rightGroup = new pros::MotorGroup({*right[0], *right[1], *right[2]});
    if (errno != PROS_SUCCESS)
    {
        printf("ERROR Initializing one or more motors!\n");
    }

    limitSwitch = new pros::ADIDigitalIn('E');
    climbController = new pros::ADIDigitalOut('G');
    leftWingController = new pros::ADIDigitalOut('H');

    imu = new pros::Imu(17);
	
    lemlib::Drivetrain drivetrain{
        leftGroup,  //left motor group
        rightGroup, //right motor group
        10.5,       //drivebase width
        3.25,       //wheel size
        360,        //rpm
        2           //default chase power (defaults 2 if all omni, 8 if traction)
    };
    
    lemlib::OdomSensors odom{
        nullptr,    //vertical tracking wheel 1 - we don't have
        nullptr,    //vertical tracking wheel 2 - we don't have
        nullptr,    //horizontal tracking wheel 1 - we don't have
        nullptr,    //horizontal tracking wheel 2 - we don't have
        imu         //intertial sensor
    };

    lemlib::ControllerSettings lateralController{
        17,         //kP
        30,          //kD
        .1,         //SmallError
        100,        //SmallErrorTimout
        .5,         //Large Error
        500,        //Large Error Timeout
        0           //Slew
    };
  
    lemlib::ControllerSettings angularController{
        5.2,       //KP
        16.6,          //KD
        .1,         //SmallError
        100,        //SmallErrorTimout
        .5,         //LARGE ERROR
        500,        //LARGE ERROR TIMEOUT
        0           //Slew
    };
  

    chassis = new lemlib::Chassis(drivetrain, lateralController, angularController, odom);
    chassis->calibrate();
    
    printf("Init done\n");
}

void screen() {
    // loop forever
    while (true) {
        lemlib::Pose pose = chassis->getPose(); // get the current position of the robot
        pros::lcd::print(0, "x: %f", pose.x); // print the x position
        pros::lcd::print(1, "y: %f", pose.y); // print the y position
        pros::lcd::print(2, "heading: %f", pose.theta); // print the heading
        pros::delay(10);
    }
}



void Competition2023::DoAutonomous()
{
    static bool defensiveSide = true;
    if (defensiveSide){
    pros::Task screenTask(screen);
    chassis->setPose(lemlib::Pose(-60, -12));
    intake->move(127);
    // Move forward and grab triball
    chassis->moveTo(-60, -9, chassis->getPose().theta, 5000, true, 0.0f, 0.6f, 120.f);
    // Move backwards to bar
    chassis->moveTo(-60, -46, chassis->getPose().theta, 5000, false);
    // Turn towards goal
    chassis->turnTo(-500, 500, 3000, true, 127.0f, false);
    // Lower wings
    leftWingController->set_value(HIGH);
    // Move into position to knock the triball free
    chassis->moveTo(-53.0, -56.5, -40.0, 3000, false);
    // Turn to heading
    chassis->moveTo(-55.8, -54.5, -78.7, 3000, true);
    chassis->moveTo(-30.0, -63.0, -86.0, 3000, false);
    leftWingController->set_value(LOW);
    chassis->moveTo(900, -62.3, chassis->getPose().theta, 300, false);
    chassis->moveTo(-37.3, -62.3, -82.0, 3000);
    chassis->turnTo(500, -62.3, 3000);
    intake->move(0);
    chassis->moveTo(900, -62.3, chassis->getPose().theta, 300, true);
    }


    static bool offensiveSide = false;
    if (offensiveSide){
     chassis->setPose(60, -48,-135);
     //Go Backward to triball
     chassis->moveTo(52, -56, chassis->getPose().theta, 5000, true,0,0,127);
    //Wing down and turn to bar
     chassis->moveTo(60, -48, chassis->getPose().theta, 5000, false,0,0,127);
      leftWingController->set_value(HIGH);
      pros::delay(200);
    chassis->turnTo(60, -180, 2000,60);
    chassis->turnTo(60, 180, 2000,60);
     leftWingController->set_value(LOW);
     //Drive to bar
    chassis->moveTo(60, -12,0, 5000, true,0,0,127);
    intake->move(-127);
    
    }
    
    
  // void lemlib::Chassis::turnTo(float x, float y, int timeout, bool forwards = true, float maxSpeed = (127.0F), bool async = true)
}

bool catapultIsMoving = false;
// When X is pressed, toggle fling mode
// This is where we continously lower and fling the catapult until fling mode is disabled
bool isFlinging = false;

void Competition2023::DoOpControl()
{
    pros::Controller* controller = new pros::Controller(CONTROLLER_MASTER);
     pros::Task screenTask(screen);
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
            //intake->set_reversed(true);
            intake->move_voltage(-12000);
        }
        else if (controller->get_digital(DIGITAL_L1))
        {
           //intake->set_reversed(true);
           intake->move_voltage(12000);
        }
        else
        {
            intake->brake();
        }

        pros::delay(2);
    }
}
