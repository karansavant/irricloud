/******************************************************************************/
/* IRQ.C: IRQ Handler                                                         */
/******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include "inc/tm4c1294ncpdt.h"
#include <ti/sysbios/knl/Task.h>	//$$NR$$//dated:01Nov15

#include <ti/drivers/GPIO.h>
//#include "inc/hw_nvic.h"
//#include "inc/hw_types.h"
//#include "driverlib/debug.h"
//#include "driverlib/interrupt.h"
//#include "driverlib/systick.h"

//#include "GlobalConst.h"               	/* Global Constants prototypes     	*/
//#include "keycodes.h"						/* Key codes Constants           	  */

/******************************************************************************/
// Constants used for Local purpose //

//#define	SYSTICK_RELOAD_COUNT		120000		// 1msec SysTick count for 120 MHz system clock.

/******************************************************************************/
// Functions used for Global purpose //

//extern void Cursor_On(void);
//extern void Cursor_Off(void);

/********************************************************/
// Functions used for Local purpose //

//void BuzzerUpdate (void);
//static void delay (uint32_t cnt);

/********************************************************/
// Variables used for Global purpose //

// Variables used for diffrent Timeout (1 msec)
//extern volatile bool
//	cursor,								// defined in lcd.c
//	CursorFlag;							// defined in lcd.c



// Variables used for diffrent Timeout (10 msec)

//extern volatile uint8_t
//	access_level,							// defined in main.c
//	access_level_timeout;						// defined in main.c

// Variables used for LEDs scanning


// Variables used for Keyboard scanning

// Variables used for Keyboard Char Editing
extern volatile uint8_t	key_press_detected;
volatile uint8_t
	key_press_time_count = 0,
	activity_timecnt_gsm_task = 0,			//$$NR$$//dated:23Aug15
	activity_timecnt_gui_task = 0,			//$$NR$$//dated:23Aug15
	activity_timecnt_program_task = 0;		//$$NR$$//dated:23Aug15

volatile uint16_t	typingdelay_count = 0;
volatile bool		typingdelay_elapsed_flag = false;	//$$NR$$//dated:19Sep15

extern int flag_30s;//for GSM Communication
extern int flag1_30s;
extern int backlight_count;
extern int pumpdelay_count;
//volatile bool flashflag = 0;						//variable for LEDs

/********************************************************/
// Variables used for Local purpose //
/******************************************************************************/
// Constants used for Local purpose //

//#define LEDBLINK_RATE 500

#define BZ_SLOW_SPEED 1
#define BZ_FAST_SPEED 2

#define BZON		400
#define BZOFF_SLOW	5000
#define BZOFF_FAST	1000

/******************************************************************************/

#define BUZZER_CNTRL	(1U << 1)	// PF1

// Deselect Buzzer: Buzzer pin low
#define BUZZER_OFF()		GPIO_PORTF_AHB_DATA_BITS_R[BUZZER_CNTRL] &= ~BUZZER_CNTRL;
// Select Buzzer: Buzzer pin high
#define BUZZER_ON()			GPIO_PORTF_AHB_DATA_BITS_R[BUZZER_CNTRL] |= BUZZER_CNTRL;

/********************************************************/
// Functions used for Local purpose //

void BuzzerUpdate (void);
//static void delay (uint32_t cnt);

/********************************************************/
uint8_t
	BuzzerStatus = 0;
// Variables used for Systick (1 msec)
volatile uint16_t ticks_10ms = 0;
volatile uint16_t ticks_500ms = 5, ticks_1000ms = 10;	//$$NR$$//dated:21Aug15
volatile uint16_t sec_ticks = 0;

volatile uint8_t Mod_Req_Timecnt = 30;		//$$NR$$//dated:19Sep15

// Variables used for Keyboard scanning
volatile uint8_t
	bz_click = 0;

volatile bool
	rtc_update = false,
	cursorflag = false;//$$NR$$//dated:21Aug15

volatile uint8_t
	bounceflag = 0;

// Variables used for Buzzer Control
volatile uint16_t
	bzcnt;

volatile uint8_t
	bactive,
	bstat,
	bfspeed = 0;		// slow = 1 / fast = 2

// Variables used for diffrent Timeout (1 sec)
volatile uint16_t
	BLitON_TimeCnt = 0,
	AccessMode_TimeCnt = 0,
	EvtScreen_TimeCnt = 0;

volatile uint8_t
	Delay_Counter_100ms = 0;

int bf_10msec=0;
int bf_1sec=0;
int bf_1min=0;

extern int mainpump_onoff;
/********************************************************/
// Testing purpose


//*****************************************************************************
//
//! Gets the current value of the SysTick counter.
//!
//! This function returns the current value of the SysTick counter, which is
//! a value between the period - 1 and zero, inclusive.
//!
//! \return Returns the current value of the SysTick counter.
//
//*****************************************************************************
/*
void SysTick_Init (void)
{
	// Disable SysTick during setup
    HWREG(NVIC_ST_CTRL) = 0;

    // Configure the SysTick counter (1msec) for 120 MHz system clock.
    SysTickPeriodSet(SYSTICK_RELOAD_COUNT);

    HWREG(NVIC_ST_CURRENT) = 0;				// Any write to current clears it

    // Enable the SysTick counter
    SysTickEnable();

    // Configure the SysTick interrupt
    SysTickIntEnable();
}
*/

/********************************************************************************/
// configured for 1 msec
/********************************************************************************/
//void periodic_isr()
//{
//
//	Delay_Counter_100ms ++;
//	if(Delay_Counter_100ms>=250)	Delay_Counter_100ms=1;
//	pumpdelay_count++;//100ms
//	if(pumpdelay_count>=60000)	pumpdelay_count=1;
//
//	if (key_press_time_count)
//	{
//		// Initiate a buzzer click.
//		if (key_press_time_count == 20)
//		{
//			bz_click = 5;//85;
//			BuzzerStatus = 1;
//
//		}
//
//		key_press_time_count--;
//	}
//
//
//	ticks_500ms--;
//	if (ticks_500ms == 0)
//	{
//		ticks_500ms = 5;
//		rtc_update = true;
//	}
//
//	// Check counter of char typing delay
//	if ((typingdelay_count) && (key_press_detected != 0x01))
//	{
//		typingdelay_count--;
//		if (typingdelay_count == 0)	typingdelay_elapsed_flag = 1;
//	}
//
//	flag_30s++;
//	if(flag_30s>=31000)	flag_30s=1;
//
//	backlight_count++;
//	if(backlight_count>=31000)	backlight_count=1;
//
//	// Update buzzer status
//	//BuzzerUpdate();
//}
void periodic_isr()
{
static uint8_t ticks_100ms = 10;

	if (ticks_10ms) ticks_10ms--;		// Check counter of general purpose delay in msecs

	ticks_100ms --;					// delay counter for 10 msec
	if (ticks_100ms == 0)
	{
		ticks_100ms = 10;
		Delay_Counter_100ms ++;
		if(Delay_Counter_100ms>=250)	Delay_Counter_100ms=1;
		pumpdelay_count++;//100ms
		if(pumpdelay_count>=60000)	pumpdelay_count=1;
		//if (key_press_time_count)	key_press_time_count--;

		//time count for long key press (lower to upper case convertion)
		if (key_press_time_count)
		{
			// Initiate a buzzer click.
			if (key_press_time_count == 10)		//$$NR$$//dated:23Aug15
			{
				bz_click = 5;//85;
				BuzzerStatus = 1;

			}

			key_press_time_count--;
		}

		if (Mod_Req_Timecnt)	Mod_Req_Timecnt--;	//$$NR$$//dated:19Sep15
	}

	ticks_500ms--;
	if (ticks_500ms == 0)
	{
		ticks_500ms = 50;
		rtc_update = true;
	}

	//$$NR$$//dated:21Aug15
	ticks_1000ms--;
	if (ticks_1000ms == 0)
	{
		ticks_1000ms = 100;
		cursorflag = true;
	}
	//$$NR$$//dated:21Aug15

	// Check counter of char typing delay
	if ((typingdelay_count) && (key_press_detected != 0x01))
	{
		typingdelay_count--;
		if (typingdelay_count == 0)	typingdelay_elapsed_flag = true;	//$$NR$$//dated:19Sep15
	}

	flag_30s++;
	if(flag_30s>=31000)	flag_30s=1;

	flag1_30s++;
	if(flag1_30s>=31000)	flag1_30s=200;

	backlight_count++;
	if(backlight_count>=31000)	backlight_count=1;

	if(mainpump_onoff == 1)//if the main pump is on
	{
		//Backflush Timer
		bf_10msec++;
		if(bf_10msec>=100)
		{
			bf_10msec = 0;
			bf_1sec++;
			if(bf_1sec>=60)
			{
				bf_1sec=0;
				bf_1min++;
				if(bf_1min >= 1440) 	bf_1min=0;
			}
		}
	}

	// Update buzzer status
	BuzzerUpdate();
}

/********************************************************************************/
/* 							Miscellaneous Functions								*/
/********************************************************************************/
// Delay in while loop cycles
/********************************************************/
//   Parameter:    cnt:    number of while cycles to delay
//   Return:
/********************************************************/
/*
static void delay (uint32_t cnt)
{
  while (cnt--);
}
*/
//*****************************************************************************
//
// Time delay using busy wait.
// The delay parameter is in units of the core clock. (units of 8.333 nsec for 120 MHz clock)
//
//*****************************************************************************
//
//void delay_usec(uint32_t delay)
//{
//volatile uint32_t elapsedTime;
//uint32_t startTime = HWREG(NVIC_ST_CURRENT);
//
//	delay *= 120;
//	do{
//		elapsedTime = (startTime - HWREG(NVIC_ST_CURRENT)) & SYSTICK_RELOAD_COUNT;
//	}
//	while(elapsedTime <= delay);
//}

/********************************************************/
// Delay of j msec
/********************************************************/

void delay_msec(uint16_t delay)
{
	ticks_10ms = delay;

	while (ticks_10ms != 0)
	{
//		if ((ticks_10ms % 100) == 0)
//	    		// take action after every 100msec
	}
}

/********************************************************/
// Delay of j msec
/********************************************************/

void delay_sec(uint16_t delay)
{
volatile uint16_t sec_ticks = 0;

	sec_ticks = delay;
	Delay_Counter_100ms = 0;
	while (sec_ticks != 0)
	{
		if (Delay_Counter_100ms >= 10)
		{
			Delay_Counter_100ms = 0;
			sec_ticks--;
		}

//		if ((sec_ticks % 2) == 0)
//	    		// take action after every 2 sec
	}
}



/********************************************************/
// Update buzzer status
/********************************************************

volatile uint16_t
	bzcnt;

volatile uint8_t
	bactive,
	bstat,
	bfspeed = 0;		// slow = 1 / fast = 2

********************************************************/
extern uint8_t test_in_progress;	//$$NR$$//dated:07Nov15

void BuzzerUpdate(void)
{
	//$$NR$$//dated:07Nov15
	// check if buzzer test in progress
	if (test_in_progress == 3)	return;
	//$$NR$$//dated:07Nov15


	if (BuzzerStatus == 0)
	{
		BUZZER_OFF();
		return;
	}

	// Check if counter used for Keypress reset, make buzzer off
	if (bz_click)
	{
		bz_click--;

		BUZZER_ON();

		if (bz_click == 0)
		{
			BUZZER_OFF();
			BuzzerStatus =0;
		}
	}

	if (bactive)
	{
		bzcnt--;
		if (bzcnt)	return;

		if (bstat)			// If bzr is to be on ...
		{
			bstat = 0;
			BUZZER_OFF();

			if (bfspeed == BZ_SLOW_SPEED)				// slow beeping
				bzcnt = BZOFF_SLOW;
			else if (bfspeed == BZ_FAST_SPEED)			// fast beeping
				bzcnt = BZOFF_FAST;
		}
		else				// and if it is to be off ...
		{
			bstat = 1;
			BUZZER_ON();

			bzcnt = BZON;
		}
	}
}

/********************************************************/
// Functions used for buzzer control
/********************************************************/

void Beep_Buzzer (int16_t interval)
{
//	if (BuzzerStatus == 0)		return; //$$NR$$//dated:07Feb16

	bactive = 0;	bstat = 0;
	bfspeed = 0;

	BUZZER_ON();
//	DelayMs (interval);
//	delay_msec (interval);
	Task_sleep(interval * 1000);	//$$NR$$//dated:01Nov15
	BUZZER_OFF();
}

/********************************************************/
// Function used for starting Buzzer in Slow / Fast Mode
/********************************************************/

//void Start_Buzzer(uint8_t control)
void Start_Buzzer(uint8_t SpeedMode)
{
	if (BuzzerStatus == 0)		return;

	bactive = 1;	bstat = 1;
	bfspeed = SpeedMode;

	bzcnt = BZON;
	BUZZER_ON();
}

/********************************************************/
// Function used for stopping Buzzer indicating
// buzzer is silenced
/********************************************************/

//void Stop_Buzzer(uint8_t control)
void Stop_Buzzer(void)
{
	bactive = 0;	bstat = 0;
	bfspeed = 0;

	BUZZER_OFF();
}

/******************************************************************************/
