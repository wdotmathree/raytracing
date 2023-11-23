#include "arr.h"
#include "consts.h"
#include "linalg.h"
#include "mat.h"
#include "transform.h"
#include "vec.h"
#include <SDL.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/mman.h>
#include <time.h>

const int FPS = 15;
const vec3 light = {0, 0, -1};
vec3 camera = {0, 0, -5};
vec2 look = {0, 0};

int numVerticies;
int numTriangles;
vec3 *verticies;
vec3 *transVerts;
vec3i *triangles;

uint32_t *frame_buffer;
uint32_t *middle_buffer;
float *z_buffer;

void fillTriangle(int lum, vec3 *a, vec3 *b, vec3 *c) {
	vec3 left, mid, right;
	if (a->x < b->x) {
		if (a->x < c->x) {
			left = *a;
			if (b->x < c->x) {
				mid = *b;
				right = *c;
			} else {
				mid = *c;
				right = *b;
			}
		} else {
			left = *c;
			mid = *a;
			right = *b;
		}
	} else {
		if (b->x < c->x) {
			left = *b;
			if (a->x < c->x) {
				mid = *a;
				right = *c;
			} else {
				mid = *c;
				right = *a;
			}
		} else {
			left = *c;
			mid = *b;
			right = *a;
		}
	}
	float m1 = (left.y - right.y) / (left.x - right.x);
	float m2 = (left.y - mid.y) / (left.x - mid.x);
	float m3 = (mid.y - right.y) / (mid.x - right.x);
	float mz1 = (left.z - right.z) / (left.x - right.x);
	float mz2 = (left.z - mid.z) / (left.x - mid.x);
	float mz3 = (mid.z - right.z) / (mid.x - right.x);
	float b1 = left.y - m1 * left.x;
	float b2 = left.y - m2 * left.x;
	float b3 = mid.y - m3 * mid.x;
	float bz1 = left.z - mz1 * left.x;
	float bz2 = left.z - mz2 * left.x;
	float bz3 = mid.z - mz3 * mid.x;
	for (int x = ceilf(left.x); x <= floorf(mid.x); x++) {
		float y1 = m1 * x + b1;
		float y2 = m2 * x + b2;
		if (y1 > y2) {
			float t = y1;
			y1 = y2;
			y2 = t;
		}
		float z1 = mz1 * x + bz1;
		float z2 = mz2 * x + bz2;
		for (int y = ceilf(y1); y <= floorf(y2); y++) {
			float z = z1 + (z2 - z1) * (y - y1) / (y2 - y1);
			if (z < z_buffer[y * WIDTH + x]) {
				z_buffer[y * WIDTH + x] = z;
				frame_buffer[y * WIDTH + x] = (lum << 16) | (lum << 8) | lum;
			}
		}
	}
	for (int x = ceilf(mid.x); x <= floorf(right.x); x++) {
		float y1 = m1 * x + b1;
		float y2 = m3 * x + b3;
		if (y1 > y2) {
			float t = y1;
			y1 = y2;
			y2 = t;
		}
		float z1 = mz1 * x + bz1;
		float z2 = mz3 * x + bz3;
		for (int y = ceilf(y1); y <= floorf(y2); y++) {
			float z = z1 + (z2 - z1) * (y - y1) / (y2 - y1);
			if (z < z_buffer[y * WIDTH + x]) {
				z_buffer[y * WIDTH + x] = z;
				frame_buffer[y * WIDTH + x] = (lum << 16) | (lum << 8) | lum;
			}
		}
	}
}

void centroid(vec3 *centroid, vec3 *a, vec3 *b, vec3 *c) {
	centroid->x = (a->x + b->x + c->x) / 3;
	centroid->y = (a->y + b->y + c->y) / 3;
	centroid->z = (a->z + b->z + c->z) / 3;
}

int parseFile() {
	// Read in the data
	FILE *fp = fopen("thing.obj", "r");
	if (!fp) {
		fprintf(stderr, "Failed to open file\n");
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	char *map = mmap(NULL, ftell(fp) + 1, PROT_READ, MAP_PRIVATE, fp->_fileno, 0);
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
	verticies = malloc(numVerticies * sizeof(vec3));
	triangles = malloc(numTriangles * sizeof(vec3i));
	transVerts = malloc(numVerticies * sizeof(vec3));
	line = map;
	int v = 0;
	int t = 0;
	float minx = 0;
	float miny = 0;
	float minz = 0;
	float maxx = 0;
	float maxy = 0;
	float maxz = 0;
	while (line) {
		if (line[0] == 'v' && line[1] == ' ') {
			vec3 *vert = verticies + v;
			sscanf(line, "v %f %f %f", &vert->x, &vert->y, &vert->z);
			minx = fminf(minx, vert->x);
			miny = fminf(miny, vert->y);
			minz = fminf(minz, vert->z);
			maxx = fmaxf(maxx, vert->x);
			maxy = fmaxf(maxy, vert->y);
			maxz = fmaxf(maxz, vert->z);
			v++;
		} else if (line[0] == 'f' && line[1] == ' ') {
			vec3i *tri = triangles + t;
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
	float scale = fmaxf(fmaxf(maxx - minx, maxy - miny), maxz - minz) / 2;
	for (int i = 0; i < numVerticies; i++) {
		vec3 *vert = verticies + i;
		vert->x -= (maxx + minx) / 2;
		vert->y -= (maxy + miny) / 2;
		vert->z -= (maxz + minz) / 2;
		vert->x /= scale;
		vert->y /= scale;
		vert->z /= scale;
	}
	munmap(map, ftell(fp) + 1);
	fclose(fp);
	return 0;
}

int frame = 0;
long long prevtime = 0;
SDL_Window *w;
SDL_Surface *s;

mat4 proj;
pthread_t t;

// Sync paint with render
pthread_mutex_t paint_mutex = PTHREAD_MUTEX_INITIALIZER;

static plane top, bottom, left, right, near, far;
static plane *planes[] = {&top, &bottom, &left, &right, &near, &far};

bool is_in(vec3 *v, int i) {
	plane *p = planes[i];
	switch (i) {
	case 0:
		return v->y < -v->z * p->z / p->y;
	case 1:
		return v->y > -v->z * p->z / p->y;
	case 2:
		return v->x < -v->z * p->z / p->x;
	case 3:
		return v->x > -v->z * p->z / p->x;
	case 4:
		return v->z > -p->w;
	case 5:
		return v->z < -p->w;
	}
	exit(1);
}

void *renderLoop(void *args) {
	(void)args;
	while (true) {
		// Wait for paint to finish
		pthread_mutex_lock(&paint_mutex);
		// Transform the verticies
		mat4 t1, t2, rot;
		rotateY(t1, -look.x);
		rotateX(t2, look.y);
		mat4_mul(rot, t1, t2);
		translate(t1, -camera.x, -camera.y, -camera.z);
		mat4_mul(t2, rot, t1);
		mat4 t3;
		// rotateY(t1, frame);
		rotateY(t1, /*76*/ frame);
		rotateX(t3, 30);
		for (int i = 0; i < numVerticies; i++) {
			vec4 v;
			vec3_tovec4(&v, verticies + i);
			mat4_mul_vec(&v, t1, &v);
			mat4_mul_vec(&v, t3, &v);
			mat4_mul_vec(&v, t2, &v);
			memcpy(transVerts + i, &v, sizeof(vec3));
		}
		// Project the verticies
		vec3 *screenVerts = malloc(numVerticies * sizeof(vec3));
		for (int i = 0; i < numVerticies; i++) {
			vec4 a, b;
			vec3_tovec4(&a, transVerts + i);
			mat4_mul_vec(&b, proj, &a);
			// Make it to screen space
			to_screen(screenVerts + i, (vec3 *)&b);
		}
		// Draw the triangles
		memset(z_buffer, 0x7f, WIDTH * HEIGHT * sizeof(int));
		memset(frame_buffer, 0, WIDTH * HEIGHT * sizeof(uint32_t));
		for (int i = 0; i < numTriangles; i++) {
			vec3 *a = transVerts + triangles[i].x;
			vec3 *b = transVerts + triangles[i].y;
			vec3 *c = transVerts + triangles[i].z;
			// get the luminance
			vec3 cent, norm, ab, ac, normcent;
			centroid(&cent, a, b, c);
			vec3_normalize(&normcent, &cent);
			vec3_sub(&ab, b, a);
			vec3_sub(&ac, c, a);
			vec3_cross(&norm, &ab, &ac);
			vec3_normalize(&norm, &norm);
			vec3_sub(&cent, &cent, &light);
			if (vec3_dot(&normcent, &norm) >= 0)
				continue;
			float lum = -vec3_dot(&normcent, &norm) * 255;
			fillTriangle(
				lum, screenVerts + triangles[i].x, screenVerts + triangles[i].y,
				screenVerts + triangles[i].z
			);
		}
		memcpy(middle_buffer, frame_buffer, WIDTH * HEIGHT * sizeof(uint32_t));
		free(screenVerts);
	}
}

int main() {
	top = (plane){0, NEAR / ASPECT, -tanf(FOV / 2 * M_PI / 180) * ASPECT, 0};
	bottom = (plane){0, NEAR / ASPECT, tanf(FOV / 2 * M_PI / 180) * ASPECT, 0};
	left = (plane){NEAR, 0, -tanf(FOV / 2 * M_PI / 180), 0};
	right = (plane){NEAR, 0, tanf(FOV / 2 * M_PI / 180), 0};
	near = (plane){0, 0, 1, -NEAR};
	far = (plane){0, 0, 1, -FAR};
	project(proj, FOV, ASPECT, NEAR, FAR);
	if (parseFile())
		return 1;
	w = SDL_CreateWindow(
		"Thing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,
		SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN
	);
	s = SDL_GetWindowSurface(w);
	frame_buffer = calloc(WIDTH * HEIGHT, sizeof(uint32_t));
	z_buffer = calloc(WIDTH * HEIGHT, sizeof(float));
	middle_buffer = calloc(WIDTH * HEIGHT, sizeof(uint32_t));
	pthread_mutex_unlock(&paint_mutex);
	pthread_create(&t, NULL, renderLoop, NULL);
	while (true) {
		// Listen for events
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				exit(0);
			if (e.type == SDL_KEYDOWN)
				if (e.key.keysym.sym == SDLK_ESCAPE)
					exit(0);
		}
		// Update screen
		long long time = SDL_GetTicks();
		SDL_LockSurface(s);
		memcpy(s->pixels, middle_buffer, WIDTH * HEIGHT * sizeof(uint32_t));
		SDL_UnlockSurface(s);
		SDL_UpdateWindowSurface(w);
		pthread_mutex_unlock(&paint_mutex);
		frame++;
		long long diff = SDL_GetTicks() - time;
		if (diff < 1000 / FPS)
			SDL_Delay(1000 / FPS - diff);
	}
	return 0;
}
