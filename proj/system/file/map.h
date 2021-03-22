#ifndef _G01_MAP_H_
#define _G01_MAP_H_

#include <lcom/lcf.h>
#include <stdint.h>

/** @defgroup map Map
 * @{
 *
 * @brief Functions and data types related to map loading and destruction.
 */

/**
 * @brief Map matrix.
 *
 * Zero indicates ground and its texture index. A positive value indicates a wall and its texture index.
 * A negative value indicates a special block with properties like finishing the game.
 */
typedef struct map {
    int32_t** matrix;
    uint16_t lines, columns;
} map_t;

/**
 * @brief Loads a map from a file given a map number.
 *
 * @param mapNumber the number of the map that identifies it.
 * @param map the loaded map
 *
 * @returns OK if no errors occurred.
 */
int(load_map)(uint16_t mapNumber, map_t* map);

/**
 * @brief Correctly frees up memory used up by map.
 *
 * @param map the loaded map
 *
 * @returns OK if no errors occurred.
 */
void(close_map)(map_t* map);
/**@}*/

#endif
