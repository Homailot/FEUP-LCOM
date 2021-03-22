#ifndef _G01_VIDEO_CMD_H_
#define _G01_VIDEO_CMD_H_

/** @defgroup video_cmd Video VBE functions
 * @{
 *
 * @brief Constants for accessing the VBE interface.
 */

/*!< VBE function to get the controller info */
#define VBE_CTRL_INFO 0x00
/*!< VBE function to get info about a ceratin mode */
#define VBE_INFO 0x01
/*!< VBE function to initialize video graphics mode */
#define VBE_FUNC 0x02
/*!< VBE function to change the address of the buffer to be drawn */
#define VBE_SET_DISPLAY_FUNC 0x07
/*!< VBE 0x07's sub-function to only change the address of buffer on the vertical retrace */
#define VBE_SET_DISPLAY_RETRACE 0x80
/*!< Identifier for all VBE calls */
#define VBE_CALL 0x4F00
/*!< Interrupt number for BIOS calls*/
#define VBE_BIOS_CALL 0x10
/*!< BIT to set linear frame */
#define LINEAR_FRAME_BUFFER BIT(14)

/**@}*/

#endif
