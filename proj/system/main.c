// IMPORTANT: you must include the following line in all your C files

#include "file/map.h"
#include "lib/kbc.h"
#include "lib/timer.h"
#include "utils/data_types.h"
#include "player/player.h"
#include "game/level_manager.h"
#include "game/game_manager.h"
#include <lcom/lcf.h>
#include <lcom/proj.h>
#include <stdint.h>
#include <time.h>



extern tick_t time_counter;
extern player_info_t player;
extern player_info_t * enemies;
extern uint8_t last_kb_byte;
map_t map;

// Any header files included below this line should have been created by you

int main(int argc, char* argv[]) {
    // sets the language of LCF messages (can be either EN-US or PT-PT)
    lcf_set_language("EN-US");

    // enables to log function invocations that are being "wrapped" by LCF
    // [comment this out if you don't want/need it]
    //lcf_trace_calls("/home/lcom/labs/g01/proj/trace.txt");

    // enables to save the output of printf function calls on a file
    // [comment this out if you don't want/need it]
    //lcf_log_output("/home/lcom/labs/g01/proj/output.txt");

    // handles control over to LCF
    // [LCF handles command line arguments and invokes the right function]
    if (lcf_start(argc, argv))
        return 1;

    // LCF clean up tasks
    // [must be the last statement before return]
    lcf_cleanup();

    return 0;
}

int(proj_main_loop)(int argc, char* argv[]) {

    uint8_t mouse_bit_no, kb_bit_no, timer_bit_no, uart_bit_no, rtc_bit_no;
    init_game(&timer_bit_no, &mouse_bit_no, &kb_bit_no, &uart_bit_no, &rtc_bit_no);

    interrupt_handler(mouse_bit_no, kb_bit_no, timer_bit_no, uart_bit_no, rtc_bit_no);

    //free_level();
    free_game();

    return 0;
}
