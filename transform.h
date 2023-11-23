#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "consts.h"
#include "mat.h"
#include "transform.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void translate(mat4 out, float x, float y, float z);

void rotateX(mat4 out, float x);

void rotateY(mat4 out, float y);

void rotateZ(mat4 out, float z);

void scale(mat4 out, float x, float y, float z);

void project(mat4 out, float fov, float aspect, float near, float far);

void to_screen(vec3 *out, vec3 *in);

#endif
