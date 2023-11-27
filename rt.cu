#include "consts.hpp"
#include "linalg.hpp"
#include "rt.hpp"

mat4 *g_proj = NULL;

uint32_t *g_buf = NULL;
float3 *g_verticies = NULL;
uint3 *g_triangles = NULL;

__device__ ray camera_ray(int x, int y, mat4 *proj) {
	float3 dir = {0, 0, 1};
	dir.x = (x / (WIDTH / 2) - 1) / (*proj)[0][0];
	dir.y = (y / (HEIGHT / 2) - 1) / (*proj)[1][1];
	vec3_normalize(&dir, &dir);

	ray r = {{0, 0, 0}, {dir.x, dir.y, dir.z}};
	return r;
}

__device__ float3 raycast(ray r, int depth) {
	/// TODO: Implement
	float3 res = {0, 0, 0};
	if (depth == 0) {
		// Go straight to the light
	} else {
		// Send out rays in many directions
	}
	/// TODO: Remove dummy code when done implementing
	res = {0.5, 0.5, 0.5};
	return res;
}

__global__ void raytrace(int numTriangles, uint32_t *buf, float3 *verticies, uint3 *triangles, mat4 *proj) {
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;

	ray r = camera_ray(x, y, proj);

	float3 res = raycast(r, 0);
	uchar3 color = {(uint8_t)(res.x * 255), (uint8_t)(res.y * 255), (uint8_t)(res.z * 255)};

	buf[y * WIDTH + x] = (color.x << 16) | (color.y << 8) | color.z;

	return;
}

void raytrace_render(uint32_t *out, float3 *verts, uint3 *tris, mat4 arg_proj, int numTris) {
	if (g_buf == NULL) {
		cudaMalloc(&g_buf, sizeof(uint32_t) * WIDTH * HEIGHT);
		cudaMalloc(&g_proj, sizeof(mat4));
		cudaMemcpy(g_proj, &arg_proj, sizeof(mat4), cudaMemcpyHostToDevice);
	}

	cudaMalloc(&g_verticies, sizeof(float3) * numTris * 3);
	cudaMalloc(&g_triangles, sizeof(uint3) * numTris);
	cudaMemcpy(g_verticies, verts, sizeof(float3) * numTris * 3, cudaMemcpyHostToDevice);
	cudaMemcpy(g_triangles, tris, sizeof(uint3) * numTris, cudaMemcpyHostToDevice);

	dim3 block(16, 16);
	dim3 grid(WIDTH / block.x, HEIGHT / block.y);

	raytrace<<<grid, block>>>(numTris, g_buf, g_verticies, g_triangles, g_proj);

	cudaError_t err = cudaDeviceSynchronize();
	if (err != cudaSuccess) {
		fprintf(stderr, "Failed to synchronize: %s\n", cudaGetErrorString(err));
		exit(1);
	}
	cudaMemcpy(out, g_buf, sizeof(uint32_t) * WIDTH * HEIGHT, cudaMemcpyDeviceToHost);

	cudaFree(g_verticies);
	cudaFree(g_triangles);
}
