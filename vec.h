#pragma once
#ifndef VEC_H
#define VEC_H

#include <math.h>
#include <stdbool.h>

typedef struct vec2 {
	float x, y;
} vec2;

typedef struct vec3 {
	float x, y, z;
} vec3;

typedef struct vec4 {
	float x, y, z, w;
} vec4;

float vec3_dot(const vec3 *a, const vec3 *b);

void vec3_cross(vec3 *out, const vec3 *a, const vec3 *b);

float vec3_length(const vec3 *a);

void vec3_normalize(vec3 *out, const vec3 *a);

void vec3_add(vec3 *out, const vec3 *a, const vec3 *b);

void vec3_sub(vec3 *out, const vec3 *a, const vec3 *b);

void vec3_scale(vec3 *out, const vec3 *a, float s);

void vec3_negate(vec3 *out, const vec3 *a);

#endif
