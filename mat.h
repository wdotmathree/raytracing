#pragma once
#ifndef MAT_H
#define MAT_H

#include "consts.h"
#include "vec.h"
#include <stdbool.h>
#include <string.h>
#include <x86intrin.h>

// Get an identity matrix

void mat2x2_identity(mat2 m);
void mat3x3_identity(mat3 m);
void mat4x4_identity(mat4 m);

// Transpose a matrix

void mat2x2_transpose(mat2 m);
void mat3x3_transpose(mat3 m);
void mat4x4_transpose(mat4 m);

// Get the determinant of a matrix

float mat2x2_det(const mat2 m);
float mat3x3_det(const mat3 m);
float mat4x4_det(const mat4 m);

// Invert a matrix

float mat2x2_invert(mat2 inv, const mat2 m);
float mat3x3_invert(mat3 inv, const mat3 m);
float mat4x4_invert(mat4 inv, const mat4 m);

// Multiply two matrices

void mat2x2_mul(mat2 out, const mat2 a, const mat2 b);
void mat3x3_mul(mat3 out, const mat3 a, const mat3 b);
void mat4x4_mul(mat4 out, const mat4 a, const mat4 b);

// Multiply matrix with a vector

void mat3x3_mul_vec3(vec3 *out, const mat3 m, const vec3 *v);
void mat4x4_mul_vec4(vec4 *out, const mat4 m, const vec4 *v);

#endif
