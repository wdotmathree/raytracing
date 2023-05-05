#include "consts.h"
#include "mat.h"
#include "transform.h"
#include "vec.h"
#include <SDL.h>
#include <stdio.h>
#include <sys/mman.h>
#include <time.h>

int numVertices;
int numTriangles;
vec3 *verticies;
vec3 *transVerts;
vec3i *triangles;

void fillTriangle(SDL_Renderer *r, vec2 *a, vec2 *b, vec2 *c) {
	vec2 left, mid, right;
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
	float b1 = left.y - m1 * left.x;
	float b2 = left.y - m2 * left.x;
	float b3 = mid.y - m3 * mid.x;
	float top = fminf(fminf(left.y, mid.y), right.y);
	float bottom = fmaxf(fmaxf(left.y, mid.y), right.y);
	for (int x = ceilf(left.x); x < mid.x; x++) {
		float y1 = m1 * x + b1;
		float y2 = m2 * x + b2;
		y1 = fminf(fmaxf(y1, top), bottom);
		y2 = fminf(fmaxf(y2, top), bottom);
		SDL_RenderDrawLine(r, x, y1, x, y2);
	}
	for (int x = ceilf(mid.x); x < right.x; x++) {
		float y1 = m1 * x + b1;
		float y2 = m3 * x + b3;
		y1 = fminf(fmaxf(y1, top), bottom);
		y2 = fminf(fmaxf(y2, top), bottom);
		SDL_RenderDrawLine(r, x, y1, x, y2);
	}
}

void centroid(vec3 *centroid, vec3i *tri) {
	centroid->x = (transVerts[tri->x].x + transVerts[tri->y].x + transVerts[tri->z].x) / 3;
	centroid->y = (transVerts[tri->x].y + transVerts[tri->y].y + transVerts[tri->z].y) / 3;
	centroid->z = (transVerts[tri->x].z + transVerts[tri->y].z + transVerts[tri->z].z) / 3;
}

void sort(vec3i *start, vec3i *end) {
	if (start >= end)
		return;
	vec3i *l = start + 1;
	vec3i *r = end;
	float p = (transVerts[start->x].z + transVerts[start->y].z + transVerts[start->z].z) / 3;
	while (l <= r) {
		while (l <= r && (transVerts[l->x].z + transVerts[l->y].z + transVerts[l->z].z) / 3 >= p)
			l++;
		while (l <= r && (transVerts[r->x].z + transVerts[r->y].z + transVerts[r->z].z) / 3 <= p)
			r--;
		if (l < r) {
			vec3i t = *l;
			*l = *r;
			*r = t;
		}
	}
	vec3i t = *start;
	*start = *r;
	*r = t;
	sort(start, r - 1);
	sort(r + 1, end);
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
	fclose(fp);
	char *line = map;
	while (line) {
		if (line[0] == 'v' && line[1] == ' ') {
			numVertices++;
		} else if (line[0] == 'f' && line[1] == ' ') {
			numTriangles++;
		}
		line = strchr(line, '\n');
		if (line)
			line++;
	}
	verticies = malloc(numVertices * sizeof(vec3));
	triangles = malloc(numTriangles * sizeof(vec3i));
	transVerts = malloc(numVertices * sizeof(vec3));
	line = map;
	int v = 0;
	int t = 0;
	float minx = 0;
	float miny = 0;
	float minz = 0;
	float maxx = 0;
	float maxy = 0;
	float maxz = 0;
	float avgx = 0;
	float avgy = 0;
	float avgz = 0;
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
			avgx += vert->x;
			avgy += vert->y;
			avgz += vert->z;
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
	avgx /= numVertices;
	avgy /= numVertices;
	avgz /= numVertices;
	float scale = fmaxf(fmaxf(maxx - minx, maxy - miny), maxz - minz);
	for (int i = 0; i < numVertices; i++) {
		vec3 *vert = verticies + i;
		vert->x -= avgx;
		vert->y -= avgy;
		vert->z -= avgz;
		vert->x /= scale;
		vert->y /= scale;
		vert->z /= scale;
	}
	munmap(map, ftell(fp) + 1);
	memcpy(transVerts, verticies, numVertices * sizeof(vec3i));
	return 0;
}

int frame = 0;

int main() {
	mat4 proj;
	project(proj, 90, (float)WIDTH / HEIGHT, 0.1, 1000);
	if (parseFile())
		return 1;
	SDL_Window *w = SDL_CreateWindow("Thing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN);
	SDL_Renderer *r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
	while (true) {
		// Listen for events
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				return 0;
		}
		// Clear the screen
		SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
		SDL_RenderClear(r);
		// Transform the verticies
		for (int i = 0; i < numVertices; i++) {
			mat4 a, b, c;
			rotateX(a, 180);
			rotateY(b, frame);
			mat4x4_mul(c, a, b);
			translate(a, 0, 0.4, 2);
			mat4x4_mul(b, a, c);
			vec4 v;
			vec3_tovec4(&v, verticies + i);
			mat4x4_mul_vec4(&v, b, &v);
			memcpy(transVerts + i, &v, sizeof(vec3));
		}
		sort(triangles, triangles + numTriangles - 1);
		// Project the verticies
		vec2 *screenVerts = malloc(numVertices * sizeof(vec2));
		for (int i = 0; i < numVertices; i++) {
			mat4 proj;
			project(proj, 90, (float)HEIGHT / WIDTH, 1, 10);
			vec4 a, b;
			vec3_tovec4(&a, transVerts + i);
			mat4x4_mul_vec4(&b, proj, &a);
			// Make it to screen space
			to_screen(screenVerts + i, (vec3 *)&b);
		}
		// Draw the triangles
		for (int i = 0; i < numTriangles; i++) {
			vec2 *a = screenVerts + triangles[i].x;
			vec2 *b = screenVerts + triangles[i].y;
			vec2 *c = screenVerts + triangles[i].z;
			// get the luminance
			vec3 cent, norm, ab, ac;
			centroid(&cent, triangles + i);
			vec3_sub(&ab, transVerts + triangles[i].y, transVerts + triangles[i].x);
			vec3_sub(&ac, transVerts + triangles[i].z, transVerts + triangles[i].x);
			vec3_cross(&norm, &ab, &ac);
			vec3_normalize(&norm, &norm);
			vec3 light = {0, 0, -1};
			vec3_sub(&cent, &cent, &light);
			vec3_normalize(&cent, &cent);
			float lum = -vec3_dot(&cent, &norm) * 255;
			SDL_SetRenderDrawColor(r, lum, lum, lum, 255);
			fillTriangle(r, a, b, c);
		}
		free(screenVerts);
		SDL_RenderPresent(r);
		frame++;
	}
	return 0;
}
