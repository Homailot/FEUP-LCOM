#ifndef _G01_CONTROLS_H_
#define _G01_CONTROLS_H_

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

#include "../utils/data_types.h"
#include "kbd_layout.h"

/** @defgroup controls Controls
 * @{
 *
 * @brief Functions for treating player input and converting to movement.
 */

/// @brief Mouse Packet received from the KBC.
typedef struct packet packet_t;

/// @brief Settings structure used to store settings.
typedef struct settings {
    /// How fast the mouse moves in relation to movement reported from the KBC.
    double mouse_sensitivity;
    /// Size of the crosshairs.
    int cross_size;
    /// Width of the crosshairs.
    int cross_width;
    /// Player's Field of Vision.
    int fov;
    /// Mouse's color.
    uint32_t mouse_color;
} settings_t;

/// @brief Structure that stores the state of keyboard related controls.
typedef struct {
    bool x_pos;
    bool x_neg;
    bool y_pos;
    bool y_neg;
    bool sprint;
    bool crouch;
    bool gun_menu;
    bool nums_press[10];
} kbd_ctrl_state;

/// @brief Key states
typedef enum key_state{
    KEY_UP,
    KEY_DOWN
} key_state_t;

/// @brief Controls structure that stores all control related objects of a certain entity.
typedef struct {
    /// @brief How much and in what direction an entity moves.
    vector_t movement;
    /// @brief The keyboard state.
    kbd_ctrl_state kbd_state;
    /// @brief Position of the mouse on the screen.
    vector_t mouse_pos;
    /// @brief Mouse packet received from the KBC.
    packet_t mouse_pkt;
    /// @brief State of the mouse
    key_state_t mouse_state;
    double mouse_desl;
    bool firing;
    bool lb_single_tap;
    bool lb_held_down;
} controls_t;


/// @brief Initializes the settings with default values.
void(init_settings)();

/**
 * @brief Gets the current settings.
 * @return The current settings.
 */
settings_t (retrieve_settings)();

/**
 * @brief Handles a keyboard event after an interrupt
 * Calls kbc_ih()
 * Evaluates important information such as make codes and double bytes
 * Calls kbd_parse_data to use that information
 */
void(handle_kbd_evt)();

/**
 * @brief Parses keyboard data
 * Finds usefull information for movement and activates variables in controls_t object from player
 * 
 * @param bytes Bytes of the keyboard (can be a double byte)
 * @param size Size of the keyboard data
 * @param make True if the keyboard data is a make code
 */
void(kbd_parse_data)(uint8_t* bytes, size_t size, bool make);

/**
 * @brief Handles a mouse event after an interrupt
 * Calls mouse_ih()
 * Evaluates important information such as current packet's index
 * Detects invalid first bytes
 * Calls parse_packet and mouse_parse_data to use that information
 */
void(handle_mouse_evt)();

/**
 * @brief Parses keyboard data
 * Finds usefull information for movement and interaction and activates variables in controls_t object from player
 */
void(mouse_parse_data)();

/**
 * @brief Detects if the mouse is within the boundings of a rectangle
 * @param ms_pos Position of the mouse
 * @param button_pos Top left corner position of the button
 * @param button_size Width and height of the button
 */
bool (ms_detect_collision)(vector_t ms_pos, pair_t button_pos, coordinate_t button_size);
/**@}*/

#endif
