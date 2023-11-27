#include "vec.hpp"

special float vec3_dot(const float3 *a, const float3 *b) {
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

special void vec3_cross(float3 *out, const float3 *a, const float3 *b) {
	out->x = a->y * b->z - a->z * b->y;
	out->y = a->z * b->x - a->x * b->z;
	out->z = a->x * b->y - a->y * b->x;
}

special float vec3_length(const float3 *a) {
	return sqrtf(vec3_dot(a, a));
}

special void vec3_normalize(float3 *out, const float3 *a) {
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

special void vec3_add(float3 *out, const float3 *a, const float3 *b) {
	out->x = a->x + b->x;
	out->y = a->y + b->y;
	out->z = a->z + b->z;
}

special void vec3_sub(float3 *out, const float3 *a, const float3 *b) {
	out->x = a->x - b->x;
	out->y = a->y - b->y;
	out->z = a->z - b->z;
}

special void vec3_scale(float3 *out, const float3 *a, const float s) {
	out->x = a->x * s;
	out->y = a->y * s;
	out->z = a->z * s;
}

special void vec3_negate(float3 *out, const float3 *a) {
	out->x = -a->x;
	out->y = -a->y;
	out->z = -a->z;
}

special void vec3_tovec4(float4 *out, const float3 *a) {
	out->x = a->x;
	out->y = a->y;
	out->z = a->z;
	out->w = 1;
}

special void vec3_lerp_x(float3 *out, const float3 *a, const float3 *b, const float x) {
	out->x = x;
	out->y = a->y + (b->y - a->y) * (x - a->x) / (b->x - a->x);
	out->z = a->z + (b->z - a->z) * (x - a->x) / (b->x - a->x);
}

special void vec3_lerp_y(float3 *out, const float3 *a, const float3 *b, const float y) {
	out->x = a->x + (b->x - a->x) * (y - a->y) / (b->y - a->y);
	out->y = y;
	out->z = a->z + (b->z - a->z) * (y - a->y) / (b->y - a->y);
}

special void vec3_lerp_z(float3 *out, const float3 *a, const float3 *b, const float z) {
	out->x = a->x + (b->x - a->x) * (z - a->z) / (b->z - a->z);
	out->y = a->y + (b->y - a->y) * (z - a->z) / (b->z - a->z);
	out->z = z;
}
