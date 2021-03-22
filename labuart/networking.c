#include "networking.h"
#include "uart.h"
#include "rs232.h"

#include <math.h>


static uint8_t idx = 0;
static network_state_t state = CLEANUP;
extern int timeCounter;

network_state_t (get_network_state)() {
    return state;
}

void (send_join_request)() {
    if (transmitter_empty()) uart_send_byte(UART_JOIN_REQUEST);
    else uart_push_transmit(UART_JOIN_REQUEST);
}

bool (uart_sync_link)() {
    uint8_t byte;
    bool empty = !uart_recv_front(&byte);

    if(empty) return false;

    if(byte == UART_ACK) {
        if(idx == UART_PASSWORD_SIZE) {
            state = LINKED;
            idx = 0;
            if (transmitter_empty()) uart_send_byte(UART_GET_RESPONSE);
            else uart_push_transmit(UART_GET_RESPONSE);
            return true;
        } 

        if (transmitter_empty()) uart_send_byte(UART_SYNC_PASSWORD[idx]);
        else uart_push_transmit(UART_SYNC_PASSWORD[idx]);

        idx++;
    } else if(byte == UART_JOIN_REQUEST) {
        empty_transmit_queue();

        if (transmitter_empty()) uart_send_byte(UART_ACK);
        else uart_push_transmit(UART_ACK);

        idx = 0;
    } else if(byte == UART_SYNC_PASSWORD[idx]) {
        if (transmitter_empty()) uart_send_byte(UART_ACK);
        else uart_push_transmit(UART_ACK);

        idx++;

        if(idx == UART_PASSWORD_SIZE) {
            state = LINKED;
            idx = 0;
        } 
    }

    return true;
}

bool (uart_sync_connect)() {
    uint8_t byte;
    bool empty = !uart_recv_front(&byte);

    if(empty) return false;

    if(byte == UART_ACK) {
        if(idx == UART_PASSWORD_SIZE) {
            state = CONNECTED;
            printf("1\n");
            return true;
        } 

        if (transmitter_empty()) uart_send_byte(UART_SYNC_RESPONSE[idx]);
        else uart_push_transmit(UART_SYNC_RESPONSE[idx]);

        idx++;
    } else if(byte == UART_GET_RESPONSE) {
        empty_transmit_queue();

        if (transmitter_empty()) uart_send_byte(UART_SYNC_RESPONSE[0]);
        else uart_push_transmit(UART_SYNC_RESPONSE[0]);

        idx = 1;
    } else if(byte == UART_SYNC_RESPONSE[idx]) {
        if (transmitter_empty()) uart_send_byte(UART_ACK);
        else uart_push_transmit(UART_ACK);

        idx++;

        if(idx == UART_PASSWORD_SIZE) {
            printf("2\n");
            state = CONNECTED;
        } 
    }

    return true;
}



void (reset_sync)() {
    state = CLEANUP;
    idx = 0;
}

void (uart_sync)() {
    uint8_t byte;
    static uint8_t tries = 0;
    bool clean = true;

    if (state == CLEANUP)
    {
        //printf(".....Cleaning up...\n");
        while(tries < UART_MAX_TRIES) {
            clean = true;
            while (uart_recv_front(&byte)) clean = false;
            while(uart_receive_byte(&byte) != 1) clean = false;

            if(clean) tries++;
            else tries = 0;
        }
        

        printf(".....Waiting for connection...\n");
        state = WAITING;
        send_join_request();
        tries = 0;
    }

    if (state == WAITING)
    {
        if(!uart_sync_link()) tries++;
        else tries = 0;
        //printf("tries: %d\n", tries);

        if(tries > UART_SYNC_TRIES) {
            reset_sync();
            tries = 0;
        } 
    }

    if(state == LINKED) {
        if(!uart_sync_connect()) tries++;
        else tries = 0;

        if(tries > UART_SYNC_TRIES) {
            reset_sync();
            tries = 0;
        } 
    }
}

int(encode_packet)()
{
    // USAR PLAYER CTRL COMO ARGUMENTO COMO ARGUMENTO
    return OK;
}

int(decode_packet)()
{
    // USAR PLAYER 2 CTRL COMO ARGUMENTO
    return OK;
}

int(encode_data)(uint8_t *data, network_packet_t pkt)
{
    data[0] = 0;
    data[0] |= pkt.shoot_down ? DATA_SHOOTDOWN : 0;
    data[0] |= pkt.ctrl_down ? DATA_CTRLDOWN : 0;
    data[0] |= pkt.shift_down ? DATA_SHIFTDOWN : 0;
    data[0] |= pkt.d_down ? DATA_DDOWN : 0;
    data[0] |= pkt.s_down ? DATA_SDOWN : 0;
    data[0] |= pkt.a_down ? DATA_ADOWN : 0;
    data[0] |= pkt.w_down ? DATA_WDOWN : 0;
    data[0] |= pkt.mouse_msb ? DATA_MOUSE_MSB : 0;

    data[1] = 0;
    data[1] = pkt.mouse_byte;

    data[2] = DATA_CONFIRM_PACKET;
    data[2] |= DATA_GUN_DOWN(pkt.gun_down);

    return OK;
}

void (print_net_packet)(network_packet_t pkt){
    printf("Packet: %d %d %d %d %d %d %d %d %d\nMouse byte %d\n",
        pkt.w_down, pkt.a_down, pkt.s_down, pkt.d_down,
        pkt.ctrl_down, pkt.shift_down, pkt.shoot_down,
        pkt.mouse_msb, pkt.gun_down, pkt.mouse_byte);
}

int(decode_data)(uint8_t *data, network_packet_t *pkt)
{
    memset(pkt, 0, sizeof(network_packet_t));

    pkt->shoot_down = data[0] & DATA_SHOOTDOWN;
    pkt->ctrl_down = data[0] & DATA_CTRLDOWN;
    pkt->shift_down = data[0] & DATA_SHIFTDOWN;
    pkt->d_down = data[0] & DATA_DDOWN;
    pkt->s_down = data[0] & DATA_SDOWN;
    pkt->a_down = data[0] & DATA_ADOWN;
    pkt->w_down = data[0] & DATA_WDOWN;
    pkt->mouse_msb = ((data[0] & DATA_MOUSE_MSB) == DATA_MOUSE_MSB);
    pkt->mouse_byte = data[1];
    uint8_t g = data[2] & DATA_GUN_BITS;
    if(     g & DATA_GUN_DOWN_1) pkt->gun_down = 1;
    else if(g & DATA_GUN_DOWN_2) pkt->gun_down = 2;
    else if(g & DATA_GUN_DOWN_3) pkt->gun_down = 3;

    return OK;
}
