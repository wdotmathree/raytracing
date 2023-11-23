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
	__m128 a1 = _mm_load_ps((float *)a);
	__m128 b1 = _mm_load_ps((float *)b);
	a1 = _mm_add_ps(a1, b1);
	_mm_store_ps((float *)out, a1);
}

void vec3_sub(vec3 *out, const vec3 *a, const vec3 *b) {
	__m128 a1 = _mm_load_ps((float *)a);
	__m128 b1 = _mm_load_ps((float *)b);
	a1 = _mm_sub_ps(a1, b1);
	_mm_store_ps((float *)out, a1);
}

void vec3_scale(vec3 *out, const vec3 *a, const float s) {
	__m128 a1 = _mm_load_ps((float *)a);
	a1 = _mm_mul_ss(a1, _mm_set_ss(s));
	_mm_store_ps((float *)out, a1);
}

void vec3_negate(vec3 *out, const vec3 *a) {
	__m128 a1 = _mm_load_ps((float *)a);
	a1 = _mm_sub_ps(_mm_setzero_ps(), a1);
	_mm_store_ps((float *)out, a1);
}

void vec3_tovec4(vec4 *out, const vec3 *a) {
	__m128 a1 = _mm_load_ps((float *)a);
	a1 = _mm_insert_ps(a1, _mm_set_ss(1), 0x30);
	_mm_store_ps((float *)out, a1);
}

void vec3_lerp_x(vec3 *out, const vec3 *a, const vec3 *b, const float x) {
	out->x = x;
	out->y = a->y + (b->y - a->y) * (x - a->x) / (b->x - a->x);
	out->z = a->z + (b->z - a->z) * (x - a->x) / (b->x - a->x);
}

void vec3_lerp_y(vec3 *out, const vec3 *a, const vec3 *b, const float y) {
	out->x = a->x + (b->x - a->x) * (y - a->y) / (b->y - a->y);
	out->y = y;
	out->z = a->z + (b->z - a->z) * (y - a->y) / (b->y - a->y);
}

void vec3_lerp_z(vec3 *out, const vec3 *a, const vec3 *b, const float z) {
	out->x = a->x + (b->x - a->x) * (z - a->z) / (b->z - a->z);
	out->y = a->y + (b->y - a->y) * (z - a->z) / (b->z - a->z);
	out->z = z;
}
