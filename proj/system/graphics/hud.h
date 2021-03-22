#ifndef _G01_HUD_H_
#define _G01_HUD_H_

#include <lcom/lcf.h>
#include <lcom/proj.h>
#include <stdint.h>

#include "../player/player.h"

/** @defgroup hud HUD
 * @{
 *
 * @brief Functions for rendering the HUD (heads-up display or user interface).
 */

/**
 * @brief Renders the user interface in-game.
 * @param ply The pointer to a player, whose information will be used to render various items in the HUD.
 */
void render_hud(player_info_t * ply);

/**@}*/

#endif
