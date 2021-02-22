#include "networking.h"
#include "../lib/uart.h"
#include "../lib/rs232.h"
#include "../controls/controls.h"
#include <math.h>


static uint8_t idx = 0;
static uint8_t packet_idx = 0;
static uint32_t packet_tries = 0;
static network_state_t state = CLEANUP;
static uint8_t player;

uint8_t get_network_player(){
    return player;
}

network_state_t (get_network_state)() {
    return state;
}

void (set_network_state)(network_state_t nstate) {
    state = nstate;
}

void (send_join_request)() {
    uart_safe_transmit(UART_JOIN_REQUEST);
}

int (uart_sync_link)() {
    int16_t byte;
    bool empty = !uart_recv_front(&byte);

    if(empty) return 2;
    else if(byte == UART_ERROR_CODE) return 1;

    if(byte == UART_ACK) {
        if(idx == UART_PASSWORD_SIZE) {
            state = LINKED;
            idx = 0;
            uart_safe_transmit(UART_GET_RESPONSE);

            return OK;
        } 

        uart_safe_transmit(UART_SYNC_PASSWORD[idx]);

        idx++;
    } else if(byte == UART_JOIN_REQUEST) {
        empty_transmit_queue();

        uart_safe_transmit(UART_ACK);
        idx = 0;
    } else if(byte == UART_SYNC_PASSWORD[idx]) {
        uart_safe_transmit(UART_ACK);
        idx++;

        if(idx == UART_PASSWORD_SIZE) {
            state = LINKED;
            idx = 0;
        } 
    } else {
        return 2;
    }

    return OK;
}

int (uart_sync_connect)() {
    int16_t byte;
    bool empty = !uart_recv_front(&byte);

    if(empty) return 2;
    else if(byte == UART_ERROR_CODE) return 1;

    if(byte == UART_ACK) {
        if(idx == UART_PASSWORD_SIZE) {
            state = CONNECTED;
            player = 1;
            empty_transmit_queue();    
            return OK;
        } 

        uart_safe_transmit(UART_SYNC_RESPONSE[idx]);
        idx++;
    } else if(byte == UART_GET_RESPONSE) {
        empty_transmit_queue();

        uart_safe_transmit(UART_SYNC_RESPONSE[0]);
        idx = 1;
    } else if(byte == UART_SYNC_RESPONSE[idx]) {
        uart_safe_transmit(UART_ACK);
        idx++;

        if(idx == UART_PASSWORD_SIZE) {
            player = 2;
            state = CONNECTED;
            empty_transmit_queue();
        } 
    } else {
        return 2;
    }


    return OK;
}

void (reset_sync)() {
    state = CLEANUP;
    idx = 0;
}

void (uart_sync)() {
    uint8_t cleanup_byte;
    static uint32_t tries = 0;
    int err;
    
    bool clean = true;

    if (state == CLEANUP)
    {
        uart_init_fifo();
        idx = 0;
        packet_idx = 0;
        packet_tries = 0;

        empty_transmit_queue();
        empty_recv_queue();
        while(tries < UART_MAX_TRIES) {
            clean = true;
            while (uart_receive_byte(&cleanup_byte) != 1) clean = false;

            if(clean) tries++;
            else tries = 0;
        }
        

        state = WAITING;
        send_join_request();
        tries = 0;
    }

    if (state == WAITING)
    {
        err = uart_sync_link();
        if(err == 1) {
            reset_sync();
        } else if(err == 2) {
            tries++;
            if(tries > UART_SYNC_TRIES) {
                idx = 0; state = INIT; tries = 0;
            }
        }
    }

    if(state == LINKED) {
        err = uart_sync_connect();
        if(err == 1) {
            reset_sync();
        } else if(err == 2){
            tries++;
            if(tries > UART_SYNC_TRIES) {
                idx = 0; state = INIT; tries = 0;
            }
        }
    }
}

int(encode_packet)(network_packet_t * pkt, player_info_t * ply)
{

    memset(pkt,0,sizeof(network_packet_t));

    pkt->w_down = ply->movm.kbd_state.x_pos;
    pkt->a_down = ply->movm.kbd_state.y_neg;
    pkt->s_down = ply->movm.kbd_state.x_neg;
    pkt->d_down = ply->movm.kbd_state.y_pos;
    pkt->shift_down = ply->movm.kbd_state.sprint;
    pkt->ctrl_down = ply->movm.kbd_state.crouch;
    pkt->gun_down = ply->gun_index + 1;
    pkt->shoot_down = ply->movm.mouse_pkt.lb;

     double x_delta = ply->movm.mouse_desl;

    if(x_delta > 1.9)
        x_delta = 1.9;
    else if(x_delta < -1.9)
        x_delta = -1.9;
    double desl = x_delta*128;

    pkt->mouse_byte = (uint8_t)fabs(desl);

    
    pkt->mouse_msb = desl >= 0 ? 0 : 1;

    pkt->shoot_down = ply->movm.mouse_pkt.lb;
    return OK;
}

int(decode_packet)(network_packet_t * pkt, player_info_t * ply)
{
    ply->movm.kbd_state.x_pos = pkt->w_down;
    ply->movm.kbd_state.x_neg = pkt->s_down;
    ply->movm.kbd_state.y_pos = pkt->d_down;
    ply->movm.kbd_state.y_neg = pkt->a_down;
    ply->movm.kbd_state.sprint = pkt->shift_down;
    ply->movm.kbd_state.crouch = pkt->ctrl_down;
    memset(ply->movm.kbd_state.nums_press, 0, sizeof(bool)*3);
    ply->movm.kbd_state.nums_press[pkt->gun_down] = true;
    double desl = (double)pkt->mouse_byte / 128;
    if(pkt->mouse_msb) desl = -desl;

    ply->movm.mouse_desl = desl;

    ply->movm.mouse_pkt.lb = pkt->shoot_down;
    return OK;
}

int(encode_data)(uint8_t *data, network_packet_t * pkt)
{
    data[2] = 0;
    data[2] |= pkt->shoot_down ? DATA_SHOOTDOWN : 0;
    data[2] |= pkt->ctrl_down ? DATA_CTRLDOWN : 0;
    data[2] |= pkt->shift_down ? DATA_SHIFTDOWN : 0;
    data[2] |= pkt->d_down ? DATA_DDOWN : 0;
    data[2] |= pkt->s_down ? DATA_SDOWN : 0;
    data[2] |= pkt->a_down ? DATA_ADOWN : 0;
    data[2] |= pkt->w_down ? DATA_WDOWN : 0;
    data[2] |= pkt->mouse_msb ? DATA_MOUSE_MSB : 0;

    data[1] = 0;
    data[1] = pkt->mouse_byte;

    data[0] = 0;
    data[0] = DATA_CONFIRM_PACKET;
    data[0] |= DATA_GUN_DOWN(pkt->gun_down);

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

    pkt->shoot_down = data[2] & DATA_SHOOTDOWN;
    pkt->ctrl_down = data[2] & DATA_CTRLDOWN;
    pkt->shift_down = data[2] & DATA_SHIFTDOWN;
    pkt->d_down = data[2] & DATA_DDOWN;
    pkt->s_down = data[2] & DATA_SDOWN;
    pkt->a_down = data[2] & DATA_ADOWN;
    pkt->w_down = data[2] & DATA_WDOWN;
    pkt->mouse_msb = ((data[2] & DATA_MOUSE_MSB) == DATA_MOUSE_MSB);
    pkt->mouse_byte = data[1];
    uint8_t g = data[0] & DATA_GUN_BITS;
    if(     g & DATA_GUN_DOWN_1) pkt->gun_down = 1;
    else if(g & DATA_GUN_DOWN_2) pkt->gun_down = 2;
    else if(g & DATA_GUN_DOWN_3) pkt->gun_down = 3;

    return OK;
}

int (uart_send_string)(const char * word, size_t size){
    uart_safe_transmit(word[0]);
    for(size_t i = 1; i < size; i++) {
        uart_push_transmit(word[i]);
    }
    return OK;
}

int (uart_send_data)(player_info_t * ply)
{
    uint8_t bytes[DATA_BYTES];
    network_packet_t pkt;
    encode_packet(&pkt, ply);
    encode_data(bytes, &pkt);

    uart_safe_transmit(bytes[0]);
    for(size_t i = 1; i < DATA_BYTES; i++) {
        uart_push_transmit(bytes[i]);
    }
    
    return OK;
}

bool uart_bytes_cmp(void * b1, void * b2, size_t size){
    char * c1 = (char *) b1;
    char * c2 = (char *) b2;
    for (size_t i = 0; i < size; i++)
        if(c1[i] != c2[i]) return false;
    return true;
}

int (uart_receive_data)(player_info_t * ply){
    int16_t byte;
    static uint8_t bytes[DATA_BYTES];

    packet_tries++;
    while (uart_recv_front(&byte))
    {
        if(packet_idx == 0 && (byte & DATA_CONFIRM_MASK) != DATA_CONFIRM_PACKET && 
            byte != UART_WIN_GAME[0] && 
            byte != UART_LOSE_GAME[0])
            {
                continue;
            }

        bytes[packet_idx] = byte;
        packet_idx++;
        if(packet_idx == DATA_BYTES){
            packet_idx = 0;
            packet_tries = 0;

            if(uart_bytes_cmp(bytes,UART_WIN_GAME,3))
                return WIN_GAME_ERR;
            else if(uart_bytes_cmp(bytes,UART_LOSE_GAME,3))
                return LOSE_GAME_ERR;
            network_packet_t pkt;
            decode_data(bytes, &pkt);
            decode_packet(&pkt, ply);
            player_int_ih(ply);
        }
    }

    if(packet_tries > UART_PACKET_TRIES) {
        set_network_state(DISCONNECTED);
    }
    return OK;
}
