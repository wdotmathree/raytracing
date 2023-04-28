#pragma once
#ifndef MAT_H
#define MAT_H

#include "vec.h"
#include <stdbool.h>
#include <string.h>
#include <x86intrin.h>

// Get an identity matrix

void mat2x2_identity(float m[2][2]);
void mat3x3_identity(float m[3][3]);
void mat4x4_identity(float m[4][4]);

// Transpose a matrix

void mat2x2_transpose(float m[2][2]);
void mat3x3_transpose(float m[3][3]);
void mat4x4_transpose(float m[4][4]);

// Get the determinant of a matrix

float mat2x2_det(const float m[2][2]);
float mat3x3_det(const float m[3][3]);
float mat4x4_det(const float m[4][4]);

// Invert a matrix

float mat2x2_invert(float inv[2][2], const float m[2][2]);
float mat3x3_invert(float inv[3][3], const float m[3][3]);
float mat4x4_invert(float inv[4][4], const float m[4][4]);

// Multiply two matrices

void mat2x2_mul(float out[2][2], const float a[2][2], const float b[2][2]);
void mat3x3_mul(float out[3][3], const float a[3][3], const float b[3][3]);
void mat4x4_mul(float out[4][4], const float a[4][4], const float b[4][4]);

// Multiply matrix with a vector

void mat3x3_mul_vec3(vec3 *out, const float m[3][3], const vec3 *v);
void mat4x4_mul_vec4(vec4 *out, const float m[4][4], const vec4 *v);

#endif
