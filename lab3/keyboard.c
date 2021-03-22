#include "keyboard.h"
#include <stdint.h>

static int * kb_subscribe_result = NULL;

int(kbc_subscribe_int)(uint8_t *bit_no) {
    kb_subscribe_result = (int *) malloc(sizeof(int));
    *kb_subscribe_result = (int) *bit_no;
    int err = sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, kb_subscribe_result);
    
    return err;
}

int(kbc_unsubscribe_int)() {
  int err = sys_irqrmpolicy(kb_subscribe_result);
  if(err != OK) return err;

  free(kb_subscribe_result);
  kb_subscribe_result = NULL;

  return OK;
}

int(check_kbc_status)(uint8_t mask, bool *checkBuffer) {

  uint8_t sb = 0;
  int port = KBC_STAT_REG;
  int err = util_sys_inb(port, &sb);
  if (err != 0)
    return err;

  // If requested to check output buffer, change it to its value
  if (*checkBuffer)
    *checkBuffer = (sb & KBC_OUT_BUFFER_FULL);

  // If there are no errors in the selected ones, returns OK
  if ((sb & mask) == OK)
    return OK;
  else
    return STATUS_ERR;
}

void(kbc_ih)(void) {

  // Checks for the existence of errors or if the data is from the mouse
  uint8_t mask = KBC_PARITY_ERR | KBC_TIMEOUT_ERR | KBC_AUX;

  bool checkOutBuff;
  int val = check_kbc_status(mask, &checkOutBuff);
  int err = OK;
  // The output buffer is full, can be read
  if (checkOutBuff) {

    uint8_t keycode;
    int port = KBC_OUT_BUF;
    err = util_sys_inb(port, &keycode);

    // Only if there are no errors in parity and timeout, the value is stored
    if (err == OK && val == OK) {
      lastKeyCode = keycode;
    }
  }
}

int(keyboard_disable_int)(int * result) {
    int err = OK;

    if(kb_subscribe_result == NULL) {
        *result = KBC_TEMPHOOKID;
        err = sys_irqsetpolicy(KBC_IRQ, IRQ_EXCLUSIVE, result);
        if(err != OK) return err;

        return sys_irqdisable(result);
    }

    return sys_irqdisable(kb_subscribe_result);
}

int(keyboard_enable_int)(int * result) {
    int err = OK;

    if(kb_subscribe_result == NULL) {
        if(result == NULL) return OK;
        
        err = sys_irqenable(result);
        if(err != OK) return err;

        return sys_irqrmpolicy(result);
    }

    return sys_irqenable(kb_subscribe_result);
}

int (issue_kbc_command) (uint8_t command) {
    uint8_t tries = 0;
    bool output_buf = false;

    while(tries < KBC_MAX_TRIES) {
        if(check_kbc_status(KBC_IN_BUFFER_FULL, &output_buf) == OK) {
            return sys_outb(KBC_IN_BUF, command);
        }

        tries++;
        tickdelay(WAIT_KBC);
    }

    return KBC_TRIES_TIMEOUT;
}

int(read_kbc_return) (uint8_t *value) {
    uint8_t tries = 0;
    int err = KBC_TRIES_TIMEOUT;
    bool output_buf = true;    

    while(tries < KBC_MAX_TRIES) {
        output_buf = true;
        if(check_kbc_status(KBC_PARITY_ERR | KBC_TIMEOUT_ERR, &output_buf) == OK && output_buf) {
            // THE RETURN VALUE OF A COMMAND IS WRITTEN TO THE OUTPUT BUFFER
            err = util_sys_inb(KBC_OUT_BUF, value);

            break;
        }

        tries++;
        tickdelay(WAIT_KBC);
    }

    

    return err;
}

int (write_kbc_command_byte) (uint8_t command_byte) {
    int err = issue_kbc_command(KBC_WRITE_CMD);
    if(err != OK) return err;

    return sys_outb(KBC_OUT_BUF, command_byte);
}

int(read_kbc_command_byte) (uint8_t *command_byte) {
  int sub_result = 0;

  // UNSUBSCRIBE FROM KEYBOARD INTERRUPTS SO THAT THE IH DOESN'T "STEAL" THE CODES
  keyboard_disable_int(&sub_result);

  int err = issue_kbc_command(KBC_READ_CMD);
  if(err != OK) return err;

  // RETURN INTERRUPTS TO FORMER STATE
  keyboard_enable_int(&sub_result);

  return read_kbc_return(command_byte);
}
