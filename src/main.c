#include "main.h"
#include "path.h"
#include "utils.h"
#include "parameters.h"

double inch_ratio = 39.0 / 3.05; // inches traveled / motor rotations

#define LEFT_WHEEL 1
#define RIGHT_WHEEL 2

#define RIGHT_SENSITIVITY 0.65
#define IMU_PORT 3

void stop_all_motors() {
	motor_move(LEFT_WHEEL, 0);
	motor_move(RIGHT_WHEEL, 0);
}

int rotate_to(int rotation) {
	int time_taken = 0;
	int start_rotation = (int)imu_get_heading(IMU_PORT);
	int current_rotation = start_rotation;
	int speed;

	int direction = direction_to(start_rotation, rotation);
	speed = distance_between(rotation, current_rotation);

	while (!(current_rotation < rotation + 1 && current_rotation > rotation - 1)) {
		if (time_taken % 100 == 0)
			printf("rotating at %d\r\n", speed);

		motor_move_velocity(LEFT_WHEEL, speed * direction);
		motor_move_velocity(RIGHT_WHEEL, -speed * direction);
		delay(2);
		time_taken += 2;
		current_rotation = (int)imu_get_heading(IMU_PORT);
		speed = distance_between(rotation, current_rotation);

		if (distance_between(rotation, current_rotation) < 10 || speed < 40)
			speed = 45;
	}
	stop_all_motors();
	printf("spin done!\r\n\r\n");
	return time_taken;
}

int travel_distance(double distance, int32_t speed, int target_rotation) {
	int time_taken = 0;

	double left_start = motor_get_position(LEFT_WHEEL);
	double right_start = motor_get_position(RIGHT_WHEEL);

	double current_distance = 0;

	int wheel_power[2];

	// convert inches/second to RPMs
	speed *= 60;
	speed /= inch_ratio;

	while (current_distance < distance) {

		wheel_power[0] = speed;
		wheel_power[1] = speed;

		// really smart rotation correction (I hope)
		double current_rotation = (int)imu_get_heading(IMU_PORT);
		int rot_distance = distance_between(current_rotation, target_rotation);
		if (rot_distance > 1) {

			double direction = direction_to(current_rotation, target_rotation) * ((double)rot_distance / 180.0);

			direction *= speed;

			wheel_power[0] += (int)direction;
			wheel_power[1] -= (int)direction;

			if (time_taken % 100 == 0) {
				printf("rotation error: %d\r\n", rot_distance);
				printf("compensating by going right %f\r\n", direction);
			}
		}

		motor_move_velocity(LEFT_WHEEL, wheel_power[0]);
		motor_move_velocity(RIGHT_WHEEL, wheel_power[1]);

		current_distance = ((motor_get_position(LEFT_WHEEL) - left_start) + (motor_get_position(RIGHT_WHEEL) - right_start)) / 2; // average delta rotations
		current_distance *= inch_ratio; // convert from rotations to inches

		time_taken += 2;
		delay(2);
	}

	stop_all_motors();

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
	lv_fs_init();
	printf("setting encoder units and directions\r\n");
	motor_set_encoder_units(LEFT_WHEEL, E_MOTOR_ENCODER_ROTATIONS);
	motor_set_encoder_units(RIGHT_WHEEL, E_MOTOR_ENCODER_ROTATIONS);

	// green gears
	motor_set_gearing(LEFT_WHEEL, E_MOTOR_GEAR_GREEN);
	motor_set_gearing(RIGHT_WHEEL, E_MOTOR_GEAR_GREEN);

	motor_set_brake_mode(LEFT_WHEEL, E_MOTOR_BRAKE_BRAKE);
	motor_set_brake_mode(RIGHT_WHEEL, E_MOTOR_BRAKE_BRAKE);
	motor_set_reversed(RIGHT_WHEEL, true);

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

void play_auton_program(char* filename) {
	Path path = load_path(filename);

	if (path.length == 0) return;

	for (int i = 0; i < path.length; i++) {
		printf("angle: %d\ndistance: %f\r\n", path.steps[i].angle, path.steps[i].distance);
		rotate_to(path.steps[i].angle);
		travel_distance(path.steps[i].distance, path.steps[i].speed, path.steps[i].angle);
		rotate_to(path.steps[i].post_angle);

		switch (path.steps[i].action) {
			case 0:
				break;
			case 1:
				// TODO: roller
				break;
			case 2:
				// TODO: shoot disk
				break;
		}

	}
}

void autonomous() {
	// rotate_to(180);
	// travel_distance(15.0 * 12.0, 40, 180);
	play_auton_program(AUTON_FILE);
}

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
	int frames = 0;
	Vector2 left_stick;
	Vector2 right_stick;
	int wheel_power[2];

	int target_rotation = (int)imu_get_heading(IMU_PORT);

	while (true) {
		left_stick = (Vector2){controller_get_analog(E_CONTROLLER_MASTER, E_CONTROLLER_ANALOG_LEFT_X), controller_get_analog(E_CONTROLLER_MASTER, E_CONTROLLER_ANALOG_LEFT_Y)};
		right_stick = (Vector2){controller_get_analog(E_CONTROLLER_MASTER, E_CONTROLLER_ANALOG_RIGHT_X), controller_get_analog(E_CONTROLLER_MASTER, E_CONTROLLER_ANALOG_RIGHT_Y)};

		wheel_power[0] = left_stick.y;
		wheel_power[1] = left_stick.y;

		if (right_stick.x != 0) {
			wheel_power[0] += right_stick.x * RIGHT_SENSITIVITY;
			wheel_power[1] -= right_stick.x * RIGHT_SENSITIVITY;
			target_rotation = (int)imu_get_heading(IMU_PORT);
		}

		motor_move(LEFT_WHEEL, wheel_power[0]);
		motor_move(RIGHT_WHEEL, wheel_power[1]);

		if (frames == 250) {
			// put peepee poopoo statements here
			double avg_wheel_dist = (motor_get_position(LEFT_WHEEL) + motor_get_position(RIGHT_WHEEL)) / 2;
			printf("distance traveled in rotations: %f\r\n", avg_wheel_dist);

			frames = 0;
		}

		delay(2);
		frames++;
	}
}
