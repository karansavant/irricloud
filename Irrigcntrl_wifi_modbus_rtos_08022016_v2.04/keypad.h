/*
 */
/** ============================================================================
 *  @file       keypad.h
 *
 *  @brief      Keypad I2C APIs
 *
 *
 *  ============================================================================
 */

#ifndef __keypad_H
#define __keypad_H

#define	ON	1
#define	OFF	0

#define	LED_ALERTS	0
#define	LED_PROGRAM	1
#define	LED_MANUAL	2
#define	LED_POWER	3

extern void Board_initkeypad (void);
extern void set_write_mode(void);
extern void set_read_mode(void);
extern void keypad_read_status(void);
extern void led_control(uint8_t led_no, uint8_t led_state);


#endif /* __keypad_H */
