#include "hud.h"
#include "../lib/sprites.h"
#include "../lib/video_graphics.h"
#include "../../data/sprites/numbers/numbers.h"
#include "../../data/sprites/hud_bar_sprite.h"
#include "../utils/utils.h"
#include "../graphics/render.h"

extern graphics_data g_data;

void render_hud(player_info_t * ply){
    
    pair_t hud_pos;
    hud_pos.x = 0;
    hud_pos.y = g_data.v_res - hud_sprite->texture_size.y;
    draw_sprite(&hud_pos, hud_sprite);

    int health = ply->health;
    if(health < 0) health = 0;
    
    // Initial position of the health
    pair_t pos;
    pos.y = g_data.v_res - 125;
    pos.x = 175; 

    render_number(health, &pos, 3);

    int ammo = ply->guns[ply->gun_index].curr_ammo;
    int packets = ply->guns[ply->gun_index].curr_packets;

    pos.x = 15;
    pos.y = g_data.v_res - 125;
    render_number(ammo, &pos, 2);

    pos.x = 410;

    render_number(packets, &pos, 1);

    pos.x = 650;

    render_number(ply->armor, &pos, 3);

    sprite_t * gun_sprite = ply->guns[ply->gun_index].gun.menu_sprite;

    pos.x = g_data.h_res/2 - gun_sprite->texture_size.x/2;
    pos.y = g_data.v_res - 60 - gun_sprite->texture_size.y/2;

    draw_sprite(&pos, gun_sprite);
}
