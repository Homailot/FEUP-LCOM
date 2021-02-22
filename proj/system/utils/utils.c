#include "data_types.h"
#include "../file/map.h"
#include <math.h>
#include <lcom/lcf.h>
#include <lcom/utils.h>
#include <stdint.h>

#include "utils.h"

extern map_t map;

int(util_sys_inb)(int port, uint8_t* value) {

    uint32_t tempAddress = 0;

    if (sys_inb(port, &tempAddress) != 0)
        return 1;

    *value = (uint8_t)(tempAddress);

    return 0;
}

void(int_to_24)(int orig, int24_t* result) {
    for (size_t i = 0; i < 3; i++) {
        result->bytes[i] = orig >> (i * 8) & 0xff;
    }
}

uint32_t (i24_to_int)(int24_t* orig) {
    uint32_t result = 0;

    for (size_t i = 0; i < 3; i++) {
        result |= ( uint32_t) (orig->bytes[i] << i*8);
    }

    return result;
}

size_t digitize(int num, size_t max_size, int * res){
    
    size_t digits = 0;
    while (num > 0)
    {
        res[max_size - 1 - digits] = num%10;
        num /= 10;
        digits++;
    }

    return max_size;
}

double prod_int(vector_t* v1, vector_t* v2) {
    return v1->x * v2->x + v1->y * v2->y;
}

double degree(vector_t* v1, vector_t* v2) {
    //double ang = acos(prod_int(v1, v2) / (vec_mag(v1) * vec_mag(v2)));

    double ang = atan2(determinant(v1,v2), prod_int(v1, v2));

    ang = ang * 180 / M_PI;

    //if (v2->y < 0)
    //    ang += 180;

    return ang;
}

double vec_mag(vector_t* v1) {
    return sqrt(pow(v1->x, 2) + pow(v1->y, 2));
}

double vec_mag_squared(vector_t* v1) {
    return pow(v1->x, 2) + pow(v1->y, 2);
}

void normalize(vector_t* v1) {
    double mag = vec_mag(v1);
    v1->x = v1->x / mag;
    v1->y = v1->y / mag;

}

double dist_positions(vector_t* v1, vector_t* v2) {
    return sqrt(pow(v1->x - v2->x, 2) + pow(v1->y - v2->y, 2));
}

double dist_positions_squared(vector_t* v1, vector_t* v2) {
    return pow(v1->x - v2->x, 2) + pow(v1->y - v2->y, 2);
}

bool check_circle_rect_collision(circle_t * circle, rectangle_t * rect) {
    vector_t closest;

    // Find the closest x position of the rectangle from the circle
    if(circle->center.x < rect->left_corner.x) closest.x = rect->left_corner.x;
    else if (circle->center.x > rect->left_corner.x + rect->width) closest.x = rect->left_corner.x + rect->width;
    else closest.x = circle->center.x;

    // Find the closest y position of the rectangle from the circle
    if(circle->center.y < rect->left_corner.y) closest.y = rect->left_corner.y;
    else if (circle->center.y > rect->left_corner.y + rect->height) closest.y = rect->left_corner.y + rect->height;
    else closest.y = circle->center.y;

    if(dist_positions_squared(&(circle->center), &closest) < (double)(circle->radius * circle->radius)) {
        return true;
    }

    return false;
}

void circle_rect_distance(circle_t * circle, rectangle_t * rect, vector_t * distances) {
    vector_t closest;

    // Find the closest x position of the rectangle from the circle
    if(circle->center.x < rect->left_corner.x) closest.x = rect->left_corner.x;
    else if (circle->center.x > rect->left_corner.x + rect->width) closest.x = rect->left_corner.x + rect->width;
    else closest.x = circle->center.x;

    // Find the closest y position of the rectangle from the circle
    if(circle->center.y < rect->left_corner.y) closest.y = rect->left_corner.y;
    else if (circle->center.y > rect->left_corner.y + rect->height) closest.y = rect->left_corner.y + rect->height;
    else closest.y = circle->center.y;

    distances->x = closest.x - circle->center.x;
    distances->y = closest.y - circle->center.y;
}

double determinant(vector_t* v1, vector_t* v2) {
    return v1->x * v2->y - v1->y * v2->x;
}

double abs_double(double val){
    if(val < 0) val *= -1.0;
    return val;
}

void(rotate_vec(vector_t * vec, double ang)){
    // Converts degrees to radians
    ang = ang*M_PI/180;
    double x = vec->x * cos(ang) - vec->y * sin(ang);
    double y = vec->x * sin(ang) + vec->y * cos(ang);

    vec->x = x;
    vec->y = y;
}

vector_t new_vec(double x, double y){
    vector_t vec;
    vec.x = x;
    vec.y = y;
    return vec;
}

void(shoot_ray)(vector_t* pos, vector_t* ray_dir,  ray_hit_t* ray_hit, double range) {

    // represents the length from the current position to the next side of the map (next position/block)
    double side_dist_x, side_dist_y;

    // position in map of ray starting position, using floor function
    coordinate_t map_pos;
    map_pos.x = (int)pos->x, map_pos.y = (int)pos->y;

    // length from one y or x side to another y or x side
    double delta_dist_x = fabs(1 / ray_dir->x);
    double delta_dist_y = fabs(1 / ray_dir->y);

    // the direction the dda algorithm goes in (is -1 or 1). if there is no direction this variable is unused.
    int step_x, step_y;
    if (ray_dir->x < 0) {
        step_x = -1;
        side_dist_x = (pos->x - map_pos.x); // distance to left side
    }
    else {
        step_x = 1;
        side_dist_x = (map_pos.x + 1 - pos->x); // distance to right side
    }
    side_dist_x *= delta_dist_x; // get length of ray to side

    if (ray_dir->y < 0) {
        step_y = -1;
        side_dist_y = (pos->y - map_pos.y); // distance to upper side
    }
    else {
        step_y = 1;
        side_dist_y = (map_pos.y + 1 - pos->y); // distance to bottom side
    }
    side_dist_y *= delta_dist_y; // get length of ray to side

    // identifies if ray hit a wall and it's color
    ray_hit->hit = 0;
    ray_hit->distance = 0;
    // identifies the side of the wall that was hit (0 hit on x side, 1 hit on y side) (for shading purposes)
    //side;

    // DDA algorithm. in each step the algorithm jumps from one space in the map to the other (be it the next side on the x coord or
    // the y coord). this is used to correctly check if a ray hits a wall and get the length of the ray, without needing to check
    // in infinitely small points.
    while (ray_hit->hit == 0) {
        if (side_dist_x < side_dist_y) {
            map_pos.x += step_x;
            ray_hit->distance = side_dist_x;
            side_dist_x += delta_dist_x;
            ray_hit->side = 0;
        }
        else {
            map_pos.y += step_y;
            ray_hit->distance = side_dist_y;
            side_dist_y += delta_dist_y;
            ray_hit->side = 1;
        }

        if (map_pos.y >= map.lines || map_pos.x >= map.columns)
            break;

        if (range > 0 && ray_hit->distance > range)
            break;

        if (map.matrix[map_pos.y][map_pos.x] > 0)
            ray_hit->hit = 1;
    }

    ray_hit->map_pos = map_pos;
}
