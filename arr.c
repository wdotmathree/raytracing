#include "arr.h"

vec3_arr vec3_arr_init(void) {
	return (vec3_arr){NULL, 0, 0};
}

vec3_arr *vec3_arr_new(void) {
	vec3_arr *arr = malloc(sizeof(vec3_arr));
	*arr = vec3_arr_init();
	return arr;
}

void vec3_arr_reserve(vec3_arr *arr, const size_t n) {
	if (arr->capacity < n) {
		arr->capacity = n;
		if (arr->data)
			arr->data = realloc(arr->data, arr->capacity * sizeof(vec3));
		else
			arr->data = malloc(arr->capacity * sizeof(vec3));
	}
}

void vec3_arr_push(vec3_arr *arr, const vec3 *x) {
	if (arr->capacity == 0) {
		arr->capacity = 1;
		arr->data = malloc(sizeof(vec3));
	} else if (arr->capacity < arr->size + 1) {
		arr->capacity *= 2;
		arr->data = realloc(arr->data, arr->capacity * sizeof(vec3));
	}
	arr->data[arr->size++] = *x;
}

void vec3_arr_pop(vec3_arr *arr, vec3 *out) {
	*out = arr->data[--arr->size];
}

void vec3_arr_clear(vec3_arr *arr) {
	arr->size = 0;
}

void vec3_arr_free(vec3_arr *arr) {
	free(arr->data);
	memset(arr, 0, sizeof(vec3_arr));
}
