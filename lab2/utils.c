#include <lcom/lcf.h>

#include <stdint.h>
#include "i8254.h" //Cant use this

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {

  *lsb = (uint8_t) (INIT_LSB & val);

  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {

  *msb = (uint8_t) ((INIT_MSB & val) >> 8);

  return 0;
}

int (util_sys_inb)(int port, uint8_t * value) {

  uint32_t tempAddress = 0;
  
  // int sys_inb(int port, u32_t *byte);
  if (sys_inb(port, &tempAddress) != 0) return 1;

  *value = (uint8_t) (tempAddress);

  //printf("%s is not yet implemented!\n", __func__);

  return 0;
}
