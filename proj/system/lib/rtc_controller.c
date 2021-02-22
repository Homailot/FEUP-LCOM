// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "rtc_controller.h"
#include "rtc.h"
#include "../game/game_manager.h"

static date_t curr_date;

static uint8_t config_a;
static uint8_t config_b;

/** @brief Variable that indicates how many periodic interrupts it received **/
static uint8_t pie_state = 0;

void (reset_pie_state)(){
    pie_state = 0;
}

date_t * get_curr_date(){
    return &curr_date;
}

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

int rtc_set_date(date_t * date) {

    memset(date,0,sizeof(date_t));

	uint8_t config = 0;

	rtc_read_reg(RTC_REGISTER_A, &config);
	

	// About to change date
	if(config&RTC_UIP)
		return 1;

    // Disabling interrupts
    reset_rtc();

	rtc_read_reg(RTC_SECONDS,&date->sec);
	rtc_read_reg(RTC_MINUTES,&date->min);
	rtc_read_reg(RTC_HOURS,&date->hour);
	rtc_read_reg(RTC_MONTH_DAY,&date->day);
	rtc_read_reg(RTC_MONTH,&date->month);
	rtc_read_reg(RTC_YEAR,&date->year);

    // Reenabling the interrupts previously deactivated
    rtc_write_reg(RTC_REGISTER_A, config);

	bcd_to_decimal(&date->sec);
	bcd_to_decimal(&date->min);
	bcd_to_decimal(&date->hour);
	bcd_to_decimal(&date->day);
	bcd_to_decimal(&date->month);
	bcd_to_decimal(&date->year);

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

int (rtc_set_periodic_500ms)(){
    uint8_t config;
    rtc_read_reg(RTC_REGISTER_A, &config);
    config |= RTC_RS0 | RTC_RS1 | RTC_RS2 | RTC_RS3;
    return rtc_write_reg(RTC_REGISTER_A, config);
}

int (rtc_reset_pie)(){
    uint8_t config;
    rtc_read_reg(RTC_REGISTER_A, &config);
    config &= RTC_RS_CLEAR;
    return rtc_write_reg(RTC_REGISTER_A, config);
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
    return rtc_write_reg(RTC_REGISTER_A, config_a);
    //return rtc_write_reg(RTC_REGISTER_B, config_b);
}

void rtc_ih(void) {
    uint8_t cause;
    int err = rtc_int_cause(&cause);
    if(err) return; 

    if(cause & RTC_AF) {
    }

    if(cause & RTC_PF) {
        if(pie_state == 1){
            int * ptr = get_round_timer();
            (*ptr)--;
            pie_state = 0;
        }
        else
            pie_state++;
    }

    if(cause & RTC_UF){
        rtc_set_date(get_curr_date());
    }

}
