#pragma once
#ifndef VEC_H
#define VEC_H

#include "consts.hpp"

special float vec3_dot(const float3 *a, const float3 *b);

special void vec3_cross(float3 *out, const float3 *a, const float3 *b);

special float vec3_length(const float3 *a);

special void vec3_normalize(float3 *out, const float3 *a);

special void vec3_add(float3 *out, const float3 *a, const float3 *b);

special void vec3_sub(float3 *out, const float3 *a, const float3 *b);

special void vec3_scale(float3 *out, const float3 *a, float s);

special void vec3_negate(float3 *out, const float3 *a);

special void vec3_tovec4(float4 *out, const float3 *a);

#endif
