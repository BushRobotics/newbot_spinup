#ifndef UTILS
#define UTILS
#include "main.h"

typedef struct {
	double x;
	double y;
} Vector2;

#define pi (double)3.1415926535897932384626
#define deg2rad(d) (d * pi / 180.0)
#define rad2deg(r) (r / (pi / 180.0))
#define is_pressed(button) controller_get_digital(E_CONTROLLER_MASTER, button)

int clamp360(int a) {
	return (a + 360) % 360;
}

double Vector2Length(Vector2 v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

Vector2 Vector2Rotate(Vector2 v, double r) {
	return (Vector2){
		cos(r) * v.x - sin(r) * v.y,
		sin(r) * v.x + cos(r) * v.y
	};
}

Vector2 Vector2Add(Vector2 v1, Vector2 v2) {
	return (Vector2){
		v1.x + v2.x,
		v1.y + v2.y
	};
}

Vector2 Vector2Subtract(Vector2 v1, Vector2 v2) {
	return (Vector2){
		v1.x - v2.x,
		v1.y - v2.y
	};
}

Vector2 Vector2Scale(Vector2 v, double s) {
	return (Vector2){
		v.x * s,
		v.y * s
	};
}

#endif
