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
 * @brief Const to access last key make code
 */ 

uint8_t lastKeyCode;


/**
 * @brief Subscribes the keyboard interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbc_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes the keyboard interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbc_unsubscribe_int)();

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
 * @brief Disables interrupts from the keyboard
 * 
 * Checks if it was previously subscribed. If it was not previously subscribed
 * the function sets a policy.
 * 
 * @param result If no policy was previously set, result of setting a new policy is stored here
 * @return Return 0 upon success and non-zero otherwise
 */
int(keyboard_disable_int)(int *result);

/**
 * @brief Enables interrupts from the keyboard
 * 
 * Checks if it was previously subscribed and if a policy was set when disabling interrupts.
 * If this is the case, the policy is removed
 * 
 * @param result Result of the disabling the keyboard interrupt. Set to null if keyboard_disable_int was not used.
 * @return Return 0 upon success and non-zero otherwise
 */
int (keyboard_enable_int)(int * result);


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
