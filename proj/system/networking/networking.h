#ifndef _G01_NETWORKING_H_
#define _G01_NETWORKING_H_

#include "../player/player.h"

#include <lcom/lab5.h>
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/** @defgroup networking Networking
 * @{
 *
 * @brief Functions used to control the communication protocol.
 */

typedef struct network_packet{
    /// @brief Mouse byte
    uint8_t mouse_byte;
    /// @brief Mouse msb indicating signal of mouse byte
    uint8_t mouse_msb;
    /// @brief W key down
    bool w_down;
    /// @brief A key down
    bool a_down;
    /// @brief S key down
    bool s_down;
    /// @brief D key down
    bool d_down;
    /// @brief Ctrl key down
    bool ctrl_down;
    /// @brief Shift key down
    bool shift_down;
    /// @brief Mouse 1 key down
    bool shoot_down;
    /** @brief Gun id - Must be between 1 and 3 **/
    uint8_t gun_down;
} network_packet_t;

/** Packet parsing */

#define DATA_SHOOTDOWN BIT(0)
#define DATA_CTRLDOWN BIT(1)
#define DATA_SHIFTDOWN BIT(2)
#define DATA_DDOWN BIT(3)
#define DATA_SDOWN BIT(4)
#define DATA_ADOWN BIT(5)
#define DATA_WDOWN BIT(6)
#define DATA_MOUSE_MSB BIT(7)
#define DATA_GUN_DOWN(n) BIT(n-1)
#define DATA_GUN_DOWN_1 BIT(0)
#define DATA_GUN_DOWN_2 BIT(1)
#define DATA_GUN_DOWN_3 BIT(2)
#define DATA_GUN_BITS (BIT(0) | BIT(1) | BIT(2))

#define DATA_CONFIRM_PACKET (BIT(7) | BIT(5) | BIT(3))
#define DATA_CONFIRM_MASK 0xf8

#define DATA_BYTES 3


/** Uart sync */

#define UART_JOIN_REQUEST  'J'
#define UART_SYNC_PASSWORD "START"
#define UART_SYNC_RESPONSE "BEGIN"
#define UART_PASSWORD_SIZE  5
#define UART_ACK           'O'
#define UART_GET_RESPONSE  'P'

#define UART_WIN_GAME  "WIN"
#define UART_LOSE_GAME "LOS"

#define LOSE_GAME_ERR 5
#define WIN_GAME_ERR  6

#define UART_SYNC_TRIES     30*60
#define UART_PACKET_TRIES   8*60

/**
 * @brief Network state
 */
typedef enum network_state{
    /// @brief Initial state
    INIT,
    /// @brief Waiting for a connection
    WAITING, // WAITING FOR ACK, NEXT BYTE, OR JOIN REQUEST
    /// @brief Both pc's are linked
    LINKED, // RECEIVED PASSSWORD, WAITING FOR RESPONSE
    /// @brief Both pc's are connected, in game
    CONNECTED,
    /// @brief Disconnected
    DISCONNECTED,
    /// @brief Cleaning up garbage data from uart
    CLEANUP
} network_state_t;

/**
 * @brief Gets the current network state
 * @return Network state
 */
network_state_t (get_network_state)();

/**
 * @brief Gets player id in the network
 * @return Player id
 */
uint8_t get_network_player();

/**
 * @brief Sets a network state
 * @param New network state
 */ 
void (set_network_state)(network_state_t nstate);

/**
 * @brief Uart sync handler
 * According to the state of the network, performs different operations to complete the sync between the 2 pc's
 */
void (uart_sync)();

/**
 * @brief Sends a data packet through the UART.
 * @param ply The player structure pointer
 * @return OK if no errors occurred.
 */
int (uart_send_data)(player_info_t * ply);

/**
 * @brief Turns a received byte array from communication into a readable format.
 * @param data The byte array received from the UART.
 * @param[out] pkt The readable format.
 * @return OK if no errors occurred.
 */
int (decode_data)(uint8_t * data, network_packet_t * pkt);

/**
 * @brief Encodes information into a byte array to be sent by the UART.
 * @param[out] data The byte array.
 * @param pkt The readable format.
 * @return OK if no errors occurred.
 */
int (encode_data)(uint8_t * data, network_packet_t * pkt);

/**
 * @brief Prints a network packet
 * Only useful for debugging
 * 
 * @param pkt Packet to print
 */
void (print_net_packet)(network_packet_t pkt);

/**
 * @brief Encodes movement and fills a network_packet_t
 */
int (encode_packet)(network_packet_t * pkt, player_info_t *ply);

/**
 * @brief Decodes a network_packet_t and fills a controls_t object
 */
int (decode_packet)(network_packet_t * pkt, player_info_t * ply);

/**
 * @brief Receives data from the enemy player
 * @param ply Enemy
 */
int (uart_receive_data)(player_info_t * ply);

/**
 * @brief Sends data to the enemy player
 * @brief ply Player's origin of data
 */
int (uart_send_data)(player_info_t * ply);

/**
 * @brief Sends a string to the uart
 * Useful when a game ends to confirm and reduce sync errors
 * @param word The string
 * @param size Size of the string
 */
int (uart_send_string)(const char * word, size_t size);

/**@}*/

#endif
