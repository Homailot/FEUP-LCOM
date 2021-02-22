#ifndef _G01_PLAYER_H_
#define _G01_PLAYER_H_

#include "../controls/controls.h"
#include "../file/map.h"
#include "../utils/data_types.h"
#include "../lib/sprites.h"
#include "../guns/gun.h"
#include "../utils/utils.h"
#include <math.h>
#include <lcom/xpm.h>

/** @defgroup player Player
 * @{
 *
 * @brief Functions to do operations related to player
 */

 /**
  * @brief Enumerated type for specifying the timer value initialization
  */

#define PLAYER_SPEED 0.05
#define STRAFE_SPEED 0.025

/**
 * @brief Enum that identifies types of players to choose the controls
 */
typedef enum player_type{
    /// @brief Normal controlled player
    PLAYER,
    /// @brief Player controlled by network
    NETWORK_PLAYER,
    /// @brief Player controlled by the system
    BOT
} player_type_t;

typedef struct player_info {
    /// @brief Controls object of the player
    controls_t movm;
    /// @brief Position of player
    vector_t position;
    /// @brief Direction of player
    vector_t direction;
    /// @brief Camera plane of player - Used to controll the view [Used in case of type PLAYER]
    vector_t camera_plane;
    /// @brief Variable indicating the collision radius of the player
    double radius;
    /// @brief Health of the player
    int health;
    /// @brief Max health of the player
    int max_health;
    /// @brief Armor of the player
    int armor;
    /// @brief Max armor of the player
    int max_armor;
    /// @brief Array of guns
    gun_instance_t * guns;
    /// @brief Player speed
    double normal_speed;
    /// @brief Player crouch speed
    double crouch_speed;
    /// @brief Player run speed
    double run_speed;
    /// @brief Player gun index being used
    int gun_index;
    /// @brief Player inventory size
    int inventory_size;
    /// @brief Player's current animations [Only used to draw other players]
    int curr_anim;
    /// @brief Number of animations
    int num_anims;
    /// @brief Type of player
    player_type_t type;
    /// @brief Variable indicating if the player should be rendered or not
    bool should_render;
    /// @brief 2D array of sprites. First dimension is the animation selection. Second dimension is the view (8 sides)
    animated_sprite_t ** sprites;
} player_info_t;

/**
 * @brief Returns player's 1 pointer
 * @return pointer to player
 */
player_info_t * get_player();

/**
 * @brief Returns player's 2 (mulitplayer) pointer
 * @return pointer to player 2
 */
player_info_t * get_mp_enemy_player();

/**
 * @brief Initializes player controls to use in menus
 */
void init_player_ctrls();

/**
 * @brief Reset player's mouse desl to 0
 * @param ply Player to apply
 */
int (reset_player_mouse_movm)(player_info_t * ply);

/**
 * @brief Initializes player animations
 * 
 * @param p The player
 */
int (init_player_anims)(player_info_t * p);

/**
 * @brief Frees the memory allocated by init_player_anims()
 * 
 * @param p The player
 */
int (free_player_anims)(player_info_t * p);

/**
 * @brief Function to construct a player or bot
 * 
 * @param type Whether it is a bot or a player
 * @param position Starting position in the map
 * @param direction Starting look direction
 * @param radius Radius of the player's collider
 * @param health Health of the player
 * @param max_health Maximum health of the player
 * @param armor Armor of the player
 * @param max_armor Maximum armor of the player
 * @param num_guns Number of weapons in the inventory
 * @param guns Guns to add to inventory
 */
void init_player(player_type_t type, vector_t * position, vector_t * direction, double radius, int health, int max_health, int armor, int max_armor, gun_instance_t * instances, int num_guns);

/**
 * @brief Frees the memory allocated by init_player
 */
void free_players();

/**
 * @brief Verifies if the second player was hit by a shot taken by p1
 * Calculates tangent points of p2 and verifies direction of p1 and checks if its within bounds of those thangent lines
 * 
 * @param dist [out] If p2 was hit, returns the distance of the player
 * @param p1 Player who shot the gun
 * @param p2 Player to check if was shot
 */
bool (hit_player)(double* dist, player_info_t * p1, player_info_t * p2);

/**
 * @brief Player int handler
 * Analyses controls and moves, shoots and select animations that should be played according to them
 * 
 * @param ply Player to affect the changes
 */
void(player_int_ih)(player_info_t * ply);

/**
 * @brief Handle bots events
 */
void(bot_int_ih)();

/**
 * @brief Function to switch the gun
 * @param ply Current player
 * @param index The index of the weapon (0 to 2)
 */
void(switch_gun)(player_info_t * ply, int index);

/** @}*/

#endif
