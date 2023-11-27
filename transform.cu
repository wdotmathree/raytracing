#include "transform.hpp"

special void translate(float out[4][4], float x, float y, float z) {
	memset(out, 0, sizeof(float) * 16);
	out[0][0] = 1;
	out[1][1] = 1;
	out[2][2] = 1;
	out[3][3] = 1;
	out[0][3] = x;
	out[1][3] = y;
	out[2][3] = z;
}

special void rotateX(float out[4][4], float x) {
	x = x * M_PI / 180;
	memset(out, 0, sizeof(float) * 16);
	out[0][0] = 1;
	out[1][1] = cos(x);
	out[1][2] = -sin(x);
	out[2][1] = sin(x);
	out[2][2] = cos(x);
	out[3][3] = 1;
}

special void rotateY(float out[4][4], float y) {
	y = y * M_PI / 180;
	memset(out, 0, sizeof(float) * 16);
	out[0][0] = cos(y);
	out[0][2] = sin(y);
	out[1][1] = 1;
	out[2][0] = -sin(y);
	out[2][2] = cos(y);
	out[3][3] = 1;
}

special void rotateZ(float out[4][4], float z) {
	z = z * M_PI / 180;
	memset(out, 0, sizeof(float) * 16);
	out[0][0] = cos(z);
	out[0][1] = -sin(z);
	out[1][0] = sin(z);
	out[1][1] = cos(z);
	out[2][2] = 1;
	out[3][3] = 1;
}

special void scale(float out[4][4], float x, float y, float z) {
	memset(out, 0, sizeof(float) * 16);
	out[0][0] = x;
	out[1][1] = y;
	out[2][2] = z;
	out[3][3] = 1;
}

special void project(float out[4][4], float fov, float aspect, float near, float far) {
	float f = 1 / tan(fov / 2 * M_PI / 180);
	float q = far / (far - near);
	memset(out, 0, sizeof(float) * 16);
	out[0][0] = aspect * f;
	out[1][1] = f;
	out[2][2] = q;
	out[2][3] = -near * q;
	out[3][3] = 1;
}

special void to_screen(float3 *out, float3 *in) {
	out->x = (in->x / in->z + 1) * WIDTH / 2;
	out->y = (in->y / in->z + 1) * HEIGHT / 2;
	out->z = in->z;
	out->x = fminf(fmaxf(out->x, 0), WIDTH - 1);
	out->y = HEIGHT - 1 - fminf(fmaxf(out->y, 0), HEIGHT - 1);
}
