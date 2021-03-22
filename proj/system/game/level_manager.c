#include "level_manager.h"

// IMPORTANT: you must include the following line in all your C files
#include "controls/controls.h"
#include "file/map.h"
#include "graphics/render.h"
#include "graphics/graphics.h"
#include "lib/i8042.h"
#include "lib/kbc.h"
#include "lib/timer.h"
#include "lib/video_graphics.h"
#include "lib/sprites.h"
#include "utils/data_types.h"
#include "player/player.h"
#include "guns/gun.h"
#include "pickups/pickups.h"
#include "ai/enemy_controller.h"
#include "../data/sprites/inv_sprites.h"
#include "../game/game_manager.h"
#include "../lib/rs232.h"
#include "../lib/uart.h"
#include "../lib/rtc_controller.h"
#include <stdint.h>



extern player_info_t player;
extern player_info_t * enemies;
map_t map;

static int8_t curr_lvl;

int8_t get_curr_lvl(){
    return curr_lvl;
}

void setup_game_1(){
    set_state(GAME);
    vector_t direction = { 1, 2 };
    vector_t position = { 2, 2 };
    gun_instance_t instances[4];
    instances[0] = create_gun_instance(0, 0, 0);
    instances[1] = create_gun_instance(1, 0, 0);
    instances[2] = create_gun_instance(2, 0, 0);

    init_player(PLAYER, &position, &direction,0.2,50,100,0,100,instances,3);

    vector_t pos = { 4, 3 };

    add_ammo(&pos, 0.5, 1, 1);

    pos.x = 4; pos.y = 3.5;
    add_medkit(&pos, 0.5, 30);

    pos.x = 4; pos.y = 2.5;
    add_armor(&pos,0.5,20);

    pos.x = 16;pos.y = 2;
    direction.x = -1; direction.y = 0;
    init_player(BOT, &pos, &direction,0.2,50,50,0,1,instances,1);

    pos.x = 20; pos.y = 2.5;

    add_ammo(&pos, 0.5, 2, 1);

    instances[0] = create_gun_instance(3, 30, 2);

    pos.x = 16;pos.y = 6;
    direction.x = -1; direction.y = 0;
    init_player(BOT, &pos, &direction,0.2,50,50,0,1,instances,1);
    pos.x = 16;pos.y = 8;
    direction.x = -1; direction.y = 0;
    init_player(BOT, &pos, &direction,0.2,50,50,0,1,instances,1);

    vector_t posi[2];

    init_enemy(&enemies[1], true, 120, 45, 10, 2, posi);
    posi[0].x = 16;posi[0].y = 7;
    posi[1].x = 16;posi[1].y = 9;
    init_enemy(&enemies[2], false, 120, 45, 10, 2, posi);

}

void setup_game_2(){

    set_state(GAME);

    vector_t direction = { 1, 2 };
    vector_t position = { 2, 2 };
    gun_instance_t instances[4];
    instances[0] = create_gun_instance(0, 0, 0);
    instances[1] = create_gun_instance(1, 11, 1);
    instances[2] = create_gun_instance(2, 0, 0);

    init_player(PLAYER, &position, &direction,0.2,50,100,0,100,instances,3);
    switch_gun(&player, 1);

    instances[0] = create_gun_instance(3, 30, 2);
    direction.x = 1, direction.y = 0;
    position.x = 2, position.y= 6;
    init_player(BOT, &position, &direction,0.2,50,50,0,1,instances,1);
    direction.x = -1, direction.y = 0;
    position.x = 17, position.y= 6;
    init_player(BOT, &position, &direction,0.2,50,50,0,1,instances,1);
    direction.x = 1, direction.y = 0;
    position.x = 13, position.y= 1;
    init_player(BOT, &position, &direction,0.2,50,50,0,10,instances,1);
    direction.x = -1, direction.y = 0;
    position.x = 13, position.y= 2;
    init_player(BOT, &position, &direction,0.2,50,50,0,10,instances,1);
    direction.x = 0, direction.y = -1;
    position.x = 16, position.y= 11;
    init_player(BOT, &position, &direction,0.2,50,50,0,100,instances,1);
    direction.x = 0, direction.y = 2;
    position.x = 24, position.y= 8;
    init_player(BOT, &position, &direction,0.2,50,50,25,100,instances,1);
    position.x = 28, position.y= 3;
    init_player(BOT, &position, &direction,0.2,50,50,25,100,instances,1);
    position.x = 32, position.y= 11;
    init_player(BOT, &position, &direction,0.2,50,50,25,100,instances,1);
    position.x = 19, position.y= 21;
    init_player(BOT, &position, &direction,0.2,50,50,50,100,instances,1);
    position.x = 3, position.y= 20;
    init_player(BOT, &position, &direction,0.2,50,50,50,100,instances,1);
    position.x = 11, position.y= 16;
    init_player(BOT, &position, &direction,0.2,50,50,50,100,instances,1);
    position.x = 7, position.y= 30.5;
    init_player(BOT, &position, &direction,0.2,100,100,100,100,instances,1);
    position.x = 11, position.y= 31;
    init_player(BOT, &position, &direction,0.2,100,100,100,100,instances,1);
    
    instances[0] = create_gun_instance(4, 30, 2);
    position.x = 32.5, position.y= 27.5;
    init_player(BOT, &position, &direction,0.2,20,20,150,150,instances,1);
    position.x = 37, position.y= 9;
    init_player(BOT, &position, &direction,0.2,10,10,10,10,instances,1);
    // init_player(true,14,3,0,1,0.2,3);
    // init_player(true,2,11,0,1,0.2,3);
    // init_player(true,14,3,0,1,0.2,3);
    // init_player(true,10,6,0,1,0.2,3);
    // init_player(true,18,6,0,1,0.2,3);

    // init_player(true,30,15,0,1,0.2,3);
    // init_player(true,23,20,0,1,0.2,3);
    // init_player(true,1,1,0,1,0.2,3);
    // init_player(true,24,18,0,1,0.2,3);
    // init_player(true,15,20,0,1,0.2,3);

    vector_t posi[2];
    posi[0].x = 8;posi[0].y = 6;
    posi[1].x = 2;posi[1].y = 6;

    init_enemy(&enemies[0], false, 120, 45, 10, 2, posi);
    posi[0].x = 8;posi[0].y = 6;
    posi[1].x = 17;posi[1].y = 6;
    init_enemy(&enemies[1], false, 120, 45, 10, 2, posi);
    posi[0].x = 16;posi[0].y = 1;
    posi[1].x = 13;posi[1].y = 1;
    init_enemy(&enemies[2], false, 120, 45, 10, 2, posi);
    posi[0].x = 10;posi[0].y = 2;
    posi[1].x = 13;posi[1].y = 2;
    init_enemy(&enemies[3], false, 120, 45, 10, 2, posi);
    posi[0].x = 16;posi[0].y = 11;
    init_enemy(&enemies[4], false, 15, 45, 6, 1, posi);
    posi[0].x = 24;posi[0].y = 8;
    init_enemy(&enemies[5], false, 15, 45, 6, 1, posi);
    posi[0].x = 28;posi[0].y = 3;
    init_enemy(&enemies[6], false, 15, 45, 6, 1, posi);
    posi[0].x = 32;posi[0].y = 11;
    init_enemy(&enemies[7], false, 15, 45, 6, 1, posi);
    posi[0].x = 19;posi[0].y = 21;
    init_enemy(&enemies[8], false, 15, 45, 6, 1, posi);
    posi[0].x = 3;posi[0].y = 20;
    init_enemy(&enemies[9], false, 15, 45, 6, 1, posi);
    posi[0].x = 11;posi[0].y = 16;
    init_enemy(&enemies[10], false, 15, 45, 6, 1, posi);
    posi[0].x = 7;posi[0].y = 30.5;
    init_enemy(&enemies[11], false, 15, 45, 6, 1, posi);
    posi[0].x = 7;posi[0].y = 30.5;
    init_enemy(&enemies[12], false, 15, 45, 6, 1, posi);
    posi[0].x = 32.5;posi[0].y = 20.5;
    posi[1].x = 32.5;posi[1].y = 27.5;
    init_enemy(&enemies[13], false, 260, 5, 15, 2, posi);
    posi[0].x = 37;posi[0].y = 9;
    init_enemy(&enemies[14], false, 5, 45, 6, 1, posi);
    // init_enemy(&enemies[2], true, 120, 45, 20, 0, NULL);
    // init_enemy(&enemies[6], true, 120, 45, 20, 0, NULL);
    // init_enemy(&enemies[7], true, 120, 45, 20, 0, NULL);
    // init_enemy(&enemies[8], true, 120, 45, 20, 0, NULL);
    // init_enemy(&enemies[9], true, 120, 45, 20, 0, NULL);

    // posi[0].x = 21;posi[0].y = 3;
    // posi[1].x = 23;posi[1].y = 7;

    // init_enemy(&enemies[3], false, 120, 45, 5, 2, posi);

    // posi[0].x = 18;posi[0].y = 6;
    // posi[1].x = 10;posi[1].y = 6;

    // init_enemy(&enemies[4], false, 120, 45, 5, 2, posi);
    
    // posi[0].x = 10;
    // posi[1].x = 18;
    // init_enemy(&enemies[5], false, 120, 45, 5, 2, posi);

    vector_t pos = { 4.5, 12.5 };

    add_ammo(&pos, 0.5, 1, 1);
    pos.x = 3.5; pos.y = 12.5;
    add_ammo(&pos, 0.5, 1, 1);
    pos.x = 8.5; pos.y = 2.5;
    add_ammo(&pos, 0.5, 0, 1);
    pos.x = 38.5; pos.y = 6.5;
    add_ammo(&pos, 0.5, 2, 1);
    pos.x = 38.5; pos.y = 12.5;
    add_ammo(&pos, 0.5, 1, 1);
    pos.x = 38.5; pos.y = 11.5;
    add_ammo(&pos, 0.5, 0, 1);
    pos.x = 19.5; pos.y = 18.5;
    add_ammo(&pos, 0.5, 0, 1);
    pos.x = 10.5; pos.y = 20.5;
    add_ammo(&pos, 0.5, 0, 1);
    pos.x = 10.5; pos.y = 19.5;
    add_ammo(&pos, 0.5, 1, 1);
    pos.x = 34.5; pos.y = 21.5;
    add_ammo(&pos, 0.5, 2, 1);
    pos.x = 26.5; pos.y = 29.5;
    add_ammo(&pos, 0.5, 0, 1);
    pos.x = 21.5; pos.y = 27.5;
    add_ammo(&pos, 0.5, 0, 1);

    pos.x = 1.5; pos.y = 9.5;
    add_medkit(&pos, 0.5, 30);
    pos.x = 7.5; pos.y = 11.5;
    add_medkit(&pos, 0.5, 30);
    pos.x = 36.5; pos.y = 2.5;
    add_medkit(&pos, 0.5, 30);
    pos.x = 4.5; pos.y = 16.5;
    add_medkit(&pos, 0.5, 30);
    pos.x = 26.5; pos.y = 31.5;
    add_medkit(&pos, 0.5, 30);
    
    pos.x = 7.5; pos.y = 2.5;
    add_armor(&pos, 0.5, 30);

    pos.x = 34.5; pos.y = 1.5;
    add_armor(&pos, 0.5, 30);
    pos.x = 37.5; pos.y = 5.5;
    add_armor(&pos, 0.5, 30);
    pos.x = 2.5; pos.y = 16.5;
    add_armor(&pos, 0.5, 30);
    pos.x = 26.5; pos.y = 32.5;
    add_armor(&pos, 0.5, 50);
}


int init_game(uint8_t * t_bit_no, uint8_t * ms_bit_no, uint8_t * kb_bit_no, uint8_t * uart_bit_no, uint8_t * rtc_bit_no){
    srand(time(NULL));

    kbc_ih();

    init_player_ctrls();
    init_settings();

    set_state(MAIN_MENU);

    void* vram = vg_init(MODE);

    if (vram == NULL){
        reset_to_text_mode();
        vram = vg_init(MODE);
        if(vram == NULL){
            reset_to_text_mode();
            return 1;
        }
    }

    if(OK != load_sprites()) return 1;

    int err = uart_set_interrupt_reg(ENABLE_LINE_STATUS | ENABLE_RECEIVE | ENABLE_TRANSMIT);
    if (err)
        return err;
        
    err = uart_subscribe_int(uart_bit_no);
    if (err)
        return err;

    uart_set_line_ctrl(BITS_PER_CHAR(8) | ODD_PARITY);
    uart_set_baud_rate(BAUD_115200);

    init_uart_queues();
    uart_init_fifo();
    uart_ih();


    err = mouse_subscribe_int(ms_bit_no);
    if (err != OK)
        return err;
    err = kb_subscribe_int(kb_bit_no);
    if (err != OK)
        return err;
    err = timer_subscribe_int(t_bit_no);
    if (err != OK)
        return err;

    kbc_disable_int();
    if ((err = enable_stream()))
        return err;
    if ((err = enable_ms()))
        return err;
    uint8_t args[2];
    args[0] = SET_SAMPLE_RATE;
    args[1] = 60;
    write_ms_cmd(2, args);
    args[0] = SET_RESOLUTION;
    args[1] = 0;
    write_ms_cmd(2, args);


    /* RTC Related initializations */

    while(init_rtc_vars() != OK);

    enable_uie_int();

    err = rtc_subscribe_int(rtc_bit_no);
    if (err)
        return err;
    rtc_ih();
    return kbc_enable_int();
}

int load_level(int num_map, int mode, int player){
    int err = load_map(num_map, &map);
    if (err) return err;
    err = init_collision_table(map.columns, map.lines);
    if (err) return err;
    
    err = init_guns();
    if (err) return err;

    if(mode == 1){
        switch (num_map)
        {
        case 1:
            curr_lvl = 1;
            setup_game_1();
            break;
        case 2:
            curr_lvl = 2;
            setup_game_2();
            break;
        default:
            break;
        }
    }

    else if(mode == 2){
        setup_multiplayer(player);
        curr_lvl = -1;
    }

    return OK;
}


int (setup_multiplayer)(int player){
    
    set_state(MP_GAME);

    vector_t p1_pos = {2, 2};
    vector_t p2_pos = {39, 17};

    vector_t p1_dir = {1,0};
    vector_t p2_dir = {-1,0};
    
    if(player == 2){
        vector_t temp = p1_pos;
        p1_pos = p2_pos;
        p2_pos = temp;

        temp = p1_dir;
        p1_dir = p2_dir;
        p2_dir = temp;
    }

    gun_instance_t instances[4];
    instances[0] = create_gun_instance(0, 0, 0);
    instances[1] = create_gun_instance(1, 0, 0);
    instances[2] = create_gun_instance(2, 0, 0);
    
    init_player(PLAYER, &p1_pos, &p1_dir,0.2,100,100,0,100,instances,3);
    init_player(NETWORK_PLAYER, &p2_pos, &p2_dir,0.2,100,100,0,100,instances,3);

    vector_t pos = { 17.5, 18.5 };

    add_ammo(&pos, 0.5, 0, 2);
    pos.x = 9.5; pos.y = 14.5;
    add_ammo(&pos, 0.5, 1, 2);
    pos.x = 9.5; pos.y = 3.5;
    add_ammo(&pos, 0.5, 2, 2);
    pos.x = 14.5; pos.y = 13.5;
    add_ammo(&pos, 0.5, 0, 2);
    pos.x = 21.5; pos.y = 13.5;
    add_ammo(&pos, 0.5, 0, 2);
    pos.x = 29.5; pos.y = 3.5;
    add_ammo(&pos, 0.5, 0, 2);
    pos.x = 26.5; pos.y = 9.5;
    add_ammo(&pos, 0.5, 1, 2);
    pos.x = 25.5; pos.y = 14.5;
    add_ammo(&pos, 0.5, 2, 2);

    pos.x = 16.5; pos.y = 8.5;
    add_armor(&pos, 0.6, 30);
    pos.x = 16.5; pos.y = 7.5;
    add_armor(&pos, 0.6, 30);
    pos.x = 28.5; pos.y = 16.5;
    add_armor(&pos, 0.6, 30);
    pos.x = 21.5; pos.y = 12.5;
    add_armor(&pos, 0.6, 30);

    return OK;
}

int free_level(){
    close_map(&map);
    free_players();
    free_controllers();
    free_collision_table();
    free_pickups();
    free_gun_anims(&player.guns[0].gun);
    free_gun_anims(&player.guns[1].gun);
    free_gun_anims(&player.guns[2].gun);
    disable_pie_int();
    rtc_reset_pie();
    return OK;
}

int free_game(){
    kbc_disable_int();
    ms_set_default();
    reset_cmd_byte();
    reset_to_text_mode();
    kbc_enable_int();

    mouse_unsubscribe_int();
    kb_unsubscribe_int();
    timer_unsubscribe_int();

    while(reset_rtc() != OK);

    rtc_unsubscribe_int();
    
    free_sprites();
    free_uart_queues();
    return OK;
}
