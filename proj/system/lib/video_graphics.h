#ifndef _G01_VIDEO_GRAPHICS_H_
#define _G01_VIDEO_GRAPHICS_H_

#include "sprites.h"
#include <lcom/lab5.h>
#include <lcom/lcf.h>
#include <machine/int86.h>

#include <stdint.h>
#include <stdio.h>

/** @defgroup video_graphics Video Graphics
 * @{
 *
 * @brief Functions used to interface with the graphics card.
 */

/**
 * @brief Struct that stores relevant information about the currently chosen video mode
 */
typedef struct
{
    /** @brief Pointer to the virtual address mapped */
    void* vram;
    /** @brief Horizontal resolution in pixels */
    unsigned h_res;
    /** @brief Vertical resolution in pixels */
    unsigned v_res;
    /// @brief Bytes per pixel used
    unsigned bytes_per_px;
    /// @brief Bits per pixel used
    unsigned bits_per_px;
    /** @brief virtual address of the buffer currently being drawn in the graphics card */
    void* drawing_address;
    /**  @brief virtual address of the second buffer (page buffer), to be drawn in the next frame */
    void* page_address;
    /// @brief Third address
    void* third_address;
    /// @brief Array of vertical lines
    line_t* lines;
    /// @brief The scanline of the drawing page on the allocated vram
    phys_bytes physical_drawing_scanline;
    /// @brief The scanline of the Physical address of scanning page on the allocated vram
    phys_bytes physical_page_scanline;
    /// @brief The scanline of the Physical address of third page on the allocated vram
    phys_bytes physical_third_scanline;
} graphics_data;

extern graphics_data g_data;

/**
 * @brief Returns number of bytes needed to represent that ammout of bits
 * @param bits - The number of bits
 * @return uint8_t - Number of bytes
 */
uint8_t(ceil_to_bytes)(uint8_t bits);

/**
 * @brief Returns memory size
 * @param info - VBE Mode info block
 */
unsigned int(get_mem_size)(vbe_mode_info_t* info);

/**
 * @brief Reads information of the graphics card
 * @param mode - The configuration of the graphics card
 * @param info - VBE Mode info block to be filled
 */
int(vbe_get_info)(uint16_t mode, vbe_mode_info_t* info);

/**
 * @brief Function to initialize a graphics mode
 * @param mode Graphical mode
 */
void* (vg_init)(uint16_t mode);

/**
 * @brief Starts a video mode and initializes the g_data structure
 *
 * @param mode the mode to be set
 * @return pointer to the virtual memory mapped
 */
void* (vg_init)(uint16_t mode);

/**
 * @brief Swaps the page buffer with the drawing buffer
 *
 * @return 0 if returns ok
 */
int(page_flip)();

/**
 * @brief Resetting minix to text mode
 */
int(reset_to_text_mode)();

/**
 * @brief Sets a pixel to a desired color in a position of the page buffer
 *
 * @param x_pos the x position in pixels counting from the left
 * @param y_pos the y position in pixels counting from the top
 * @param color the color to be drawn
 * @return 0 if returns ok.
 */
inline int(set_px)(unsigned x_pos, unsigned y_pos, uint32_t color) {
    unsigned real_bit_x = x_pos;
    unsigned real_bit_y = y_pos * g_data.h_res;

    int32_t* restrict buf_c = g_data.page_address;

    buf_c[real_bit_x + real_bit_y] = color;

    return OK;
}

/**
 * @brief Draws a horizontal line in the page buffer
 *
 * @param x the x starting position
 * @param y the y starting position
 * @param len the length of the line
 * @param color the color of the line
 * @return 0 if returns ok
 */
int(vg_draw_hor_line)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

/**
 * @brief Draws a rectangle
 *
 * @param x the x position (the column)
 * @param y the y position (the line)
 * @param x_end the x ending position
 * @param y_end the y ending position
 * @param color the color of the rectangle
 * @return 0 if returns ok.
 */
int(draw_rect)(uint16_t x, uint16_t y, uint16_t x_end, uint16_t y_end, uint32_t color);

/**
 * @brief Darkens an area of the screen
 * 
 * Quite usefull for menus to obscure some part of the game
 *
 * @param x the x position (the column)
 * @param y the y position (the line)
 * @param x_end the x ending position
 * @param y_end the y ending position
 * @param factor The factor used by the darken function
 * @return 0 if returns ok.
 */
int(darken_rect)(uint16_t x, uint16_t y, uint16_t x_end, uint16_t y_end, double factor);

/**
 * @brief Darkens an horizontal line of the screen
 * 
 * Quite useful for menus to obscure some part of the game
 *
 * @param x the x position (the column)
 * @param y the y position (the line)
 * @param len the width of the line
 * @param factor The factor used by the darken function
 * @return 0 if returns ok.
 */
int(darken_hor_line)(uint16_t x, uint16_t y, uint16_t len, double factor);

/**
 * @brief Darkens a pixel of the screen
 * 
 * @param x the x position (the column)
 * @param y the y position (the line)
 * @param factor The factor used by the darken function
 * @return 0 if returns ok.
 */
int(darken_pix)(uint16_t x, uint16_t y, double factor);

/**
 * @brief Clears the screen
 */
int(clear_screen)();

/**
 * @brief Draws a sprite in a specified position of the map creating a distance between the player and that sprite.
 * Adapted from the following tutorial: https://lodev.org/cgtutor/raycasting3.html
 * 
 * The initial and final values should be calculated beforehand
 * The function will draw the pixels from offset to end_pos, skipping/copying the necessary colors to apply the scaling
 * With this scaling method, a sprite will appear further away/close to the player according to its position
 * 
 * @param offset Start position
 * @param end_pos End position
 * @param bounding_box Width and height of the object to be drawn
 * @param sprite Sprite to be drawn
 * @param sprite_dist Distance from the player to the sprite to know if it is behind a wall and that which parts should be drawn
 * @param factor Factor of darkening color to make the object look farther away
 */
int(draw_rect_sprite)(pair_t * end_pos, pair_t * offset, pair_t * bounding_box, sprite_t * sprite, double sprite_dist, double factor);

/**
 * @brief Draws a rectangle that darkens with distance and is not rendered behind walls
 * @param x X position on screen
 * @param y Y position on screen
 * @param x_end Final X position on screen
 * @param y_end Final Y position on screen
 * @param color Color of rectangle
 * @param factor Darkening factor of color
 * @param dist Dist to the rectangle
 */
int(draw_darken_rect)(uint16_t x, uint16_t y, uint16_t x_end, uint16_t y_end, uint32_t color, double factor, double dist);

/**
 * @brief Puts the current buffer to the screen
 * @return OK if no errors occurred.
 */ 
int(put_buffer)();

/**
 * @brief Draws a sprite on the screen.
 * 
 * @param start_pos Pointer to the position on screen where the sprite will be drawn.
 * @param sprite Pointer to the sprite being drawn.
 * 
 * @return OK if no errors occurred.
 */
int(draw_sprite) (pair_t * start_pos, sprite_t * sprite);

/**
 * @brief Draws a sprite on the screen, using its 'shape' as a color mask.
 * 
 * @param start_pos Pointer to the position on screen where the sprite will be drawn.
 * @param sprite Pointer to the sprite being drawn.
 * @param color Color being used to draw the sprite.
 * 
 * @return OK if no errors occurred.
 */
int(draw_sprite_color_mask)(pair_t * start_pos, sprite_t * sprite, uint32_t color);

/**
 * @brief Darkens a color by a factor
 * @param color the color to be darkened
 * @param factor the factor to darken
 * @return the darkened color
 */
inline uint32_t darken(uint32_t color, double factor) {
    color_t * color_s = (void*)&color;
    color_s->rgb[0]= color_s->rgb[0] * factor;
    color_s->rgb[1]=color_s->rgb[1] * factor;
    color_s->rgb[2]=color_s->rgb[2] * factor;

    uint32_t * result = (void*)color_s;


    return *result;
}

/**@}*/

#endif
