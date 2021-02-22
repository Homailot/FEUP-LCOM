#include "game_manager.h"
#include "level_manager.h"
#include "../player/player.h"
#include "level_manager.h"
#include "../networking/networking.h"
#include "../lib/uart.h"
#include "../ai/enemy_controller.h"
#include "../pickups/pickups.h"
#include "../graphics/render.h"
#include "../lib/rtc_controller.h"
#include "../lib/rtc.h"
#include "../controls/controls.h"
#include "../lib/timer.h"
#include <lcom/lcf.h>
#include <lcom/proj.h>
#include <stdint.h>
#include <time.h>

extern tick_t time_counter;
extern player_info_t player;
extern player_info_t * enemies;
extern uint8_t last_kb_byte;

static int game_timer;

extern map_t map;

game_state_t get_state() {
    return state;
}

int * get_round_timer(){
    return &game_timer;
}

void set_state(game_state_t t_state) {
    if(state == MAIN_MENU){
        // Reenabling UIE interrupts when joining the menu
        enable_uie_int();
    }
    state = t_state;
}

int gm_init_game(int map_num, int game_mode, int player) {

    // Disabling UIE interrupts when entering a game
    // RTC Probably loses some interrupt sometimes mid game and breaks the interrupts from rtc
    disable_uie_int();

    int err = load_level(map_num, game_mode, player);
    if (err != OK) return err;

    set_state(GAME);
    game_timer = ROUND_TIMER;
    reset_pie_state();
    rtc_set_periodic_500ms();
    enable_pie_int();
    return OK;
}

void gm_reset_mp_game() {
    set_network_state(CLEANUP);
    free_level();
    init_player_ctrls();
}

void update_game_state() {
    switch (state) {
        case GAME:
            // CHECK PLAYER'S POSITION IF IN END BLOCK
            if (get_player()->health <= 0 || *get_round_timer()==0) {
                set_state(LOSE_GAME);
                free_level();
                init_player_ctrls();
                break;
            }
            vector_t pos = get_player()->position;
            int tex = map.matrix[(int)pos.y][(int)pos.x];
            
            // End game portal
            if(tex == -3){
                set_state(WIN_GAME);
                free_level();
                init_player_ctrls();
            }

            break;
        case MP_GAME:
            if (get_player()->health <= 0 || get_mp_enemy_player()->health <= 0) {
                gm_reset_mp_game();
                if (get_player()->health <= 0) {
                    set_state(LOSE_GAME);
                    gm_reset_mp_game(); 
                    uart_send_string(UART_WIN_GAME, 3);
                } else {
                    set_state(WIN_GAME);
                    gm_reset_mp_game(); 
                    uart_send_string(UART_LOSE_GAME, 3);
                }
            }
            break;
        case END_S_GAME:
            set_state(MAIN_MENU);
            free_level();
            init_player_ctrls();
            break;
        case END_M_GAME:
            set_state(LOSE_GAME);
            gm_reset_mp_game(); 
            uart_send_string(UART_WIN_GAME, 3);
            break;
        default:

            break;
    }
}

void (game_handler)() {
    network_state_t network_state;
    if (state == MP_GAME) {
        int out = uart_receive_data(&enemies[0]);
        if (out == LOSE_GAME_ERR) {
            set_state(LOSE_GAME);
            gm_reset_mp_game();
        } else if (out == WIN_GAME_ERR) {
            set_state(WIN_GAME);
            gm_reset_mp_game();
        }
        if (get_network_state() == DISCONNECTED) set_state(DISCONNECT);
    }

    if (state == GAME || state == MP_GAME) {
        handle_enemies();
        player_int_ih(&player);
        if (get_state() == GAME) bot_int_ih();
        update_pickups();

    } else if (state == MP_MENU) {
        uart_sync();
        network_state = get_network_state();
        if (network_state == CONNECTED) {
            load_level(3, 2, get_network_player());
            time_counter = 0;
        } else if (network_state == INIT) {
            set_state(MAIN_MENU);
            set_network_state(CLEANUP);
        }
    }

    if (state == MP_GAME)
        uart_send_data(&player);

    display(10);
    reset_player_mouse_movm(&player);
    update_game_state();
}

void(interrupt_handler)(uint8_t mouse_bit_no, uint8_t kb_bit_no, uint8_t timer_bit_no, uint8_t uart_bit_no, uint8_t rtc_bit_no) {
    int kb_irq_set    = BIT(kb_bit_no),
        mouse_irq_set = BIT(mouse_bit_no),
        timer_irq_set = BIT(timer_bit_no),
        uart_irq_set  = BIT(uart_bit_no),
        rtc_irq_set   = BIT(rtc_bit_no);

    int ipc_status,r;
    message msg;
    game_state_t game_state;
    while (last_kb_byte != BREAKCODE_ESC && (game_state = get_state())!=EXIT){
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != OK) {
            continue;
        }

        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & mouse_irq_set) {
                        handle_mouse_evt();
                    }

                    if (msg.m_notify.interrupts & kb_irq_set) {
                        handle_kbd_evt();
                    }

                    if (msg.m_notify.interrupts & uart_irq_set)
                    {
                        uart_ih();
                    }

                    if (msg.m_notify.interrupts & rtc_irq_set){
                        rtc_ih();
                    }

                    if (msg.m_notify.interrupts & timer_irq_set) {
                        timer_int_handler();
                        game_handler();
                    }
                    break;

                default:
                    break;
            }
        }
    }
}
