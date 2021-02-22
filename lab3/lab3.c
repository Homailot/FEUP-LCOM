#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include "i8254.h"
#include "keyboard.h"
#include <stdbool.h>
#include <stdint.h>

extern uint8_t lastKeyCode;

extern int timeCounter;

uint32_t sys_IN_CALLS;

int main(int argc, char *argv[]) {

  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/g01/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g01/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv)) {
    printf("Bizarro1\n");

    return 1;
  }
  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  sys_IN_CALLS = 0;

  uint8_t bit_no = KBC_IRQ;
  uint8_t irq_set = BIT(bit_no);

  int err;
  err = kbc_subscribe_int(&bit_no);
  if (err != OK)
    return err;

  int ipc_status, r;
  message msg;

  bool lastDoubleByte = false;

  while (lastKeyCode != KBC_ESC_CODE) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("Driver failed: %d\n", r);
    }

    if (is_ipc_notify(ipc_status)) {

      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {

            kbc_ih();

            if (lastKeyCode == KBC_DOUBLE_BYTE) {
              lastDoubleByte = true;
            }
            else {
              bool make = !(lastKeyCode & KBC_BREAK_CODE);

              uint8_t size = lastDoubleByte ? 2 : 1;

              uint8_t bytes[size];

              bytes[size-1] = lastKeyCode;
              if (lastDoubleByte) bytes[0] = KBC_DOUBLE_BYTE;

              kbd_print_scancode(make, size, bytes);

              lastDoubleByte = false;
            }
          }

          break;

        default:
          break;
      }
    }
  }
  kbc_unsubscribe_int();
  kbd_print_no_sysinb( sys_IN_CALLS );

  return OK;
}


int(kbd_test_poll)() {
  uint8_t command_byte = 0;
  bool output_buf = true;
  int err = OK;
  sys_IN_CALLS = 0;

  bool lastDoubleByte = false;

  while(lastKeyCode != BREAKCODE_ESC) {
    output_buf = true;
    err = check_kbc_status(KBC_AUX, &output_buf);
    if(err == OK && output_buf) {
      kbc_ih();
      
      if (lastKeyCode == KBC_DOUBLE_BYTE) {
        lastDoubleByte = true;
      }
      else {
        bool make = !(lastKeyCode & KBC_BREAK_CODE);

        uint8_t size = lastDoubleByte ? 2 : 1;

        uint8_t * bytes = (uint8_t *) malloc(size * sizeof(uint8_t));

        bytes[size-1] = lastKeyCode;
        if (lastDoubleByte) bytes[0] = KBC_DOUBLE_BYTE;

        kbd_print_scancode(make, size, bytes);

        lastDoubleByte = false;

        free(bytes);
      }
    }

    tickdelay(WAIT_KBC);
  }
  // READ THE CURRENT KBC COMMAND BYTE TO LATER WRITE IT BACK
  err = read_kbc_command_byte(&command_byte);
  if(err != OK) return err;
  
  // ENABLE INTERRUPTIONS
  command_byte |= KBC_CMDB_INT;

  kbd_print_no_sysinb( sys_IN_CALLS );

  return write_kbc_command_byte(command_byte);
}

int(kbd_test_timed_scan)(uint8_t n) {
  
  uint8_t bit_no_t = TIMER0_IRQ, bit_no_kbd = KBC_IRQ; 

  timer_subscribe_int(&bit_no_t);
  kbc_subscribe_int(&bit_no_kbd);

  int irq_set_timer = BIT(bit_no_t);
  int irq_set_kbd = BIT(bit_no_kbd);


  int ipc_status, r;
  message msg;

  bool lastDoubleByte = false;


  while (lastKeyCode != KBC_ESC_CODE && timeCounter < n * 60) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("Driver failed: %d\n", r);
    }

    if (is_ipc_notify(ipc_status)) {

      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set_kbd) {
            
            timeCounter = 0;

            kbc_ih();

            if (lastKeyCode == KBC_DOUBLE_BYTE) {
              lastDoubleByte = true;
            }
            else {
              bool make = !(lastKeyCode & KBC_BREAK_CODE);

              uint8_t size = lastDoubleByte ? 2 : 1;

              uint8_t bytes[size];

              bytes[size-1] = lastKeyCode;
              if (lastDoubleByte) bytes[0] = KBC_DOUBLE_BYTE;

              kbd_print_scancode(make, size, bytes);

              lastDoubleByte = false;
            }
          }

          if(msg.m_notify.interrupts & irq_set_timer){
            timer_int_handler();
          }

          break;

        default:
          break;
      }
    }
  }

  timer_unsubscribe_int();
  kbc_unsubscribe_int();

  return OK;
}
