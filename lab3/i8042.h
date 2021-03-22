#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the Keyboard. Needs to be completed.
 */

/* Keyboard interrupts  */

#define KBC_IRQ 1 /**< @brief Keyboard IRQ line */
#define KBC_TEMPHOOKID 0x0001f


/* I/O port addresses  */

#define KBC_STAT_REG 0x64
#define KBC_IN_BUF 0x64
#define KBC_OUT_BUF 0x60


/* KBC TIMERS */

#define WAIT_KBC micros_to_ticks(20000)
#define KBC_MAX_TRIES 5


/* KBC COMMANDS */

#define KBC_WRITE_CMD 0x60
#define KBC_READ_CMD 0x20


/* Status Byte Parsing  */

#define KBC_NONE 0 // Usefull when only need to know output buffer
#define KBC_OUT_BUFFER_FULL BIT(0)
#define KBC_IN_BUFFER_FULL BIT(1)
#define KBC_AUX BIT(5)
#define KBC_TIMEOUT_ERR BIT(6)
#define KBC_PARITY_ERR BIT(7)


/* Code parsing */

#define KBC_DOUBLE_BYTE 0xE0
#define KBC_IS_SINGLE_BYTE(code) ((code >> 8) == 0)
#define KBC_BYTE_MASK 0xFF
#define KBC_ESC_CODE 0x81
#define KBC_BREAK_CODE BIT(7)
#define KBC_GET_BYTE(val, i) (val >> (8*i))


/* COMMAND BYTE PARSING */

#define BIT_TO_ZERO(num, bit) num & ~(1 << bit)
#define KBC_CMDB_INT BIT(0)
#define KBC_CMDB_INT2 BIT(0)


/* KBC KEYCODES */

#define BREAKCODE_ESC 0x81


/* Errors */

#define STATUS_ERR 100
#define KBC_TRIES_TIMEOUT 101

/**@}*/

#endif /* _LCOM_I8042_H */
