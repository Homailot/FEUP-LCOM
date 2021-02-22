#include "kbc.h"
#include "../utils/utils.h"
#include <stdlib.h>
#include <stdint.h>

static int kb_subscribe_result;
static int mouse_subscribe_result;

bool valid_kb_byte = false;
bool valid_mouse_byte = false;

uint8_t last_kb_byte;
uint8_t last_mouse_byte;

int(kb_subscribe_int)(uint8_t* hook_id) {
    *hook_id = KB_HOOK_ID;
    kb_subscribe_result = (int)*hook_id;

    return sys_irqsetpolicy(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kb_subscribe_result);
}

int(mouse_subscribe_int)(uint8_t* hook_id) {
    *hook_id = MOUSE_HOOK_ID;
    mouse_subscribe_result = (int)*hook_id;

    return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_subscribe_result);
}

int(kb_unsubscribe_int)() {
    return sys_irqrmpolicy(&kb_subscribe_result);
}

int(mouse_unsubscribe_int)() {
    return sys_irqrmpolicy(&mouse_subscribe_result);
}

int(kbc_disable_int)() {
    int err = OK;

    err = sys_irqdisable(&mouse_subscribe_result);
    err = sys_irqdisable(&kb_subscribe_result);

    return err;
}

int(kbc_enable_int)() {
    int err = OK;

    err = sys_irqenable(&mouse_subscribe_result);
    err = sys_irqenable(&kb_subscribe_result);

    return err;
}

int(get_status_kbc)(uint8_t * st) {
    return util_sys_inb(KBC_IN_BUF, st);
}

int(write_byte_kbc)(uint8_t byte) {
    uint8_t tries = 0, status;
    int err;

    while(tries < KBC_MAX_TRIES) {
        tries++;
        tickdelay(micros_to_ticks(20));

        err = get_status_kbc(&status);
        if(err) continue;

        if((status & (KBC_PARITY_ERR | KBC_TIMEOUT_ERR | KBC_IN_BUFFER_FULL)) == OK) {
            err = sys_outb(KBC_IN_BUF, byte);
            if(err) continue;

            return OK;
        }  
    }

    return 1;
}

int(write_arg_kbc)(uint8_t byte) {
    uint8_t tries = 0, status;
    int err;

    while(tries < KBC_MAX_TRIES) {
        tries++;
        tickdelay(micros_to_ticks(20));

        err = get_status_kbc(&status);
        if(err) continue;

        if((status & (KBC_PARITY_ERR | KBC_TIMEOUT_ERR | KBC_OUT_BUFFER_FULL)) == OK) {
            err = sys_outb(KBC_OUT_BUF, byte);
            if(err) continue;

            return OK;
        }  
    }

    return 1;
}

int (read_byte_kbc)(uint8_t * byte) {
    uint8_t tries = 0, status;
    int err;

    while(tries < KBC_MAX_TRIES) {
        tickdelay(micros_to_ticks(20));
        tries++;

        err = get_status_kbc(&status);
        if(err) continue;

        if((status & (KBC_PARITY_ERR | KBC_TIMEOUT_ERR | KBC_OUT_BUFFER_FULL)) == KBC_OUT_BUFFER_FULL) {
            err = util_sys_inb(KBC_OUT_BUF, byte);
            if(err) continue;

            return OK;
        }
    }

    return 1;
}

int (write_ms_cmd)(size_t arg_size, uint8_t * args) {
    uint8_t tries = 0, ack;
    int err;
    size_t i;

    while(tries < KBC_MAX_TRIES) {
        tries++;

        for(i = 0; i < arg_size; i++) {
            err = write_byte_kbc(MOUSE_WRITE_CMD);
            if(err) break;
            err = write_arg_kbc(args[i]);
            if(err) break;

            err = read_byte_kbc(&ack);
            if(err) break;

            if(ack != ACK) break;
        }

        if(i < arg_size) continue;
        return OK;
    }

    return 1;
}

int(read_ms_cmd)(size_t arg_size, uint8_t * args) {
    uint8_t tries = 0;
    size_t i = 0;
    int err;

    while(tries < KBC_MAX_TRIES) {
        err = read_byte_kbc(&args[i]);
        if(err) {
            tries++;
            continue;
        } 

        i++;
        if(i >= arg_size) return OK;
    }

    return 1;
}

int (write_kbc_cmd_byte)(uint8_t byte) {
    int err = write_byte_kbc(KBC_WRITE_CMD);
    if(err) return err;

    return write_arg_kbc(byte);
}

int (enable_ms)() {
    return write_byte_kbc(MOUSE_ENABLE);
}

int (disable_ms)() {
    return write_byte_kbc(MOUSE_DISABLE);
}

int(enable_data_reporting)() {
    uint8_t args[1] = {ENABLE_DATA_REPORT};
    return write_ms_cmd(1, args);
}

int(disable_data_reporting)() {
    uint8_t args[1] = {DISABLE_DATA_REPORT};
    return write_ms_cmd(1, args);
}

int(enable_stream)() {
    uint8_t args[1] = {SET_STREAM_MODE};
    int err = disable_data_reporting();
    if(err) return err;
    err = write_ms_cmd(1, args);
    if(err) return err;

    return enable_data_reporting();
}

int(enable_remote)() {
    uint8_t args[1] = {SET_REMOTE_MODE};
    int err = disable_data_reporting();
    if(err) return err;

    return write_ms_cmd(1, args);
}

int(ms_set_default)(){
    uint8_t args[1] = {SET_DEFAULTS};
    int err = disable_data_reporting();
    if(err) return err;

    return write_ms_cmd(1, args);
}

void(kbc_ih)(void) {
    // Checks for the existence of errors or if the data is from the mouse
    uint8_t mask = KBC_PARITY_ERR | KBC_TIMEOUT_ERR | KBC_AUX, sb;
    last_kb_byte = 0;
    valid_kb_byte = false;

    int err = get_status_kbc(&sb);
    // The output buffer is full, can be read
    if (sb & KBC_OUT_BUFFER_FULL) {
        uint8_t byte;
        err = util_sys_inb(KBC_OUT_BUF, &byte);

        // Only if there are no errors in parity and timeout, the value is stored
        if (err == OK && (sb & mask) == OK) {
            last_kb_byte = byte;
            valid_kb_byte = true;
        }
    }
}

void(mouse_ih)() {
    // Checks for the existence of errors or if the data is from the mouse
    uint8_t mask = KBC_PARITY_ERR | KBC_TIMEOUT_ERR | KBC_AUX, sb;
    valid_mouse_byte = false;

    int err = get_status_kbc(&sb);
    // The output buffer is full, can be read
    if (sb & KBC_OUT_BUFFER_FULL) {
        err = util_sys_inb(KBC_OUT_BUF, &last_mouse_byte);

        // Only if there are no errors in parity and timeout, the value is stored
        if (err == OK && (sb & mask) == KBC_AUX) {
            valid_mouse_byte = true;
        }
    }
}


int(reset_cmd_byte)() {
    uint8_t normal_status = minix_get_dflt_kbc_cmd_byte();

    return write_kbc_cmd_byte(normal_status);
}

void(int9_to_int16)(uint8_t lsb, bool negative, int16_t* result) {
    *result = lsb;

    if (negative) {
        int16_t mask = 0xff00;

        *result |= mask;
    }
}

void(parse_packet)(uint8_t bytes[3], struct packet* pkt) {
    uint8_t first_byte = bytes[0];

    pkt->bytes[0] = first_byte;
    pkt->bytes[1] = bytes[1];
    pkt->bytes[2] = bytes[2];

    pkt->lb = first_byte & MOUSE_LB;
    pkt->rb = first_byte & MOUSE_RB;
    pkt->mb = first_byte & MOUSE_MB;
    pkt->x_ov = first_byte & MOUSE_X_OVFL;
    pkt->y_ov = first_byte & MOUSE_Y_OVFL;

    int16_t x_delta;
    int16_t y_delta;

    int9_to_int16(bytes[1], first_byte & MOUSE_MSB_X_DELTA, &x_delta);
    int9_to_int16(bytes[2], first_byte & MOUSE_MSB_Y_DELTA, &y_delta);

    pkt->delta_x = x_delta;
    pkt->delta_y = y_delta;
}
