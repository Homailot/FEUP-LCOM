#include "pickups.h"
#include "../lib/errors.h"
#include "../lib/sprites.h"
#include "../../data/sprites/pickups/pickup_sprites.h"

pickup_t * pickups = NULL;
collision_table_t collision_table;
size_t pickups_size = 0;

int (init_ammo_anims)(pickup_t * pickup, int ammo_index) {
    animated_sprite_t * anim = &(pickup->animation);
    if(ammo_index >= 3) ammo_index = 0; 
    sprite_t * ptr = ammos[ammo_index];

    anim->cur_frame = 0;
    anim->frames = malloc(sizeof(sprite_t *));
    if(anim->frames == NULL) return BAD_ALLOC;
    anim->frames[0] = ptr;
    anim->frames_per_sprite = 1;
    anim->num_fig = 1;
    anim->sprite = ptr;

    return OK;
}

int (init_medi_anims)(pickup_t * pickup) {
    animated_sprite_t * anim = &(pickup->animation);

    anim->cur_frame = 0;
    anim->frames = malloc(sizeof(sprite_t *));
    if(anim->frames == NULL) return BAD_ALLOC;
    anim->frames[0] = media;
    anim->frames_per_sprite = 1;
    anim->num_fig = 1;
    anim->sprite = media;

    return OK;
}

int (init_armor_anims)(pickup_t * pickup) {
    animated_sprite_t * anim = &(pickup->animation);

    anim->cur_frame = 0;
    anim->frames = malloc(2*sizeof(sprite_t *));
    if(anim->frames == NULL) return BAD_ALLOC;
    anim->frames[0] = arm1a;
    anim->frames[1] = arm1b;
    anim->frames_per_sprite = 10;
    anim->num_fig = 2;
    anim->sprite = arm1a;

    return OK;
}

/**
 * @brief Pickup handler for ammo pickups. Overrides virtual function
 */
static bool (pickup_ammo)(player_info_t * player, void * pickup_det) {
    ammo_t * ammo = (ammo_t *) pickup_det;

    for(int idx = 0; idx < player->inventory_size; idx++) {
        gun_instance_t * gun_inst = &(player->guns[idx]);

        if(gun_inst->gun.ammo_index == ammo->ammo_index) {
            if(gun_inst->curr_packets >= gun_inst->gun.max_packets) return false;

            gun_inst->curr_packets += ammo->packets;
            if(gun_inst->curr_packets > gun_inst->gun.max_packets) {
                gun_inst->curr_packets = gun_inst->gun.max_packets;
            }

            return true;
        }
    }

    return false;
}
/**
 * @brief sets the virtual table for ammunition pickups.
 */
const pickup_vtable_t AMMO[] = {{ &pickup_ammo }};

size_t add_ammo(vector_t * position, double radius, uint16_t ammo_index, uint16_t packets) {
    pickup_t new_pickup;
    ammo_t * ammo = malloc(sizeof(ammo_t));
    if(ammo == NULL) return 0;
    ammo->ammo_index = ammo_index;
    ammo->packets = packets;
    
    init_ammo_anims(&new_pickup, ammo_index);
    new_pickup.picked_up = false;
    new_pickup.collider.center = *position;
    new_pickup.collider.radius = radius;
    new_pickup.pickup_details = ammo;
    new_pickup.vtable = AMMO;
    
    return add_pickup(&new_pickup);
}

/**
 * @brief Pickup handler for medkit pickups. Overrides virtual function
 */
static bool (pickup_medkit)(player_info_t * player, void * pickup_det) {
    medkit_t * medkit = (medkit_t *) pickup_det;

    if(player->health >= player->max_health) return false;

    player->health += medkit->health;

    if(player->health >= player->max_health) player->health = player->max_health;

    return true;
}

/**
 * @brief sets the virtual table for medkit pickups.
 */
const pickup_vtable_t MEDKIT[] = {{ &pickup_medkit }};

size_t add_medkit(vector_t * position, double radius, uint16_t health) {
    pickup_t new_pickup;
    medkit_t * medkit = malloc(sizeof(medkit_t));
    if(medkit == NULL) return 0;
    medkit->health = health;
    
    init_medi_anims(&new_pickup);
    new_pickup.picked_up = false;
    new_pickup.collider.center = *position;
    new_pickup.collider.radius = radius;
    new_pickup.pickup_details = medkit;
    new_pickup.vtable = MEDKIT;
    
    return add_pickup(&new_pickup);
}

/**
 * @brief Pickup handler for armor pickups. Overrides virtual function
 */
static bool (pickup_armor)(player_info_t * player, void * pickup_det) {
    armor_t * armor = (armor_t *) pickup_det;

    if(player->armor >= player->max_armor) return false;

    player->armor += armor->armor;

    if(player->armor >= player->max_armor) player->armor = player->max_armor;

    return true;
}

/**
 * @brief sets the virtual table for armor pickups.
 */
const pickup_vtable_t ARMOR[] = {{ &pickup_armor }};

size_t add_armor(vector_t * position, double radius, uint16_t armor_quantity) {
    pickup_t new_pickup;
    armor_t * armor = malloc(sizeof(armor_t));
    if(armor == NULL) return 0;
    armor->armor = armor_quantity;
    
    init_armor_anims(&new_pickup);
    new_pickup.picked_up = false;
    new_pickup.collider.center = *position;
    new_pickup.collider.radius = radius;
    new_pickup.pickup_details = armor;
    new_pickup.vtable = ARMOR;
    
    return add_pickup(&new_pickup);
}

int (init_collision_table)(size_t width, size_t height) {
    collision_table.height = height;
    collision_table.width = width;

    collision_table.table = (collision_line_t *) malloc(height * sizeof(collision_line_t));
    if(collision_table.table == NULL) return BAD_ALLOC;
    for(size_t row = 0; row < height; row++) {
        collision_table.table[row] = (collision_list_t *) malloc(width * sizeof(collision_list_t));
        if(collision_table.table[row] == NULL) return BAD_ALLOC;
        for(size_t col = 0; col < width; col++) {
            collision_table.table[row][col].num_of_indexes = 0;
            collision_table.table[row][col].max_indexes = 0;
            collision_table.table[row][col].indexes = NULL;
        }
    }

    return OK;
}

void (update_pickups)() {
    for(size_t idx = 1; idx < pickups_size; idx++) {
        update_anim(&(pickups[idx].animation));
    }
}

void (free_collision_table)() {
    for(size_t row = 0; row < collision_table.height; row++) {
        for(size_t col = 0; col < collision_table.width; col++) {
            if(collision_table.table[row][col].indexes) {
                free(collision_table.table[row][col].indexes);
            }
        }
        free((collision_table.table[row]));
    }
    free(collision_table.table);
}

// ONLY WORKS WITH MAX CIRCLE RADIUS OF 1!!!
int (add_collision)(pickup_t * pickup, size_t pickup_index) {
    circle_t * collider = &(pickup->collider);
    rectangle_t rect;
    size_t table_index;
    pair_t start_coord = {(int) collider->center.x-1, (int) collider->center.y-1};
    pair_t end_coord = {(int) collider->center.x+1, (int) collider->center.y+1};

    if(start_coord.x < 0) start_coord.x = 0;
    if(end_coord.x >= (long)collision_table.width) end_coord.x = collision_table.width - 1;
    if(start_coord.y < 0) start_coord.y = 0;
    if(end_coord.y >= (long)collision_table.height) end_coord.y = collision_table.height - 1;

    for(long x = start_coord.x; x <= end_coord.x; x++) {
        for(long y = start_coord.y; y <= end_coord.y; y++) {
            rect.left_corner.x = x; rect.left_corner.y = y;
            rect.height = rect.width = 1;

            if(check_circle_rect_collision(collider, &rect)) {
                collision_list_t * list = &(collision_table.table[y][x]);
                table_index = list->num_of_indexes;
                
                if(list->num_of_indexes == 0) {
                    list->indexes = (size_t *) malloc(COLLISION_LIST_ALLOC_STEP*sizeof(size_t));
                    if(list->indexes == NULL) return BAD_ALLOC;
                    list->max_indexes += 5;
                } else if(list->num_of_indexes >= list->max_indexes) {
                    list->indexes = realloc((void *)list->indexes, (list->num_of_indexes+COLLISION_LIST_ALLOC_STEP) * sizeof(size_t));
                    if(list->indexes == NULL) return BAD_ALLOC;
                    list->max_indexes += 5;
                }

                list->indexes[table_index] = pickup_index;  
                list->num_of_indexes++;
            }
        }
    }

    return OK;
}

void (remove_collision)(vector_t * position, size_t pickup_index) {
    pair_t start_coord = {(int) position->x-1, (int) position->y-1};
    pair_t end_coord = {(int) position->x+1, (int) position->y+1};

    if(start_coord.x < 0) start_coord.x = 0;
    if(end_coord.x >= (long)collision_table.width) end_coord.x = collision_table.width - 1;
    if(start_coord.y < 0) start_coord.y = 0;
    if(end_coord.y >= (long)collision_table.height) end_coord.y = collision_table.height - 1;

    for(long x = start_coord.x; x <= end_coord.x; x++) {
        for(long y = start_coord.y; y <= end_coord.y; y++) {
            collision_list_t * list = &(collision_table.table[y][x]);

            for(size_t table_idx = 0; table_idx < list->num_of_indexes; table_idx++) {
                if(list->indexes[table_idx] == pickup_index) {
                    list->indexes[table_idx] = 0;
                }
            }
        }
    }
}

size_t (check_player_pickup_collision)(player_info_t * player) {
    size_t pickup_index = 0;
    double smallest_distance = -1;
    double distance;
    coordinate_t player_coord = {(int) player->position.x, (int) player->position.y};
    vector_t player_pos = player->position; 

    if(player_pos.x < 0 || player_pos.y < 0 || player_pos.y >= collision_table.height || player_pos.x >= collision_table.width) return 0;

    collision_list_t *list = &(collision_table.table[player_coord.y][player_coord.x]);
    for(size_t t_index = 0; t_index < list->num_of_indexes; t_index++) {
        pickup_t * pickup = &(pickups[list->indexes[t_index]]);
        // Circle collision from https://www.lazyfoo.net/tutorials/SDL/29_circular_collision_detection/index.php
        if((distance = dist_positions_squared(&player_pos, &(pickup->collider.center))) <= pickup->collider.radius * pickup->collider.radius) {
            if(smallest_distance == -1 || distance < smallest_distance) {
                smallest_distance = distance;

                pickup_index = list->indexes[t_index];
            }
        }
    }

    return pickup_index;
}

void (print_table)() {
    for(size_t row = 0; row < collision_table.height; row++) {
        for(size_t col = 0; col < collision_table.width; col++) {
            collision_list_t clist = collision_table.table[row][col];
            printf( "[");
            for(size_t idx = 0; idx < clist.num_of_indexes; idx++) {
                printf("%d,", clist.indexes[idx]);
            }
            printf( "],");
        }
        printf("\n");
    }
    printf("\n");
}

size_t (add_pickup)(pickup_t * pickup) {
    pickup_t * ptr;

    if(pickups_size == 0) {
        pickups = (pickup_t *) malloc(2*sizeof(pickup_t)); // index 0 has no pickups, used for dummy pickup, indexes start at 1
        if(pickups == NULL) return 0;
        pickups_size++;
    } 
    else pickups = realloc((void *)pickups, (pickups_size+1) * sizeof(pickup_t));
    size_t index = pickups_size;

    int err = add_collision(pickup, index);
    if(err) return 0;

    pickup->picked_up = false;

    ptr = pickups + pickups_size;
    pickups_size++;

    *ptr = *pickup;

    return index;
}

bool (pickup_index)(player_info_t * player, unsigned index) {
    bool picked_up = pickup_handler(player, &pickups[index]);

    if(picked_up) delete_pickup(index);

    return picked_up;
}

pickup_t (get_pickup)(unsigned index) {
    if(index >= pickups_size) {
        pickup_t dummy;
        dummy.picked_up = true;

        return dummy;
    }

    return pickups[index];
}

size_t (get_pickups_size)() {
    return pickups_size;
}

void (delete_pickup)(unsigned index) {
    if(index >= pickups_size) {
        return;
    }
    pickup_t * pickup = (pickups + index);
    pickup->picked_up = true;
    remove_collision(&(pickup->collider.center), index);
}

void (free_pickups)() {
    for(size_t idx = 1; idx < pickups_size; idx++) {
        free(pickups[idx].pickup_details);
    }
    pickups_size = 0;
    free(pickups);
}
