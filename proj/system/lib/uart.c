// IMPORTANT: you must include the following line in all your C files
#include "uart.h"
#include "rs232.h"
#include "queue.h"
#include <lcom/lcf.h>
#include <lcom/lab5.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

static int uart_subscribe_result;

static queue_t *in_fifo;
static queue_t *out_fifo;

int(init_uart_queues)()
{
    in_fifo = queue_init();
    out_fifo = queue_init();
    return OK;
}

int(free_uart_queues)()
{
    queue_delete(in_fifo);
    queue_delete(out_fifo);
    return OK;
}

int(uart_subscribe_int)(uint8_t *bit_no)
{
    uart_subscribe_result = COM1_UART_IRQ;
    *bit_no = COM1_UART_IRQ;
    int err = sys_irqsetpolicy(COM1_UART_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &uart_subscribe_result);

    return err;
}

int(uart_disable_int)()
{
    return sys_irqdisable(&uart_subscribe_result);
}

int(uart_enable_int)()
{
    return sys_irqenable(&uart_subscribe_result);
}

int(uart_init_fifo)()
{
    return sys_outb(COM1_UART_ADDRESS + UART_FCR, FIFO_ENABLE | FIFO_CLR_SEND | FIFO_CLR_RCVR | FIFO_ONE_BYTE);
}

int(uart_unsubscribe_int)()
{
    return sys_irqrmpolicy(&uart_subscribe_result);
}

int(uart_get_line_status)(uint8_t *ctrl)
{
    return util_sys_inb(COM1_UART_ADDRESS + UART_LSR, ctrl);
}

int(uart_get_line_ctrl)(uint8_t *ctrl)
{
    return util_sys_inb(COM1_UART_ADDRESS + UART_LCR, ctrl);
}

int(uart_get_int_id)(uint8_t *byte)
{
    return util_sys_inb(COM1_UART_ADDRESS + UART_IIR, byte);
}

int(uart_set_line_ctrl)(uint8_t ctrl)
{
    return sys_outb(COM1_UART_ADDRESS + UART_LCR, ctrl);
}

int(uart_set_interrupt_reg)(uint8_t ctrl)
{
    return sys_outb(COM1_UART_ADDRESS + UART_IER, ctrl);
}

int(uart_set_baud_rate)(uint16_t rate)
{
    uint8_t ctrl;

    int err = uart_get_line_ctrl(&ctrl);
    if (err)
        return err;

    err = uart_set_line_ctrl(ctrl | DLAB_ON);
    if (err)
        return err;

    err = sys_outb(COM1_UART_ADDRESS + UART_DLL, GET_LSB(rate));
    if (err)
        return err;

    err = sys_outb(COM1_UART_ADDRESS + UART_DLH, GET_MSB(rate));
    if (err)
        return err;

    return uart_set_line_ctrl((ctrl)&DLAB_OFF);
}

int(uart_send_byte)(uint8_t byte)
{
    uint8_t status, tries = 0;
    
    while (tries < UART_MAX_TRIES)
    {
        uart_get_line_status(&status);
        if ((status & TRANS_HOLD_EMPTY))
        {
            return sys_outb(COM1_UART_ADDRESS + UART_THR, byte);
        }
        micro_delay(UART_DELAY);
        tries++;
    }
    return 1;
}

int(uart_receive_byte)(uint8_t *byte)
{
    uint8_t status, tries = 0;
    int err;
    while (tries < UART_MAX_TRIES)
    {
        err = uart_get_line_status(&status);
        if (err) {
            tries++; micro_delay(UART_DELAY); continue;
        }

        err = 1;
    
        if (status & RECEIVE_DATA)
        {
            err = util_sys_inb(COM1_UART_ADDRESS + UART_RBR, byte);
        }

        if (status & (OVERRUN_ERR | PARITY_ERR | FRAMING_ERR | FIFO_ERR))
        {
            return 2;
        }
        
        if(err == OK) return OK;
        tries++;
        micro_delay(UART_DELAY);
    }

    return 1;
}

bool(uart_recv_front)(int16_t *byte)
{
    bool empty = queue_empty(in_fifo);
    if (empty)
        return false;
    *byte = queue_front(in_fifo);
    queue_pop(in_fifo);
    return !empty;
}

static bool can_send = true;

void(uart_push_transmit)(int16_t byte)
{
    queue_push(out_fifo, byte);
    can_send = false;
}

bool(transmitter_empty)()
{
    return can_send;
}

void uart_ih()
{
    uint8_t byte, int_id;
    int err;

    while(true) {
        if (uart_get_int_id(&int_id) != OK)
            return;
        if ((int_id & IIR_INT)) break;

        switch (int_id & INT_MASK)
        {
        case IIR_STATUS:
            uart_get_line_status(&byte);
            break;
        case IIR_RECV_DATA_AVAIL:
            while ((err = uart_receive_byte(&byte)) != 1)
            {
                if(err == 2) queue_push(in_fifo, UART_ERROR_CODE);
                queue_push(in_fifo, byte);
            }
            break;
        
        case IIR_TRNSMT_EMPTY:
            if (queue_empty(out_fifo))
            {
                can_send = true;
                break;
            }

            while(!queue_empty(out_fifo)){
                err = uart_send_byte(queue_front(out_fifo));
                if(err) break;
                queue_pop(out_fifo);
            }
            can_send = false;

            break;

        default:
            printf("uncommon id: %x\n", int_id);
            break;
        }
    }
}

void(empty_transmit_queue)() {
    queue_clean(out_fifo);
}

void (empty_recv_queue)(){
    queue_clean(in_fifo);
}
