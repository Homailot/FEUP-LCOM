// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include "i8042.h"
#include "kbc.h"
#include "mouse_state.h"

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

extern bool validByte;
extern uint8_t lastByte;
extern int timeCounter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/g01/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g01/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(mouse_test_packet)(uint32_t cnt) {
  uint32_t cur_cnt = 0;
  uint8_t bit_no;
  uint8_t counter = 0;
  uint8_t packet_storage[3];

  int err = mouse_subscribe_int(&bit_no);
  int irq_set = BIT(bit_no);
  if (err != OK)
    return err;

  if( (err = ms_enable_data_rep())) return err;
  if( (err = enable_stream_mode())) return err;

  int ipc_status, r;
  message msg;

  while (cur_cnt < cnt) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("Driver failed: %d\n", r);
      return r;
    }

    if(is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
        
          if (msg.m_notify.interrupts & irq_set) {
            mouse_ih();

            if(!validByte) {
              counter = 0;
              break;
            }

            if(counter == 0 && !(lastByte & MOUSE_FIRST_BIT)) {
              break;
            }

            packet_storage[counter] = lastByte;

            counter++;

            if(counter == 3) {
              struct packet currPkt;
              parse_packet(packet_storage, &currPkt);

              mouse_print_packet(&currPkt);
              counter = 0;
              cur_cnt++;
            }
          }
          break;

        default:
          break;
      }
    }
  }

  if((err = mouse_unsubscribe_int())) return err;
  if((err = ms_disable_data_rep())) return err;

  return OK;
}

int(mouse_test_async)(uint8_t idle_time) {
 
  uint8_t bit_no;
  uint8_t counter = 0;
  uint8_t packet_storage[3];

  int err = mouse_subscribe_int(&bit_no);
  int mouse_irq = BIT(bit_no);
  if (err != OK)
    return err;


  err = timer_subscribe_int(&bit_no);
  int timer_irq = BIT(bit_no);
  if (err != OK)
    return err;

  if( (err = ms_enable_data_rep())) return err;
  if( (err = enable_stream_mode())) return err;

  int ipc_status, r;
  message msg;

  while (timeCounter < idle_time*60) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("Driver failed: %d\n", r);
      return r;
    }

    if(is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
        
          if (msg.m_notify.interrupts & mouse_irq) {
            mouse_ih();

            if(!validByte) {
              counter = 0;
              break;
            }

            if(counter == 0 && !(lastByte & MOUSE_FIRST_BIT)) {
              break;
            }


            packet_storage[counter] = lastByte;

            counter++;

            if(counter == 3) {
              struct packet currPkt;
              parse_packet(packet_storage, &currPkt);

              mouse_print_packet(&currPkt);
              counter = 0;
              
              timeCounter = 0;

            }
          }
          
          if(msg.m_notify.interrupts & timer_irq){
              timer_int_handler();
          }
          
          break;

        default:
          break;
      }
    }
  }

  if((err = mouse_unsubscribe_int())) return err;
  if((err = ms_disable_data_rep())) return err;

  return OK;


}

int(mouse_test_gesture)(uint8_t xlen, uint8_t tolerance) {
  uint8_t bit_no;
  uint8_t counter = 0;
  uint8_t packet_storage[3];

  int err = mouse_subscribe_int(&bit_no);
  int irq_set = BIT(bit_no);
  if (err != OK)
    return err;

  if( (err = ms_enable_data_rep())) return err;
  if( (err = enable_stream_mode())) return err;

  int ipc_status, r;
  message msg;


  // Initializing states
  enum states result = INITIAL;
  
  init_data();

  while (result != EXIT) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("Driver failed: %d\n", r);
      return r;
    }

    if(is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
        
          if (msg.m_notify.interrupts & irq_set) {
            mouse_ih();

            if(!validByte) {
              counter = 0;
              break;
            }

            if(counter == 0 && !(lastByte & MOUSE_FIRST_BIT)) {
              break;
            }

            packet_storage[counter] = lastByte;

            counter++;

            if(counter == 3) {
              struct packet currPkt;
              parse_packet(packet_storage, &currPkt);

              mouse_print_packet(&currPkt);

              result = handle_mouse_event(&currPkt, tolerance, xlen);
              printf("STATE %d\n", (int) result );
              counter = 0;
            }
          }
          break;

        default:
          break;
      }
    }
  }

  if((err = mouse_unsubscribe_int())) return err;
  if((err = ms_disable_data_rep())) return err;

  return OK;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    int err;
  
    uint8_t numPackets = 0;
    uint8_t packets[3];

  
    while (numPackets < cnt)
    {
      
      if(request_data_packet() != OK) continue;
      
      int iter = 0;

      while (iter < 3)
      {
          mouse_poll_handler();

          if(!validByte) continue;

          if( iter == 0 && !(lastByte & MOUSE_FIRST_BIT) ) continue;

          packets[iter] = lastByte;

          if(iter == 2){
            struct packet currPkt; 
            parse_packet(packets, &currPkt);

            mouse_print_packet(&currPkt);
          }
          iter++;

          tickdelay(WAIT_KBC);
      }
      

      numPackets++;
      tickdelay(micros_to_ticks(period*1000));

    }

    err = ms_disable_data_rep(); if(err != OK) return err;
    err = enable_stream_mode(); if(err!= OK) return err;
    err = ms_set_default(); if(err!= OK) return err;

    return reset_cmd_byte();
}
