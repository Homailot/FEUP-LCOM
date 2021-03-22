#ifndef _G01_INV_MENU_H_
#define _G01_INV_MENU_H_

#include <lcom/lcf.h>
#include <stdint.h>
#include "../lib/video_graphics.h"
#include "../utils/data_types.h"
#include "../guns/gun.h"

/** @defgroup inv_menu Inventory Menu
 * @{
 *
 * @brief Functions and data_types related to the inventory popup menu
 */

/**
 * @brief State of a popup menu
 */
enum menu_state{
    /// @brief Menu is closed
    CLOSED,
    /// @brief Menu is being displayed
    DISPLAYING
};

/**
 * @brief State of the inventory menu
 * @return State of the inventory menu
 */
enum menu_state get_inv_menu_state();

/**
 * @brief Display gun selection menu
 */
void display_menu(gun_instance_t * guns, int num_guns);

/**@}*/

#endif
