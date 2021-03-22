#ifndef _G01_RTCDEF_H_
#define _G01_RTCDEF_H_

/** @defgroup rtc_defs RTC defines
 * @{
 *
 * @brief Defines for using the RTC interface
 */

/** Addresses */

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71

/** RTC Data registers */

#define RTC_SECONDS 0
#define RTC_ALARM_SECONDS 1
#define RTC_MINUTES 2
#define RTC_ALARM_MINUTES 3
#define RTC_HOURS 4
#define RTC_ALARM_HOURS 5
#define RTC_WEEK_DAY 6
#define RTC_MONTH_DAY 7
#define RTC_MONTH 8
#define RTC_YEAR 9
#define RTC_REGISTER_A 10
#define RTC_REGISTER_B 11
#define RTC_REGISTER_C 12   

/** Interrupts */

#define RTC_DONT_CARE 0xC0

#define RTC_UIP BIT(7)
#define RTC_RS0 BIT(0)
#define RTC_RS1 BIT(1)
#define RTC_RS2 BIT(2)
#define RTC_RS3 BIT(3)
#define RTC_RS_CLEAR 0xf0

#define RTC_PIE BIT(6)
#define RTC_AIE BIT(5)
#define RTC_UIE BIT(4)

#define RTC_PF  BIT(6)
#define RTC_AF  BIT(5)
#define RTC_UF BIT(4)

#define RTC_IRQ 8

/** @}*/

#endif
