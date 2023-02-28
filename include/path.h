#ifndef PATH
#define PATH
#include "main.h"
#include "utils.h"

typedef struct {
	int angle; // angle to rotate to before move
	int direction; // move forward or backward. 1 for forward, -1 for backward
	double distance; // distance to travel in inches
	int speed; // speed to travel in inches per second
	int post_angle; // angle to rotate to after move
	int action; // action to run after everything else
} PathStep;

typedef struct {
	PathStep* steps;
	int length;
} Path;

// useful for loading in a path
typedef struct {
	Vector2 pos;
	int speed;
	int action;
	bool has_post_angle;
	int post_angle;
} TempPathStep;


Path load_path(char* filename) {
	FILE* f = fopen(filename, "r");
	if (!f) {
		printf("file not found\r\n");
		return (Path){NULL, 0};
	}

	int steps = 0;

	printf("loading path from %s\r\n", filename);

	TempPathStep* temp_path = malloc(sizeof(TempPathStep));

	int x_int; // in case the positions happen to be whole numbers.
	int y_int;

	char buf[40];

	while (fgets(buf, sizeof(buf), f) != NULL) {
		if (sscanf(buf, "%lf %lf %d %d %d", &(temp_path[steps].pos.x), &(temp_path[steps].pos.y), &(temp_path[steps].speed), &(temp_path[steps].action), &(temp_path[steps].post_angle)) == 5) {
			temp_path[steps].has_post_angle = true;
		}
		else if (sscanf(buf, "%lf %lf %d %d %*s", &(temp_path[steps].pos.x), &(temp_path[steps].pos.y), &(temp_path[steps].speed), &(temp_path[steps].action)) == 4) {
			temp_path[steps].has_post_angle = false;
		}
		else if (sscanf(buf, "%d %d %d %d %d", &x_int, &y_int, &(temp_path[steps].speed), &(temp_path[steps].action), &(temp_path[steps].post_angle)) == 5) {
			temp_path[steps].has_post_angle = true;
			temp_path[steps].pos.x = (double)x_int;
			temp_path[steps].pos.y = (double)y_int;
		}
		else if (sscanf(buf, "%d %d %d %d %*s", &x_int, &y_int, &(temp_path[steps].speed), &(temp_path[steps].action)) == 4) {
			temp_path[steps].has_post_angle = false;
			temp_path[steps].pos.x = (double)x_int;
			temp_path[steps].pos.y = (double)y_int;
		}
		steps++;
		temp_path = realloc(temp_path, sizeof(TempPathStep) * (steps + 2));
	}

	fclose(f);

	Path path = {
		malloc(sizeof(PathStep) * steps),
		steps
	};

	for (int i = 0; i < steps; i++) {
		path.steps[i].angle = 0;
		path.steps[i].distance = 0;
		path.steps[i].direction = 1;

		if (i != 0) {
			path.steps[i].angle = path.steps[i - 1].angle;
			path.steps[i].distance = Vector2Length(Vector2Subtract(temp_path[i - 1].pos, temp_path[i].pos));
			if (path.steps[i].distance != 0) {
				double temp_angle = asin((temp_path[i - 1].pos.x - temp_path[i].pos.x) / path.steps[i].distance);
				path.steps[i].angle = (int)rad2deg(-temp_angle);
			}

			if (temp_path[i].pos.y - temp_path[i - 1].pos.y < 0) {
				path.steps[i].angle *= -1;
				path.steps[i].angle += 180;
			}
			path.steps[i].angle = clamp360(path.steps[i].angle);
		}

		int prev_angle = i != 0 ? path.steps[i - 1].post_angle : 0;

		// drive backwards if it's more efficient
		if (distance_between(prev_angle, path.steps[i].angle) > distance_between(prev_angle, path.steps[i].angle + 180)) {
			path.steps[i].direction = -1;
			path.steps[i].angle = clamp360(path.steps[i].angle + 180);
		}

		path.steps[i].action = temp_path[i].action;
		path.steps[i].speed = temp_path[i].speed;
		path.steps[i].post_angle = temp_path[i].has_post_angle ? temp_path[i].post_angle : path.steps[i].angle;
	}

	return path;
}

#endif
