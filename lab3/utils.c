#include <lcom/lcf.h>
#include <lcom/utils.h>
#include <stdint.h>

#ifdef LAB3
extern int sys_IN_CALLS;
#endif

int (util_sys_inb)(int port, uint8_t * value) {

  uint32_t tempAddress = 0;


  #ifdef LAB3
  sys_IN_CALLS ++;
  #endif

  // int sys_inb(int port, u32_t *byte);
  if (sys_inb(port, &tempAddress) != 0) return 1;

  *value = (uint8_t) (tempAddress);

  //printf("%s is not yet implemented!\n", __func__);

  return 0;
}
