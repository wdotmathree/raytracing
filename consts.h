#pragma once
#ifndef DEFINES_H
#define DEFINES_H

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>

static const int WIDTH = 1920;
static const int HEIGHT = 1080;

typedef float mat2[2][2];
typedef float mat3[3][3];
typedef float mat4[4][4];

static const int FOV = 90;
static const float ASPECT = (float)HEIGHT / WIDTH;
static const float NEAR = 1;
static const float FAR = 10;

#endif
