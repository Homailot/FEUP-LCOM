#include "menu.h"
#include "../game/game_manager.h"
#include "../player/player.h"
#include "../lib/video_graphics.h"
#include "../utils/data_types.h"
#include "../../data/sprites/inv_sprites.h"
#include "../../data/sprites/menu_sprites.h"
#include "../lib/sprites.h"
#include "../graphics/render.h"

extern graphics_data g_data;

extern player_info_t player;

extern controls_t movm;

enum menu_state gun_menu_state;

enum menu_state get_inv_menu_state(){
    return gun_menu_state;
}

void display_menu(gun_instance_t * guns, int num_guns){

    int width = g_data.h_res;
    int height = g_data.v_res;
    
    switch (gun_menu_state)
    {
    case CLOSED:
        player.movm.mouse_pos.x = width/2;
        player.movm.mouse_pos.y = height/2;
        gun_menu_state = DISPLAYING;
        break;
    default:
        break;
    }



    pair_t gun1, gun2, gun3;

    if(num_guns >= 1){
        gun1.x = width/3  - guns[0].gun.menu_sprite->texture_size.x/2;
        gun1.y = height/3 - guns[0].gun.menu_sprite->texture_size.y/2;
    }


    if(num_guns >=2){
        gun2.x = 2*width/3 - guns[1].gun.menu_sprite->texture_size.x/2;
        gun2.y = height/3  - guns[1].gun.menu_sprite->texture_size.y/2;
    }

    if(num_guns == 3){
        gun3.x = width/2     - guns[2].gun.menu_sprite->texture_size.x/2;
        gun3.y = 2*height/3  - guns[2].gun.menu_sprite->texture_size.y/2;        
    }

    pair_t epos = {g_data.h_res - exit_menu_button->texture_size.x - 10, 0 + 0.01*g_data.v_res + 10};


    int idx = -1;
    
    vector_t ms_pos = player.movm.mouse_pos;

    if(player.movm.lb_single_tap){
        player.movm.lb_single_tap = false;
        if( ms_detect_collision(ms_pos, gun1, guns[0].gun.menu_sprite->texture_size) )
            idx = 0;
        if( ms_detect_collision(ms_pos, gun2, guns[1].gun.menu_sprite->texture_size) )
            idx = 1;

        if( ms_detect_collision(ms_pos, gun3, guns[2].gun.menu_sprite->texture_size) )
            idx = 2;

        if (ms_detect_collision(ms_pos, epos, exit_menu_button->texture_size)) {
            game_state_t state = get_state();
            if(state == MP_GAME) set_state(END_M_GAME);
            else set_state(END_S_GAME);
        }
    }

    if(idx != -1) switch_gun(&player, idx);

    // Cool gray color
    uint32_t selected_color = 0x0066cc;
    uint32_t color = 0xb3b3b3;

    for (int i = 0; i < num_guns; i++)
    {
        pair_t * frame = NULL;
        if(i == 0) frame = &gun1;
        else if(i == 1) frame = &gun2;
        else if(i == 2) frame = &gun3;

        coordinate_t sprite_size = guns[i].gun.menu_sprite->texture_size;

        draw_rect(  frame->x - 5,
                    frame->y - 5, 
                    frame->x + sprite_size.x + 5, 
                    frame->y + sprite_size.y + 5, 
                    player.gun_index == i ? selected_color : color);
    }


    if(num_guns >= 1){
        // Drawing a frame on the selected one
        draw_sprite(&gun1, guns[0].gun.menu_sprite);
    } 

    if(num_guns >=2) draw_sprite(&gun2, guns[1].gun.menu_sprite);

    if(num_guns == 3) draw_sprite(&gun3, guns[2].gun.menu_sprite);

    draw_sprite(&epos, exit_menu_button);

    draw_mouse();
}
