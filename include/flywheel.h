#ifndef FLYWHEEL_HEADER
#define FLYWHEEL_HEADER
#include "main.h"
#include "utils.h"

#define FLYWHEEL_1 15
#define FLYWHEEL_2 16

void start_flywheel() {
	motor_move(FLYWHEEL_1, 127);
	motor_move(FLYWHEEL_2, 127);
}
#endif
