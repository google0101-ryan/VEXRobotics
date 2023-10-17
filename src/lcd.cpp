#include "lcd.h"
#include "Competition.h"

void CLcd::Initialize()
{
    if (!pros::lcd::initialize())
        curCompetition->Error("Error during initialization of LCD class\n");
    m_isInitialized = true;

    pros::lcd::set_background_color(lv_color_t{240, 152, 98, 255});
}

CLcd lcd_local;
CLcd* lcd = &lcd_local;