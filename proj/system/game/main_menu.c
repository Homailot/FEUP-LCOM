#include <lcom/lcf.h>
#include <lcom/proj.h>
#include <stdint.h>
#include <time.h>

#include "../../data/sprites/menu_sprites.h"
#include "../../data/sprites/menus_bg.h"
#include "../../data/sprites/numbers/numbers.h"
#include "../controls/controls.h"
#include "../graphics/hud.h"
#include "../graphics/render.h"
#include "../lib/video_graphics.h"
#include "../lib/rtc_controller.h"
#include "../player/player.h"
#include "../networking/networking.h"
#include "level_manager.h"
#include "game_manager.h"
#include "main_menu.h"

extern player_info_t player;

void render_menu() {

  pair_t pos = {0, 0};
  draw_sprite(&pos, menu_background);


  date_t date = *get_curr_date();

  pair_t spos = {g_data.h_res / 2 - single_menu_button->texture_size.x / 2, 200};
  draw_sprite(&spos, single_menu_button);

  pair_t mpos = {g_data.h_res / 2 - multi_menu_button->texture_size.x / 2, spos.y + 200};
  draw_sprite(&mpos, multi_menu_button);

  long y_pos = mpos.y + 350;

  pair_t epos = {g_data.h_res - exit_menu_button->texture_size.x - 0.01*g_data.h_res, y_pos - exit_menu_button->texture_size.y/2};
  draw_sprite(&epos, exit_menu_button);


  pos.x = 0.01*g_data.h_res; pos.y = y_pos - date_bg->texture_size.y/2;

  draw_sprite(&pos, date_bg);


  pos.x = 0.08*g_data.h_res; pos.y = y_pos - (75+70)/2;

  render_number(date.day,&pos,2);
  draw_sprite(&pos,number[11]); pos.x += 75;
  render_number(date.month,&pos,2);
  draw_sprite(&pos,number[11]); pos.x += 75;
  render_number(date.year,&pos,2);

  pos.x = 0.08*g_data.h_res; pos.y = epos.y + 75;


  render_time_number(date.hour,&pos,2);
  draw_sprite(&pos,number[10]); pos.x += 55;
  render_time_number(date.min,&pos,2);
  draw_sprite(&pos,number[10]); pos.x += 55;
  render_time_number(date.sec,&pos,2);

  draw_mouse();
  // IF MOUSE DOWN OK?

  if (player.movm.lb_held_down) {
    player.movm.lb_held_down = false;
    if (ms_detect_collision(player.movm.mouse_pos, spos, single_menu_button->texture_size)) {
        gm_init_game(1, 1, 1);
    }
    else if (ms_detect_collision(player.movm.mouse_pos, mpos, multi_menu_button->texture_size)) {
        set_state(MP_MENU);      
    } else if(ms_detect_collision(player.movm.mouse_pos, epos, exit_menu_button->texture_size)) {
        set_state(EXIT);
    }
  }
}

void render_mp_menu() {
    pair_t pos = {0, 0};
    draw_sprite(&pos, multiplayer_background);
    pos .y = 700; pos.x = g_data.h_res/2 - exit_menu_button->texture_size.x/2;
    draw_sprite(&pos, exit_menu_button);
    draw_mouse();

  if (player.movm.lb_held_down) {
    player.movm.lb_held_down = false;
    if (ms_detect_collision(player.movm.mouse_pos, pos, exit_menu_button->texture_size)){
        set_state(MAIN_MENU);
        set_network_state(INIT);
    }
  }
}

void render_win_lose_screen(){

  int8_t lvl = get_curr_lvl();
  game_state_t st = get_state();

  sprite_t * to_render = st == WIN_GAME ? win_background : lose_background;


  pair_t pos = {0,0};

  draw_sprite(&pos, to_render);
  
  pair_t bpos = {0,0};

  if(lvl == -1 || st == LOSE_GAME){
    bpos.y = 450; bpos.x = g_data.h_res/2 - restart_menu_button->texture_size.x/2;
    draw_sprite(&bpos, restart_menu_button);
  }
  else if(lvl != LAST_LVL && st == WIN_GAME){
    bpos.y = 450; bpos.x = g_data.h_res/2 - next_menu_button->texture_size.x/2;
    draw_sprite(&bpos, next_menu_button);  
  }
  pos .y = 700; pos.x = g_data.h_res/2 - exit_menu_button->texture_size.x/2;
  draw_sprite(&pos, exit_menu_button);
  draw_mouse();

  if (player.movm.lb_held_down) {
    player.movm.lb_held_down = false;
    if (ms_detect_collision(player.movm.mouse_pos, pos, exit_menu_button->texture_size)){
        set_state(MAIN_MENU);      
    }
    else if (lvl == -1 && ms_detect_collision(player.movm.mouse_pos, bpos, restart_menu_button->texture_size)){
        set_state(MP_MENU);
    }
    else if (lvl != LAST_LVL && st == WIN_GAME && ms_detect_collision(player.movm.mouse_pos, bpos, restart_menu_button->texture_size)){
        gm_init_game(lvl+1,1,1);
    }
    else if (st == LOSE_GAME && ms_detect_collision(player.movm.mouse_pos, bpos, restart_menu_button->texture_size)){
        gm_init_game(lvl,1,1);
    }
  }
}

void render_disconnect_screen() {
  pair_t pos = {0,0};

  draw_sprite(&pos, disconnect_background);
  pos .y = 700; pos.x = g_data.h_res/2 - exit_menu_button->texture_size.x/2;
  draw_sprite(&pos, exit_menu_button);
  draw_mouse();

  if (player.movm.lb_held_down) {
    player.movm.lb_held_down = false;
    if (ms_detect_collision(player.movm.mouse_pos, pos, exit_menu_button->texture_size)){
        set_state(MAIN_MENU);   
    }
  }
}

void render_loading_screen() {
}
