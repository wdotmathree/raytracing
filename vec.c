#include "vec.h"

float vec3_dot(const vec3 *a, const vec3 *b) { return a->x * b->x + a->y * b->y + a->z * b->z; }

void vec3_cross(vec3 *out, const vec3 *a, const vec3 *b) {
	out->x = a->y * b->z - a->z * b->y;
	out->y = a->z * b->x - a->x * b->z;
	out->z = a->x * b->y - a->y * b->x;
}

float vec3_length(const vec3 *a) { return sqrtf(vec3_dot(a, a)); }

void vec3_normalize(vec3 *out, const vec3 *a) {
	float len = vec3_length(a);
	if (__builtin_expect(len == 0, false)) {
		out->x = 0;
		out->y = 0;
		out->z = 0;
		return;
	}
	out->x = a->x / len;
	out->y = a->y / len;
	out->z = a->z / len;
}

void vec3_add(vec3 *out, const vec3 *a, const vec3 *b) {
	out->x = a->x + b->x;
	out->y = a->y + b->y;
	out->z = a->z + b->z;
}

void vec3_sub(vec3 *out, const vec3 *a, const vec3 *b) {
	out->x = a->x - b->x;
	out->y = a->y - b->y;
	out->z = a->z - b->z;
}

void vec3_scale(vec3 *out, const vec3 *a, const float s) {
	out->x = a->x * s;
	out->y = a->y * s;
	out->z = a->z * s;
}

void vec3_negate(vec3 *out, const vec3 *a) {
	out->x = -a->x;
	out->y = -a->y;
	out->z = -a->z;
}

void vec3_tovec4(vec4 *out, const vec3 *a) {
	out->x = a->x;
	out->y = a->y;
	out->z = a->z;
	out->w = 1;
}
