#ifndef _G01_GRAPHICS_H_
#define _G01_GRAPHICS_H_

/** @defgroup graphics Graphic Constants
 * @{
 *
 * @brief Constants for accessing the graphics card.
 */

/** @brief The default video mode*/
#define MODE 0x14C
/** @brief How much darker a wall is on it's y side*/
#define SIDE_DARKEN_FACTOR 0.25 
/** @brief The max value one rgb component of a color can have*/
#define MAX_COLOR 255
/** @brief How much darker a wall is by how far it is, used in range 0 modes */
#define DISTANCE_DARKENING 4
/** @brief The distance it takes to start darkening something **/
#define START_DARKEN 1
/**@}*/

#endif
