#ifndef _G01_INIT_LEVEL_H_
#define _G01_INIT_LEVEL_H_

#include <stdint.h>
#include <stdio.h>

/** @defgroup init_lvl Level Loader
 * @{
 *
 * @brief Functions to properly handle the game's memory while loading levels, games, and sprites.
 */

/** Identifies the last single player level **/
#define LAST_LVL 2

/**
 * @brief Returns level number to identify in the menu if there is a next level or not
 * The value -1 represents multiplayer
 * @return curr_lvl
 */
int8_t get_curr_lvl();

/**
 *
 * @brief Load modules of the game from the lib and initializes everything.
 *
 * @param t_bit_no Timer interrupt bit number.
 * @param ms_bit_no Mouse interrupt bit number.
 * @param kb_bit_no Keyboard interrupt bit number.
 * @param uart_bit_no UART interrupt bit number.
 * @param rtc_bit_no RTC interrupt bit number.
 * @return OK if no errors occurred.
 */
int (init_game)(uint8_t * t_bit_no, uint8_t * ms_bit_no, uint8_t * kb_bit_no, uint8_t * uart_bit_no, uint8_t * rtc_bit_no);

/**
 * @brief Loads a specified level.
 * @param num_map The number of the map to load.
 * @param mode The game mode.
 * @param player Player 1 or player 2.
 * @return Error code.
 */
int (load_level)(int num_map, int mode, int player);

/**
 * @brief Setups multiplayer game mode and level.
 *
 * @param player The player number.
 */
int (setup_multiplayer)(int player);

/**
 * @brief Free memory allocated by load level and setup_game_x
 * @return Error code
 */
int free_level();

/**
 * @brief Setup level 2
 */
void setup_game_2();

/**
 * @brief Free the memory allocated by load_game
 */
int free_game();
/**@}*/

#endif
