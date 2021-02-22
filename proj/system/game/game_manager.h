#ifndef _G01_GAME_MANAGER_H_
#define _G01_GAME_MANAGER_H_

#include<stdint.h>


/** @defgroup game_manager Game Manager
 * @{
 *
 * @brief Functions that change the state of the game.
 */

#define ROUND_TIMER 160


/// @brief The different game states.
typedef enum game_state{
    /// @brief Game is in the main menu.
    MAIN_MENU,
    /// @brief Game is in the multi player screen (waiting for connection).
    MP_MENU,
    /// @brief Game is running in the single player mode.
    GAME,
    /// @brief Game is running in the multi player mode.
    MP_GAME,
    /// @brief Game is in the Lose/Win screen.
    DEAD_MENU,
    /// @brief Game is in the Next level/Single player win screen.
    NEXT_LVL_MENU,
    /// @brief A request to end the single player game has been made.
    END_S_GAME,
    /// @brief A request to end the multi player game has been made.
    END_M_GAME,
    /// @brief Game is in the win screen.
    WIN_GAME,
    /// @brief Game is in the lose screen.
    LOSE_GAME,
    /// @brief The other player in multi player mode has disconnected.
    DISCONNECT,
    /// @brief A request to close the game has been made.
    EXIT
} game_state_t;

/// @brief The current game state.
static game_state_t state;

/**
 * @brief Gets the current game state.
 * @return The current game state.
 */
game_state_t get_state();

/**
 * @brief Returns memory addres of round timer
 * @return round timer
 */
int * get_round_timer();

/**
 * @brief Starts the game loop.
 *
 * Allocates all needed memory and loads in everything.
 *
 * @param map_num The number of the map being loaded.
 * @param game_mode The mode (single player -- 1, multi player -- 2)
 * @param player The number of the player who is playing.
 * @return
 */
int gm_init_game(int map_num, int game_mode, int player);

/**
 * @brief Closes the multi player game
 *
 * Frees all possible memory used up.
 */
void gm_reset_mp_game();

/// @brief Updates the game state based on actions made by the players.
void update_game_state();

/**
 * @brief Sets the game state.
 * @param state The new game state.
 */
void set_state(game_state_t state);

/**
 * @brief Handles the game's functions and changes the game's state according to input and current state.
 */
void (game_handler)();

/**
 * @brief Handles interrupts and is essentially the main game loop.
 * 
 * @param mouse_bit_no Timer interrupt bit number.
 * @param kb_bit_no Mouse interrupt bit number.
 * @param timer_bit_no Keyboard interrupt bit number.
 * @param uart_bit_no UART interrupt bit number.
 * @param rtc_bit_no RTC interrupt bit number.
 */
void(interrupt_handler)(uint8_t mouse_bit_no, uint8_t kb_bit_no, uint8_t timer_bit_no, uint8_t uart_bit_no, uint8_t rtc_bit_no);


/**@}*/

#endif
