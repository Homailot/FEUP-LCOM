// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "rtc_controller.h"
#include "rtc.h"

static uint8_t config_a;
static uint8_t config_b;

int bcd_to_decimal(uint8_t * val){
	uint8_t temp = 0;

    //GET LS half byte
	temp = (*val)&0xf;

    // GET MS half byte real value and add
	temp += 10*(((*val)>>4)&0xf);
	*val = temp;
	return OK;
}

int binary_to_bcd(uint8_t * val) {
    uint8_t temp = 0;

    temp = (*val)%10 | ((*val)/10) << 4;

    *val = temp;

    return OK;
}

/**
 * @brief Chooses what register to use for the next operation
 * @param reg Register
 */
int rtc_set_reg(uint8_t reg){
	return sys_outb(RTC_ADDR_REG,reg);
}

/**
 * @brief Reads a specified register
 * @param reg Register to read
 * @param [out] byte Output value  
 */
int rtc_read_reg(uint8_t reg, uint8_t * byte){
	rtc_set_reg(reg);
	return util_sys_inb(RTC_DATA_REG, byte);
}

/**
 * @brief Writes to a specified register
 * @param reg Register to write
 * @param byte Value to write
 */
int rtc_write_reg(uint8_t reg, uint8_t byte) {
    rtc_set_reg(reg);
    return sys_outb(RTC_DATA_REG, byte);
}

int rtc_test_date(void) {
	uint8_t sec,min,hour,day,month,year;

	uint8_t config = 0;

	rtc_read_reg(RTC_REGISTER_A, &config);
	

	// About to change date
	if(config&RTC_UIP)
		return 1;

	rtc_read_reg(RTC_SECONDS,&sec);
	rtc_read_reg(RTC_MINUTES,&min);
	rtc_read_reg(RTC_HOURS,&hour);
	rtc_read_reg(RTC_MONTH_DAY,&day);
	rtc_read_reg(RTC_MONTH,&month);
	rtc_read_reg(RTC_YEAR,&year);

	bcd_to_decimal(&sec);
	bcd_to_decimal(&min);
	bcd_to_decimal(&hour);
	bcd_to_decimal(&day);
	bcd_to_decimal(&month);
	bcd_to_decimal(&year);

	printf("Config: %d\n", config);

	printf("%d/%d/%d %d-%d-%d\n",day,month,year, hour,min,sec);

	return OK;
}

int rtc_subscribe;

int (rtc_subscribe_int)(uint8_t * bit_no) {
    *bit_no = RTC_IRQ;
    rtc_subscribe = (int) *bit_no;
    return sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &rtc_subscribe);
}

int(rtc_unsubscribe_int)() {
    return sys_irqrmpolicy(&rtc_subscribe);
}

bool alarm_wait = true;

int (rtc_int_cause)(uint8_t * cause){
    int err = sys_outb(RTC_ADDR_REG, RTC_REGISTER_C);
    if(err) return err;

    return util_sys_inb(RTC_DATA_REG, cause);
}

int (enable_pie_int)(){
    uint8_t b_config;
    rtc_read_reg(RTC_REGISTER_B, &b_config);
    return rtc_write_reg(RTC_REGISTER_B, b_config | RTC_PIE);
}

int (enable_aie_int)(){
    uint8_t b_config;
    rtc_read_reg(RTC_REGISTER_B, &b_config);
    return rtc_write_reg(RTC_REGISTER_B, b_config | RTC_AIE);
}

int (enable_uie_int)(){
    uint8_t b_config;
    rtc_read_reg(RTC_REGISTER_B, &b_config);
    return rtc_write_reg(RTC_REGISTER_B, b_config | RTC_UIE);
}

int (disable_pie_int)(){
    uint8_t b_config;
    rtc_read_reg(RTC_REGISTER_B, &b_config);
    return rtc_write_reg(RTC_REGISTER_B, b_config & ~RTC_PIE);
}

int (disable_aie_int)(){
    uint8_t b_config;
    rtc_read_reg(RTC_REGISTER_B, &b_config);
    return rtc_write_reg(RTC_REGISTER_B, b_config & ~RTC_AIE);
}

int (disable_uie_int)(){
    uint8_t b_config;
    rtc_read_reg(RTC_REGISTER_B, &b_config);
    return rtc_write_reg(RTC_REGISTER_B, b_config & ~RTC_UIE);
}

int (rtc_set_alarm)(uint8_t sec, uint8_t min, uint8_t hour){
    if(sec  != RTC_DONT_CARE) binary_to_bcd(&sec);
    if(min  != RTC_DONT_CARE) binary_to_bcd(&min);
    if(hour != RTC_DONT_CARE) binary_to_bcd(&hour);

    // ALARM EVERY MINUTE
    rtc_write_reg(RTC_ALARM_SECONDS, sec);
    rtc_write_reg(RTC_ALARM_MINUTES, min);
    return rtc_write_reg(RTC_ALARM_HOURS, hour);
}

int (init_rtc_vars)(){
    rtc_read_reg(RTC_REGISTER_A, &config_a);
    return rtc_read_reg(RTC_REGISTER_B, &config_b);
}

int (reset_rtc)(){
    rtc_write_reg(RTC_REGISTER_A, config_a);
    return rtc_write_reg(RTC_REGISTER_B, config_b);
}

void rtc_ih(void) {
    uint8_t cause;
    int err = rtc_int_cause(&cause);
    if(err) return; 

    if(cause & RTC_AF) {
        printf("Alarm");
        alarm_wait = false;
    }

    if(cause & RTC_PF) {
        printf("period");
    }

    if(cause & RTC_UF){
        rtc_test_date();
    }

}
