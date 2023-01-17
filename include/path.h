#ifndef PATH
#define PATH
#include "main.h"
#include "utils.h"

typedef struct {
	int angle; // angle to rotate to before move
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

// fgets - Read a string from the specified stream
char* better_fgets(char *str, int num, FILE *stream) {
	printf("fgetsing lol\r\n");
	// Required by standard
	char *ptr = str;
	int value;
	if (feof(stream)) return NULL;

	printf("not EOF\r\n");
	// Read at most num-1 characters
	for (; num > 1 && (value = fgetc(stream)) != EOF; num--) {
		*ptr++ = (char)value;
		// Exit loop (including new line) when a new line is found
		if ((char)value == '\n') break;
	}
	printf("made it to the end. adding null terminator\r\n");
	// Add null terminator
	*ptr = '\0';
	return str;
}

Path load_path(char* filename) {
	FILE* f = fopen(filename, "r");
	int steps = 0;

	printf("loading path from %s\r\n", filename);

	TempPathStep* temp_path = malloc(sizeof(TempPathStep));

	printf("allocated temp path\r\n");

	int x_int; // in case the positions happen to be whole numbers.
	int y_int;

	char buf[40];

	while (better_fgets(buf, sizeof(buf), f) != NULL) {
		printf("got to step %d\r\n", steps);
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

	printf("allocating real path\r\n");

	Path path = {
		malloc(sizeof(PathStep) * steps),
		steps
	};

	for (int i = 0; i < steps; i++) {
		path.steps[i].angle = 0;
		path.steps[i].distance = 0;

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

		path.steps[i].action = temp_path[i].action;
		path.steps[i].speed = temp_path[i].speed;
		path.steps[i].post_angle = temp_path[i].has_post_angle ? temp_path[i].post_angle : path.steps[i].angle;
	}

	return path;
}

#endif
