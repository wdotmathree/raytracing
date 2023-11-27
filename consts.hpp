#pragma once
#ifndef DEFINES_H
#define DEFINES_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define special __host__ __device__

#define LINALG_EPSILON 0.0001f

static const int WIDTH = 1920;
static const int HEIGHT = 1080;

typedef float mat2[2][2];
typedef float mat3[3][3];
typedef float mat4[4][4];

static const int FOV = 90;
static const float ASPECT = (float)HEIGHT / WIDTH;
static const float NEAR = 0.1f;
static const float FAR = 100.0f;

#endif
