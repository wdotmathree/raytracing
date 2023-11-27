#include "consts.hpp"
#include "linalg.hpp"
#include "rt.hpp"

mat4 *g_proj = NULL;
uint32_t *g_buf = NULL;
float3 *g_light = NULL;

__device__ ray camera_ray(int x, int y, mat4 *proj) {
	float3 dir = {0, 0, 1};
	dir.x = (x / (WIDTH / 2.0) - 1) / (*proj)[0][0];
	dir.y = (y / (HEIGHT / 2.0) - 1) / (*proj)[1][1];

	ray r = {{0, 0, 0}, {dir.x, dir.y, dir.z}};
	return r;
}

__device__ float3 raycast(ray r, int numTriangles, float3 *verticies, uint3 *triangles, float3 *light, int depth) {
	float3 res = {0, 0, 0};
	// Search for the closest
	int closest = -1;
	float closestDist = INFINITY;
	float3 closestPoint;
	float3 normal;
	for (int i = 0; i < numTriangles; i++) {
		float3 *a = verticies + triangles[i].x;
		float3 *b = verticies + triangles[i].y;
		float3 *c = verticies + triangles[i].z;

		plane p;
		float3 ab, ac;
		vec3_sub(&ab, b, a);
		vec3_sub(&ac, c, a);
		vec3_cross((float3 *)&p, &ab, &ac);
		vec3_normalize((float3 *)&p, (float3 *)&p);
		p.w = -vec3_dot((float3 *)&p, a);

		float3 point;
		if (intersect_plane_line(&point, &p, &r)) {
			if (point.z > FAR || point.z < NEAR)
				continue;
			if (point_in_triangle(&point, a, b, c)) {
				float dist = sqrtf(point.x * point.x + point.y * point.y + point.z * point.z);
				if (dist < closestDist) {
					closest = i;
					closestDist = dist;
					closestPoint = point;
					normal = *(float3 *)&p;
				}
			}
		}
	}
	if (depth == 0) {
		if (closest != -1) {
			float3 lightDir;
			vec3_sub(&lightDir, light, &closestPoint);
			vec3_normalize(&lightDir, &lightDir);
			float lightDot = fabsf(vec3_dot(&lightDir, &normal));
			if (lightDot < 0)
				lightDot = 0;
			res.x = lightDot;
			res.y = lightDot;
			res.z = lightDot;
		} else {
			res.x = 0;
			res.y = 0;
			res.z = 0;
		}
	} else {
		// Send out rays in many directions
	}
	return res;
}

__global__ void raytrace(int numTriangles, uint32_t *buf, float3 *verticies, uint3 *triangles, mat4 *proj, float3 *light) {
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;

	ray r = camera_ray(x, HEIGHT - 1 - y, proj);

	float3 res = raycast(r, numTriangles, verticies, triangles, light, 0);
	uchar3 color = {(uint8_t)(res.x * 255), (uint8_t)(res.y * 255), (uint8_t)(res.z * 255)};

	buf[y * WIDTH + x] = (color.x << 16) | (color.y << 8) | color.z;

	return;
}

void raytrace_render(uint32_t *out, float3 *verts, uint3 *tris, int numTris, mat4 arg_proj, float3 *light) {
	if (g_buf == NULL) {
		cudaMalloc(&g_buf, sizeof(uint32_t) * WIDTH * HEIGHT);
		cudaMalloc(&g_proj, sizeof(mat4));
		cudaMemcpy(*g_proj, arg_proj, sizeof(mat4), cudaMemcpyHostToDevice);
		cudaMalloc(&g_light, sizeof(float3));
	}

	float3 *verticies;
	uint3 *triangles;

	cudaMalloc(&verticies, sizeof(float3) * numTris * 3);
	cudaMalloc(&triangles, sizeof(uint3) * numTris);
	cudaMemcpy(verticies, verts, sizeof(float3) * numTris * 3, cudaMemcpyHostToDevice);
	cudaMemcpy(triangles, tris, sizeof(uint3) * numTris, cudaMemcpyHostToDevice);
	cudaMemcpy(g_light, light, sizeof(float3), cudaMemcpyHostToDevice);

	dim3 block(8, 8);
	dim3 grid(WIDTH / block.x, HEIGHT / block.y);

	raytrace<<<grid, block>>>(numTris, g_buf, verticies, triangles, g_proj, g_light);

	cudaError_t err = cudaDeviceSynchronize();
	if (err != cudaSuccess) {
		fprintf(stderr, "Failed to synchronize: %s\n", cudaGetErrorString(err));
		exit(1);
	}
	cudaMemcpy(out, g_buf, sizeof(uint32_t) * WIDTH * HEIGHT, cudaMemcpyDeviceToHost);

	cudaFree(verticies);
	cudaFree(triangles);
}
