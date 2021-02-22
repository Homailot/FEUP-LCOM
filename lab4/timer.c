#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>
#include "i8254.h"

static int subscribe_result;

int timeCounter;


int (timer_set_frequency)(uint8_t timer, uint32_t freq) {

  if (timer < 0 || timer > 2) return 1;

  uint8_t control_word = 0;
  uint8_t status_byte;
  /* To be implemented by the students */
  //printf("%s is not yet implemented!\n", __func__);

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
  if(sys_outb(TIMER_CTRL, (uint32_t) control_word) != OK) return EINVAL;

  // Write the initial value to the timer of choice.
  if(sys_outb(port, (uint32_t) lsb_init_value) != OK) return EINVAL;
  if(sys_outb(port, (uint32_t) msb_init_value) != OK) return EINVAL;
  

  return OK;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  subscribe_result = TIMER0_IRQ;
  *bit_no = TIMER0_IRQ;
  int err = sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &subscribe_result);

  return err;
}

int (timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&subscribe_result);
}

void (timer_int_handler)() {

  timeCounter++;
  
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  if (timer < 0 || timer > 2) return 1;


  int timerAddress = TIMER_ADDR_SEL(timer);
  int readBackAddress = TIMER_CTRL;

  // Controll word for write-back
  uint8_t controlWord = TIMER_RB_CMD | TIMER_RB_SEL(timer) | TIMER_RB_COUNT_;

  //printf("Control Word: %x\n", controlWord);

  // Writing control word - Is this correct?
  if (sys_outb(readBackAddress, controlWord) != OK) return 1;

  
  // Read-back Command
  if(util_sys_inb(timerAddress, st) != OK) return 1;

  //printf("%x: st ye", *st);
  return OK;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {

  
  if (timer < 0 || timer > 2) return 1;                        

  union timer_status_field_val value;

  switch(field){
    case tsf_all:
      value.byte = st;
      break;

    case tsf_initial:
      if((st & (TIMER_LSB)) == TIMER_LSB) 
        value.in_mode = LSB_only;
      else if((st & (TIMER_MSB)) == TIMER_MSB) 
        value.in_mode = MSB_only;
      else if((st & (TIMER_LSB | TIMER_MSB)) == (TIMER_LSB | TIMER_MSB))
        value.in_mode = MSB_after_LSB;
      else 
        value.in_mode = INVAL_val;
      break;

    case tsf_mode:
      value.count_mode = PROGGRAMED_MODE(st);
      break;
    
    case tsf_base:
      value.bcd = (st & BCD_BIT) == BCD_BIT;
    break;
  }

  timer_print_config(timer, field, value);

  return OK;
}
