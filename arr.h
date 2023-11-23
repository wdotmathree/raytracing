#include "consts.h"
#include "vec.h"

typedef struct vec3_arr {
	vec3 *data;
	size_t size;
	size_t capacity;
} vec3_arr;

/**
 * @brief Initialize a vec3_arr
 *
 * @return The initialized vec3_arr
 */
vec3_arr vec3_arr_init(void);

/**
 * @brief Reserve space in a vec3_arr
 *
 * @param arr The vec3_arr to reserve space for
 */
void vec3_arr_reserve(vec3_arr *, size_t);

/**
 * @brief Add an element to the end of a vec3_arr
 *
 * @param arr The vec3_arr to add to
 * @param x The element to add
 */
void vec3_arr_push(vec3_arr *, const vec3 *);

/**
 * @brief Remove an element from the end of a vec3_arr
 *
 * @param arr The vec3_arr to remove from
 * @param out The removed element
 */
void vec3_arr_pop(vec3_arr *, vec3 *);

/**
 * @brief Clear the vec3_arr
 *
 * @param arr The vec3_arr to clear
 */
void vec3_arr_clear(vec3_arr *);

/**
 * @brief Free the memory used by a vec3_arr
 *
 * @param arr The vec3_arr to free
 */
void vec3_arr_free(vec3_arr *);
