#ifndef _G01_GUN_H_
#define _G01_GUN_H_

#include "../lib/sprites.h"
#include "../utils/data_types.h"
#include <lcom/lcf.h>
#include <lcom/proj.h>
#include <stdint.h>

/** @defgroup gun GUN
 * @{
 *
 * @brief Functions and data_types related to the guns
 */

/**
 * @brief State of the weapon
 */
typedef enum gun_state{
    /// @brief Currently only stored in inventory
    INVENTORY,
    /// @brief Taken out of the inventory
    INITIAL,
    /// @brief Time after taking weapon from inventory
    CHARGING,
    /// @brief Ready to be shot
    READY,
    /// @brief Waiting time between shots
    COOLDOWN,
    /// @brief No ammo in gun
    NO_AMMO,
    /// @brief Reloading
    RELOADING
} gun_state_t;

/**
 * @brief Data structure that describes a gun type
 */
typedef struct gun{
    const char* name;
    /// @brief Time in ticks it takes the weapon to reload
    tick_t reload_time; // in ticks
    /// @brief Time in ticks it takes the weapon to use after selecting it
    tick_t charge_time; // in ticks
    /// @brief Time in ticks it takes between each shot
    unsigned cooldown_time; //in ticks
    /// @brief Range of the weapon in block distance
    double range; // in blocks
    /// @brief Damage of the weapon
    int damage;
    /// @brief Ammo per packet
    unsigned short ammo_per_packet;
    /// @brief Ammo shot per fire
    unsigned short ammo_per_fire;
    /// @brief Max ammo packets in the inventory
    unsigned short max_packets;
    /// @brief True if automatic weapon. If false, the weapon needs to be tapped
    bool automatic;
    /// @brief Array of animation corresponding to the several states, eg.: shooting, idle
    animated_sprite_t * animations;
    /// @brief Sprite associated to the choosing weapon menu
    sprite_t * menu_sprite;
    /// @brief Number of animations
    int num_anims;
    /// @brief Type of ammo used
    int ammo_index;
    /// @brief Spread of the gun [Currently not implemented]
    bool spread;
    /// @brief True if the gun is a projectile launcher [Currently not implemented]
    bool projectile_launcher;
    /// @brief True if the gun is a burst type of gun [Currently not implemented]
    bool burst;

} gun_t;

/**
 * @brief Structure that describes an active gun
 */
typedef struct gun_instance {
    /// @brief Copy of gun
    gun_t gun;
    /// @brief Index of the gun in the array guns
    int gun_index;
    /// @brief Current animation being played
    int curr_anim;
    /// @brief Current ammo
    unsigned short curr_ammo;
    /// @brief Current ammo packets in inventory
    unsigned short curr_packets;
    /**
     * @brief Tick that describes the last operation
     * Useful to identify when a weapon is ready to be shot, reloaded
     */
    time_t last_op_tick;
    /// @brief Current state of the gun_instance
    gun_state_t state;
} gun_instance_t;


/**
 * @brief Function called to update a gun's state
 * Call this function upon after trying to shoot a bullet
 * @param gun Gun instance pointer
 * @param shot If the gun was shot or not
 */
void update_gun_state(gun_instance_t * gun, bool shot, bool player);

/**
 * @brief Adds packets to the gun
 * @param gun Gun instance pointer
 * @param num_packets Number of packets to be added
 */
void add_packets(gun_instance_t * gun, unsigned int num_packets);

/**
 * @brief Function to reload the gun
 * @param Gun pointer
 */
void (reload_gun)(gun_instance_t * gun);

/**
 * @brief Sets guns to default settings and adds them to gun array.
 * 
 * @returns 0 on success.
 */
int (init_guns)();

/**
 * @brief Gets a gun from the gun array.
 * 
 * @param gun_index index of the gun array.
 * 
 * @returns a copy of the gun type.
 */ 
gun_t (get_gun)(int gun_index);

/**
 * @brief Creates a gun instance to be used in a player.
 * 
 * @param gun_index index of the gun.
 * @param start_ammo starting ammunition
 * @param start_packets starting number of ammo packets
 * 
 * @returns a copy of the gun type.
 */ 
gun_instance_t (create_gun_instance)(int gun_index, int start_ammo, int start_packets);

int (free_gun_anims)(gun_t * g);

/**
 * @brief Initialize gun animation
 * 
 * @param g Pointer to the corresponding gun
 * @param anim_speed_multiplier Value to multiply the maximum number of ticks calculated per sprite. Should be between ]0, 1]
 */
int (init_gun_anims)(gun_t * g, double anim_speed_multiplier);

/**
 * @brief Initialize rifle animation
 * 
 * @param g Pointer to the corresponding gun
 * @param anim_speed_multiplier Value to multiply the maximum number of ticks calculated per sprite. Should be between ]0, 1]
 */
int (init_rifle_anims)(gun_t * g, double anim_speed_multiplier);

/**
 * @brief Initialize ray animation
 * 
 * @param g Pointer to the corresponding gun
 * @param anim_speed_multiplier Value to multiply the maximum number of ticks calculated per sprite. Should be between ]0, 1]
 */
int (init_ray_anims)(gun_t * g, double anim_speed_multiplier);

/**@}*/

#endif
