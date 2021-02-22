#include "player.h"
#include "../graphics/render.h"
#include "../utils/utils.h"
#include "../lib/sprites.h"
#include "../../data/sprites/player/player_sprites.h"
#include "../pickups/pickups.h"
#include <math.h>
#include "../game/game_manager.h"
#include <lcom/xpm.h>
#include <lcom/lcf.h>
#include <stdint.h>

player_info_t player;

player_info_t * enemies;

size_t num_enemies = 0;

extern map_t map;
extern tick_t time_counter;
settings_t settings;

int (reset_player_mouse_movm)(player_info_t * ply){
    ply->movm.mouse_desl = 0;
    return OK;
}

player_info_t * get_player(){
    return &player;
}

player_info_t * get_mp_enemy_player(){
    return &enemies[0];
}

int (init_player_anims)(player_info_t * p){

    size_t num_anims = 6;

    p->sprites = malloc(num_anims * sizeof(animated_sprite_t *));
    p->num_anims = num_anims;
    p->curr_anim = 0;
    /** Idle animation **/
    
    p->sprites[0] = malloc(8*sizeof(animated_sprite_t));

    for (size_t i = 0; i < 8; i++)
    {
        animated_sprite_t anime;
        memset(&anime,0,sizeof(animated_sprite_t));
        anime.cur_frame = 0;
        anime.frames_per_sprite = 1;
        anime.num_fig = 1;
        anime.frames = malloc(sizeof(sprite_t *));

        anime.frames[0] = playe[i];
        anime.sprite = playe[i];

        p->sprites[0][i] = anime;
    }

    
    /** Crouch animation **/
    
    p->sprites[1] = malloc(8*sizeof(animated_sprite_t));


    for (size_t i = 0; i < 8; i++)
    {
        animated_sprite_t anim_crouch;
        memset(&anim_crouch,0,sizeof(animated_sprite_t));
        anim_crouch.cur_frame = 0;
        anim_crouch.frames_per_sprite = 20;
        anim_crouch.num_fig = 4;
        anim_crouch.frames = malloc(4*sizeof(sprite_t *));
        
        anim_crouch.frames[0] = playa[i];
        anim_crouch.frames[1] = playb[i];
        anim_crouch.frames[2] = playc[i];
        anim_crouch.frames[3] = playd[i];
        
        anim_crouch.sprite = playa[i];

        p->sprites[1][i] = anim_crouch;
    }


    /** Walk and run animations **/

    p->sprites[2] = malloc(8*sizeof(animated_sprite_t));
    p->sprites[3] = malloc(8*sizeof(animated_sprite_t));


    for (size_t i = 0; i < 8; i++)
    {
        // Both the same as crouch animation, just a different speed
        p->sprites[2][i] = p->sprites[1][i];
        p->sprites[2][i].frames_per_sprite = 10;

        p->sprites[3][i] = p->sprites[1][i];
        p->sprites[3][i].frames_per_sprite = 5;

    }


    /** Fire animation **/

    p->sprites[4] = malloc(8*sizeof(animated_sprite_t));

    for (size_t i = 0; i < 8; i++)
    {
        animated_sprite_t fire_anim;
        memset(&fire_anim,0,sizeof(animated_sprite_t));
        fire_anim.cur_frame = 0;
        fire_anim.num_fig = 3;
        fire_anim.frames = malloc(3*sizeof(sprite_t *));
        fire_anim.frames[0] = playe[i];
        fire_anim.frames[1] = playf[i];
        fire_anim.frames[2] = playe[i];

        fire_anim.frames_per_sprite = 5; //gun_rate/2;
        fire_anim.sprite = playe[i];
        p->sprites[4][i] = fire_anim;
    }


    p->sprites[5] = malloc(8*sizeof(animated_sprite_t));

    for (size_t i = 0; i < 8; i++)
    {
        animated_sprite_t death_anim;
        memset(&death_anim,0,sizeof(animated_sprite_t));
        death_anim.cur_frame = 0;
        death_anim.num_fig = 8;
        death_anim.frames = malloc(8*sizeof(sprite_t *));

        for (size_t j = 0; j < 8; j++)
        {
            death_anim.frames[j] = dead[j];
        }

        death_anim.frames_per_sprite = 5;
        death_anim.sprite = dead[0];
        p->sprites[5][i] = death_anim;
    }

    return OK;
}

int (free_player_anims)(player_info_t * p){
    
    for (int i = 0; i < p->num_anims; i++)
    {
        free(p->sprites[i]->frames);
        free(p->sprites[i]);
    } 
    
    return OK;
}

void init_player_ctrls(){
    memset(&(player.movm), 0, sizeof(controls_t));
}

void init_player(player_type_t type, vector_t * position, vector_t * direction, double radius, int health, int max_health, int armor, int max_armor, gun_instance_t * instances, int num_guns){
    
    player_info_t * ptr;

    if(type != PLAYER) {
        if(num_enemies == 0) enemies = (player_info_t *) malloc(sizeof(player_info_t));
        else enemies = realloc((void *)enemies, (num_enemies+1) * sizeof(player_info_t));
        ptr = enemies + num_enemies;
    }
    else ptr = &player;

    memset(ptr, 0, sizeof(player_info_t));
    normalize(direction);
    
    ptr->type = type;
    ptr->health = health;
    ptr->max_health = max_health;
    ptr->armor = armor;
    ptr->max_armor = max_armor;
    ptr->inventory_size = num_guns;
    ptr->position.x = position->x;
    ptr->position.y = position->y;
    ptr->direction.x = direction->x;
    ptr->direction.y = direction->y;
    ptr->radius = radius;
    ptr->gun_index = 0;
    ptr->should_render = true;
    memset(&(ptr->movm), 0, sizeof(controls_t));
    ptr->movm.mouse_state = KEY_UP;
    ptr->guns = (gun_instance_t * ) malloc(num_guns*sizeof(gun_instance_t));
    if(ptr->guns == NULL) return;

    for(int i = 0; i < num_guns; i++) {
        ptr->guns[i] = instances[i];
    }
    ptr->guns[0].state = INITIAL;

    if(type == BOT){
        ptr->guns[0].gun.cooldown_time = 30;
        ptr->guns[0].gun.range = 5;
    }

    if(type != PLAYER){
        init_player_anims(ptr);
        num_enemies++;    
    }else{
        double factor = tan((double)retrieve_settings().fov*M_PI/360);
        ptr->camera_plane.x = -(ptr->direction.y)*factor;
        ptr->camera_plane.y = ptr->direction.x*factor;
    }

}

void free_players(){
    for(size_t i = 0; i < num_enemies; i++) {
        free(enemies[i].guns);
    }
    free(player.guns);
    free(enemies);
    enemies = NULL;
    num_enemies = 0;
}

/**
 * @brief State machine for the mouse to detect single tap events
 * @param The player to detect
 */
void (update_player_controls)(player_info_t * ply){

    ply->movm.lb_held_down = ply->movm.mouse_pkt.lb;

    switch (ply->movm.mouse_state)
    {
    case KEY_UP:
        if(ply->movm.mouse_pkt.lb){
            ply->movm.lb_single_tap = true;
            ply->movm.mouse_state = KEY_DOWN;
        }
        break;
    case KEY_DOWN:
        ply->movm.lb_single_tap = false;
        if(!ply->movm.mouse_pkt.lb){
            ply->movm.mouse_state = KEY_UP;
        }
    default:
        break;
    }
}

bool (hit_player)(double* dist, player_info_t * p1, player_info_t * p2) {
    vector_t tangent_hit_left;

    vector_t player_dir;
    player_dir.x = p2->position.x - p1->position.x;
    player_dir.y = p2->position.y - p1->position.y;

    vector_t tang_dir;
    tang_dir.x = -player_dir.y;
    tang_dir.y = player_dir.x;

    normalize(&tang_dir);

    tangent_hit_left.x = p2->position.x - tang_dir.x * p2->radius;
    tangent_hit_left.y = p2->position.y - tang_dir.y * p2->radius;

    vector_t left_max;
    left_max.x = tangent_hit_left.x - p1->position.x;
    left_max.y = tangent_hit_left.y - p1->position.y;

    double max_ang = degree(&left_max, &player_dir);

    double ray_ang = degree(&p1->direction, &player_dir);

    //dprintf(2,"Max ang: %f   -   Curr Angle: %f\n", max_ang, ray_ang);

    *dist = vec_mag(&player_dir);

    return abs_double(ray_ang) <= max_ang;
}

void(switch_gun)(player_info_t * ply,int index){
    if(ply->gun_index != index){
        ply->guns[ply->gun_index].state = INVENTORY;
        ply->gun_index = index;
        ply->guns[index].state = INITIAL;
    }
}

bool (fire)(player_info_t * ply) {
    ray_hit_t ray_hit;
    gun_instance_t* gun_inst = &(ply->guns[ply->gun_index]);
    gun_t * gun = &(gun_inst->gun);

    if (gun_inst->state != READY) {
        return false;
    }

    gun_inst->curr_anim = 1;
    for (unsigned short bullet = 0; bullet < gun->ammo_per_fire && gun_inst->curr_ammo > 0; gun_inst->curr_ammo--, bullet++) {
        player_info_t * target = &player;

        // Storing the closest enemy
        player_info_t * hit_target = NULL;

        bool hit;

        double dist = -1;

        for (size_t j = 0; j < ((ply->type == PLAYER) ? num_enemies : 1); j++)
        {
            // If it is a player, retrieves a target
            // Otherwise the target will solely be the player
            if(ply->type == PLAYER) target = &enemies[j];
            // Distance to the player hit
            double player_dist;
            bool hit_local = target->health >0 && hit_player(&player_dist, ply, target) && player_dist <= ply->guns[ply->gun_index].gun.range;
            
            // Adding a small interval when player is way too close to the wall
            // This wont allow hits behind the wall because wall has 1 distance value
            if(hit_local && (dist < 0 || player_dist < dist + 0.2) ){
                hit = true;
                hit_target = target;
                dist = player_dist;
            }
        }

        shoot_ray(&ply->position, &ply->direction, &ray_hit, ply->guns[ply->gun_index].gun.range);


        if ((ray_hit.hit && ray_hit.distance < dist)) {
            // Instanciar bullet hit
            hit = false;
        }

        if(hit){
            double damage = gun->damage;
            if (hit_target->armor > 0) {
                hit_target->armor -= 0.5*damage;
                if(hit_target->armor < 0) {
                    damage = -hit_target->armor;
                    hit_target->armor = 0;
                } else continue;
            }

            hit_target->health -= damage;
            if(hit_target->health < 0) hit_target->health = 0;
        }
    }
    return true;
}

vector_t (player_wall_collision)(player_info_t * ply, vector_t * new_pos) {
    circle_t collider = {*new_pos, ply->radius};
    rectangle_t rect;
    vector_t distances;
    pair_t start_coord = {(int) collider.center.x-1, (int) collider.center.y-1};
    pair_t end_coord = {(int) collider.center.x+1, (int) collider.center.y+1};
    double squared_distance, squared_radius, factor;

    if(new_pos->x < 0 || new_pos->y < 0 || new_pos->y >= map.lines || new_pos->x >= map.columns) return *new_pos;

    if(start_coord.x < 0) start_coord.x = 0;
    if(end_coord.x >= (long)map.columns) end_coord.x = map.columns - 1;
    if(start_coord.y < 0) start_coord.y = 0;
    if(end_coord.y >= (long)map.lines) end_coord.y = map.lines - 1;

    for(long x = start_coord.x; x <= end_coord.x; x++) {
        for(long y = start_coord.y; y <= end_coord.y; y++) {
            if(map.matrix[y][x] <= 0) continue;

            rect.left_corner.x = x; rect.left_corner.y = y;
            rect.height = rect.width = 1;

            circle_rect_distance(&collider, &rect, &distances);
            squared_distance = vec_mag_squared(&distances);
            squared_radius =  (double)(collider.radius * collider.radius);

            if(squared_distance >= squared_radius) continue;

            factor = sqrt(squared_radius / squared_distance);
            if(squared_distance == 0) {
                //inside the wall
                collider.center.x = (int) (collider.center.x + 1);
                collider.center.y = (int) (collider.center.y + 1); 
            } else {
                collider.center.x += distances.x - factor*(distances.x);
                collider.center.y += distances.y - factor*(distances.y);
            }
        }
    }

    return collider.center;
}

void(player_int_ih)(player_info_t * ply) {

    // Updating player movment object according to its keyboard state

    if(ply->type == NETWORK_PLAYER){
        ply->movm.movement.x = (double)ply->movm.kbd_state.x_pos - (double)ply->movm.kbd_state.x_neg;
        ply->movm.movement.y = (double)ply->movm.kbd_state.y_pos - (double)ply->movm.kbd_state.y_neg;
    }

    if(ply->type == PLAYER || ply->type == NETWORK_PLAYER){
        /* Rotating player according to the mouse movement*/
        double tmp = ply->direction.x;      
        double x_delta = ply->movm.mouse_desl;

        if(x_delta > 1.9)
            x_delta = 1.9;
        else if(x_delta < -1.9)
            x_delta = -1.9;
        
        uint8_t signal = x_delta >= 0 ? 0 : 1;

        uint8_t x_d = (uint8_t)(fabs(x_delta)*128);
        x_delta = (double)x_d/128;
        if(signal == 1)
            x_delta = -x_delta;
        
        // Multiply both the direction vector and the camera plane by the rotation matrix.
        ply->direction.x = tmp * cos(x_delta) - ply->direction.y * sin(x_delta);
        ply->direction.y = tmp * sin(x_delta) + ply->direction.y * cos(x_delta);
        tmp = ply->camera_plane.x;

        //rotate_vec(&ply->direction, x_delta*sensitivity);
        //rotate_vec(&ply->camera_plane, x_delta*sensitivity);
        // Multiply both the direction vector and the camera plane by the rotation matrix.
        ply->camera_plane.x = tmp * cos(x_delta) - ply->camera_plane.y * sin(x_delta);
        ply->camera_plane.y = tmp * sin(x_delta) + ply->camera_plane.y * cos(x_delta);
    }

  
  
    vector_t dir = ply->direction;
    vector_t mov = ply->movm.movement;

    vector_t perp_dir;
    perp_dir.x = -dir.y;
    perp_dir.y = dir.x;

    double speed_mult = 1 - 0.5 * ply->movm.kbd_state.crouch + 0.5 * ply->movm.kbd_state.sprint;

    double x_movement = speed_mult * (PLAYER_SPEED * mov.x * ply->direction.x + STRAFE_SPEED * mov.y * perp_dir.x);
    double y_movement = speed_mult * (PLAYER_SPEED * mov.x * ply->direction.y + STRAFE_SPEED * mov.y * perp_dir.y);
    double player_pos_y = (ply->position.y + y_movement);
    double player_pos_x = (ply->position.x + x_movement);

    vector_t pos;
    pos.x = player_pos_x;
    pos.y = player_pos_y;
    
    ply->position = player_wall_collision(ply, &pos);


    if(ply->type != BOT){
        // The number keys
        bool * nums = ply->movm.kbd_state.nums_press;
        
        // First gun will have priority over the secundary ones if multiple buttons are pressed
        // The actual switching functionality will only happen if the current gun is different from the desired one - Checked inside function
        for (size_t i = 1; i <= 3; i++)
        {
            if(nums[i]){
                nums[i] =  false;
                switch_gun(ply,i-1);
                break;
            }
        }
    }

    if(ply->type != BOT)
        update_player_controls(ply);

    size_t idx = check_player_pickup_collision(ply);
    if(idx > 0) {
        pickup_index(ply, idx);
    }

    bool shot = (get_state() == GAME || get_state() == MP_GAME) && 
                    get_inv_menu_state() == CLOSED &&
                    ((ply->movm.lb_held_down && ply->guns[ply->gun_index].gun.automatic) || ply->movm.lb_single_tap);
    if (shot){
        shot = fire(ply);
    }


    // Updating animations in all directions
    if(ply->type != PLAYER){
        // If the fire animation is on going, shouldnt be replaced
        anim_state_t fire_anim_state = ply->sprites[4][0].state;


        if(ply->health <= 0) ply->curr_anim = 5;
        else if( fire_anim_state == PROGRESS || shot) ply->curr_anim = 4;
        else if(fabs(ply->movm.movement.x) < 0.05 && fabs(ply->movm.movement.y) < 0.05){
            // Idle animation
            ply->curr_anim = 0;
        }
        else{
            if((ply->movm.kbd_state.crouch && ply->movm.kbd_state.sprint) || (!ply->movm.kbd_state.crouch && !ply->movm.kbd_state.sprint))
                ply->curr_anim = 2;
            else if(ply->movm.kbd_state.crouch)
                ply->curr_anim = 1;
            else
                ply->curr_anim = 3;
        }

        for (size_t i = 0; i < 8; i++)
        {
            // OK VBOM APETITE
            if(update_anim(ply->sprites[ply->curr_anim] + i)){
                if(ply->curr_anim == 5) ply->should_render = false;
            };
        }
    }
    update_gun_state(&(ply->guns[ply->gun_index]), shot, ply->type == PLAYER);
}


void(bot_int_ih)(){
    for (size_t i = 0; i < num_enemies; i++)
    {
        if(enemies[i].should_render)
            player_int_ih(&enemies[i]);
    }
}
