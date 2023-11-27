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
const vec3 light = {0, 0, -10};
vec3 camera = {2, 1, -5};
vec2 look = {0, 0};

bool raytrace = false;

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
		// if (z1 < z_buffer[(int)ceilf(y1) * WIDTH + x]) {
		// 	z_buffer[(int)ceilf(y1) * WIDTH + x] = z1;
		// 	frame_buffer[(int)ceilf(y1) * WIDTH + x] = 0;
		// }
		// if (z2 < z_buffer[(int)floorf(y2) * WIDTH + x]) {
		// 	z_buffer[(int)floorf(y2) * WIDTH + x] = z2;
		// 	frame_buffer[(int)floorf(y2) * WIDTH + x] = 0;
		// }
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
		// if (z1 < z_buffer[(int)ceilf(y1) * WIDTH + x]) {
		// 	z_buffer[(int)ceilf(y1) * WIDTH + x] = z1;
		// 	frame_buffer[(int)ceilf(y1) * WIDTH + x] = 0;
		// }
		// if (z2 < z_buffer[(int)floorf(y2) * WIDTH + x]) {
		// 	z_buffer[(int)floorf(y2) * WIDTH + x] = z2;
		// 	frame_buffer[(int)floorf(y2) * WIDTH + x] = 0;
		// }
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
	// float minx = 0;
	// float miny = 0;
	// float minz = 0;
	// float maxx = 0;
	// float maxy = 0;
	// float maxz = 0;
	while (line) {
		if (line[0] == 'v' && line[1] == ' ') {
			vec3 *vert = verticies + v;
			sscanf(line, "v %f %f %f", &vert->x, &vert->y, &vert->z);
			// minx = fminf(minx, vert->x);
			// miny = fminf(miny, vert->y);
			// minz = fminf(minz, vert->z);
			// maxx = fmaxf(maxx, vert->x);
			// maxy = fmaxf(maxy, vert->y);
			// maxz = fmaxf(maxz, vert->z);
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
	// float scale = fmaxf(fmaxf(maxx - minx, maxy - miny), maxz - minz) / 2;
	// for (int i = 0; i < numVerticies; i++) {
	// 	vec3 *vert = verticies + i;
	// 	vert->x -= (maxx + minx) / 2;
	// 	vert->y -= (maxy + miny) / 2;
	// 	vert->z -= (maxz + minz) / 2;
	// 	vert->x /= scale;
	// 	vert->y /= scale;
	// 	vert->z /= scale;
	// }
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

// top, bottom, left, right, near, far
bool is_in(vec3 *v, int selector) {
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

vec3_arr *clip(vec3 **verts, int numVerts) {
	// Prepare arrays
	vec3_arr *clipped = vec3_arr_new();
	vec3_arr *tmp = vec3_arr_new();
	vec3_arr inside = vec3_arr_init();
	vec3_arr_reserve(clipped, numVerts * 2);
	vec3_arr_reserve(tmp, numVerts * 2);
	for (int i = 0; i < numVerts; i++)
		vec3_arr_push(tmp, verts[i]);
	// Clip against each plane
	for (int i = 0; i < 6; i++) {
		vec3_arr_clear(clipped);
		for (int j = 0; j < tmp->size; j += 3) {
			vec3_arr_clear(&inside);
			vec3 *a = &tmp->data[j];
			vec3 *b = &tmp->data[j + 1];
			vec3 *c = &tmp->data[j + 2];
			// Check if the triangle is inside the plane
			if (is_in(a, i) && is_in(b, i) && is_in(c, i)) {
				vec3_arr_push(clipped, a);
				vec3_arr_push(clipped, b);
				vec3_arr_push(clipped, c);
				continue;
			}
			// Check if the triangle is outside the plane
			if (!is_in(a, i) && !is_in(b, i) && !is_in(c, i))
				continue;
			// Check if the triangle intersects the plane
			vec3 out1, out2;
			if (intersect_plane_triangle(&out1, &out2, planes[i], a, b, c) < 2) {
				vec3_arr_push(clipped, a);
				vec3_arr_push(clipped, b);
				vec3_arr_push(clipped, c);
				continue;
			}
			/// TODO: Fix this part
			bool used = false; // If we have used out1 yet
			int outside = 0; // How many verticies are outside the visible area
			// Inside are the points A,B,C (in order) and their replacements (out1, out2) if they are outside
			if (is_in(a, i)) {
				vec3_arr_push(&inside, a);
			} else {
				// Use out1 as a replacement for A
				vec3_arr_push(&inside, &out1);
				used = true;
				outside++;
			}
			if (is_in(b, i)) {
				vec3_arr_push(&inside, b);
			} else {
				// Use out2 if we have already used out1, otherwise use out1
				vec3_arr_push(&inside, used ? &out2 : &out1);
				used = true;
				outside++;
			}
			if (is_in(c, i)) {
				vec3_arr_push(&inside, c);
			} else {
				vec3_arr_push(&inside, used ? &out2 : &out1);
				used = true;
				outside++;
			}
			if (outside == 1) {
				// Cutting a corner off, break into 2 triangles
				vec3_arr_push(clipped, &inside.data[0]);
				vec3_arr_push(clipped, &inside.data[1]);
				vec3_arr_push(clipped, &inside.data[2]);
				vec3_arr_push(clipped, &inside.data[2]);
				vec3_arr_push(clipped, &out2);
				vec3_arr_push(clipped, &inside.data[0]);
			} else {
				// Ctting a side off, still only 1 triangle
				vec3_arr_push(clipped, &inside.data[0]);
				vec3_arr_push(clipped, &inside.data[1]);
				vec3_arr_push(clipped, &inside.data[2]);
			}
		}
		vec3_arr *t = tmp;
		tmp = clipped;
		clipped = t;
	}
	vec3_arr_free(tmp);
	free(tmp);
	vec3_arr_free(&inside);
	return clipped;
}

void *renderLoop(void *args) {
	(void)args; // unused
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
		for (int i = 0; i < numVerticies; i++) {
			vec4 v;
			vec3_tovec4(&v, &verticies[i]);
			mat4_mul_vec(&v, t2, &v);
			memcpy(&transVerts[i], &v, sizeof(vec3));
		}
		if (raytrace) {
			// Cast a ray for each pixel
			ray r = {{0, 0, proj[2][3]}, {0, 0, 1}};
			for (int x = 0; x < WIDTH; x++) {
				r.direction.x = (x / (WIDTH / 2.0f) - 1) / proj[0][0];
				for (int y = 0; y < HEIGHT; y++) {
					if (x == 200 && y == 180)
						printf("test");
					r.direction.y = (y / (HEIGHT / 2.0f) - 1) / proj[1][1];
					// Find the closest intersection
					float closest = INFINITY;
					vec3 closestPoint;
					for (int i = 0; i < numTriangles; i++) {
						vec3 *a = transVerts + triangles[i].x;
						vec3 *b = transVerts + triangles[i].y;
						vec3 *c = transVerts + triangles[i].z;
						plane p;
						vec3 ab, ac;
						vec3_sub(&ab, b, a);
						vec3_sub(&ac, c, a);
						vec3_cross((vec3 *)&p, &ab, &ac);
						vec3_normalize((vec3 *)&p, (vec3 *)&p);
						p.w = -vec3_dot((vec3 *)&p, a);
						vec3 point;
						if (intersect_plane_line(&point, &p, &r)) {
							if (point_in_triangle(&point, a, b, c)) {
								vec3_sub(&point, &point, &r.origin);
								float dist = vec3_length(&point);
								if (dist < closest) {
									closest = dist;
									closestPoint = point;
								}
							}
						}
					}
					if (closest != INFINITY) {
						middle_buffer[(HEIGHT - 1 - y) * WIDTH + x] = 0xeeeeee;
					} else {
						middle_buffer[(HEIGHT - 1 - y) * WIDTH + x] = 0x111111;
					}
				}
			}
		} else {
			// Collect the verticies into triangles
			vec3 **triVerticies = malloc(numTriangles * 3 * sizeof(vec3 *));
			for (int i = 0; i < numTriangles; i++) {
				triVerticies[i * 3 + 0] = transVerts + triangles[i].x;
				triVerticies[i * 3 + 1] = transVerts + triangles[i].y;
				triVerticies[i * 3 + 2] = transVerts + triangles[i].z;
			}
			// Clip the triangles
			vec3_arr *clipped = clip(triVerticies, numTriangles * 3);
			// Project the verticies
			vec3 *screenVerts = malloc(clipped->size * sizeof(vec3));
			for (int i = 0; i < clipped->size; i++) {
				vec4 a, b;
				vec3_tovec4(&a, &clipped->data[i]);
				mat4_mul_vec(&b, proj, &a);
				// Make it to screen space
				to_screen(&screenVerts[i], (vec3 *)&b);
			}
			// Draw the triangles
			memset(z_buffer, 0x7f, WIDTH * HEIGHT * sizeof(int));
			memset(frame_buffer, 0, WIDTH * HEIGHT * sizeof(uint32_t));
			for (int i = 0; i < clipped->size; i += 3) {
				vec3 *a = &clipped->data[i + 0];
				vec3 *b = &clipped->data[i + 1];
				vec3 *c = &clipped->data[i + 2];
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
				// float lum = 255;
				fillTriangle(lum, &screenVerts[i], &screenVerts[i + 1], &screenVerts[i + 2]);
			}
			memcpy(middle_buffer, frame_buffer, WIDTH * HEIGHT * sizeof(uint32_t));
			free(screenVerts);
			free(triVerticies);
			vec3_arr_free(clipped);
			free(clipped);
		}
	}
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
	w = SDL_CreateWindow("Thing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN);
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
				else if (e.key.keysym.sym == SDLK_w)
					camera.z += 0.1;
				else if (e.key.keysym.sym == SDLK_s)
					camera.z -= 0.1;
				else if (e.key.keysym.sym == SDLK_a)
					camera.x -= 0.1;
				else if (e.key.keysym.sym == SDLK_d)
					camera.x += 0.1;
				else if (e.key.keysym.sym == SDLK_q)
					camera.y -= 0.1;
				else if (e.key.keysym.sym == SDLK_e)
					camera.y += 0.1;
				else if (e.key.keysym.sym == SDLK_r)
					raytrace = !raytrace;
		}
		printf("%f %f %f\n", camera.x, camera.y, camera.z);
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
