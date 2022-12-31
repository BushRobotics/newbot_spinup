#include "main.h"
#include "path.h"
#include "utils.h"
#include "parameters.h"

double inch_ratio = 1.0 / 1.0; // inches traveled / motor rotations

#define LEFT_WHEEL 1
#define RIGHT_WHEEL 2

#define RIGHT_SENSITIVITY 0.65
#define IMU_PORT 12


int rotate_to(int rotation, int32_t speed) {
	int time_taken = 0;
	int start_rotation = (int)imu_get_heading(IMU_PORT);
	int current_rotation = start_rotation;

	speed *= direction_to(start_rotation, rotation);

	while (!(current_rotation < rotation + 2 && current_rotation > rotation - 2)) {
		motor_move_velocity(LEFT_WHEEL, speed);
		motor_move_velocity(RIGHT_WHEEL, -speed);
		delay(2); // delay until within 4 degrees of position
		time_taken += 2;
		current_rotation = (int)imu_get_rotation(IMU_PORT);
	}
	stop_all_motors();
	printf("\r\n\r\n\tSpin done!\r\n\r\n");
	return time_taken;
}

int travel_distance(double distance, int32_t speed) {
	int time_taken = 0;

	double left_start = motor_get_position(LEFT_WHEEL);
	double right_start = motor_get_position(RIGHT_WHEEL);

	double current_distance = 0;
	bool not_there_yet = true;

	if (distance < 0)
		speed *= -1;

	while (not_there_yet) {
		motor_move_velocity(LEFT_WHEEL, speed);
		motor_move_velocity(RIGHT_WHEEL, speed);

		current_distance = ((motor_get_position(LEFT_WHEEL) - left_start) + (motor_get_position(RIGHT_WHEEL) - right_start)) / 2; // average delta rotations
		current_distance *= inch_ratio; // convert from rotations to inches

		not_there_yet = distance > 0 ? current_distance < distance : current_distance > distance;
		time_taken += 2;
		delay(2);
	}

	return time_taken;
}

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	printf("setting encoder units and directions\r\n");
	motor_set_encoder_units(LEFT_WHEEL, E_MOTOR_ENCODER_ROTATIONS);
	motor_set_encoder_units(RIGHT_WHEEL, E_MOTOR_ENCODER_ROTATIONS);

	motor_set_brake_mode(LEFT_WHEEL, E_MOTOR_BRAKE_BRAKE);
	motor_set_brake_mode(RIGHT_WHEEL, E_MOTOR_BRAKE_BRAKE);
	motor_set_reversed(LEFT_WHEEL, true);

	printf("calibrating inertial sensor...\r\n");
	imu_reset_blocking(IMU_PORT);
	printf("calibrated!\r\n");
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	while (true) {
		delay(2);
	}
}
