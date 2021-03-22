#include <lcom/lcf.h>
#include <lcom/utils.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>

extern int timeCounter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/g01/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g01/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv)) {
    return 1;
  }
    

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {

  uint8_t statusByte = 0;

  if(timer_get_conf(timer, &statusByte) != 0) return 1;

  //printf("Result: %x\n", statusByte & 0xff);
  //print_binary(statusByte); printf("\n");

  timer_display_conf(timer, statusByte, field);
 
  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  return timer_set_frequency(timer, freq);
}

int(timer_test_int)(uint8_t time) {
  uint8_t bit_no = 0;
  uint8_t irq_set = BIT(bit_no);
  //printf("IRQ: %x\n", irq_set);

  // Lidar com estes erros
  int err;
  if ((err = timer_subscribe_int(&bit_no)) != OK){
    return err;
  };

  int ipc_status, r;
  message msg;

  while ( timeCounter < time * 60 )
  {
    if((r = driver_receive(ANY, &msg, &ipc_status)) != 0){
      printf("Driver failed: %d\n", r);
    }
    
    if(is_ipc_notify(ipc_status)){
      switch (_ENDPOINT_P(msg.m_source))
      {
      case HARDWARE:
          
        if(msg.m_notify.interrupts & irq_set){
          timer_int_handler();

          if(timeCounter % 60 == 0){
            timer_print_elapsed_time();
          }

        }
        break;
      
      default:
        break;
      }
    }
  }
  
  timer_unsubscribe_int();
  return OK;

}
