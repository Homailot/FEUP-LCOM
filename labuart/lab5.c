// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "queue.h"

// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#include "uart.h"
#include "rs232.h"

#include "networking.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[])
{
    // sets the language of LCF messages (can be either EN-US or PT-PT)
    lcf_set_language("EN-US");

    // enables to log function invocations that are being "wrapped" by LCF
    // [comment this out if you don't want/need it]
    lcf_trace_calls("/home/lcom/labs/labuart/trace.txt");

    // enables to save the output of printf function calls on a file
    // [comment this out if you don't want/need it]
    lcf_log_output("/home/lcom/labs/labuart/output.txt");

    // handles control over to LCF
    // [LCF handles command line arguments and invokes the right function]
    if (lcf_start(argc, argv))
        return 1;

    // LCF clean up tasks
    // [must be the last statement before return]
    lcf_cleanup();

    return 0;
}

extern int timeCounter;

int(video_test_init)(uint16_t mode, uint8_t delay)
{

    uart_set_line_ctrl(BITS_PER_CHAR(8) | ODD_PARITY);
    uart_set_baud_rate(BAUD_115200);

    init_uart_queues();

    uint8_t bit_no;

    int err = timer_subscribe_int(&bit_no);
    int timer_irq = BIT(bit_no);
    if (err != OK)
        return err;

    err = uart_subscribe_int(&bit_no);
    if (err)
        return err;
    int uart_irq = BIT(bit_no);

    uart_init_fifo();
    uart_set_interrupt_reg(ENABLE_TRANSMIT | ENABLE_LINE_STATUS);

    uint8_t i = 0;

    int ipc_status, r;
    message msg;

    while (timeCounter < 5 * 60)
    {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
        {
            printf("Driver failed: %d\n", r);
            return r;
        }

        if (is_ipc_notify(ipc_status))
        {
            switch (_ENDPOINT_P(msg.m_source))
            {
            case HARDWARE:
                if (msg.m_notify.interrupts & timer_irq)
                {
                    timer_int_handler();
                    printf("Sent: %d\n", i);
                    if (transmitter_empty())
                        uart_send_byte(i);
                    else
                        uart_push_transmit(i);
                    uart_push_transmit(i);
                    uart_push_transmit(i);
                    i++;
                }

                if (msg.m_notify.interrupts & uart_irq)
                {
                    uart_ih();
                }

                break;

            default:
                break;
            }
        }
    }

    uart_set_interrupt_reg(0);
    err = uart_unsubscribe_int();
    if (err)
        return err;

    return timer_unsubscribe_int();
}

int(video_test_controller)()
{

    network_packet_t pkt;
    memset(&pkt, 0, sizeof(network_packet_t));
    pkt.ctrl_down = true;
    pkt.a_down = true;
    pkt.mouse_msb = true;
    pkt.mouse_byte = 0x23;
    pkt.gun_down = 2;

    pkt.shoot_down = true;

    print_net_packet(pkt);

    uint8_t data[3];

    encode_data(data, pkt);

    decode_data(data, &pkt);

    print_net_packet(pkt);

    uint8_t ctrl;

    uart_set_line_ctrl(BITS_PER_CHAR(8) | ODD_PARITY);
    uart_set_baud_rate(BAUD_115200);

    init_uart_queues();
    uart_init_fifo();

    uart_get_int_id(&ctrl);
    if ((ctrl & IIR_RECV_DATA_AVAIL))
    {
        uart_ih();
    }
    int err = uart_set_interrupt_reg(ENABLE_LINE_STATUS | ENABLE_RECEIVE);
    if (err)
        return err;

    uint8_t bit_no;

    err = timer_subscribe_int(&bit_no);
    int timer_irq = BIT(bit_no);
    if (err != OK)
        return err;

    err = uart_subscribe_int(&bit_no);
    if (err)
        return err;

    int uart_irq = BIT(bit_no);

    int ipc_status, r;
    message msg;

    while (timeCounter < 10 * 60)
    {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
        {
            printf("Driver failed: %d\n", r);
            return r;
        }

        if (is_ipc_notify(ipc_status))
        {
            switch (_ENDPOINT_P(msg.m_source))
            {
            case HARDWARE:
                if (msg.m_notify.interrupts & timer_irq)
                {
                    timer_int_handler();
                    uint8_t byte;

                    while (uart_recv_front(&byte))
                    {
                        printf("byte yes: %d\n", byte);
                    }
                }

                if (msg.m_notify.interrupts & uart_irq)
                {
                    uart_ih();
                }

                break;

            default:
                break;
            }
        }
    }

    uart_set_interrupt_reg(0);
    err = uart_unsubscribe_int();
    if (err)
        return err;

    return timer_unsubscribe_int();
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y)
{
    uint8_t ctrl;

    uart_set_line_ctrl(BITS_PER_CHAR(8) | ODD_PARITY);
    uart_set_baud_rate(BAUD_115200);

    init_uart_queues();
    uart_init_fifo();

    uart_get_int_id(&ctrl);
    int err = uart_set_interrupt_reg(ENABLE_LINE_STATUS | ENABLE_RECEIVE | ENABLE_TRANSMIT);
    if (err)
        return err;

    uint8_t bit_no;

    err = timer_subscribe_int(&bit_no);
    int timer_irq = BIT(bit_no);
    if (err != OK)
        return err;

    err = uart_subscribe_int(&bit_no);
    if (err)
        return err;

    int uart_irq = BIT(bit_no);

    int ipc_status, r;
    message msg;

    int game_state = 0;
    network_state_t state;

    while (game_state == 0)
    {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
        {
            printf("Driver failed: %d\n", r);
            return r;
        }

        if (is_ipc_notify(ipc_status))
        {
            switch (_ENDPOINT_P(msg.m_source))
            {
            case HARDWARE:
                if (msg.m_notify.interrupts & timer_irq)
                {
                    timer_int_handler();

                    if(game_state == 0) {
                        uart_sync();
                        state = get_network_state();
                        if(state == CONNECTED) {
                            timeCounter = 0;
                            printf("Start game!\n");
                            game_state = 1;
                        }
                    }
                }

                if (msg.m_notify.interrupts & uart_irq)
                {
                    uart_ih();
                }

                break;

            default:
                break;
            }
        }
    }

    uart_set_interrupt_reg(0);
    err = uart_unsubscribe_int();
    if (err)
        return err;

    return timer_unsubscribe_int();
}
