#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "consts.hpp"
#include "mat.hpp"
#include "transform.hpp"

special void translate(mat4 out, float x, float y, float z);

special void rotateX(mat4 out, float x);

special void rotateY(mat4 out, float y);

special void rotateZ(mat4 out, float z);

special void scale(mat4 out, float x, float y, float z);

special void project(mat4 out, float fov, float aspect, float near, float far);

special void to_screen(float3 *out, float3 *in);

#endif
