#ifndef _G01_ENEM_CONTROL_H_
#define _G01_ENEM_CONTROL_H_

#include "../lib/sprites.h"
#include "../utils/data_types.h"
#include <lcom/lcf.h>
#include <lcom/proj.h>
#include <stdint.h>
#include "../player/player.h"

/** @defgroup enemy_controller Enemy AI Controllers
 * @{
 *
 * @brief Functions used to control enemies.
 */

/**
 * @brief Structure that controls the enemies
 */
typedef struct enemy_ctrl{
    /// Whether is a random position or predefined
    bool random;
    /// Array of predefined positions
    vector_t * positions;
    /// Current index of array
    int curr_idx;
    /// Current destination
    vector_t curr_dest;
    /// Size of array
    uint8_t num_positions;
    /// Last operation done
    time_t last_op;
    /// If he cant reach the defined position, advances to the next one
    time_t timeout_movm;
    /// Range of sight
    double range;
    /// FOV In degrees
    double fov;
    player_info_t * player;

} enemy_ctrl_t;

/**
 * @brief Initialize an AI controller
 * 
 * @param enemy The player to control
 * @param random Follows random positions on predefined ones
 * @param timeout_move The time it takes to complete the operation if the destination isn't reached
 * @param fov The max angle the enemy can spot the player
 * @param range The max distance the enemy can spot the player
 * @param num_positions The number of predefined positions
 * @param pos Array containing the predefined positions
 */
void init_enemy(player_info_t * enemy, bool random, time_t timeout_move, double fov, double range, int num_positions, vector_t * pos);

/**
 * @brief Free the memory allocated by init_enemy()
 */
void free_controllers();

/**
 * @brief Handles all of the enemies
 * Calls handle_enemy in each of the controllers to do operations over them
 */
void handle_enemies();
/**@}*/

#endif
