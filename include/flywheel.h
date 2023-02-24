#ifndef FLYWHEEL_HEADER
#define FLYWHEEL_HEADER
#include "main.h"
#include "utils.h"

#define FLYWHEEL_1 15
#define FLYWHEEL_2 16

#define FLYWHEEL_SPEED 400

enum FlywheelState {
	STOPPED,
	STARTING,
	M1,
	M2,
	SHOOTING
} flywheel_state;

int flywheel_time;

void start_flywheel() {
	printf("spinning up flywheel...\r\n");
	flywheel_state = STARTING;
	flywheel_time = 0;
	motor_set_brake_mode(FLYWHEEL_1, E_MOTOR_BRAKE_COAST);
	motor_set_brake_mode(FLYWHEEL_2, E_MOTOR_BRAKE_COAST);
	motor_move_velocity(FLYWHEEL_1, FLYWHEEL_SPEED);
	motor_move_velocity(FLYWHEEL_2, FLYWHEEL_SPEED);
}

void stop_flywheel() {
	printf("stopping flywheel...\r\n");
	flywheel_state = STOPPED;
	motor_brake(FLYWHEEL_1);
	motor_brake(FLYWHEEL_2);
}

void update_flywheel(int delta, bool shooting) {
	flywheel_time += delta;

	if (flywheel_state == STARTING) {
		if (flywheel_time >= 4000) {
			printf("switching to motor two\r\n");
			flywheel_state = M2;
			motor_brake(FLYWHEEL_1);
		}
		return;
	}

	if (shooting) {
		flywheel_state = SHOOTING;
		motor_move_velocity(FLYWHEEL_1, FLYWHEEL_SPEED);
		motor_move_velocity(FLYWHEEL_2, FLYWHEEL_SPEED);
		return;
	}

	if (flywheel_time % 2000 != 0 && flywheel_state != SHOOTING) return;

	if (flywheel_state == M2) {
		printf("switching to motor one\r\n");
		flywheel_state = M1;
		motor_brake(FLYWHEEL_2);
		motor_move_velocity(FLYWHEEL_1, FLYWHEEL_SPEED);
		return;
	}

	printf("switching to motor two\r\n");
	flywheel_state = M2;
	motor_move_velocity(FLYWHEEL_2, FLYWHEEL_SPEED);
	motor_brake(FLYWHEEL_1);
}
#endif
