#include "controls.h"
#include "../graphics/render.h"
#include "../lib/kbc.h"
#include "../player/player.h"
#include "../utils/utils.h"
#include "../lib/video_graphics.h"
#include "../game/game_manager.h"
#include <math.h>

extern graphics_data g_data;
static bool lastDoubleByte = false;
extern bool valid_mouse_byte;
extern bool valid_kb_byte;
extern uint8_t last_kb_byte;
extern uint8_t last_mouse_byte;

extern enum menu_state gun_menu_state;

static uint8_t ms_data[3];
static uint8_t ms_idx;

static packet_t currPkt;

settings_t settings;
controls_t movm;
controls_t bot_movm;

extern player_info_t player;

bool (ms_detect_collision)(vector_t ms_pos, pair_t button_pos, coordinate_t button_size){
    return (ms_pos.x <= button_pos.x + button_size.x && 
            ms_pos.x >= button_pos.x                 && 
            ms_pos.y <= button_pos.y + button_size.y && 
            ms_pos.y >= button_pos.y );
}

void(handle_kbd_evt)() {

    kbc_ih();

    if (valid_kb_byte) {
        if (last_kb_byte == KBC_DOUBLE_BYTE) {
            lastDoubleByte = true;
        }
        else {
            bool make = !(last_kb_byte & KBC_BREAK_CODE);

            uint8_t size = lastDoubleByte ? 2 : 1;

            uint8_t bytes[size];

            bytes[size - 1] = last_kb_byte;
            if (lastDoubleByte) bytes[0] = KBC_DOUBLE_BYTE;

            kbd_parse_data(bytes, size, make);

            lastDoubleByte = false;
        }
    }
}

void(kbd_parse_data)(uint8_t* bytes, size_t size, bool make) {

    uint8_t key = PARSE_BREAK_CODE(bytes[size - 1]);


    // Solving issue on keyboard getting stuck sometimes after pressing two buttons of the same axis
    if (W_KEY == key)          player.movm.kbd_state.x_pos = make;
    else if (S_KEY == key)     player.movm.kbd_state.x_neg = make;
    else if (D_KEY == key)     player.movm.kbd_state.y_pos = make;
    else if (A_KEY == key)     player.movm.kbd_state.y_neg = make;
    else if (PT_SHIFT == key)  player.movm.kbd_state.sprint = make;
    else if (PT_CTRL == key)   player.movm.kbd_state.crouch = make;
    else if (Q_KEY == key)     player.movm.kbd_state.gun_menu = make;
    else if( key >= PT_1 && key <= PT_0 )
    {
        if(key == PT_0) player.movm.kbd_state.nums_press[0] = make;
        else player.movm.kbd_state.nums_press[key-1] = make;
    }
    
    // True  -> 1 -> 1.0
    // False -> 0 -> 0.0
    player.movm.movement.x = (double)player.movm.kbd_state.x_pos - (double)player.movm.kbd_state.x_neg;
    player.movm.movement.y = (double)player.movm.kbd_state.y_pos - (double)player.movm.kbd_state.y_neg;

}

void(handle_mouse_evt)() {
    mouse_ih();

    if (!valid_mouse_byte) {
        ms_idx = 0;
        return;
    }

    if (ms_idx == 0 && !(last_mouse_byte & MOUSE_FIRST_BIT)) {
        return;
    }

    ms_data[ms_idx] = last_mouse_byte;

    ms_idx++;

    if (ms_idx == 3) {
        parse_packet(ms_data, &currPkt);

        mouse_parse_data(&currPkt);
        ms_idx = 0;
    }
}

void(init_settings)() {
    settings.mouse_sensitivity = 0.002;
    settings.fov = 66;
    settings.mouse_color = 0x5bfe03;
}

settings_t (retrieve_settings)() {
    return settings;
}

void(mouse_parse_data)() {

    
    int16_t x_delta = currPkt.delta_x;
    int16_t y_delta = currPkt.delta_y;



    if(!currPkt.x_ov && !currPkt.y_ov)
    {

    if(player.movm.mouse_pos.x + x_delta >= 0 && player.movm.mouse_pos.x + x_delta <= g_data.h_res)
        player.movm.mouse_pos.x += x_delta;
    if(player.movm.mouse_pos.y - y_delta >= 0 && player.movm.mouse_pos.y - y_delta <= g_data.v_res)
        player.movm.mouse_pos.y -= y_delta;
    
    }

    player.movm.lb_held_down = currPkt.lb == 1;
    player.movm.mouse_pkt = currPkt;
    game_state_t st = get_state();
    if((st == GAME || st == MP_GAME) && gun_menu_state == CLOSED){
        /*double tmp = player.direction.x;
        // Multiply both the direction vector and the camera plane by the rotation matrix.
        player.direction.x = tmp * cos(x_delta * sensitivity) - player.direction.y * sin(x_delta * sensitivity);
        player.direction.y = tmp * sin(x_delta * sensitivity) + player.direction.y * cos(x_delta * sensitivity);
        tmp = player.camera_plane.x;
        // Multiply both the direction vector and the camera plane by the rotation matrix.
        player.camera_plane.x = tmp * cos(x_delta * sensitivity) - player.camera_plane.y * sin(x_delta * sensitivity);
        player.camera_plane.y = tmp * sin(x_delta * sensitivity) + player.camera_plane.y * cos(x_delta * sensitivity);
        */
       player.movm.mouse_desl += x_delta*settings.mouse_sensitivity;
    }
}
