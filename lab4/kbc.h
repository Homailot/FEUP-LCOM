#ifndef __KEYBOARD_H
#define __KEYBOARD_H



#include <lcom/lcf.h>

#include <lcom/utils.h>
#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"

/** @defgroup keyboard keyboard
 * @{
 *
 * Functions for using the keyboard
 */


/**
 * @brief Subscribes the keyboard interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int (kb_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Subscribes the mouse interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int (mouse_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes the keyboard interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int (kb_unsubscribe_int)();

/**
 * @brief Unsubscribes the mouse interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int (mouse_unsubscribe_int)();

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
int (check_kbc_status)(uint8_t mask, bool * checkBuffer);


#endif /* __KEYBOARD_H */

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
int (kbc_enable_int)();


/**
 * @brief Issues a command to the kbc.
 * 
 * Has to read the status register and has to wait for the input buffer
 * to be empty.
 * 
 * @param command The command to be written to the keyboard input buffer
 * @return Return 0 upon success and non-zero otherwise
 */
int (issue_kbc_command) (uint8_t command);

/**
 * @brief Reads the return value of a command sent to the kbc.
 * 
 * Has to read the status register and make sure the interrupt handler
 * does not read from the output buffer.
 * 
 * @param value The value where the result will be stored
 * @return Return 0 upon success and non-zero otherwise
 */
int (read_kbc_return) (uint8_t *value);

/**
 * @brief Writes a command byte to the kbc
 * 
 * @param command_byte The command byte to be written
 * @return Return 0 upon success and non-zero otherwise
 */
int(write_kbc_command_byte) (uint8_t command_byte);

/**
 * @brief Reads the command byte in the kbc
 * 
 * @param command_byte Used to store the value of the command byte
 * @return Return 0 upon success and non-zero otherwise
 */
int(read_kbc_command_byte) (uint8_t *command_byte);


/**
 * Function to reset the command byte
 * @return Return 0 upon success and non-zero otherwise
 */
int(reset_cmd_byte)();

/**
 * Enables/disables data reporting from mouse
 * 
 * @param command the command to enable/disable data reporting
 * @return Return OK upon sucess
 */
int(ms_toggle_data_rep)(uint8_t command);
/**
 * Enables data reporting from mouse
 * @return Return OK upon sucess
 */
int(ms_enable_data_rep)();

/**
 * Disables data reporting from mouse
 * @return Return OK upon sucess
 */
int(ms_disable_data_rep)();

/**
 * @brief Issues a command to the kbc.
 * 
 * Has to read the status register and has to wait for the input buffer
 * to be empty.
 * 
 * @param command The command to be written to the keyboard input buffer
 * @return Return 0 upon success and non-zero otherwise
 */
int(issue_mouse_cmd)(uint8_t cmd);

/**
 * Function to write a command to the mouse with no return value
 * @param cmd - The command to be written to the mouse
 * @return Return 0 upon success and non-zero otherwise
 */
int(ms_cmd_arg)(uint8_t cmd);

/**
 * Function to write a command to the mouse with no return value
 * @param cmd - The command to be written to the mouse
 * @param return_val - The result of the command
 * @param size - Number of returns
 * @return Return 0 upon success and non-zero otherwise
 */
int(ms_cmd_return)(uint8_t cmd, uint8_t * return_val, uint8_t size);
// NESTE CASO ITERAR SIZE VEZES E GUARDAR OS VARIOS VALORES NO RETURN VAL

/**
 * Requests a data packet
 * @return Return 0 upon success and non-zero otherwise
 */
int(request_data_packet)();

/**
 * Enables stream mode from mouse
 * @return Return OK upon sucess
 */
int(enable_stream_mode)();

/**
 * Enables remote mode from mouse
 * @return Return OK upon sucess
 */
int(enable_remote_mode)();

/**
 * Sets mouse to default values
 * @return Return OK upon sucess
 */
int(ms_set_default)();





/**
 * Turns a 9 bit two's complement number to a 16 bit number.
 * 
 * @param lsb The 8 bit least significant byte.
 * @param negative negative.
 * @param result The result paramater.
 * 
 */
void (int9_to_int16)(uint8_t lsb, bool negative, int16_t * result);

void (parse_packet)(uint8_t bytes[3], struct packet * pkt);


void (mouse_poll_handler)();
