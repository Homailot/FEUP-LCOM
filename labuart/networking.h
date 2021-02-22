#ifndef _G01_NETWORKING_H_
#define _G01_NETWORKING_H_

#include <lcom/lab5.h>
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


typedef struct network_packet{
    uint8_t mouse_byte;
    uint8_t mouse_msb;
    uint8_t health;
    bool w_down;
    bool a_down;
    bool s_down;
    bool d_down;
    bool ctrl_down;
    bool shift_down;
    bool shoot_down;
    /** @brief Gun id - Must be between 1 and 3 **/
    uint8_t gun_down;
} network_packet_t;


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

#define UART_JOIN_REQUEST  'J'
#define UART_SYNC_PASSWORD "START"
#define UART_SYNC_RESPONSE "BEGIN"
#define UART_PASSWORD_SIZE  5
#define UART_ACK           'O'
#define UART_GET_RESPONSE  'P'

#define UART_SYNC_TRIES     60

typedef enum network_state{
    WAITING, // WAITING FOR ACK, NEXT BYTE, OR JOIN REQUEST
    LINKED, // RECEIVED PASSSWORD, WAITING FOR RESPONSE
    CONNECTED,
    DISCONNECTED,
    CLEANUP
} network_state_t;

network_state_t (get_network_state)();

void (uart_sync)();

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
int (encode_data)(uint8_t * data, network_packet_t pkt);

/**
 * @brief Encodes movement and fills a network_packet_t
 */
int (encode_packet)();

void (print_net_packet)(network_packet_t pkt);

/**
 * @brief Decodes a network_packet_t and fills a controls_t object
 */
int (decode_packet)();

#endif
