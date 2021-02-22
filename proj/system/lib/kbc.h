#ifndef _G01_KBC_H_
#define _G01_KBC_H_

#  include <lcom/lcf.h>

#  include "i8042.h"
#  include <lcom/utils.h>
#  include <stdbool.h>
#  include <stdint.h>

/** @defgroup kbc KBC functions
 * @{
 *
 * @brief Functions for using the KBC interface
 */

 /**
  * @brief Subscribes the keyboard interrupts
  * @return Return 0 upon success and non-zero otherwise
  */
int(kb_subscribe_int)(uint8_t* bit_no);

/**
 * @brief Subscribes the mouse interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int(mouse_subscribe_int)(uint8_t* bit_no);

/**
 * @brief Unsubscribes the keyboard interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int(kb_unsubscribe_int)();

/**
 * @brief Unsubscribes the mouse interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int(mouse_unsubscribe_int)();

/**
 * @brief Checks the kbc status according to the flags desired in mask
 * Example: mask =  KBC_IN_FULL | KC_PARITY_ERR will checks if the input buffer is full and there is a parity error
 *          If that happens, returns err
 * If you want to check the output buffer, select checkBuffer to true
 * If the output buffer is full means that there is no error and is able to read
 * @param mask - Pointer to the mask desired
 * @param checkBuffer - Pointer to the boolean to return the value of the buffer
 * @return Return 0 upon success and non-zero otherwise
 */
//int(check_kbc_status)(uint8_t mask, bool* checkBuffer);

/**
 * @brief Disables interrupts from the kbc (keyboard + mouse)
 *
 * Checks if it was previously subscribed. If it was not previously subscribed
 * the function sets a policy.
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbc_disable_int)();

/**
 * @brief Enables interrupts from the kbc (keyboard + mouse)
 *
 * Checks if it was previously subscribed and if a policy was set when disabling interrupts.
 * If this is the case, the policy is removed
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbc_enable_int)();

/**
 * @brief Issues a command to the kbc.
 *
 * Has to read the status register and has to wait for the input buffer
 * to be empty.
 *
 * @param command The command to be written to the keyboard input buffer
 * @return Return 0 upon success and non-zero otherwise
 */
int(write_byte_kbc)(uint8_t byte);

/**
 * @brief Reads the return value of a command sent to the kbc.
 *
 * Has to read the status register and make sure the interrupt handler
 * does not read from the output buffer.
 *
 * @param value The value where the result will be stored
 * @return Return 0 upon success and non-zero otherwise
 */
int (read_byte_kbc)(uint8_t * byte);

/**
 * @brief Writes an argument to a command to the kbc.
 *
 * Has to read the status register (checking that the output buffer is empty) and make sure the interrupt handler
 * does not read from the output buffer.
 *
 * @param value The argument to be written
 * @return Return 0 upon success and non-zero otherwise
 */
int(write_arg_kbc)(uint8_t byte);

/**
 * @brief Writes a command byte to the kbc
 *
 * @param command_byte The command byte to be written
 * @return Return 0 upon success and non-zero otherwise
 */
int (write_kbc_cmd_byte)(uint8_t byte);

/**
 * @brief Function to reset the command byte
 * @return Return 0 upon success and non-zero otherwise
 */
int(reset_cmd_byte)();

/**
 * @brief Enables data reporting from mouse
 * @return Return OK upon sucess
 */
int(enable_data_reporting)();

/**
 * @brief Disables data reporting from mouse
 * @return Return OK upon sucess
 */
int(disable_data_reporting)();

/**
 * @brief Issues a command to the kbc.
 *
 * Has to read the status register and has to wait for the input buffer
 * to be empty. Tries multiple times when recieving acknowledgment errors.
 *
 * @param arg_size The number of arguments (including the command itself)
 * @param args an array of the arguments to be sent
 * @return Return 0 upon success and non-zero otherwise
 */
int (write_ms_cmd)(size_t arg_size, uint8_t * args);

/**
 * @brief Function to read the result of a mouse command.
 * @param arg_size Number of returns
 * @param args an array of arguments to be recieved
 * @return Return 0 upon success and non-zero otherwise
 */
int(read_ms_cmd)(size_t arg_size, uint8_t * args);

/**
 * @brief Enables stream mode from mouse
 * @return Return OK upon sucess
 */
int(enable_stream)();

/**
 * @brief Enables remote mode from mouse
 * @return Return OK upon sucess
 */
int(enable_remote)();

/**
 * @brief Enables mouse
 * @return Return OK upon sucess
 */
int (enable_ms)();

/**
 * @brief Sets mouse to default values
 * @return Return OK upon sucess
 */
int(ms_set_default)();

/**
 * @brief Turns a 9 bit two's complement number to a 16 bit number.
 *
 * @param lsb The 8 bit least significant byte.
 * @param negative negative.
 * @param result The result paramater.
 *
 */
void(int9_to_int16)(uint8_t lsb, bool negative, int16_t* result);

/**
 * @brief Turns a 3 byte array into a packet structure
 *
 * @param bytes the 3 byte array
 * @param pkt pointer where the result will be put
 */
void(parse_packet)(uint8_t bytes[3], struct packet* pkt);

/**
 * @brief Interrupt handler for the keyboard
 */
void(kbc_ih)(void);

/**
 * @brief Interrupt handler for the mouse
 */
void(mouse_ih)();

/**@}*/

#endif
