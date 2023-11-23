#include "linalg.h"

bool intersect_plane_plane(ray *out, const plane *a, const plane *b) {
	vec3 rhs;
	vec3_cross(&out->direction, (vec3 *)&a, (vec3 *)&b);
	mat3 t1, t2;
	t1[0][0] = a->x;
	t1[0][1] = a->y;
	t1[0][2] = a->z;
	t1[1][0] = b->x;
	t1[1][1] = b->y;
	t1[1][2] = b->z;
	t1[2][0] = out->direction.x;
	t1[2][1] = out->direction.y;
	t1[2][2] = out->direction.z;
	if (__builtin_expect(mat3_invert(t2, t1) == 0, false))
		return false;
	rhs.x = a->w;
	rhs.y = b->w;
	rhs.z = 0;
	mat3_mul_vec(&out->origin, t2, &rhs);
	return true;
}

bool intersect_plane_line(vec3 *out, const plane *a, const ray *b) {
	float t = -(a->x * b->origin.x + a->y * b->origin.y + a->z * b->origin.z + a->w);
	float t1 = a->x * b->direction.x + a->y * b->direction.y + a->x * b->direction.z;
	if (__builtin_expect(t1 == 0, false))
		return false;
	t /= t1;
	out->x = b->origin.x + t * b->direction.x;
	out->y = b->origin.y + t * b->direction.y;
	out->z = b->origin.z + t * b->direction.z;
	return true;
}

bool intersect_line_line(vec3 *out, const ray *a, const ray *b) {
	mat2 t1;
	t1[0][0] = a->direction.x;
	t1[0][1] = -b->direction.x;
	t1[1][0] = a->direction.y;
	t1[1][1] = -b->direction.y;
	vec2 t2 = {b->origin.x - a->origin.x, b->origin.y - a->origin.y}, t3;
	if (__builtin_expect(mat2_invert(t1, t1) == 0, false))
		return false;
	mat2_mul_vec(&t3, t1, &t2);
	if (a->origin.z + t3.x * a->direction.z != b->origin.z + t3.y * b->direction.z)
		return false;
	out->x = a->origin.x + t3.x * a->direction.x;
	out->y = a->origin.y + t3.x * a->direction.y;
	out->z = a->origin.z + t3.x * a->direction.z;
	return true;
}

bool point_in_triangle(const vec3 *point, const vec3 *a, const vec3 *b, const vec3 *c) {
	vec3 pa, pb, pc;
	vec3_sub(&pa, point, a);
	vec3_sub(&pb, point, b);
	vec3_sub(&pc, point, c);
	float a1 = vec3_dot(&pa, &pb);
	float a2 = vec3_dot(&pb, &pc);
	float a3 = vec3_dot(&pc, &pa);
	return (a1 >= 0 && a2 >= 0 && a3 >= 0) || (a1 <= 0 && a2 <= 0 && a3 <= 0);
}

int intersect_plane_triangle(vec3 *out1, vec3 *out2, const plane *a, const vec3 *b, const vec3 *c, const vec3 *d) {
	plane tri;
	vec3_cross((vec3 *)&tri, b, c);
	tri.w = -(tri.x * b->x + tri.y * b->y + tri.z * b->z);
	ray l1, l2;
	intersect_plane_plane(&l1, a, &tri);
	vec3 p1;
	int ans = 0;
	vec3_sub(&l2.direction, c, b);
	l2.origin = *b;
	intersect_line_line(&p1, &l1, &l2);
	if (point_in_triangle(&p1, b, c, d)) {
		*out1 = p1;
		ans++;
	}
	vec3_sub(&l2.direction, d, c);
	l2.origin = *c;
	intersect_line_line(&p1, &l1, &l2);
	if (point_in_triangle(&p1, b, c, d)) {
		if (ans == 0)
			*out1 = p1;
		else if (p1.x == out1->x && p1.y == out1->y && p1.z == out1->z)
			ans--;
		else
			*out2 = p1;
		ans++;
	}
	vec3_sub(&l2.direction, b, d);
	l2.origin = *d;
	intersect_line_line(&p1, &l1, &l2);
	if (point_in_triangle(&p1, b, c, d)) {
		if (ans == 0)
			*out1 = p1;
		else if (p1.x == out1->x && p1.y == out1->y && p1.z == out1->z)
			ans--;
		else
			*out2 = p1;
		ans++;
	}
	return ans;
}
