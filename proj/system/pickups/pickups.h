#ifndef _G01_PICKUPS_H_
#define _G01_PICKUPS_H_

#include "../utils/utils.h"
#include "../lib/sprites.h"
#include "../player/player.h"
#include "../file/map.h"

/** @defgroup pickups Pickups
 * @{
 *
 * @brief Functions used to handle pickups.
 */

#define COLLISION_LIST_ALLOC_STEP 5

/**
 * @brief Represents a list of collisions (of pickups) in a certain position of a map. 
 */
typedef struct collision_list {
    /**
     * @brief a list of pickup indexes
     */ 
    size_t * indexes;
    /// @brief Number of pickups in that list
    size_t num_of_indexes;
    /// @brief Max number of pickups in that list
    size_t max_indexes;
} collision_list_t;

/// @brief Line of a collision table
typedef collision_list_t* collision_line_t;

/**
 * @brief Collision table
 */
typedef struct collision_table {
    /// @brief Array of collision lines
    collision_line_t* table;
    /// @brief Width and height of table
    size_t width, height;
} collision_table_t;

// IDEAS FROM https://stackoverflow.com/questions/8194250/polymorphism-in-c

/**
 * @brief Struct that represents a virtual table with 'virtual'-like functions.
 */
typedef struct pickup_vtable {
    bool (*handler)(player_info_t * player, void * pickup_details);
} pickup_vtable_t;

/**
 * @brief Base class for pickups, has a virtual table for calling functions for different types of pickups.
 */
typedef struct pickup {
    /// @brief Function associated to that pickup
    const pickup_vtable_t *vtable;
    //// @brief True if it was already used
    bool picked_up;
    /// @brief Circle collider to detect if the player is within its range
    circle_t collider;
    /// @brief Animation of the pickup
    animated_sprite_t animation;
    /**
     * @brief pointer to a struct that has information to be used in the pickup handler function.
     */
    void * pickup_details;
} pickup_t;

/**
 * @brief struct that represents the ammo pickups.
 */
typedef struct ammo {
    /**
     * @brief the packets this pickup has.
     */
    uint16_t packets;
    /**
     * @brief the type of ammo.
     */
    uint16_t ammo_index;
} ammo_t;

/**
 * @brief Medkit struct
 */
typedef struct medkit {
    /// @brief Health to be added to the player
    uint16_t health;
} medkit_t;

/**
 * @brief Armor stuct
 */
typedef struct armor {
    /// @brief Armor to be added to the player
    uint16_t armor;
} armor_t;

/**
 * @brief Function wrapper for calling a pickup handler based on its type using the chosen virtual table
 *
 * @param pickup the item being picked up
 * @param player the player picking up the item
 * @return returns true if item was picked up, false if item was not picked up
 */
static inline bool pickup_handler(player_info_t * player, pickup_t * pickup) {
    return pickup->vtable->handler(player, pickup->pickup_details);
}

/**
 * @brief Gathers all possible virtual tables to be used in other files
 */
extern const pickup_vtable_t AMMO[], MEDKIT[], ARMOR[];

/**
 * @brief Adds ammo to the pickups
 *
 * @param position pointer to position in map
 * @param radius collider radius of pickup
 * @param ammo_index the type of ammo
 * @param packets ammount of packets added
 * 
 * @return returns the index of the pickup.
 */
size_t add_ammo(vector_t * position, double radius, uint16_t ammo_index, uint16_t packets);

/**
 * @brief Adds medkit to the pickups
 *
 * @param position pointer to position in map
 * @param radius collider radius of pickup
 * @param health health added.
 * 
 * @return returns the index of the pickup.
 */
size_t add_medkit(vector_t * position, double radius, uint16_t health);

/**
 * @brief Updates pickup animations
 */
void (update_pickups)();

/**
 * @brief Adds armor to the pickups
 *
 * @param position pointer to position in map
 * @param radius collider radius of pickup
 * @param armor_quantity quantity of armor added.
 * 
 * @return returns the index of the pickup.
 */
size_t add_armor(vector_t * position, double radius, uint16_t armor_quantity);

/**
 * @brief Initializes the pickup collision table
 *
 * @param width width of the map
 * @param height height of the map
 * 
 * @return 0 if ok, non-zero if not.
 */
int init_collision_table(size_t width, size_t height);

/**
 * @brief Frees resources used up by the collision table.
 */
void free_collision_table();

void print_table();

/**
 * @brief Checks if a player can pick up an item.
 *
 * @param player pointer to a player structure
 * @return returns the index of the closest pickup
 */
size_t check_player_pickup_collision(player_info_t * player);

/**
 * @brief Picks up an item with a given index.
 *
 * @param player pointer to a player structure
 * @param index pickup index
 * 
 * @return returns true if picked up
 */
bool pickup_index(player_info_t * player, unsigned index);

/**
 * @brief Adds pickup to pickup array and sets its position in the collision table
 *
 * @param pickup the pickup being added
 * @return returns the index of the pickup
 */
size_t add_pickup(pickup_t * pickup);

/**
 * @brief Gets a copy of a pickup from the pickup array given its index.
 *
 * @param index index of the pickup
 * @return returns the pickup with that index or a dummy pickup, if it does not exist.
 */
pickup_t get_pickup(unsigned index);

/**
 * @brief Gets the number of pickups + 1.
 * 
 * @return returns the number of pickups + 1.
 */
size_t (get_pickups_size)();

/**
 * @brief Removes a pickup from the collision table and ignores it in the pickup array
 *
 * @param index index of the pickup
 */
void delete_pickup(unsigned index);

/**
 * @brief Frees resources used up by the pickup array.
 */
void free_pickups();

/**@}*/

#endif