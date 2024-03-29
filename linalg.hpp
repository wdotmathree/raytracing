#include "consts.hpp"
#include "mat.hpp"
#include "vec.hpp"

typedef struct ray {
	float3 origin;
	float3 direction;
} ray;

typedef float4 plane;

/**
 * @brief Get the line of intersection between two planes
 *
 * @param out The line of intersection
 * @param a The first plane
 * @param b The second plane
 * @return false if the planes are parallel, true otherwise
 */
special bool intersect_plane_plane(ray *, const plane *, const plane *);

/**
 * @brief Get the point of intersection between a plane and a line
 *
 * @param out The point of intersection
 * @param a The plane
 * @param b The line
 * @return false if the plane and line are parallel, true otherwise
 */
special bool intersect_plane_line(float3 *, const plane *, const ray *);

/**
 * @brief Get the point of intersection between two lines
 *
 * @param out The point of intersection
 * @param a The first line
 * @param b The second line
 * @return false if the lines are parallel, true otherwise
 */
special bool intersect_line_line(float3 *, const ray *, const ray *);

/**
 * @brief Check if a point is in a triangle
 * @remark The point is assumed to be coplanar with the triangle
 *
 * @param point The point to check against
 * @param a p1 of the triangle
 * @param b p2 of the triangle
 * @param c p3 of the triangle
 * @return Whether the point is in the triangle
 */
special bool point_in_triangle(const float3 *, const float3 *, const float3 *, const float3 *);

/**
 * @brief Get the point(s) of intersection between a plane and a triangle
 *
 * @param out1 The first point of intersection (not modified if doesn't exist)
 * @param out2 The second point of intersection (not modified if doesn't exist)
 * @param a The plane
 * @param b p1 of the triangle
 * @param c p2 of the triangle
 * @param d p3 of the triangle
 * @return Number of intersections
 */
special int intersect_plane_triangle(float3 *, float3 *, const plane *, const float3 *, const float3 *, const float3 *);
