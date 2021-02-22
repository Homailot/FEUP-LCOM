#ifndef __G01_SPRITE_H
#define __G01_SPRITE_H

#include "../utils/data_types.h"
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

/** @defgroup sprites Sprites
 * {@
 * 
 * @brief Functions related to sprites
 */

#define MAX_SPRITE_WIDTH 80
#define MAX_SPRITE_HEIGHT 80

/**
 * @brief Animation state
 */
typedef enum anim_state{
    /// @brief Finished playing the animation
    FINISHED,
    /// @brief Animation in progress
    PROGRESS
} anim_state_t;

/**
 * @brief Struct that describes an animated sprite
 */
typedef struct animated_sprite {
    /// @brief Current sprite of 
    sprite_t * sprite;
    /// @brief Number of frames per sprite
    int frames_per_sprite; 
    /// @brief Current frame of the animation
    int cur_frame;
    /// @brief Number of sprites
    int num_fig;
    /// @brief Current state of the animation [Only used by some specific animations that require this information]
    anim_state_t state;
    /// @brief Array of sprites
    sprite_t **frames;
} animated_sprite_t;

/**
 * Loads all sprites necessary for the project
 * @return Error code
 */
int load_sprites();

/**
 * Frees all memory allocated by load_sprites
 */
int free_sprites();

/**
 * Updates an animation
 * 
 * @param anim Pointer to the animation
 * @returns true if animation finished
 */
bool (update_anim)(animated_sprite_t * anim);

/**
 * @brief Loads a sprite from a xpm data type
 * 
 * @param map Image in xpm_map_t
 * @param sprite Sprite pointer
 */
int (load_sprite)(xpm_map_t map, sprite_t * sprite);

/**
 * @brief Loads, allocates and stores the sprites relevant to the background
 */
int load_background();

/**
 * @brief Frees the memory allocated by load_background()
 */
int free_background();

/**
 * @brief Loads, allocates and stores the sprites relevant to the menu
 */
int (load_menu_sprites)();

/**
 * @brief Frees the memory allocated by load_menu_sprites()
 */
int (free_menu_sprites)();

/**
 * @brief Loads, allocates and stores the sprites relevant to the player
 */
int (load_player_sprites)();

/**
 * @brief Loads, allocates and stores the sprites relevant to the pickups
 */
int (load_pickup_sprites)();

/**
 * @brief Loads, allocates and stores the sprites relevant to the guns
 */
int (load_gun_sprites)();

/**
 * @brief Loads, allocates and stores the sprites relevant to the guns on the menu
 */
int (init_gun_menu_sprites)();

/**
 * @brief Frees the memory allocated by init_gun_menu_sprites()
 */
int (free_gun_menu_sprites)();

/**
 * @brief Frees the memory allocated by init_player_sprites()
 */
int (free_player_sprites)();

/**
 * @brief Initializes cursor sprite
 */
int (init_cursor_sprite)();

/**
 * @brief Frees the memory allocated by init_cursor_sprite
 */
int (free_cursor_sprite)();

/**
 * @brief Frees the memory allocated by load_pickup_sprites
 */
int (free_pickup_sprites)();

/**
 * @brief Frees the memory allocated by load_gun_sprites()
 */
int (free_gun_sprites)();

/**
 * @brief Loads, allocates and stores the sprites relevant to number characters
 */
int load_numbers_sprites();

/**
 * @brief Frees the memory allocated by load_numbers_sprites()
 */
int free_numbers_sprites();

/**
 * @brief Loads, allocates and stores the sprite relevant to the hud
 */
int load_hud_sprite();

/**
 * @brief Frees the memory allocated by load_hud_sprite()
 */
int free_hud_sprite();

/**
 * @brief Initialize wall textures
 */
int (init_walls_sprites)();

/**
 * @brief Free wall textures
 */
int (free_walls_sprites)();

#endif
