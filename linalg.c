#include "linalg.h"

bool intersect_plane_plane(ray *out, const plane *a, const plane *b) {
	// Cross product of normals is direction
	vec3_cross(&out->direction, (const vec3 *)a, (const vec3 *)b);
	// Solve for origin
	// Setup system of equations
	mat3 t1 = {{a->x, a->y, a->z}, {b->x, b->y, b->z}, {out->direction.x, out->direction.y, out->direction.z}};
	mat3 t2;
	// Invert matrix
	// If determinant is 0, planes are parallel
	if (__builtin_expect(mat3_invert(t2, t1) == 0, false))
		return false;
	// Results are in last column (ax + by + cz = d)
	vec3 t3;
	t3.x = -a->w;
	t3.y = -b->w;
	t3.z = 0;
	mat3_mul_vec(&out->origin, t2, &t3);
	return true;
}

bool intersect_plane_line(vec3 *out, const plane *p, const ray *l) {
	// Put line equation into plane equation and solve for t
	// A1(A2t + A3) + B1(B2t + B3) + C1(C2t + C3) + D1 = 0
	// (A1A2 + B1B2 + C1C2)t + (A1A3 + B1B3 + C1C3 + D1) = 0
	// t = -(A1A3 + B1B3 + C1C3 + D1) / (A1A2 + B1B2 + C1C2)
	float top = -(p->x * l->origin.x + p->y * l->origin.y + p->z * l->origin.z + p->w);
	float bot = p->x * l->direction.x + p->y * l->direction.y + p->z * l->direction.z;
	if (__builtin_expect(bot == 0, false))
		return false;
	float t = top / bot;
	// Solve for point using t
	out->x = l->origin.x + t * l->direction.x;
	out->y = l->origin.y + t * l->direction.y;
	out->z = l->origin.z + t * l->direction.z;
	return true;
}

bool intersect_line_line(vec3 *out, const ray *a, const ray *b) {
	// System of equations
	// x = A1t + A2
	// y = B1t + B2
	// x = C1u + C2
	// y = D1u + D2
	// A1t + A2 = C1u + C2
	// B1t + B2 = D1u + D2
	// A1t - C1u = C2 - A2
	// B1t - D1u = D2 - B2
	// Setup matrix
	mat2 lhs = {
		{a->direction.x, -b->direction.x},
		{a->direction.y, -b->direction.y},
	};
	// Setup vector
	vec2 rhs = {
		b->origin.x - a->origin.x,
		b->origin.y - a->origin.y,
	};
	// Invert matrix
	mat2 lhs_inv;
	if (__builtin_expect(mat2_invert(lhs_inv, lhs) == 0, false)) {
		lhs[1][0] = a->direction.z;
		lhs[1][1] = -b->direction.z;
		rhs.y = b->origin.z - a->origin.z;
		if (__builtin_expect(mat2_invert(lhs_inv, lhs) == 0, false)) {
			lhs[0][0] = a->direction.y;
			lhs[0][1] = -b->direction.y;
			rhs.x = b->origin.y - a->origin.y;
			if (__builtin_expect(mat2_invert(lhs_inv, lhs) == 0, false))
				return false;
		}
	}
	// Multiply
	vec2 vars;
	mat2_mul_vec(&vars, lhs_inv, &rhs);
	// Calculate result (vars.x = t, vars.y = u)
	out->x = a->origin.x + vars.x * a->direction.x;
	out->y = a->origin.y + vars.x * a->direction.y;
	out->z = a->origin.z + vars.x * a->direction.z;
	return true;
}

bool point_in_triangle(const vec3 *point, const vec3 *a, const vec3 *b, const vec3 *c) {
	// Get vectors from point to each vertex
	vec3 pa, pb, pc;
	vec3_sub(&pa, a, point);
	vec3_sub(&pb, b, point);
	vec3_sub(&pc, c, point);
	// Check if the point is one of the vertices
	if (__builtin_expect(fabsf(pa.x) + fabsf(pa.y) + fabsf(pa.z) < 3 * LINALG_EPSILON, false))
		return true;
	if (__builtin_expect(fabsf(pb.x) + fabsf(pa.y) + fabsf(pa.z) < 3 * LINALG_EPSILON, false))
		return true;
	if (__builtin_expect(fabsf(pc.x) + fabsf(pa.y) + fabsf(pa.z) < 3 * LINALG_EPSILON, false))
		return true;
	vec3_normalize(&pa, &pa);
	vec3_normalize(&pb, &pb);
	vec3_normalize(&pc, &pc);
	// Check if point is on the edge of the triangle
	// Check if point is on edge AB
	// P->A and P->B opposite each other (dot product = -1)
	if (__builtin_expect(vec3_dot(&pa, &pb) < -1 + LINALG_EPSILON, false))
		return true;
	// Check if point is on edge BC
	if (__builtin_expect(vec3_dot(&pb, &pc) < -1 + LINALG_EPSILON, false))
		return true;
	// Check if point is on edge CA
	if (__builtin_expect(vec3_dot(&pc, &pa) < -1 + LINALG_EPSILON, false))
		return true;
	// Check if point is coplanar with triangle
	// Get normal vector for triangle plane
	vec3 ab, bc, normal;
	vec3_sub(&ab, b, a);
	vec3_sub(&bc, c, b);
	vec3_cross(&normal, &ab, &bc);
	vec3_normalize(&normal, &normal);
	// If point is coplanar, dot product of normal and point vectors will be 0
	// since the normal of the plane is perpendicular to the vector of a triangle edge
	if (__builtin_expect(fabsf(vec3_dot(&normal, &pa)) > LINALG_EPSILON, false))
		return false;
	// Get cross products (will be normal to the plane, if all are pointing the same direction then the point is inside)
	vec3 c1, c2, c3;
	vec3_cross(&c1, &pa, &pb);
	vec3_cross(&c2, &pb, &pc);
	vec3_cross(&c3, &pc, &pa);
	vec3_normalize(&c1, &c1);
	vec3_normalize(&c2, &c2);
	vec3_normalize(&c3, &c3);
	if (fabsf(c1.x - c2.x) > LINALG_EPSILON)
		return false;
	if (fabsf(c1.x - c3.x) > LINALG_EPSILON)
		return false;
	if (fabsf(c1.y - c2.y) > LINALG_EPSILON)
		return false;
	if (fabsf(c1.y - c3.y) > LINALG_EPSILON)
		return false;
	if (fabsf(c1.z - c2.z) > LINALG_EPSILON)
		return false;
	return fabsf(c1.z - c3.z) < LINALG_EPSILON;
}

int intersect_plane_triangle(vec3 *out1, vec3 *out2, const plane *p, const vec3 *a, const vec3 *b, const vec3 *c) {
	// Get the equation for the plane of the triangle
	plane tri;
	vec3 ab, ac;
	vec3_sub(&ab, b, a);
	vec3_sub(&ac, c, a);
	vec3_cross((vec3 *)&tri, &ab, &ac);
	tri.w = -(tri.x * a->x + tri.y * a->y + tri.z * a->z);
	// Get the line of intersection between the two planes
	ray l_int, l_edge;
	if (!intersect_plane_plane(&l_int, p, &tri))
		return 0;
	// Test each edge of the triangle
	vec3 p_int;
	int num = 0; // Number of intersections
	// Edge AB
	vec3_sub(&l_edge.direction, b, a);
	l_edge.origin = *a;
	if (intersect_line_line(&p_int, &l_int, &l_edge)) {
		if (point_in_triangle(&p_int, a, b, c)) {
			*out1 = p_int;
			num++;
		}
	}
	// Edge BC
	vec3_sub(&l_edge.direction, c, b);
	l_edge.origin = *b;
	if (intersect_line_line(&p_int, &l_int, &l_edge)) {
		if (point_in_triangle(&p_int, a, b, c)) {
			if (num == 0)
				*out1 = p_int;
			else if (p_int.x == out1->x && p_int.y == out1->y && p_int.z == out1->z) // Prevent duplicate points
				num--;
			else
				*out2 = p_int;
			num++;
		}
	}
	// Edge CA
	vec3_sub(&l_edge.direction, a, c);
	l_edge.origin = *c;
	if (intersect_line_line(&p_int, &l_int, &l_edge)) {
		if (point_in_triangle(&p_int, a, b, c)) {
			if (num == 0)
				*out1 = p_int;
			else if (p_int.x == out1->x && p_int.y == out1->y && p_int.z == out1->z) // Prevent duplicate points
				num--;
			else
				*out2 = p_int;
			num++;
		}
	}
	if (num > 2) {
		// This should never happen
		// If it does, something is wrong
		volatile int *a = 0;
		*a = 0;
		return -1;
	}
	return num;
}
