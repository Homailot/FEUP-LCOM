#pragma once

#include "data_types.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

/** @defgroup utils Auxiliary functions
 * @{
 *
 * @brief Functions that auxiliate other methods
 */

#ifndef BIT
#  define BIT(n) (1 << (n))
#endif

/**
 * @brief structure with information about a shot ray.
 */
typedef struct ray_hit {
    /**@brief Indicates if the ray hit a wall */
    int hit;
    /**@brief If the ray hit the wall on its x side (0) or on its y side(1) */
    int side;
    /**@brief Coordinates on the map to the wall that was hit */
    coordinate_t map_pos;
    /**@brief Distance from the ray's starting position to its last. To get the actual distance (in blocks),
      * because of an optimization in the ray shooting algorithm, it is necessary to multiply this value by
      * the magnitude of the ray's direction vector.*/
    double distance;
} ray_hit_t;

/**
 * @brief Invokes sys_inb() system call but reads the value into a uint8_t variable.
 *
 * @param port the input port that is to be read
 * @param value a pointer to the variable that is to be update with the value read
 * @return Return 0 upon success and non-zero otherwise
 */
int(util_sys_inb)(int port, uint8_t* value);

/**
 * @brief Converts an integer to int24_t
 * @param int Input value
 * @param [out] return value
 */
void(int_to_24)(int, int24_t*);

/**
 * @brief Converts int24_t to uint32_t
 * @param orig Input value
 * @return Converted value
 */
uint32_t (i24_to_int)(int24_t* orig);

/**
 * @brief Returns the digits of the number in an array
 * 
 * @param num The value to digitize
 * @param max_size Max digits - Truncates the first digits
 * @param res Array to fill
 */
size_t digitize(int num, size_t max_size, int * res);

/**
 * @brief Calculates the internal product of two vectors
 * @param v1 Pointer to first vector
 * @param v2 Pointer to second vector
 */
double prod_int(vector_t* v1, vector_t* v2);

/**
 * @brief Calculates the angle of two vectors between -180 and 180
 * @param v1 Pointer to first vector
 * @param v2 Pointer to second vector
 */
double degree(vector_t* v1, vector_t* v2);

/**
 * @brief Calculates the vector magnitude
 * @param v1 Pointer to first vector
 */
double vec_mag(vector_t* v1);

double vec_mag_squared(vector_t* v1);

/**
 * Normalizes a vector
 * @param v1 pointer to the input (and return) vector
 */
void normalize(vector_t* v1);

/**
 * @brief Calculates the distance between two positions.
 * @param v1 Pointer to first vector position
 * @param v2 Pointer to second vector position
 * 
 * @return the distance between positions.
 */
double dist_positions(vector_t* v1, vector_t* v2);

/**
 * @brief Calculates the squared distance between two positions.
 * @param v1 Pointer to first vector position
 * @param v2 Pointer to second vector position
 * 
 * @return the squared distance between positions.
 */
double dist_positions_squared(vector_t* v1, vector_t* v2);

/**
 * @brief Checks if a circle and a rectangle collide. SOURCE: https://www.lazyfoo.net/tutorials/SDL/29_circular_collision_detection/index.php
 * @param circle Pointer to a circle_t.
 * @param rect Pointer to a rectangle_t.
 * 
 * @return True if collision occurs, false if not.
 */
bool check_circle_rect_collision(circle_t * circle, rectangle_t * rect);

/**
 * @brief Finds the closest distances of a circle to a rectangle
 * 
 * @param circle Circle
 * @param rect Rectangle
 * @param [out] distances Resulting vector
 */
void circle_rect_distance(circle_t * circle, rectangle_t * rect, vector_t * distances);

/**
 * @brief Calculates the determinant of a 2x2 matrix
 * @param v1 Pointer to first the vector representing the first column
 * @param v2 Pointer to first the vector representing the first column
 */
double determinant(vector_t* v1, vector_t* v2);

/**
 * @brief Returns the absolute value of a double
 * @param val The value to return the absolute functions
 * @return Absolute value of a double as a double
 */
double abs_double(double val);

/**
 * @brief Rotates a vector counter clockwise by ang degrees
 * Formula discovered in https://en.wikipedia.org/wiki/Rotation_matrix
 * 
 * @param vec Vector to rotate
 * @param ang Angle in degrees to rotate
 */
void(rotate_vec(vector_t * vec, double ang));

/**
 * @brief Shoots a ray from a given position in a given direction.
 *
 * Uses the DDA algorithm (described in more detail here: https://lodev.org/cgtutor/raycasting.html) to traverse the map and check
 * collision with walls. Can be used with a range.
 *
 * @param pos pointer to the starting position of the ray
 * @param ray_dir pointer to the direction of the ray
 * @param ray_hit pointer to a ray_hit_t structure. The return of this function is stored here.
 * @param range the maximum range of the wall (in blocks), 0 if unlimited
 */
void(shoot_ray)(vector_t* pos, vector_t* ray_dir, ray_hit_t* ray_hit, double range);

/**
 * @brief Generates a new vector
 * @param x X value
 * @param y Y value
 * @return Converted vector_t
 */
vector_t new_vec(double x, double y);

/** @}*/
