#include "consts.hpp"
#include "linalg.hpp"
#include "mat.hpp"
#include "rt.hpp"
#include "transform.hpp"
#include "vec.hpp"

#include <stdio.h>
#include <sys/mman.h>

#include <vector>

const float3 g_light = {0, 0, -10};
float3 camera = {2, 1, -5};
float2 look = {0, 0};

int numVerticies;
int numTriangles;
float3 *verticies;
float3 *transVerts;
uint3 *triangles;

uint32_t *frame_buffer;

void sort(uint3 *arr, int n) {
	if (n < 2)
		return;
	float pivot = verticies[arr[n / 2].x].z + verticies[arr[n / 2].y].z + verticies[arr[n / 2].z].z;
	int l = 0;
	int r = n - 1;
	while (l <= r) {
		if (verticies[arr[l].x].z + verticies[arr[l].y].z + verticies[arr[l].z].z < pivot) {
			l++;
			continue;
		}
		if (verticies[arr[r].x].z + verticies[arr[r].y].z + verticies[arr[r].z].z > pivot) {
			r--;
			continue;
		}
		uint3 tmp = arr[l];
		arr[l] = arr[r];
		arr[r] = tmp;
		l++;
		r--;
	}
	sort(arr, r + 1);
	sort(arr + l, n - l);
}

int parseFile() {
	// Read in the data
	FILE *fp = fopen("thing.obj", "r");
	if (!fp) {
		fprintf(stderr, "Failed to open file\n");
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	char *map = (char *)mmap(NULL, ftell(fp) + 1, PROT_READ, MAP_PRIVATE, fp->_fileno, 0);
	char *line = map;
	while (line) {
		if (line[0] == 'v' && line[1] == ' ') {
			numVerticies++;
		} else if (line[0] == 'f' && line[1] == ' ') {
			numTriangles++;
		}
		line = strchr(line, '\n');
		if (line)
			line++;
	}
	verticies = (float3 *)malloc(numVerticies * sizeof(float3));
	triangles = (uint3 *)malloc(numTriangles * sizeof(uint3));
	transVerts = (float3 *)malloc(numVerticies * sizeof(float3));
	line = map;
	int v = 0;
	int t = 0;
	// float minx = 0;
	// float miny = 0;
	// float minz = 0;
	// float maxx = 0;
	// float maxy = 0;
	// float maxz = 0;
	while (line) {
		if (line[0] == 'v' && line[1] == ' ') {
			float3 *vert = verticies + v;
			sscanf(line, "v %f %f %f", &vert->x, &vert->y, &vert->z);
			// minx = fminf(minx, vert->x);
			// miny = fminf(miny, vert->y);
			// minz = fminf(minz, vert->z);
			// maxx = fmaxf(maxx, vert->x);
			// maxy = fmaxf(maxy, vert->y);
			// maxz = fmaxf(maxz, vert->z);
			v++;
		} else if (line[0] == 'f' && line[1] == ' ') {
			uint3 *tri = triangles + t;
			sscanf(line, "f %d %d %d", &tri->x, &tri->y, &tri->z);
			tri->x--;
			tri->y--;
			tri->z--;
			t++;
		}
		line = strchr(line, '\n');
		if (line)
			line++;
	}
	// float scale = fmaxf(fmaxf(maxx - minx, maxy - miny), maxz - minz) / 2;
	// for (int i = 0; i < numVerticies; i++) {
	// 	float3 *vert = verticies + i;
	// 	vert->x -= (maxx + minx) / 2;
	// 	vert->y -= (maxy + miny) / 2;
	// 	vert->z -= (maxz + minz) / 2;
	// 	vert->x /= scale;
	// 	vert->y /= scale;
	// 	vert->z /= scale;
	// }
	munmap(map, ftell(fp) + 1);
	fclose(fp);

	// Sort the triangles
	sort(triangles, numTriangles);

	return 0;
}

mat4 proj;

static plane top, bottom, left, right, near, far;
static plane *planes[] = {&top, &bottom, &left, &right, &near, &far};

// top, bottom, left, right, near, far
bool is_in(float3 *v, int selector) {
	plane *p = planes[selector];
	switch (selector) {
	case 0:
	case 1:
		return v->y * p->y > (NEAR - v->z) * p->z;
	case 2:
	case 3:
		return v->x * p->x > (NEAR - v->z) * p->z;
	case 4:
		return v->z > -p->w;
	case 5:
		return v->z < -p->w;
	}
	exit(1);
}

void render() {
	// Transform the verticies
	mat4 t1, t2, rot;
	rotateY(t1, -look.x);
	rotateX(t2, look.y);
	mat4_mul(rot, t1, t2);
	translate(t1, -camera.x, -camera.y, -camera.z);
	mat4_mul(t2, rot, t1);
	for (int i = 0; i < numVerticies; i++) {
		float4 v;
		vec3_tovec4(&v, &verticies[i]);
		mat4_mul_vec(&v, t2, &v);
		memcpy(&transVerts[i], &v, sizeof(float3));
	}
	// Transform the light source
	float4 tmp_light;
	vec3_tovec4(&tmp_light, &g_light);
	mat4_mul_vec(&tmp_light, t2, &tmp_light);
	float3 light;
	light.x = tmp_light.x;
	light.y = tmp_light.y;
	light.z = tmp_light.z;
	raytrace_render(frame_buffer, transVerts, triangles, numTriangles, proj, (float3 *)&light);
}

int main() {
	top = (plane){0, -tanf(FOV / 2 * M_PI / 180), FAR / (FAR - NEAR), -(FAR * NEAR) / (FAR - NEAR)};
	bottom = (plane){0, tanf(FOV / 2 * M_PI / 180), FAR / (FAR - NEAR), -(FAR * NEAR) / (FAR - NEAR)};
	left = (plane){ASPECT * tanf(FOV / 2 * M_PI / 180), 0, FAR / (FAR - NEAR), -(FAR * NEAR) / (FAR - NEAR)};
	right = (plane){-ASPECT * tanf(FOV / 2 * M_PI / 180), 0, FAR / (FAR - NEAR), -(FAR * NEAR) / (FAR - NEAR)};
	near = (plane){0, 0, 1, -NEAR};
	far = (plane){0, 0, 1, -FAR};
	project(proj, FOV, ASPECT, NEAR, FAR);
	if (parseFile())
		return 1;
	frame_buffer = (uint32_t *)calloc(WIDTH * HEIGHT, sizeof(uint32_t));
	render();
	return 0;
}
