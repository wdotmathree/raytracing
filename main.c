#include "mat.h"
#include <stdio.h>
#include <sys/mman.h>
#include <time.h>

int numVertices;
int numTriangles;
vec3 *verticies;
vec3 *transVerts;
vec3i *triangles;

inline void centroid(vec3 *centroid, vec3i *tri) {
	centroid->x = (verticies[tri->x].x + verticies[tri->y].x + verticies[tri->z].x) / 3;
	centroid->y = (verticies[tri->x].y + verticies[tri->y].y + verticies[tri->z].y) / 3;
	centroid->z = (verticies[tri->x].z + verticies[tri->y].z + verticies[tri->z].z) / 3;
}

void sort(vec3i *start, vec3i *end) {
	if (start >= end)
		return;
	vec3i *l = start + 1;
	vec3i *r = end;
	float p = (transVerts[start->x].z + transVerts[start->y].z + transVerts[start->z].z) / 3;
	while (l <= r) {
		while (l <= r && (transVerts[l->x].z + transVerts[l->y].z + transVerts[l->z].z) <= p)
			l++;
		while (l <= r && (transVerts[r->x].z + transVerts[r->y].z + transVerts[r->z].z) > p)
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
	while (line) {
		if (line[0] == 'v' && line[1] == ' ') {
			vec3 *vert = verticies + v;
			sscanf(line, "v %f %f %f", &vert->x, &vert->y, &vert->z);
			v++;
		} else if (line[0] == 'f' && line[1] == ' ') {
			vec3i *tri = triangles + t;
			sscanf(line, "f %d %d %d", &tri->x, &tri->y, &tri->z);
			t++;
		}
		line = strchr(line, '\n');
		if (line)
			line++;
	}
	munmap(map, ftell(fp) + 1);
	memcpy(transVerts, verticies, numVertices * sizeof(vec3i));
	return 0;
}

int main() {
	if (parseFile())
		return 1;
	printf("Num Verticies: %d\n", numVertices);
	// while (true) {
	clock_t start = clock();
	sort(triangles, triangles + numTriangles - 1);
	clock_t end = clock();
	printf("%f\n", (double)(end - start) / CLOCKS_PER_SEC);
	// }
	return 0;
}
