#include "timer.h"
#include "i8254.h"
#include "../utils/utils.h"
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdlib.h>

static int* subscribe_result = NULL;

tick_t time_counter = 0;

int(timer_set_frequency)(uint8_t timer, uint32_t freq) {

    if (timer < 0 || timer > 2)
        return 1;

    uint8_t control_word = 0;
    uint8_t status_byte;

    // Get the timer's port.
    int port = TIMER_ADDR_SEL(timer);

    // Calculate the initial value written to the wanted clock.
    uint16_t init_value = TIMER_FREQ / freq;
    //uint8_t lsb_init_value = INIT_LSB & init_value;
    //uint8_t msb_init_value = (INIT_MSB & init_value) >> 8;

    uint8_t lsb_init_value = 0, msb_init_value = 0;

    util_get_LSB(init_value, &lsb_init_value);
    util_get_MSB(init_value, &msb_init_value);

    // Get the timer's initial configuration
    timer_get_conf(timer, &status_byte);

    // Get the first 4 bits of the status byte
    status_byte &= STATUS_CONFIG;

    // Creating the control word, selecting the timer to configure and initialization mode (both lsb and msb because we need to overwrite the existing value)
    control_word |= status_byte;
    control_word |= TIMER_LSB_MSB;
    control_word |= TIMER_CMD_SEL(timer);

    // Write the control word to the control port.
    if (sys_outb(TIMER_CTRL, (uint32_t)control_word) != OK)
        return EINVAL;

    // Write the initial value to the timer of choice.
    if (sys_outb(port, (uint32_t)lsb_init_value) != OK)
        return EINVAL;
    if (sys_outb(port, (uint32_t)msb_init_value) != OK)
        return EINVAL;

    return OK;
}

int(timer_subscribe_int)(uint8_t* bit_no) {
    *bit_no = TIMER_SUBSCRIPTION_BITNO;

    subscribe_result = (int*)malloc(sizeof(int));
    *subscribe_result = (int)*bit_no;
    int err = sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, subscribe_result);

    return err;
}

int(timer_unsubscribe_int)() {
    int err = sys_irqrmpolicy(subscribe_result);

    free(subscribe_result);
    subscribe_result = NULL;

    return err;
}

void(timer_int_handler)() {
    time_counter++;
}

int(timer_get_conf)(uint8_t timer, uint8_t* st) {
    if (timer < 0 || timer > 2)
        return 1;

    uint8_t read_back_command = TIMER_RB_CMD;
    int timer_port = TIMER_ADDR_SEL(timer);
    int err = OK;
    read_back_command |= TIMER_RB_SEL(timer) | TIMER_RB_COUNT_;

    err = sys_outb(TIMER_CTRL, read_back_command);
    if (err != OK)
        return err;

    err = util_sys_inb(timer_port, st);

    return OK;
}

int(timer_display_conf)(uint8_t timer, uint8_t st,
    enum timer_status_field field) {

    union timer_status_field_val value;
    value.byte = st;

    switch (field) {
    case tsf_initial:
        value.in_mode = (enum timer_init)((st & TIMER_LSB_MSB) >> TIMER_INIT_BIT);
        break;

    case tsf_mode:
        value.count_mode = ((st & MODE_BITS) >> TIMER_MODE_BIT);
        break;

    case tsf_base:
        value.bcd = (st & BCD_BIT);

    default:
        break;
    }

    return timer_print_config(timer, field, value);
}
