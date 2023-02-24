#ifndef FLYWHEEL_HEADER
#define FLYWHEEL_HEADER
#include "main.h"
#include "utils.h"

#define FLYWHEEL_1 15
#define FLYWHEEL_2 16

#define FLYWHEEL_SPEED 600

enum FlywheelState {
	STOPPED,
	STARTING,
	M1,
	M2
} flywheel_state;

int flywheel_time;

void start_flywheel() {
	flywheel_state = STARTING;
	flywheel_time = 0;
	motor_move_velocity(FLYWHEEL_1, FLYWHEEL_SPEED);
	motor_move_velocity(FLYWHEEL_2, FLYWHEEL_SPEED);
}

void stop_flywheel() {
	flywheel_state = STOPPED;
	motor_move_velocity(FLYWHEEL_1, FLYWHEEL_SPEED);
	motor_move_velocity(FLYWHEEL_2, FLYWHEEL_SPEED);
}

void update_flywheel(int delta) {
	flywheel_time += delta;

	if (flywheel_state == STARTING) {
		if (flywheel_time >= 10000) {
			flywheel_state = M2;
			motor_move_velocity(FLYWHEEL_1, 0);
		}
		return;
	}

	if (flywheel_time % 2000 > 200) return;

	if (flywheel_state == M2) {
		motor_move_velocity(FLYWHEEL_2, 0);
		motor_move_velocity(FLYWHEEL_1, FLYWHEEL_SPEED);
		return
	}

	motor_move_velocity(FLYWHEEL_2, FLYWHEEL_SPEED);
	motor_move_velocity(FLYWHEEL_1, 0);
}
#endif
