#include "render.h"
#include "../file/map.h"
#include "../lib/timer.h"
#include "../lib/video_graphics.h"
#include "../player/player.h"
#include "../utils/data_types.h"
#include "../utils/utils.h"
#include "../graphics/graphics.h"
#include "../../data/textures/walls.h"
#include "../controls/controls.h"
#include "../../data/sprites/hud_bar_sprite.h"
#include "hud.h"
#include "../game/game_manager.h"
#include "../game/main_menu.h"
#include "../pickups/pickups.h"
#include "../../data/sprites/inv_sprites.h"
#include "../../data/sprites/menu_sprites.h"
#include "../../data/sprites/numbers/numbers.h"
#include <math.h>
#include <string.h>

extern graphics_data g_data;
extern player_info_t player;
extern player_info_t * enemies;
extern size_t num_enemies;
extern unsigned time_counter;
extern map_t map;
extern enum menu_state gun_menu_state;
extern enum game_state state_game;
extern controls_t movm;

void render_number(int value, pair_t * pos, int max_digits){
    int nums[max_digits];
    memset(nums, 0, sizeof(int)*max_digits);

    size_t digits = digitize(value, max_digits, nums);

    // Variable indicating if already got a digit
    // Used to ensure that the sprites will be 15 instead of 015
    bool got_num = false;


    for (size_t i = 0; i < digits; i++)
    {
        int val = nums[i];
        if(val != 0){
            got_num = true;
        }

        sprite_t * ptr = number[val];

        if(got_num || i == digits-1){
            draw_sprite(pos, ptr);
            pos->x += ptr->texture_size.x + 2;
        }
    }
}

void render_time_number(int value, pair_t * pos, int max_digits){
    int nums[max_digits];
    memset(nums, 0, sizeof(int)*max_digits);

    size_t digits = digitize(value, max_digits, nums);

    for (size_t i = 0; i < digits; i++)
    {
        int val = nums[i];

        sprite_t * ptr = number[val];

        draw_sprite(pos, ptr);
        pos->x += ptr->texture_size.x + 2;
    }
}

int (render_health_bar)(player_info_t * player, double player_dist, double x, double y, double* walls_dist_buf, void* buf){
    return OK;
}

int (render_crosshair)(){

    // Replace with settings crosshair settings

    int screen_width = g_data.h_res;
    int screen_height = g_data.v_res;

    int half_x = screen_width/2;
    int half_y = screen_height/2;

    int draw_x1_start = half_x - CROSSHAIR_SIZE/2;
    int draw_x1_end   = half_x - CROSSHAIR_SPACE;
   
    int draw_x2_start = screen_width/2 + CROSSHAIR_SPACE;
    int draw_x2_end   = screen_width/2 + CROSSHAIR_SIZE/2;


    int draw_y1_start = screen_height/2 - CROSSHAIR_SIZE/2;
    int draw_y1_end   = screen_height/2 - CROSSHAIR_SPACE;
    
    int draw_y2_start = screen_height/2 + CROSSHAIR_SPACE;
    int draw_y2_end   = screen_height/2 + CROSSHAIR_SIZE/2;


    int min_y = half_y-CROSSHAIR_SIDE/2;
    int max_y = half_y + CROSSHAIR_SIDE/2;

    int min_x = half_x-CROSSHAIR_SIDE/2;
    int max_x = half_x + CROSSHAIR_SIDE/2;

    draw_rect(draw_x1_start, min_y, draw_x1_end, max_y, CROSSHAIR_COLOR);
    
    draw_rect(draw_x2_start, min_y, draw_x2_end, half_y + CROSSHAIR_SIDE/2, CROSSHAIR_COLOR);
    
    draw_rect(min_x, draw_y1_start, max_x, draw_y1_end, CROSSHAIR_COLOR);
    
    draw_rect(min_x, draw_y2_start, max_x, draw_y2_end, CROSSHAIR_COLOR);

    return OK;
}

int (render_gun)() {
    gun_instance_t * gun_inst = &player.guns[player.gun_index];
    gun_t * gun = &(gun_inst->gun);
    sprite_t * sprite = gun->animations[gun_inst->curr_anim].sprite;
    pair_t start_pos; start_pos.x = g_data.h_res/2 - gun->animations[gun_inst->curr_anim].sprite->texture_size.x/2 + 10;

    // This + 2 solves a gap between the hud and the weapon
    start_pos.y = g_data.v_res - hud_sprite->texture_size.y /2 - gun->animations[gun_inst->curr_anim].sprite->texture_size.y - 1;

    return draw_sprite(&start_pos, sprite);
}

int(render_object)(render_t * render, vector_t * rel_position, double range) {
    int screen_width = g_data.h_res;
    int screen_height = g_data.v_res;

    double dist = vec_mag(rel_position);
    if (range > 0 && dist > range)
        return 1;

    vector_t look_dir;
    look_dir.x = player.direction.x;
    look_dir.y = player.direction.y;

    normalize(&look_dir);
    vector_t rel_norm = *rel_position;
    normalize(&rel_norm);
    double angle = (180 - retrieve_settings().fov) / 2;
    angle = angle * M_PI / 180;
    double prod_comp = cos(angle); 

    // Will be negative if looking in the opposite direction of the player
    double view_prodint = prod_int(&rel_norm, &look_dir);

    if (view_prodint <= prod_comp)
        return 1;


    vector_t cam_plane;
    cam_plane.x = player.camera_plane.x;
    cam_plane.y = player.camera_plane.y;

    vector_t inv_cam_matrix;

    double inv_det = 1 / determinant(&cam_plane, &look_dir);

    inv_cam_matrix.x = inv_det * (look_dir.y * rel_position->x - look_dir.x * rel_position->y);
    inv_cam_matrix.y = inv_det * (cam_plane.x * rel_position->y - cam_plane.y * rel_position->x);

    int sprite_screen_x = (int)((screen_width / 2.0) * (1.0 + inv_cam_matrix.x / inv_cam_matrix.y));

    // Calculates the relative height of the sprite
    int sprite_height = abs((int)(screen_height / inv_cam_matrix.y));

    pair_t start_pos, end_pos, offset, bounding_box;

    // Y Drawing start position
    start_pos.y = offset.y = screen_height * 256 - sprite_height * 256;
    start_pos.y /= 512;

    // Y Drawing end position
    end_pos.y = start_pos.y + sprite_height;

    // Calculates the relative width of the sprite
    int sprite_width = abs((int)(screen_height / inv_cam_matrix.y));

    // X Drawing start position
    start_pos.x = offset.x = sprite_screen_x * 512 - sprite_width * 256;
    start_pos.x /= 512;

    // X Drawing end position
    end_pos.x = start_pos.x + sprite_width;

    if (start_pos.x >= end_pos.x)
        return 1;

    bounding_box.x = sprite_width;
    bounding_box.y = sprite_height;

    double factor = 1-dist/range;
    
    render->end_pos = end_pos;
    render->offset = offset;
    render->bounding_box = bounding_box;
    render->distance = dist;
    render->factor = factor;

    return OK;
}

int(render_pickup)(player_info_t *p1, pickup_t * pickup, double range, render_t * pickup_render) {
    memset(pickup_render, 0, sizeof(render_t));

     // Vector joining both position
    vector_t rel_position;
    rel_position.x = pickup->collider.center.x - p1->position.x;
    rel_position.y = pickup->collider.center.y - p1->position.y;

    if(render_object(pickup_render, &rel_position, range))
        return 1;

    pickup_render->sprite = pickup->animation.sprite;
    pickup_render->has_sprite = true;

    return OK;
}

int(render_player)(player_info_t* p1, player_info_t* p2, double range, render_t * player_render, render_t * health_render, render_t * armor_render) {
    memset(player_render, 0, sizeof(render_t));
    memset(health_render, 0, sizeof(render_t));
    memset(armor_render, 0, sizeof(render_t));

    // Vector joining both players position
    vector_t rel_position;
    rel_position.x = p2->position.x - p1->position.x;
    rel_position.y = p2->position.y - p1->position.y;

    double ang = degree(&rel_position, &p2->direction);
    ang += 180;

    int idx = (int)round(ang / 45);

    idx = idx%8;


    // Weird front-left/ front-right shooting animation
    if(p2->curr_anim == 4 && (idx == 7 || idx < 2))
        idx = 0;

    int screen_width = g_data.h_res;
    int screen_height = g_data.v_res;

    if(render_object(player_render, &rel_position, range))
        return 1;

    player_render->sprite = p2->sprites[p2->curr_anim][idx].sprite;
    player_render->has_sprite = true;
    // Setting up rendering health

    double rem_health = (double)p2->health/(double)p2->max_health;
    double rem_armor = (double)p2->armor/(double)p2->max_armor;

    int sprite_height = player_render->bounding_box.y;
    int sprite_width = player_render->bounding_box.x;
    
    int bar_width = 0.5*sprite_width;
    int init_draw = player_render->end_pos.x - sprite_width/2 - bar_width/2;
    int max_draw  = init_draw + rem_health*bar_width;
    int max_draw_arm  = init_draw + rem_armor*bar_width;
    if(init_draw < 0) init_draw = 0;
    if(init_draw >= screen_width) init_draw = screen_width - 1;
    if(max_draw < 0) max_draw = 0;
    if(max_draw_arm < 0) max_draw_arm = 0;
    if(max_draw >= screen_width) max_draw = screen_width - 1;
    if(max_draw_arm >= screen_width) max_draw_arm = screen_width - 1;

    int y_draw = player_render->end_pos.y - sprite_height;
    int y_draw_end = player_render->end_pos.y - sprite_height*0.9;
    if(y_draw < 0) y_draw = 0;
    if(y_draw >= screen_height) y_draw = screen_height - 1;
    if(y_draw_end < 0) y_draw_end = 0;
    if(y_draw_end >= screen_height) y_draw_end = screen_height - 1;

    static uint32_t health_colors[6] = {
        0xff0d0d,
        0xff4e11,
        0xff8e15,
        0xfab733,
        0xacb334,
        0x69b34c
    };

    pair_t offset;
    offset.x = init_draw;
    offset.y = y_draw;

    pair_t end_pos;
    end_pos.x = max_draw;
    end_pos.y = y_draw_end;

    pair_t bounding_box;
    bounding_box.x = bar_width;
    bounding_box.y = 0.1*sprite_height;

    health_render->offset = offset;
    health_render->bounding_box = bounding_box;
    health_render->end_pos = end_pos;
    health_render->distance = player_render->distance;
    health_render->factor = player_render->factor;
    health_render->has_sprite = false;
    
    // Calculating the color to choose according to his current health
    int health_idx = ceil(6*((double)p2->health)/p2->max_health) - 1;

    if(health_idx < 0) health_idx = 0;

    health_render->color = health_colors[health_idx];

    
    end_pos.x = max_draw_arm;

    armor_render->offset = offset;
    armor_render->bounding_box = bounding_box;
    armor_render->end_pos = end_pos;
    armor_render->distance = player_render->distance-0.00390625;
    armor_render->factor = player_render->factor;
    armor_render->has_sprite = false;
    armor_render->color = 0x0e80ea;

    return OK;
}

void renders_insert_sorted(render_t * renders, render_t * value, size_t * size){
    if(value->distance <= 0.0) return;
    *size += 1;

    for (size_t i = 0; i < *size; i++)
    {
        // Currently not allocated any sprite
        if(i == *size-1) {
            renders[i] = *value;
        } else if(renders[i].distance < value->distance){
            render_t temp,val = *value;
            for (size_t j = i; j < *size; j++)
            {
                temp = renders[j];
                renders[j] = val;
                val = temp;
                if(val.distance <= 0.0) break;
            }
            break;
        }
    }
}

size_t setup_renders(render_t * renders, double range){
    size_t size = 0;
    for (size_t i = 0; i < num_enemies; i++)
    {
        if(!enemies[i].should_render)
            continue;

        render_t player_render;
        render_t health_render;
        render_t armor_render;
        if (OK != render_player(&player, &enemies[i], range, &player_render, &health_render, &armor_render)) {
            continue;
        }
            

        renders_insert_sorted(renders, &player_render,&size);
        renders_insert_sorted(renders, &health_render,&size);
        renders_insert_sorted(renders, &armor_render,&size);
        // Add render for health bar
    }
    size_t pickups_size = get_pickups_size();
    for(size_t i = 1; i < pickups_size; i++) {
        pickup_t pickup = get_pickup(i);

        if(pickup.picked_up) continue;

        render_t pickup_render;
        if (OK != render_pickup(&player, &pickup, range, &pickup_render))
            continue;

        renders_insert_sorted(renders, &pickup_render, &size);
    }
    return size;
}

void render_object_renders(render_t * renders, size_t num_renders){
    for (size_t i = 0; i < num_renders; i++)
    {
        render_t * render = &renders[i];
        // Adding a small interval to render the whole body even close to a wall 

        render->distance -= 0.2;

        if(render->has_sprite) {
            draw_rect_sprite(&render->end_pos, &render->offset, &render->bounding_box, render->sprite, render->distance, render->factor);
        }else {
            draw_darken_rect(render->offset.x, render->offset.y, render->end_pos.x, render->end_pos.y, render->color, render->factor, render->distance);
        }
    }  
}

int(draw_walls)(int32_t highest_wall, int32_t lowest_wall) {
    int err;
    int y_idx = -1, x_idx;//c=0;
    int last_y_idx = -1, last_x_idx = -1;
    int32_t width = g_data.h_res;
    line_t * wall_line;
    sprite_t * last_tex = NULL, * tex = NULL;
    uint32_t color;

    for(int32_t line = highest_wall; line < lowest_wall; line++) {
        for(int32_t col = 0; col < width; col++) {
            wall_line = &g_data.lines[col];

            if(wall_line->draw_higher <= line && wall_line->draw_lower >= line) {
                y_idx = (int)wall_line->texture.tex_y; // & (MAX_SPRITE_HEIGHT - 1);
                x_idx = wall_line->texture.tex_hit_x;
                tex = wall_line->texture.tex;

                if (wall_line->texture.last_tex_y != y_idx) {
                    if(tex == last_tex) {
                        if(last_x_idx != x_idx || last_y_idx != y_idx) {
                            color = tex->map[MAX_SPRITE_WIDTH * y_idx + x_idx];

                            if (wall_line->texture.darken)
                                color = darken(color, wall_line->texture.darken_factor);
                        }
                    } else {
                        color = tex->map[MAX_SPRITE_WIDTH * y_idx + x_idx];

                        if (wall_line->texture.darken)
                            color = darken(color, wall_line->texture.darken_factor);
                    }
                    
                    wall_line->texture.last_tex_y = y_idx;
                    wall_line->color = color;
                } else {
                    color = wall_line->color;
                }
                 
                err=set_px(col, line, color);

                last_tex = tex;
                last_y_idx = y_idx;
                last_x_idx = x_idx;
                wall_line->texture.tex_y += wall_line->texture.step;
            }
        }
    }

    return OK;
}

int(display_map)(double range) {
    vector_t pos;
    pos.x = player.position.x;
    pos.y = player.position.y;
    vector_t dir;
    dir.x = player.direction.x;
    dir.y = player.direction.y;
    vector_t plane;
    plane.x = player.camera_plane.x;
    plane.y = player.camera_plane.y;
    unsigned highest_wall = g_data.v_res, lowest_wall = 0;
    unsigned height = g_data.v_res;
    unsigned width = g_data.h_res;
    memset(g_data.lines, 0, g_data.h_res*sizeof(line_t));
    if (g_data.lines == NULL) {
        memset(g_data.lines, 0,  g_data.h_res*sizeof(line_t));
        if (g_data.lines == NULL)
            return 1;
    }
    range += START_DARKEN;

    for (unsigned x = 0; x < width; x++) {
        // camera_plane_it ranges from -1 to 1 and selects which part of the plane is being iterated.
        double camera_plane_it = (double)2 * x / width - 1;
        vector_t ray_dir;
        ray_dir.x = dir.x + plane.x * camera_plane_it;
        ray_dir.y = dir.y + plane.y * camera_plane_it;

        ray_hit_t ray_hit;
        shoot_ray(&player.position, &ray_dir, &ray_hit, range);
        int hit = ray_hit.hit;
        int side = ray_hit.side;
        coordinate_t map_pos = ray_hit.map_pos;

        double perpendicular_wall_dist;

        // draw wall
        if (hit) {
            // need to offset map_pos_x position by one to the right if the ray has a negative x ray direction:
            //           wall
            //      map_pos_x  ->  |   | <- map_pos_x + 1         |  <- camera plane with negative x ray direction
            //                     |   |------------------------- |
            //                                     ray
            int wall_offset_x = 0, wall_offset_y = 0;

            if (ray_dir.x < 0)
                wall_offset_x++;
            if (ray_dir.y < 0)
                wall_offset_y++;

            if (side == 0) {
                double x_dist = (map_pos.x + wall_offset_x - pos.x);
                perpendicular_wall_dist = x_dist / ray_dir.x;
            }
            else {
                double y_dist = (map_pos.y + wall_offset_y - pos.y);
                perpendicular_wall_dist = y_dist / ray_dir.y;
            }
            g_data.lines[x].dist = perpendicular_wall_dist;
            // The height of the line to be drawn
            unsigned line_height = (int)(height / perpendicular_wall_dist);

            // The starting and ending positions of the line
            g_data.lines[x].draw_higher = (height / 2) - (line_height / 2);
            if (g_data.lines[x].draw_higher < 0)
                g_data.lines[x].draw_higher = 0;
            g_data.lines[x].draw_lower = g_data.lines[x].draw_higher + line_height;
            if (g_data.lines[x].draw_lower >= (int)height) {
                g_data.lines[x].draw_lower = height - 1;
            }

            if((unsigned) g_data.lines[x].draw_higher < highest_wall) highest_wall = g_data.lines[x].draw_higher;
            if((unsigned) g_data.lines[x].draw_lower > lowest_wall) lowest_wall = g_data.lines[x].draw_lower;

            sprite_t * sprite;
            int tex = map.matrix[map_pos.y][map_pos.x]%7-1;
            sprite = walls_textures[tex];

            // Hit on the wall
            double x_hit;

            if(side == 0)
                x_hit = pos.y + perpendicular_wall_dist * ray_dir.y;
            else    
                x_hit = pos.x + perpendicular_wall_dist * ray_dir.x;
            
            x_hit -= floor(x_hit);
            

            // The x position on the texture
            int tex_hit_x = (int)(x_hit * (double) MAX_SPRITE_WIDTH);
            
            if((side == 0 && ray_dir.x > 0) || (side == 1 && ray_dir.y < 0))
                tex_hit_x = MAX_SPRITE_WIDTH - tex_hit_x - 1;

            
            // Calculating the step
            double step = (double)MAX_SPRITE_HEIGHT/(double)line_height;
            double tex_y = (g_data.lines[x].draw_higher - g_data.v_res/2 + line_height/2)*step;

            g_data.lines[x].texture.darken = perpendicular_wall_dist >= START_DARKEN;
            // Calculating x position in the sprite
            g_data.lines[x].texture.tex_hit_x = tex_hit_x;
            g_data.lines[x].texture.last_tex_y = -1;
            // Starting position of the y position on the texture
            g_data.lines[x].texture.tex_y = tex_y;
            
            g_data.lines[x].texture.step = step;
            g_data.lines[x].texture.side = side;


            // Making the darkening far away but smoother             
            g_data.lines[x].texture.darken_factor = range > 0 
                    ? 1 - (((ray_hit.distance-START_DARKEN)*1.0)/(range-START_DARKEN))
                    : 0;
            
            g_data.lines[x].texture.tex = sprite;
        }
        else {
           g_data.lines[x].dist = -1;
           g_data.lines[x].draw_higher = g_data.v_res;
        }
    }
    if(clear_screen()==2) return 1;
    return draw_walls(highest_wall, lowest_wall);
}

int render_timer(){
    int timer = *get_round_timer();
    uint8_t min = timer/60;
    uint8_t sec = timer%60;

    pair_t pos = {g_data.h_res - exit_menu_button->texture_size.x - 10, 0 + 0.01*g_data.v_res + 10};
    draw_sprite(&pos, background_button);
    pos.x += 60;
    pos.y += 25;
    render_number(min,&pos,1);
    draw_sprite(&pos,number[10]);
    pos.x += 53;
    render_time_number(sec,&pos,2);
    return OK;
}

int render_game(double range){
    display_map(range);

    // Num enemies + respective health bar + Num pickups
    size_t num_items = 3*num_enemies + get_pickups_size();
    render_t renders[num_items];

    size_t renders_size = setup_renders(renders, range);

    render_object_renders(renders, renders_size);

    render_crosshair();

    render_gun();

    // Timer will only be available in single player mode
    if(get_state() == GAME)
        render_timer();
    
    bool key_down = player.movm.kbd_state.gun_menu;
    //game_state_t st = get_state();
    if(gun_menu_state == DISPLAYING && !key_down){
        gun_menu_state = CLOSED;
    } else if(key_down){
        display_menu(player.guns, 3);
        gun_menu_state = DISPLAYING;
    }


    render_hud(&player);


    return OK;
}

void (draw_mouse)(){
    pair_t pos;
    pos.x = player.movm.mouse_pos.x - cursor_sprite->texture_size.x/2;
    pos.y = player.movm.mouse_pos.y - cursor_sprite->texture_size.y/2;
    draw_sprite_color_mask(&pos, cursor_sprite, retrieve_settings().mouse_color);
}

int display(double range){

    game_state_t st = get_state();

    switch (st)
    {
    case GAME:
        render_game(range);
        break;
    case MP_GAME:
        render_game(range);
        break;
    case MAIN_MENU:
        render_menu();
        break;
    case MP_MENU:
        render_mp_menu();
        break;
    case WIN_GAME:
    case LOSE_GAME:
        render_win_lose_screen();
        break;
    case DISCONNECT:
        render_disconnect_screen();       
        break;
    default:
        break;
    }

    put_buffer();
    return OK;
}


