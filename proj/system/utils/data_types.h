#ifndef _G01_DATA_TYPES_H_
#define _G01_DATA_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

/** @defgroup data_types Data types
 * @{
 *
 * @brief Structures defining important types
 */


/**
 * @brief Structure that simulates a 2D vector
 */
typedef struct vector {
    double x, y;
} vector_t;

/**
 * @brief Structure that representes positive coordinates
 * Good to use as a screen position
 */
typedef struct coordinate {
    uint16_t x, y;
} coordinate_t;

/**
 * @brief Structure that represents a pair of long values
 */
typedef struct pair {
    long x, y;
} pair_t;

/// @brief Variable that identifies a tick variable, used by frames, animations, guns
typedef uint64_t tick_t;

/**
 * @brief Structure that simulates an integer represented by 24 bits
 */
typedef struct int24 {
    unsigned char bytes[3];
} int24_t;

/**
 * @brief Structure that defines a color with its 3 rgb components
 */
typedef struct color {
    uint8_t rgb[3];
} color_t;

/**
 * @brief Structure that defines a circle according to its center and radius
 */
typedef struct circle {
    vector_t center;
    double radius;
} circle_t;

/**
 * @brief Strcuture that defines a rectangle according to the top left corner position and its dimensions
 */
typedef struct rectangle {
    vector_t left_corner;
    int width, height;
} rectangle_t;

/**
 * @brief Custom sprite structre
 */
typedef struct sprite {
    /// @brief Position on the screen
    coordinate_t screen_pos;
    /// @brief Dimensions of the sprite
    coordinate_t texture_size;
    /// @brief Corresponding invisible xpm color
    uint32_t invis_color;
    int x_offset;
    int y_offset;
    /// @brief Array of colors
    uint32_t * map;
} sprite_t;

/**
 * @brief Verical texture line
 */
typedef struct line_tex {
    /// @brief True if the line should be darkened
    bool darken;
    /// @brief Index of x texture
    int tex_hit_x;
    /// @brief Index of y texture
    double tex_y;
    /// @brief Step to skip/extend colors
    double step;
    int last_tex_y;
    /// @brief What side of the wall [Useful to put a different color on both walls to be distinguishable]
    int side;
    /// @brief Value to darken
    double darken_factor;
    /// @brief Texture of the line
    sprite_t * tex;
} line_tex_t;

/**
 * @brief Vertical rendering line
 */
typedef struct line {
    /// @brief Highest value to be drawn
    int draw_higher;
    /// @brief lowest value to be drawn
    int draw_lower;
    /// @brief Texture containing sprites and colors
    line_tex_t texture;
    /// @brief Color of the line
    uint32_t color;
    /// @brief Distance to the camera
    double dist;
} line_t;

/**
 * Structure containing date information
 */
typedef struct date {
    uint8_t year,month,day,hour,min,sec;
} date_t;

/** @}*/

#endif
