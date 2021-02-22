#ifndef _G01_RENDER_H_
#define _G01_RENDER_H_

#include "../file/map.h"
#include "../player/player.h"
#include "../utils/data_types.h"
#include "../inventory/menu.h"

/** @defgroup render Rendering Functions
 * @{
 *
 * @brief Functions used to render the game's objects.
 */

#define CROSSHAIR_SIZE 50
#define CROSSHAIR_SIDE 5
#define CROSSHAIR_SPACE 5
#define CROSSHAIR_COLOR 0XFF0000

typedef struct render {
  /** @brief Structure with information of objects to render */
  /** @brief Indicates if it is a sprite or a color */
  bool has_sprite;
  /** @brief Sprite pointer */
  sprite_t * sprite;
  /** @brief Color to paint */
  uint32_t color;
  /** @brief End position of render object */
  pair_t end_pos;
  /** @brief Offset of render object */
  pair_t offset;
  /** @brief Bouding box of render object */
  pair_t bounding_box;
  /** @brief Distance to the render object */
  double distance;
  /** @brief Darkening factor */
  double factor;
} render_t;

/**
 * @brief Renders the currently loaded map using the ray casting render technique.
 *
 * The map is rendered by projecting walls into an imaginary camera plane in front of the player, perpendicular to the
 * player's direction.
 *
 * This is done by shooting rays from each of the screens' columns and determining if it hits a wall
 * (can be adjusted with the range parameter).
 *
 * If it hits a wall, it calculates the perpendicular distance from the camera plane to the wall and calculates
 * the height to be drawn by dividing a constant by the perpendicular distance. In this specific case, the constant
 * chosen was the height (or vertical resolution) of the screen.
 * (source and more details: https://lodev.org/cgtutor/raycasting.html)
 *
 * (FIRST APPROACH) To draw the columns, an auxiliary buffer is used that is rotated by 90 degrees, making it so
 * columns are drawn horizontally. This greatly increases performance as it reduces the number of cache misses, since
 * the elements of the columns are now stored one after the other (contiguously) in cache.
 * However, the downside is that this buffer later needs to be transposed.
 *
 * (SECOND APPROACH (current)) All columns are stored in a simple data structure that stores it's starting position,
 * ending position, texture index used and last color used. To allow for better cache optimization, these columns are
 * drawn bit by bit in each horizontal line (scan line effect).
 *
 * A range parameter is given, which defines the range the player can see and appropriately darkens textures to give
 * a darkness effect, or like the player is holding a sphere of light.
 *
 * @param range the range the player can see (in blocks), 0 if player can see forever.
 * @return 0 if returns ok.
 */
int(display_map)(double range);


/**
 * @brief Function to render a red crosshair
 *  The crosshair is always placed in the center of the screen according to the values specified
 */
int (render_crosshair)();

/**
 * @brief Renders a player on the screen.
 *
 * Displays a sprite/animation from a set of 8 to create a 3D illusion depending on the direction of both players. 
 * Adapted from the following tutorial: https://lodev.org/cgtutor/raycasting3.html
 * 
 * Does not allow for players to be drawn behind walls and darkens sprite if far away.
 * This is possible because we store the distances to the walls and compare the distance of the other player.
 * If the distance to the player is smaller than a wall, we draw the corresponding vertical line
 * 
 * The second player is darkened by a factor calculated with the distance between both players
 * 
 * Above the player a health bar is also rendered according to the remaining health of the second player
 * 
 * @param p1 pointer to the player
 * @param p2 pointer to the player to be drawn
 * @param range the range the player can be seen (in blocks), 0 if can always be seen
 * @param player_render Render object of player to be filled
 * @param health_render Render object of health bar
 * @param armor_render Render object of armor bar
 * @return 0 if returns ok
 */
int(render_player)(player_info_t* p1, player_info_t* p2, double range, render_t * render, render_t * health_render, render_t * armor_render);

/**
 * @brief Draws the mouse
 */
void (draw_mouse)();

/**
 * @brief Function to render a number
 * 
 * @param value Number to render
 * @param pos Initial position
 * @param max_digits Max digits to render - Truncates the first digits
 */
void render_number(int value, pair_t * pos, int max_digits);

/**
 * @brief Function to render a number filling the initial gaps with 0
 * 
 * @param value Number to render
 * @param pos Initial position
 * @param max_digits Max digits to render - Truncates the first digits
 */
void render_time_number(int value, pair_t * pos, int max_digits);


/**
 * @brief General function that renders the screen
 * Chooses what to render based on the state of the game
 */
int display(double range);

/**@}*/

#endif
