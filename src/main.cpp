#include "main.h"
#include "Competition.h"

void initialize()
{
	curCompetition->Initialize();
}

void opcontrol()
{
	curCompetition->DoOpControl();
}

void autonomous()
{
	curCompetition->DoAutonomous();
}