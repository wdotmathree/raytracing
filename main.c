#include "mat.h"
#include <stdio.h>
#include <sys/mman.h>
#include <time.h>

int numVertices;
int numTriangles;
float *verticies;
float *transVerts;
int *triangles;

inline void centroid(float centroid[3], int *tri) {
	centroid[0] = (verticies[tri[0] * 3] + verticies[tri[1] * 3] + verticies[tri[2] * 3]) / 3;
	centroid[1] = (verticies[tri[0] * 3 + 1] + verticies[tri[1] * 3 + 1] + verticies[tri[2] * 3 + 1]) / 3;
	centroid[2] = (verticies[tri[0] * 3 + 2] + verticies[tri[1] * 3 + 2] + verticies[tri[2] * 3 + 2]) / 3;
}

void sort(int *start, int *end) {
	if (start >= end)
		return;
	int *l = start + 3;
	int *r = end;
	float p = (transVerts[start[0] * 3 + 2] + transVerts[start[1] * 3 + 2] + transVerts[start[2] * 3 + 2]) / 3;
	while (l <= r) {
		while (l <= r && (transVerts[l[0] * 3 + 2] + transVerts[l[1] * 3 + 2] + transVerts[l[2] * 3 + 2]) <= p)
			l += 3;
		while (l <= r && (transVerts[r[0] * 3 + 2] + transVerts[r[1] * 3 + 2] + transVerts[r[2] * 3 + 2]) > p)
			r -= 3;
		if (l < r) {
			int t = *l;
			*l = *r;
			*r = t;
			t = *(l + 1);
			*(l + 1) = *(r + 1);
			*(r + 1) = t;
			t = *(l + 2);
			*(l + 2) = *(r + 2);
			*(r + 2) = t;
		}
	}
	int t = *start;
	*start = *r;
	*r = t;
	t = *(start + 1);
	*(start + 1) = *(r + 1);
	*(r + 1) = t;
	t = *(start + 2);
	*(start + 2) = *(r + 2);
	*(r + 2) = t;
	sort(start, r - 3);
	sort(r + 3, end);
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
	verticies = malloc(numVertices * 3 * sizeof(float));
	triangles = malloc(numTriangles * 3 * sizeof(int));
	transVerts = malloc(numVertices * 3 * sizeof(float));
	line = map;
	int v = 0;
	int t = 0;
	while (line) {
		if (line[0] == 'v' && line[1] == ' ') {
			sscanf(line, "v %f %f %f", &verticies[v], &verticies[v + 1], &verticies[v + 2]);
			v += 3;
		} else if (line[0] == 'f' && line[1] == ' ') {
			sscanf(line, "f %d %d %d", &triangles[t], &triangles[t + 1], &triangles[t + 2]);
			triangles[t]--;
			triangles[t + 1]--;
			triangles[t + 2]--;
			t += 3;
		}
		line = strchr(line, '\n');
		if (line)
			line++;
	}
	munmap(map, ftell(fp) + 1);
	memcpy(transVerts, verticies, numVertices * 3 * sizeof(float));
	return 0;
}

// int main() {
// 	if (parseFile())
// 		return 1;
// 	printf("Num Verticies: %d\n", numVertices);
// 	// while (true) {
// 	clock_t start = clock();
// 	sort(triangles, triangles + numTriangles * 3 - 1);
// 	clock_t end = clock();
// 	printf("%f\n", (double)(end - start) / CLOCKS_PER_SEC);
// 	// }
// 	return 0;
// }
