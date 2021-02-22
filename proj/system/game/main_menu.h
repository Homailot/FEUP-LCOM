#ifndef _G01_MAIN_MENU_H_
#define _G01_MAIN_MENU_H_

#include "../utils/data_types.h"


/** @defgroup main_menu Main Menu
 * @{
 *
 * @brief Functions to display the main menu and functionalities.
 */

/**
 * @brief Render the main menu
 */
void render_menu();

/**
 * @brief Renders the multiplayer connection screen.
 */ 
void render_mp_menu();

/**
 * @brief Renders a win/lose screen according to the player's condition
 */
void render_win_lose_screen();

/**
 * @brief Returns the date object stored in the main menu
 */
date_t * get_menu_date();

/**
 * @brief Renders the disconnected screen.
 */
void render_disconnect_screen();

/**@}*/

#endif
