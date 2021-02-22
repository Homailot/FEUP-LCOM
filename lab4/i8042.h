#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the Keyboard. Needs to be completed.
 */

/* Keyboard interrupts  */

#define KB_IRQ 1 /**< @brief Keyboard IRQ line */
#define KB_HOOK_ID 1
#define MOUSE_IRQ 12
#define MOUSE_HOOK_ID 12
#define KB_TMP_HOOK_ID 0x0001f
#define MOUSE_TMP_HOOK_ID 0x0001e


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

/* Mouse */

/*     Packet parsing */
    #define MOUSE_FIRST_BIT BIT(3)
    #define MOUSE_LB BIT(0)
    #define MOUSE_RB BIT(1)
    #define MOUSE_MB BIT(2)
    #define MOUSE_MSB_X_DELTA BIT(4)
    #define MOUSE_MSB_Y_DELTA BIT(5)
    #define MOUSE_X_OVFL BIT(6)
    #define MOUSE_Y_OVFL BIT(7)
    #define MOUSE_GET_MSB_X_DELTA(n) ((n & MOUSE_MSB_X_DELTA) >> 6)
    #define MOUSE_GET_MSB_Y_DELTA(n) ((n & MOUSE_MSB_Y_DELTA) >> 7)

/*     MOUSE COMMANDS */
    #define MOUSE_WRITE_CMD 0xD4
    #define ENABLE_DATA_REPORT 0xF4
    #define DISABLE_DATA_REPORT 0xF5
    #define SET_STREAM_MODE 0xEA
    #define SET_REMOTE_MODE 0xF0
    #define REQUEST_DATA 0xEB
    #define SET_DEFAULTS 0xF6

    #define ACK 0xFA
    #define NACK 0xFE
    #define ERROR 0xFC


/*     GESTURE RECOGNITION */
    #define PACKET_SLOPE_COUNT 20

/**@}*/

#endif /* _LCOM_I8042_H */
