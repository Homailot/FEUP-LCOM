#include "sprites.h"
#include "../file/file.h"
#include "../../data/sprites/player/player_sprites.h"
#include "../../data/sprites/guns/pistol_sprites.h"
#include "../../data/sprites/guns/rifle_sprites.h"
#include "../../data/sprites/guns/ray_sprites.h"
#include "../../data/sprites/numbers/numbers.h"
#include "../../data/sprites/hud_bar_sprite.h"
#include "../../data/sprites/pickups/pickup_sprites.h"
#include "../../data/sprites/inv_sprites.h"
#include "../../data/sprites/menu_sprites.h"
#include "../../data/sprites/menus_bg.h"
#include "../utils/utils.h"
#include "../../data/textures/walls.h"
#include <stdint.h>
#include <lcom/xpm.h>

extern time_t time_counter;

int (load_mp_background)() {
    multiplayer_background = (sprite_t *) malloc(sizeof(sprite_t));
    return load_sprite(multiplayer_back_xpm, multiplayer_background);
}

int (load_win_background)() {
    win_background = (sprite_t *) malloc(sizeof(sprite_t));
    return load_sprite(win_back_xpm, win_background);
}

int (load_lose_background)() {
    lose_background = (sprite_t *) malloc(sizeof(sprite_t));
    return load_sprite(lose_back_xpm, lose_background);
}

int (load_disc_background)() {
    disconnect_background = (sprite_t *) malloc(sizeof(sprite_t));
    return load_sprite(disconnect_back_xpm, disconnect_background);
}


int load_sprites(){
    if(OK != init_gun_menu_sprites()) return 1;
    if(OK != init_cursor_sprite()) return 1;
    if(OK != init_walls_sprites()) return 1;
    if(OK != load_player_sprites()) return 1;
    if(OK != load_pickup_sprites()) return 1;
    if(OK != load_gun_sprites()) return 1;
    if(OK != load_numbers_sprites()) return 1;
    if(OK != load_menu_sprites()) return 1;
    if(OK != load_background()) return 1; 
    if(OK != load_mp_background()) return 1;
    if(OK != load_win_background()) return 1;
    if(OK != load_lose_background()) return 1;
    if(OK != load_disc_background()) return 1;

    return load_hud_sprite();
}

int free_sprites(){
    free_walls_sprites();
    free_cursor_sprite();
    free_gun_menu_sprites();
    free_numbers_sprites();
    free_hud_sprite();
    free_pickup_sprites();
    free_player_sprites();
    free_gun_sprites();
    free_menu_sprites();
    free_background();
    return OK;
}

int (load_xpm_file)(const char * file_path, xpm_image_t * image) {
    //xpm_map_t xpm_map;
    //char * cur_row = calloc(100, sizeof(char));
    //xpm_row_t row = cur_row;

    FILE* file = fopen(file_path, "r");
    if (!file) {
        printf("error: %s\n", strerror(errno));
        return BAD_FILE;
    }

    char buffer[100];
    fgets(buffer, 100, file);
    fgets(buffer, 100, file);
    //char c;



    fclose(file);

    //xpm_load()

    return OK;
}

int (load_sprite)(xpm_map_t map, sprite_t * sprite) {
    xpm_image_t image;

    if( NULL == xpm_load(map, XPM_8_8_8, &image))
        xpm_load(map, XPM_8_8_8, &image);
    
    int32_t new_size = image.height*image.width;
    int24_t * pixmap = (int24_t*) image.bytes;

    sprite->map = (uint32_t *) malloc(new_size*sizeof(uint32_t));
    if(sprite->map == NULL) return 1;

    for(int i = 0; i < new_size; i++) {
        int24_t color24 = pixmap[i];

        sprite->map[i] = i24_to_int(&color24);
    }
    sprite->texture_size.x = image.width;
    sprite->texture_size.y = image.height;
    sprite->x_offset = (MAX_SPRITE_WIDTH - sprite->texture_size.x) / 2;
    sprite->y_offset = (MAX_SPRITE_HEIGHT - sprite->texture_size.y);
    sprite->invis_color = xpm_transparency_color(XPM_8_8_8);

    free(image.bytes);

    return OK;
}

int (load_background)(){
    menu_background = (sprite_t *) malloc(sizeof(sprite_t));
    return load_sprite(BACKGROUND_xpm, menu_background);
}

int (free_background)(){
    free(menu_background);
    free(multiplayer_background);
    free(win_background);
    free(lose_background);
    free(disconnect_background);

    return OK;
}

int (load_menu_sprites)(){
    single_menu_button = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(single_xpm, single_menu_button)!= OK) return 1;
    multi_menu_button = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(multi_xpm, multi_menu_button)!= OK) return 1;
    exit_menu_button = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(exit_xpm, exit_menu_button)!= OK) return 1;
    date_bg = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(date_bg_xpm, date_bg)!= OK) return 1;
    background_button = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(button_bg_xpm, background_button)!= OK) return 1;
    next_menu_button = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(next_button_xpm, next_menu_button)!= OK) return 1;
    restart_menu_button = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(restart_button_xpm, restart_menu_button)!= OK) return 1;
    
    return OK;
}

int (free_menu_sprites)(){
    free(single_menu_button);
    free(multi_menu_button);
    free(exit_menu_button);
    free(date_bg);
    free(background_button);
    free(next_menu_button);
    free(restart_menu_button);
    return OK;
}

int (load_player_sprites)(){
    for (size_t i = 0; i < 8; i++)
    {
        sprite_t * ptr = (sprite_t *) malloc(sizeof(sprite_t));
        if(load_sprite(xpm_playa[i], ptr)!= OK) return 1;
        playa[i] = ptr;

        ptr = (sprite_t *) malloc(sizeof(sprite_t));
        if(load_sprite(xpm_playb[i], ptr)!= OK) return 1;
        playb[i] = ptr;

        ptr = (sprite_t *) malloc(sizeof(sprite_t));
        if(load_sprite(xpm_playc[i], ptr)!= OK) return 1;
        playc[i] = ptr;

        ptr = (sprite_t *) malloc(sizeof(sprite_t));
        if(load_sprite(xpm_playd[i], ptr)!= OK) return 1;
        playd[i] = ptr;

        ptr = (sprite_t *) malloc(sizeof(sprite_t));
        if(load_sprite(xpm_playe[i], ptr)!= OK) return 1;
        playe[i] = ptr;

        ptr = (sprite_t *) malloc(sizeof(sprite_t));
        if(load_sprite(xpm_playf[i], ptr)!= OK) return 1;
        playf[i] = ptr;

        ptr = (sprite_t *) malloc(sizeof(sprite_t));
        if(load_sprite(xpm_dead[i], ptr)!= OK) return 1;
        dead[i] = ptr;
    }
    return OK;
}

int (load_pickup_sprites)() {
    sprite_t * ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(ammoa0, ptr) != OK) return 1;
    ammos[0] = ptr;

    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(clipa0, ptr) != OK) return 1;
    ammos[1] = ptr;

    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(celpa0, ptr) != OK) return 1;
    ammos[2] = ptr;

    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(media0, ptr) != OK) return 1;
    media = ptr;

    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(arm1a0, ptr) != OK) return 1;
    arm1a = ptr;

    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(arm1b0, ptr) != OK) return 1;
    arm1b = ptr;


    return OK;
}

int (load_gun_sprites)() {
    sprite_t * ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(pisia0, ptr) != OK) return 1;
    pisia = ptr;

    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(pisfa0, ptr) != OK) return 1;
    pisfa = ptr;

    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(pisfb0, ptr) != OK) return 1;
    pisfb = ptr;

    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(pisfc0, ptr) != OK) return 1;
    pisfc = ptr;

    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(pisfd0, ptr) != OK) return 1;
    pisfd = ptr;

    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(pisfe0, ptr) != OK) return 1;
    pisfe = ptr;


    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(rifle1_xpm, ptr) != OK) return 1;
    rifle1 = ptr;

    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(rifle2_xpm, ptr) != OK) return 1;
    rifle2 = ptr;

    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(rifle3_xpm, ptr) != OK) return 1;
    rifle3 = ptr;
    
    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(rifle4_xpm, ptr) != OK) return 1;
    rifle4 = ptr;

    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(raygun0_xpm, ptr) != OK) return 1;
    ray0 = ptr;

    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(raygun1_xpm, ptr) != OK) return 1;
    ray1 = ptr;

    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(raygun2_xpm, ptr) != OK) return 1;
    ray2 = ptr;

        ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(raygun3_xpm, ptr) != OK) return 1;
    ray3 = ptr;

    ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(raygun4_xpm, ptr) != OK) return 1;
    ray4 = ptr;

    return OK;
}

int load_numbers_sprites(){
    for (size_t i = 0; i < 12; i++)
    {
        sprite_t * ptr = (sprite_t *) malloc(sizeof(sprite_t));
        if(load_sprite(xpm_numbers[i], ptr) != OK) return 1;
        number[i] = ptr;
    }
    return OK;
}

int load_hud_sprite(){
    sprite_t * ptr = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(HUD_xpm, ptr) != OK) return 1;
    hud_sprite = ptr;
    return OK;
}

int free_numbers_sprites(){
    for (size_t i = 0; i < 10; i++)
    {
        free(number[i]);
    }
    return OK;
}

int free_hud_sprite(){
    free(hud_sprite);
    hud_sprite = NULL;
    return OK;
}

int (init_cursor_sprite)(){
    cursor_sprite = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(cursor_xpm, cursor_sprite) != OK) return 1;
    return OK;
}

int (init_gun_menu_sprites)(){
    gun_menu_sprites[0] = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(rifle_menu_xpm, gun_menu_sprites[0]) != OK) return 1;

    gun_menu_sprites[1] = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(pistol_menu_xpm, gun_menu_sprites[1]) != OK) return 1;

    gun_menu_sprites[2] = (sprite_t *) malloc(sizeof(sprite_t));
    if(load_sprite(plasma_menu_xpm, gun_menu_sprites[2]) != OK) return 1;
    
    return OK;
}

int (free_gun_menu_sprites)(){
    for (size_t i = 0; i < 3; i++)
    {
        free(gun_menu_sprites[i]);
    }
    return OK;
}

int (free_player_sprites)(){
    for (size_t i = 0; i < 8; i++)
    {
        free(playa[i]);
        free(playb[i]);
        free(playc[i]);
        free(playd[i]);
        free(playe[i]);
        free(playf[i]);
        free(dead[i]);
    }
    return OK;
}

int (init_walls_sprites)(){
    int num_sprites = 6;

    walls_textures = (sprite_t **)malloc(num_sprites * sizeof(sprite_t *));

    for (int i = 0; i < num_sprites; i++)
    {
        sprite_t * ptr = (sprite_t*)malloc(sizeof(sprite_t));
        if(load_sprite(wall_tex_xpm[i], ptr)!= OK) return 1;
        walls_textures[i] = ptr;

    }

    return OK;
}

int (free_walls_sprites)(){
    int num_sprites = 6;
    
    for (size_t i = 0; i < (size_t)num_sprites; i++)
    {
        free(walls_textures[i]);
    }

    free(walls_textures);   
    
    return OK;
}

int (free_gun_sprites)(){
    free(pisfa);
    free(pisfb);
    free(pisfc);
    free(pisfd);
    free(pisfe);
    free(pisia);
    free(rifle1);
    free(rifle2);
    free(rifle3);
    free(rifle4);
    free(ray0);
    free(ray1);
    free(ray2);
    free(ray3);
    free(ray4);
    return OK;
}

int (free_cursor_sprite)(){
    free(cursor_sprite);
    return OK;
}

int (free_pickup_sprites)() {
    free(ammos[0]);
    free(ammos[1]);
    free(ammos[2]);
    free(media);
    free(arm1a);
    free(arm1b);
    return OK;
}

bool (update_anim)(animated_sprite_t * anim){
    int ints = anim->frames_per_sprite;
    anim->state = PROGRESS;
    if(time_counter % ints == 0){
        anim->cur_frame ++;
        anim->cur_frame = anim->cur_frame % anim->num_fig;
        anim->sprite = anim->frames[anim->cur_frame];

        switch (anim->state)
        {
        case PROGRESS:
            if(anim->cur_frame == 0){
                anim->state = FINISHED;
                return true;
            } 
            break;
        default:
            break;
        }
    }

    return false;
}
