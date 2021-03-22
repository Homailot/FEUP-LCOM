
#include "video_graphics.h"
#include "../utils/data_types.h"
#include "../utils/utils.h"
#include "video_cmd.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <lcom/lcf.h>

#include "../graphics/graphics.h"

graphics_data g_data;

uint8_t(ceil_to_bytes)(uint8_t bits) {
    if (bits % 8 != 0) {
        bits += (8 - bits % 8);
    }
    return bits / 8;
}

/**
 * Returns size of vram in bytes
 */
unsigned int(get_mem_size)(vbe_mode_info_t* info) {

    uint8_t bits = info->BitsPerPixel;

    return info->XResolution * info->YResolution * ceil_to_bytes(bits);
}

int(vbe_get_info)(uint16_t mode, vbe_mode_info_t* info) {
    mmap_t map;

    reg86_t reg;

    size_t info_size = sizeof(*info);

    memset(&map, 0, sizeof(map));
    memset(&reg, 0, sizeof(reg));
    memset(info, 0, sizeof(*info));

    void* vmem = lm_alloc(info_size, &map);

    if (vmem == NULL)
        return 1;

    reg.intno = VBE_BIOS_CALL;
    reg.ax = VBE_CALL | VBE_INFO;
    reg.cx = mode;
    reg.di = PB2OFF((long)map.phys);
    reg.es = PB2BASE((long)map.phys);

    if (sys_int86(&reg) != OK)
        return 1;

    vbe_mode_info_t* tmp = (vbe_mode_info_t*)vmem;

    info[0] = tmp[0];

    lm_free(&map);

    return OK;
}


void* (vg_init)(uint16_t mode) {

    vbe_mode_info_t info;

    if (vbe_get_info(mode, &info) != OK)
        return NULL;

    phys_bytes address = info.PhysBasePtr;

    unsigned int phys_size = get_mem_size(&info);

    struct minix_mem_range mr;

    int err;

    mr.mr_base = address;
    mr.mr_limit = address + phys_size*3;

    if (OK != (err = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
        panic("sys_privctl (ADD_MEM) failed: %d\n", err);

    void * video_mem = vm_map_phys(SELF, (void*)mr.mr_base, phys_size);

    g_data.vram = video_mem;
    g_data.h_res = info.XResolution;
    g_data.v_res = info.YResolution;
    g_data.bytes_per_px = ceil_to_bytes(info.BitsPerPixel);
    g_data.bits_per_px = info.BitsPerPixel;

    if (video_mem == MAP_FAILED)
        panic("couldn't map video memory");

    void * video_mem2 = vm_map_phys(SELF, (void*)(mr.mr_base+phys_size), phys_size);

     if (video_mem2 == MAP_FAILED)
        panic("couldn't map video memory");

    void * video_mem3 = vm_map_phys(SELF, (void*)(mr.mr_base+phys_size*2), phys_size);

    if (video_mem2 == MAP_FAILED)
        panic("couldn't map video memory");

    reg86_t reg;

    memset(&reg, 0, sizeof(reg));

    reg.intno = VBE_BIOS_CALL;
    reg.ax = VBE_CALL | VBE_FUNC;

    reg.bx = LINEAR_FRAME_BUFFER | mode;

    if (sys_int86(&reg) != OK)
        return NULL;

    g_data.drawing_address = video_mem;
    g_data.page_address = video_mem2;
    g_data.third_address = video_mem3;
    g_data.physical_drawing_scanline = 0;
    g_data.physical_page_scanline = g_data.v_res;
    g_data.physical_third_scanline = g_data.v_res*2;

    memset(g_data.page_address, 0, g_data.h_res*g_data.v_res*g_data.bytes_per_px);
    memset(g_data.drawing_address, 0, g_data.h_res*g_data.v_res*g_data.bytes_per_px);
    memset(g_data.third_address, 0, g_data.h_res*g_data.v_res*g_data.bytes_per_px);

    g_data.lines = calloc(g_data.h_res, sizeof(line_t));
    if(g_data.lines == NULL) {
        g_data.lines = calloc(g_data.h_res, sizeof(line_t));
        if(g_data.lines == NULL) return NULL;
    }

    return video_mem;
}

int(page_flip)() {
    reg86_t reg;
    memset(&reg, 0, sizeof(reg));

    reg.intno = VBE_BIOS_CALL;
    reg.ax = VBE_CALL | VBE_SET_DISPLAY_FUNC;
    reg.bx = VBE_SET_DISPLAY_RETRACE;
    reg.cx = 0;
    reg.dx = g_data.physical_page_scanline;

    if (sys_int86(&reg) != OK)
        return 1;

    void* tmp = g_data.page_address;
    g_data.page_address = g_data.third_address;
    g_data.third_address = g_data.drawing_address;
    g_data.drawing_address = tmp;
    
    phys_bytes tmp_phys = g_data.physical_page_scanline;
    g_data.physical_page_scanline = g_data.physical_third_scanline;
    g_data.physical_third_scanline = g_data.physical_drawing_scanline;
    g_data.physical_drawing_scanline = tmp_phys;

    return OK;
}

int(reset_to_text_mode)() {
    free(g_data.page_address);
    free(g_data.drawing_address);
    free(g_data.third_address);
    free(g_data.lines);

    memset(&g_data, 0, sizeof(graphics_data));

    return vg_exit();
}

int(clear_screen)() {
    if(memset(g_data.page_address, 0, g_data.h_res * g_data.v_res * g_data.bytes_per_px) == NULL) {
        if(memset(g_data.page_address, 0, g_data.h_res * g_data.v_res * g_data.bytes_per_px) == NULL) {
            return 2;
        }
    }

    return OK;
}

int(vg_draw_hor_line)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (size_t i = 0; i < len; i++) {
        if (set_px(x + i, y, color) == 2)
            return 1;
    }
    return OK;
}

int(draw_rect)(uint16_t x, uint16_t y, uint16_t x_end, uint16_t y_end, uint32_t color) {
    int len = x_end - x;
    while (y < y_end) {
        vg_draw_hor_line(x,y, len, color);
        y++;
    }
    return OK;
}

int(draw_dists_hor_line)(uint16_t x, uint16_t y, uint16_t len, uint32_t color, double dist) {
    for (size_t i = 0; i < len; i++) {
        if(dist < g_data.lines[x+i].dist || g_data.lines[x+i].dist < 0){
            if (set_px(x + i, y, color) == 2)
                return 1;
       }
    }
    return OK;
}

int(draw_darken_rect)(uint16_t x, uint16_t y, uint16_t x_end, uint16_t y_end, uint32_t color, double factor, double dist) {
    color = darken(color, factor);
    int len = x_end - x;
    while (y < y_end) {
        draw_dists_hor_line(x,y, len, color, dist);
        y++;
    }

    return OK;
}


int(draw_rect_sprite)(pair_t * end_pos, pair_t * offset, pair_t * bounding_box, sprite_t * sprite, double sprite_dist, double factor) {        
    if(end_pos->y >= (long)g_data.v_res)
        end_pos->y = g_data.v_res - 1;
    if(end_pos->x >= (long)g_data.h_res)
        end_pos->x = g_data.h_res - 1;
    if(end_pos->y < 0 )
        end_pos->y = 0;
    if(end_pos->x < 0)
        end_pos->x = 0;

    int32_t start_line = (512*bounding_box->y*sprite->y_offset+MAX_SPRITE_HEIGHT*offset->y)/(512*MAX_SPRITE_HEIGHT);
    int32_t start_col = (512*bounding_box->x*sprite->x_offset+MAX_SPRITE_WIDTH*offset->x)/(512*MAX_SPRITE_WIDTH);
    
    if(start_line < 0) start_line = 0;
    if(start_col < 0) start_col = 0;
    if(start_line >= (long)g_data.v_res)
        start_line = g_data.v_res - 1;
    if(start_col >= (long)g_data.h_res)
        start_col = g_data.h_res - 1;

    double start_tex_x = (((start_col - offset->x/512.0) *  MAX_SPRITE_WIDTH) / bounding_box->x) - sprite->x_offset;
    double start_tex_y = (((start_line- offset->y/512.0) *  MAX_SPRITE_HEIGHT) / bounding_box->y)  - sprite->y_offset;
 
    if(start_tex_x < 0) start_tex_x = 0;
    if(start_tex_y < 0) start_tex_y = 0;
    
    double step_y = ((double)MAX_SPRITE_HEIGHT)/bounding_box->y;
    double step_x = ((double)MAX_SPRITE_WIDTH)/bounding_box->x;
    
    double count_y = start_tex_y, count_x = start_tex_x;
    int32_t texture_y, texture_x, old_texture_x = -1;
    uint32_t real_texture_y, color = 0, d_color = 0;
    for(int32_t line = start_line; line <= end_pos->y ; line++) {
        texture_y = (int32_t)count_y;

        if(texture_y >= sprite->texture_size.y)  break;
        real_texture_y = texture_y * sprite->texture_size.x;

        for (int32_t i = start_col; i <= end_pos->x; i++) {
            texture_x = (int32_t)count_x;

            if(texture_x >= sprite->texture_size.x) break;

            if(g_data.lines[i].dist < 0 || g_data.lines[i].dist >= sprite_dist) {
                if(old_texture_x != texture_x) {
                    old_texture_x = texture_x;
                    color = sprite->map[texture_x + real_texture_y];

                    if(color != sprite->invis_color) {
                        if(sprite_dist >= START_DARKEN)
                            d_color = darken(color, factor);
                        else d_color = color;
                        set_px(i, line, d_color);
                    }
                } else if(color != sprite->invis_color) {
                    set_px(i, line, d_color);
                }                
            }

            count_x += step_x;
        }

        count_y += step_y;
        count_x = start_tex_x;
    }

    return OK;
}

int(darken_rect)(uint16_t x, uint16_t y, uint16_t x_end, uint16_t y_end, double factor){
  int len = x_end - x;
  while (y < y_end) {
    darken_hor_line(x, y, len, factor);
    y++;
  }
  return OK;
}

int(darken_hor_line)(uint16_t x, uint16_t y, uint16_t len, double factor) {
    for (size_t i = 0; i < len; i++) {
        if (darken_pix(x + i, y, factor) == 2)
            return 1;
    }
    return OK;
}

int(darken_pix)(uint16_t x, uint16_t y, double factor){
    if (x >= g_data.h_res || y >= g_data.v_res)
        return 1;

    unsigned real_bit_x = x;
    unsigned real_bit_y = y * g_data.h_res;

    uint32_t* restrict buf_c = g_data.page_address;
    uint32_t * color_24 = (void*) (buf_c + real_bit_x + real_bit_y) ;

    buf_c[real_bit_x + real_bit_y] = darken(*color_24, factor);
    return OK;
}

int(draw_sprite) (pair_t * start_pos, sprite_t * sprite) {
    unsigned tex_y = 0, tex_x = 0, color;
    unsigned y_start = start_pos->y, x_start = start_pos->x;

    for(unsigned y = y_start; y < (unsigned)y_start + sprite->texture_size.y - 1; y++, tex_y += sprite->texture_size.x) {
        if(y >= g_data.v_res) break;
        tex_x = 0;

        for(unsigned x = x_start; tex_x < (unsigned)sprite->texture_size.x; x++, tex_x++) {
            if(x >= g_data.h_res) break;
            color = sprite->map[tex_y + tex_x];

            if(color != sprite->invis_color) set_px(x, y, color);
        }
    }

    return OK;
}

int(draw_sprite_color_mask) (pair_t * start_pos, sprite_t * sprite, uint32_t color) {
    unsigned tex_y = 0, tex_x = 0, i_color;
    unsigned y_start = start_pos->y, x_start = start_pos->x;

    for(unsigned y = y_start; y < (unsigned)y_start + sprite->texture_size.y - 1; y++, tex_y += sprite->texture_size.x) {
        if(y >= g_data.v_res) break;
        tex_x = 0;

        for(unsigned x = x_start; tex_x < (unsigned)sprite->texture_size.x; x++, tex_x++) {
            if(x >= g_data.h_res) break;
            i_color = sprite->map[tex_y + tex_x];

            if(i_color != sprite->invis_color) set_px(x, y, color);
        }
    }

    return OK;
}

int(put_buffer)() {
    if (page_flip() != OK) return 2;

    return OK;
}
