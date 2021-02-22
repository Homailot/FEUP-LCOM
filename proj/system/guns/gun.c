#include "gun.h"
#include "../data/sprites/guns/pistol_sprites.h"
#include "../data/sprites/guns/rifle_sprites.h"
#include "../data/sprites/guns/ray_sprites.h"
#include "../lib/sprites.h"
#include "../../data/sprites/inv_sprites.h"

extern tick_t time_counter;
static gun_t guns[5];


void update_gun_state(gun_instance_t * gun_inst, bool shot, bool player){
    animated_sprite_t * anim = &(gun_inst->gun.animations[gun_inst->curr_anim]);
    gun_t * gun = &(gun_inst->gun);

    
    if(player && update_anim(anim)) {
        gun_inst->curr_anim = 0;
    }

    gun_state_t curr = gun_inst->state;
    switch (curr)
    {
    case INITIAL:
        gun_inst->last_op_tick = time_counter;
        if(gun_inst->curr_ammo <= 0){
            if(gun_inst->curr_packets > 0){
                gun_inst->state = RELOADING;
                gun_inst->last_op_tick = time_counter;
            }
            else gun_inst->state = NO_AMMO;
        }
        else gun_inst->state = CHARGING;
        break;
    case CHARGING:
        if(gun_inst->last_op_tick + (tick_t)gun->charge_time <= time_counter)
            gun_inst->state = READY;
        break;
    case READY:
        if(gun_inst->curr_ammo <= 0){
            if(gun_inst->curr_packets != 0){
                gun_inst->state = RELOADING;
                gun_inst->last_op_tick = time_counter;
            }
            else gun_inst->state = NO_AMMO;
        }
        else if(shot){
            gun_inst->last_op_tick = time_counter;
            gun_inst->state = COOLDOWN;
        }
        break;
    case COOLDOWN:
        if(gun_inst->last_op_tick + (tick_t)gun->cooldown_time <= time_counter){
            gun_inst->state = READY;
        };
        break;
    case RELOADING:
        if(gun_inst->last_op_tick + gun->reload_time <= time_counter){
            reload_gun(gun_inst);
            gun_inst->state = READY;
        }
        break;
    case NO_AMMO:
        if(gun_inst->curr_packets != 0){
            gun_inst->state = RELOADING;
            gun_inst->last_op_tick = time_counter;
        }
        break;
    default:
        break;
    }
}

void add_packets(gun_instance_t * gun, unsigned int num_packets){
    gun->curr_packets += num_packets;
}

void (reload_gun)(gun_instance_t * gun_inst) {
    if (gun_inst->curr_packets > 0) {
        gun_inst->curr_packets--;
        gun_inst->curr_ammo = gun_inst->gun.ammo_per_packet; // on purpose to avoid unneeded reloads (strategic design, let's go with that :) )
    }
}

int (free_gun_anims)(gun_t * g){
    free(g->animations);
    
    return OK;
}

int (init_guns)() {
    gun_t gun1;
    gun1.name = "Minigun";
    gun1.ammo_per_fire = 1;
    gun1.ammo_per_packet = 30;
    gun1.max_packets = 5;
    gun1.automatic = true;
    gun1.burst = false;
    gun1.charge_time = 15;
    gun1.cooldown_time = 10;
    gun1.range = 20;
    gun1.spread = false;
    gun1.damage = 15;
    gun1.reload_time = 90;
    gun1.menu_sprite = gun_menu_sprites[0];
    gun1.ammo_index = 0;

    gun_t gun2;
    gun2.name = "Service Pistol";
    gun2.ammo_per_fire = 1;
    gun2.ammo_per_packet = 11;
    gun2.max_packets = 3;
    gun2.automatic = false;
    gun2.burst = false;
    gun2.charge_time = 10;
    gun2.cooldown_time = 15;
    gun2.range = 15;
    gun2.spread = false;
    gun2.damage = 15;
    gun2.reload_time = 60;
    gun2.menu_sprite = gun_menu_sprites[1];
    gun2.ammo_index = 1;

    gun_t gun3;
    gun3.name = "BLMG 5100";
    gun3.ammo_per_fire = 1;
    gun3.ammo_per_packet = 2;
    gun3.max_packets = 5;
    gun3.automatic = false;
    gun3.burst = false;
    gun3.charge_time = 30;
    gun3.cooldown_time = 60;
    gun3.range = 40;
    gun3.spread = false;
    gun3.damage = 120;
    gun3.reload_time = 120;
    gun3.menu_sprite = gun_menu_sprites[2];
    gun3.ammo_index = 2;

    gun_t bot_gun1;
    bot_gun1.name = "Auto Weak Bot Gun";
    bot_gun1.ammo_per_fire = 1;
    bot_gun1.ammo_per_packet = 30;
    bot_gun1.max_packets = 5;
    bot_gun1.automatic = true;
    bot_gun1.burst = false;
    bot_gun1.charge_time = 0;
    bot_gun1.cooldown_time = 15;
    bot_gun1.range = 30;
    bot_gun1.spread = false;
    bot_gun1.damage = 5;
    bot_gun1.reload_time = 60;
    bot_gun1.menu_sprite = gun_menu_sprites[0];
    bot_gun1.ammo_index = 0;

    gun_t bot_gun2;
    bot_gun2.name = "Auto STRONG Bot Gun";
    bot_gun2.ammo_per_fire = 1;
    bot_gun2.ammo_per_packet = 30;
    bot_gun2.max_packets = 5;
    bot_gun2.automatic = true;
    bot_gun2.burst = false;
    bot_gun2.charge_time = 0;
    bot_gun2.cooldown_time = 1;
    bot_gun2.range = 30;
    bot_gun2.spread = false;
    bot_gun2.damage = 30;
    bot_gun2.reload_time = 60;
    bot_gun2.menu_sprite = gun_menu_sprites[0];
    bot_gun2.ammo_index = 0;

    int err = init_rifle_anims(&gun1, 1);
    if(err) return err;
    err = init_gun_anims(&gun2, 1);
    if(err) return err;
    err = init_ray_anims(&gun3, 0.3);
    if(err) return err;
    err = init_rifle_anims(&bot_gun1, 1);
    if(err) return err;
    err = init_rifle_anims(&bot_gun2, 1);
    if(err) return err;

    guns[0] = gun1;
    guns[1] = gun2;
    guns[2] = gun3;
    guns[3] = bot_gun1;
    guns[4] = bot_gun2;

    return OK;
}

gun_t (get_gun)(int gun_index) {
    if(gun_index >= 5) {
        gun_t dummy;

        return dummy;
    }
    return guns[gun_index];
}

gun_instance_t (create_gun_instance)(int gun_index, int start_ammo, int start_packets) {
    gun_instance_t gun_instance;

    gun_instance.gun = get_gun(gun_index);
    gun_instance.gun_index = gun_index;
    gun_instance.curr_anim = 0;
    gun_instance.curr_ammo = start_ammo > gun_instance.gun.ammo_per_packet ? gun_instance.gun.ammo_per_packet : start_ammo;
    gun_instance.curr_packets = start_packets > gun_instance.gun.max_packets ? gun_instance.gun.max_packets : start_packets;
    gun_instance.last_op_tick = 0;
    gun_instance.state = INVENTORY;

    return gun_instance;
}

int (init_gun_anims)(gun_t * g, double anim_speed_multiplier){
    size_t num_anims = 2;

    g->animations = malloc(num_anims * sizeof(animated_sprite_t));
    g->num_anims = num_anims;
    /** Idle animation **/

    animated_sprite_t anime;
    anime.cur_frame = 0;
    anime.frames_per_sprite = 1;
    anime.num_fig = 1;
    anime.frames = malloc(sizeof(sprite_t *));

    anime.frames[0] = pisia;
    anime.sprite = pisia;

    g->animations[0] = anime;

    /** Fire animation **/

    animated_sprite_t anim_fire;
    anim_fire.cur_frame = 0;
    anim_fire.num_fig = 7;

    int num_ticks = g->cooldown_time / anim_fire.num_fig;

    if(num_ticks * anim_speed_multiplier * anim_fire.num_fig < g->cooldown_time)
        num_ticks *= anim_speed_multiplier;

    if(num_ticks == 0) num_ticks = 1;

    anim_fire.frames_per_sprite = num_ticks;
    anim_fire.frames = malloc(anim_fire.num_fig*sizeof(sprite_t *));
    
    anim_fire.frames[0] = pisfa;
    anim_fire.frames[1] = pisfa;
    anim_fire.frames[2] = pisfb;
    anim_fire.frames[3] = pisfb;
    anim_fire.frames[4] = pisfc;
    anim_fire.frames[5] = pisfd;
    anim_fire.frames[6] = pisfe;
    
    anim_fire.sprite = pisfa;

    g->animations[1] = anim_fire;

    return OK;
}

int (init_rifle_anims)(gun_t * g, double anim_speed_multiplier){
        size_t num_anims = 2;

    g->animations = malloc(num_anims * sizeof(animated_sprite_t));
    g->num_anims = num_anims;
    /** Idle animation **/

    animated_sprite_t anime;
    anime.cur_frame = 0;
    anime.frames_per_sprite = 1;
    anime.num_fig = 1;
    anime.frames = malloc(sizeof(sprite_t *));

    anime.frames[0] = rifle1;
    anime.sprite = rifle1;

    g->animations[0] = anime;

    /** Fire animation **/

    animated_sprite_t anim_fire;
    anim_fire.cur_frame = 0;
    anim_fire.num_fig = 4;

    int num_ticks = g->cooldown_time / anim_fire.num_fig;

    if(num_ticks * anim_speed_multiplier * anim_fire.num_fig < g->cooldown_time)
        num_ticks *= anim_speed_multiplier;

    if(num_ticks == 0) num_ticks = 1;

    anim_fire.frames_per_sprite = num_ticks;
    anim_fire.frames = malloc(anim_fire.num_fig*sizeof(sprite_t *));
    
    anim_fire.frames[0] = rifle2;
    anim_fire.frames[1] = rifle3;
    anim_fire.frames[2] = rifle4;
    anim_fire.frames[3] = rifle1;
    
    anim_fire.sprite = rifle2;

    g->animations[1] = anim_fire;

    return OK;
}

int (init_ray_anims)(gun_t * g, double anim_speed_multiplier){
        size_t num_anims = 2;

    g->animations = malloc(num_anims * sizeof(animated_sprite_t));
    g->num_anims = num_anims;
    /** Idle animation **/

    animated_sprite_t anime;
    anime.cur_frame = 0;
    anime.frames_per_sprite = 1;
    anime.num_fig = 1;
    anime.frames = malloc(sizeof(sprite_t *));

    anime.frames[0] = ray0;
    anime.sprite = ray0;

    g->animations[0] = anime;

    /** Fire animation **/

    animated_sprite_t anim_fire;
    anim_fire.cur_frame = 0;
    anim_fire.num_fig = 4;

    int num_ticks = g->cooldown_time / anim_fire.num_fig;

    if(num_ticks * anim_speed_multiplier * anim_fire.num_fig < g->cooldown_time)
        num_ticks *= anim_speed_multiplier;

    if(num_ticks == 0) num_ticks = 1;

    anim_fire.frames_per_sprite = num_ticks;
    anim_fire.frames = malloc(anim_fire.num_fig*sizeof(sprite_t *));
    
    anim_fire.frames[0] = ray1;
    anim_fire.frames[1] = ray2;
    anim_fire.frames[2] = ray3;
    anim_fire.frames[3] = ray4;
    
    anim_fire.sprite = ray1;

    g->animations[1] = anim_fire;

    return OK;
}
