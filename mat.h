#pragma once
#ifndef MAT_H
#define MAT_H

#include "consts.h"
#include "vec.h"

// Get an identity matrix

void mat2_identity(mat2 m);
void mat3_identity(mat3 m);
void mat4_identity(mat4 m);

// Transpose a matrix

void mat2_transpose(mat2 m);
void mat3_transpose(mat3 m);
void mat4_transpose(mat4 m);

// Get the determinant of a matrix

float mat2_det(const mat2 m);
float mat3_det(const mat3 m);
float mat4_det(const mat4 m);

// Invert a matrix

float mat2_invert(mat2 inv, const mat2 m);
float mat3_invert(mat3 inv, const mat3 m);
float mat4_invert(mat4 inv, const mat4 m);

// Multiply two matrices

void mat2_mul(mat2 out, const mat2 a, const mat2 b);
void mat3_mul(mat3 out, const mat3 a, const mat3 b);
void mat4_mul(mat4 out, const mat4 a, const mat4 b);

// Multiply matrix with a vector

void mat2_mul_vec(vec2 *out, const mat2 m, const vec2 *v);
void mat3_mul_vec(vec3 *out, const mat3 m, const vec3 *v);
void mat4_mul_vec(vec4 *out, const mat4 m, const vec4 *v);

#endif
