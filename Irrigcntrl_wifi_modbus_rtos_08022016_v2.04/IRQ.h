
/******************************************************************************/
/* IRQ.h Interrupts function prototypes                                       */
/******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IRQ_H
#define __IRQ_H

/******************************************************************************/

/******************************************************************************/

//extern static void delay (uint32_t cnt);
//extern void delay_usec(uint32_t delay);
extern void delay_msec(uint16_t delay);
extern void delay_sec(uint16_t delay);

extern void Beep_Buzzer (uint16_t interval);
extern void Start_Buzzer(uint8_t SpeedMode);
extern void Stop_Buzzer(void);

#endif
/******************************************************************************/

