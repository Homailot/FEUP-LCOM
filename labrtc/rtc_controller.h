
/**
 * @brief Converts a bcd number to decimal
 * @param [out] Input and ouput
 */
int bcd_to_decimal(uint8_t * val);

/**
 * @brief Converts a binary number to bcd mode
 * @param [out] Input and output
 */
int binary_to_bcd(uint8_t * val);

/**
 * @brief Subscribes to the interrupts of the rtc
 * @param [out] Bit number chosen
 */
int (rtc_subscribe_int)(uint8_t * bit_no);

/**
 * @brief Unsibscribes rtc interrupts
 */
int(rtc_unsubscribe_int)();

/**
 * @brief Gets the cause of the interrupt
 * @param [out] cause Output value 
 */
int (rtc_int_cause)(uint8_t * cause);

/**
 * @brief Enables periodic interupts
 */
int (enable_pie_int)();

/**
 * @brief Enables alarm interrupts
 */
int (enable_aie_int)();

/**
 * @brief Enable update interrupts
 */
int (enable_uie_int)();

/**
 * @brief Enables periodic interupts
 */
int (disable_pie_int)();

/**
 * @brief Enables alarm interrupts
 */
int (disable_aie_int)();

/**
 * @brief Enable update interrupts
 */
int (disable_uie_int)();

/**
 * @brief Enables an alarm interrupt
 * @param sec Seconds
 * @param min Minutes
 * @param hour Hours
 */
int (rtc_set_alarm)(uint8_t sec, uint8_t min, uint8_t hour);

/**
 * @brief Initializes variables to the minix configuration to restore it later
 */
int (init_rtc_vars)();

/**
 * @brief Resets rtc to minix default
 */
int (reset_rtc)();

void rtc_ih(void);
