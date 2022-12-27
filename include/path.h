#ifndef PATH
#define PATH
#include "main.h"
#include "utils.h"

typedef struct {
	int angle; // angle to rotate to before move
	double distance; // distance to travel in inches
	int post_angle; // angle to rotate to after move
	int action; // action to run after everything else
} PathStep;

typedef struct {
	PathStep* steps;
	int length;
} Path;

#endif
