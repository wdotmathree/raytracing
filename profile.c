#include "mat.h"
#include "vec.h"
#include <stdio.h>
#include <time.h>

int main() {
	float garbage[1000000];
	FILE *fp = fopen("/dev/urandom", "r");
	for (int i = 0; i < 1000000; i++) {
		float m[4][4];
		vec4 v, n;
		fread(m, sizeof(float), 16, fp);
		fread(&v, sizeof(float), 4, fp);
		fread(&n, sizeof(float), 4, fp);
		for (int g = 0; g < 1000000; g++)
			garbage[g] = rand();
		mat4x4_mul_vec4(&n, m, &v);
		for (int g = 0; g < 1000000; g++)
			garbage[g] = rand();
	}
	return 0;
}
