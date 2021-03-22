#include "kbc.h"
#include <stdint.h>

static int * kb_subscribe_result = NULL;
static int * mouse_subscribe_result = NULL;

bool validByte = false;

uint8_t lastByte;

int(kb_subscribe_int)(uint8_t *hook_id) {
    *hook_id = KB_HOOK_ID;
    kb_subscribe_result = (int*) malloc(sizeof(int));
    *kb_subscribe_result = (int) *hook_id;

    return sys_irqsetpolicy(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, kb_subscribe_result);
}

int (mouse_subscribe_int)(uint8_t *hook_id) {
    *hook_id = MOUSE_HOOK_ID;
    mouse_subscribe_result = (int *) malloc(sizeof(int));
    *mouse_subscribe_result = (int) *hook_id;

    return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, mouse_subscribe_result);
} 

int(kb_unsubscribe_int)() {
  int err = sys_irqrmpolicy(kb_subscribe_result);

  free(kb_subscribe_result);
  kb_subscribe_result = NULL;

  return err;
}

int(mouse_unsubscribe_int)() {
  int err = sys_irqrmpolicy(mouse_subscribe_result);

  free(mouse_subscribe_result);
  mouse_subscribe_result = NULL;

  return err;
}

int(kbc_disable_int)() {
  int err = OK;

  if (mouse_subscribe_result != NULL)
    err = sys_irqdisable(mouse_subscribe_result);
  if (kb_subscribe_result != NULL)
    err = sys_irqdisable(kb_subscribe_result);

  return err;
}

int(kbc_enable_int)() {
  int err = OK;

  if (mouse_subscribe_result != NULL)
    err = sys_irqenable(mouse_subscribe_result);
  if (kb_subscribe_result != NULL)
    err = sys_irqenable(kb_subscribe_result);

  return err;
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
  return (sb & mask);
}

void(kbc_ih)(void) {

  // Checks for the existence of errors or if the data is from the mouse
  uint8_t mask = KBC_PARITY_ERR | KBC_TIMEOUT_ERR;
  lastByte = 0;
  validByte = false;

  bool checkOutBuff;
  int val = check_kbc_status(mask, &checkOutBuff);
  int err = OK;
  // The output buffer is full, can be read
  if (checkOutBuff) {
    uint8_t byte;
    err = util_sys_inb(KBC_OUT_BUF, &byte);

    // Only if there are no errors in parity and timeout, the value is stored
    if (err == OK && val == OK) {
      lastByte = byte;
      validByte = true;
    }
  }
}


void (mouse_poll_handler)(){
  // Checks for the existence of errors or if the data is from the mouse
  uint8_t mask = KBC_PARITY_ERR | KBC_TIMEOUT_ERR | KBC_AUX;
  lastByte = 0;
  validByte = false;

  bool checkOutBuff;
  int val = check_kbc_status(mask, &checkOutBuff);
  int err = OK;
  // The output buffer is full, can be read
  if (checkOutBuff) {
    uint8_t byte;
    err = util_sys_inb(KBC_OUT_BUF, &byte);

    // Only if there are no errors in parity and timeout, the value is stored
    if (err == OK && val == KBC_AUX) {
      lastByte = byte;
      validByte = true;
    }
  }
}

void(mouse_ih)() {
  kbc_ih();
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
    int err = KBC_TRIES_TIMEOUT, rerr;
    bool output_buf = true;    

    if((rerr = kbc_disable_int()) != OK) return rerr;

    while(tries < KBC_MAX_TRIES) {
      tickdelay(WAIT_KBC);
        output_buf = true;
        if(check_kbc_status(KBC_PARITY_ERR | KBC_TIMEOUT_ERR, &output_buf) == OK && output_buf) {
            // THE RETURN VALUE OF A COMMAND IS WRITTEN TO THE OUTPUT BUFFER
            err = util_sys_inb(KBC_OUT_BUF, value);

            break;
        }

        tries++;
    }

    return kbc_enable_int();
}

int (write_kbc_command_byte) (uint8_t command_byte) {
    int err = issue_kbc_command(KBC_WRITE_CMD);
    if(err != OK) return err;

    int tries = 0; bool output_buf = false;
    while(tries < KBC_MAX_TRIES) {
        if(check_kbc_status(KBC_IN_BUFFER_FULL, &output_buf) == OK) {
            return sys_outb(KBC_OUT_BUF, command_byte);
        }

        tries++;
        tickdelay(WAIT_KBC);
    }

    return KBC_TIMEOUT_ERR;
}


int(reset_cmd_byte)(){
    uint8_t normal_status = minix_get_dflt_kbc_cmd_byte();
    
    return write_kbc_command_byte(normal_status);
}


int(read_kbc_command_byte) (uint8_t *command_byte) {
  int err = issue_kbc_command(KBC_READ_CMD);
  if(err != OK) return err;

  return read_kbc_return(command_byte);
}

int(write_mouse_command_byte)(uint8_t cmd){
    int err = issue_kbc_command(MOUSE_WRITE_CMD);
    if(err != OK) return err;

    int tries = 0; bool output_buf = false;
    while(tries < KBC_MAX_TRIES) {
        if(check_kbc_status(KBC_IN_BUFFER_FULL, &output_buf) == OK) {
            return sys_outb(KBC_OUT_BUF, cmd);
        }

        tries++;
        tickdelay(WAIT_KBC);
    }

    return KBC_TIMEOUT_ERR;
}


int(ms_cmd_arg)(uint8_t cmd){
  int err = ms_disable_data_rep(); if(err != OK) return err;

  uint8_t cmd_response;

  uint8_t tries = 0;

  while (tries < KBC_MAX_TRIES)
  {
    err = write_mouse_command_byte(cmd); if(err != OK) return err;

    err = read_kbc_return(&cmd_response); if(err != OK) return err;

    if(cmd_response == ACK){
      return ms_enable_data_rep(); 
    }
    tries++;
    tickdelay(WAIT_KBC);
  }
  
  err = ms_enable_data_rep(); if(err != OK) return err;
  
  return KBC_TRIES_TIMEOUT;
}


int(ms_cmd_with_args)(uint8_t cmd, uint8_t * args, size_t size){
  write_mouse_command_byte(cmd);

  uint8_t ret_val;

  read_kbc_return(&ret_val);

  printf("Resp 1 %x\n", ret_val);

  for (size_t i = 0; i < size; i++)
  {
      printf("Arg:%d\n", args[i]);
      write_mouse_command_byte(args[i]);
      read_kbc_return(&ret_val);
      printf("Return value %x\n",ret_val);
  }
  return OK;
}

int(ms_cmd_return)(uint8_t cmd, uint8_t * return_val, uint8_t size){

  int err = write_mouse_command_byte(cmd);
  if(err != OK) return err;

  uint8_t st;
  err = read_kbc_return(&st);

  if(st != ACK) return 1;

  for (size_t i = 0; i < size; i++)
  {
    if (OK != read_kbc_return(return_val + i)) return 2;
  }
  return OK;
}


int (enable_stream_mode)(){
  return ms_cmd_arg(SET_STREAM_MODE);
}

int (enable_remote_mode)(){
  return ms_cmd_arg(SET_REMOTE_MODE);
}

int(ms_set_default)() {
  return ms_cmd_arg(SET_DEFAULTS);
}

int(ms_toggle_data_rep)(uint8_t cmd){
  int err;

  uint8_t cmd_response;

  uint8_t tries = 0;

  while (tries < KBC_MAX_TRIES)
  {
    err = write_mouse_command_byte(cmd); if(err != OK) return err;

    err = read_kbc_return(&cmd_response); if(err != OK) return err;

    if(cmd_response == ACK){
      return OK; 
    }
    tries++;
    tickdelay(WAIT_KBC);
  }
    
  return KBC_TRIES_TIMEOUT;
}

int(request_data_packet)(){
  return ms_toggle_data_rep(REQUEST_DATA);
}

int (ms_enable_data_rep)(){
  return ms_toggle_data_rep(ENABLE_DATA_REPORT);
}

int (ms_disable_data_rep)(){
  return ms_toggle_data_rep(DISABLE_DATA_REPORT);
}

void (int9_to_int16)(uint8_t lsb, bool negative, int16_t * result){
  *result = lsb;

  if(negative) {
    int16_t mask = 0xff00;
    
    *result |= mask;
  }

}

void (parse_packet)(uint8_t bytes[3], struct packet * pkt)
{
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
