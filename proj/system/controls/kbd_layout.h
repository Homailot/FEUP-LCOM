#ifndef _G01_KBDLAY_H_
#define _G01_KBDLAY_H_

#include "../lib/i8042.h"

/** @defgroup kbd_layout Keyboard Layout
 * @{
 *
 * @brief Converts keycodes to characters.
 */

#define PARSE_BREAK_CODE(x) (x &(~KBC_BREAK_CODE))
#define Q_KEY 0x10
#define W_KEY 0x11
#define A_KEY 0x1E
#define S_KEY 0x1F
#define D_KEY 0x20
#define PT_SHIFT 0x2A
#define PT_CTRL 0x1D

#define PT_1 0x02
#define PT_0 0x0B
/**@}*/

#endif