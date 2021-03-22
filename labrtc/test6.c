// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "rtc.h"
#include "rtc_controller.h"

extern bool alarm_wait;

int rtc_test_int() { 
    rtc_ih();

    init_rtc_vars();

    enable_uie_int();
    enable_aie_int();

    rtc_set_alarm(0, RTC_DONT_CARE, RTC_DONT_CARE);

    uint8_t rtc_bit_no;

    int err = rtc_subscribe_int(&rtc_bit_no);
    if(err) return err;
    int rtc_irq_set = BIT(rtc_bit_no);


    int ipc_status, r;
    message msg;

    int sum = 0;

    while(alarm_wait) {
        if((r = driver_receive(ANY, &msg, &ipc_status))) {
            break;
        }

        if(is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source))
            {
            case HARDWARE:
                if(msg.m_notify.interrupts & rtc_irq_set) {
                  rtc_ih();
                  sum ++;
                  if(sum == 10)
                    disable_uie_int();
                }
            
            default:
                break;
            }
        }
    }



    err = rtc_unsubscribe_int();
    if(err) return err;

    reset_rtc();

    return OK;
}
