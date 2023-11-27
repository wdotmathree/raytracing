#pragma once
#ifndef MAT_H
#define MAT_H

#include "consts.hpp"
#include "vec.hpp"

// Get an identity matrix

special void mat2_identity(mat2 m);
special void mat3_identity(mat3 m);
special void mat4_identity(mat4 m);

// Transpose a matrix

special void mat2_transpose(mat2 m);
special void mat3_transpose(mat3 m);
special void mat4_transpose(mat4 m);

// Get the determinant of a matrix

special float mat2_det(const mat2 m);
special float mat3_det(const mat3 m);
special float mat4_det(const mat4 m);

// Invert a matrix

special float mat2_invert(mat2 inv, const mat2 m);
special float mat3_invert(mat3 inv, const mat3 m);
special float mat4_invert(mat4 inv, const mat4 m);

// Multiply two matrices

special void mat2_mul(mat2 out, const mat2 a, const mat2 b);
special void mat3_mul(mat3 out, const mat3 a, const mat3 b);
special void mat4_mul(mat4 out, const mat4 a, const mat4 b);

// Multiply matrix with a vector

special void mat2_mul_vec(float2 *out, const mat2 m, const float2 *v);
special void mat3_mul_vec(float3 *out, const mat3 m, const float3 *v);
special void mat4_mul_vec(float4 *out, const mat4 m, const float4 *v);

#endif
