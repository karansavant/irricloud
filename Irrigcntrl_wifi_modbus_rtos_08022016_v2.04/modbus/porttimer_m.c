/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: porttimer_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
/*
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "utils/uartstdio.h"
*/

#include <stdint.h>
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
//#include <xdc/runtime/Error.h>
//#include <inc/hw_memmap.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
//#include <ti/sysbios/knl/Task.h>
//#include <ti/sysbios/family/arm/m3/Hwi.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
//#include "driverlib/UART.h"
//#include <ti/drivers/I2C.h>
//#include <ti/drivers/Watchdog.h>

/* Example/Board Header files */
#include "../Board.h"

#include "mb.h"
#include "mb_m.h"
#include "mbport.h"
#include "mbconfig.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0

#define MBTIMER_START	TRUE
#define MBTIMER_STOP	FALSE

/* ----------------------- Variables ----------------------------------------*/
//static USHORT usT35TimeOut50us;
//static struct rt_timer timer;
volatile Bool mbtimer_state = MBTIMER_STOP;
volatile uint16_t mbtimer_tick_cnt = 0;
uint16_t wTimerVal = 0;
//uint32_t timer_tick_T35=0, timer_tick_delay=0, timer_tick_timeout=0;

/* ----------------------- static functions ---------------------------------*/
//static void prvvTIMERExpiredISR(void);
//static void timer_timeout_ind(void* parameter);

/* ----------------------- Start implementation -----------------------------*/
//BOOL xMBMasterPortTimersInit(USHORT usTimeOut50us)
BOOL xMBMasterPortTimersInit( uint32_t g_ui32SysClock )
{
#if 0
#if MB_TIMER == 0
/*
    // backup T35 ticks
    usT35TimeOut50us = usTimeOut50us;

    rt_timer_init(&timer, "master timer",
                   timer_timeout_ind, // bind timeout callback function
                   RT_NULL,
                   (50 * usT35TimeOut50us) / (1000 * 1000 / RT_TICK_PER_SECOND),
                   RT_TIMER_FLAG_ONE_SHOT); // one shot
*/
	// Enable the peripherals used by this example.
	 SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	// Enable processor interrupts.
	 IntMasterEnable();

	// timer count = (timerPeriod * Fosc)
//	timer_tick_T35 = ((4.01 / 1000) * g_ui32SysClock);			// timerPeriod = 0.00401s (4.01msec) & Fosc = SysClock
	timer_tick_T35 = (0.00401 * g_ui32SysClock);

//	timer_tick_delay = (MB_MASTER_DELAY_MS_CONVERT * g_ui32SysClock / 1000);
	timer_tick_delay = (((float)MB_MASTER_DELAY_MS_CONVERT / 1000) * g_ui32SysClock);
//	timer_tick_delay = (0.2 * g_ui32SysClock);

//	timer_tick_timeout = (MB_MASTER_TIMEOUT_MS_RESPOND * g_ui32SysClock / 1000);
	timer_tick_timeout = (((float)MB_MASTER_TIMEOUT_MS_RESPOND / 1000) * g_ui32SysClock);
//	timer_tick_delay = (0.1 * g_ui32SysClock);

#endif
#endif

    return TRUE;
}

void vMBMasterPortTimersT35Enable()
{
	// timer count = (timerPeriod * Fosc)
//	wTimerVal = (0.00401 * g_ui32SysClock);			// timerPeriod = 0.00401s (4.01msec) & Fosc = SysClock
	wTimerVal = 4;	//timer_tick_T35;

    // Set current timer mode, don't change it.
    vMBMasterSetCurTimerMode(MB_TMODE_T35);

	// Configure the two 32-bit periodic timers.
//	 TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
//	 TimerLoadSet(TIMER0_BASE, TIMER_A, wTimerVal);		//g_ui32SysClock
//	Timer_setPeriodMicroSecs(ti_sysbios_family_arm_lm4_Timer0, wTimerVal);
	mbtimer_tick_cnt = wTimerVal;

	// Setup the interrupts for the timer timeouts.
//	 IntEnable(INT_TIMER0A);
//	 TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	// Enable the timers.
//	 TimerEnable(TIMER0_BASE, TIMER_A);
//	Timer_start(ti_sysbios_family_arm_lm4_Timer0);
	mbtimer_state = MBTIMER_START;

}

void vMBMasterPortTimersConvertDelayEnable()
{
	// Enable the peripherals used by this example.
//	 SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	// Enable processor interrupts.
//	 IntMasterEnable();

	// timer count = (timerPeriod * Fosc)
//	wTimerVal = (MB_MASTER_DELAY_MS_CONVERT * g_ui32SysClock / 1000);
	wTimerVal = MB_MASTER_DELAY_MS_CONVERT;		//timer_tick_delay;

	// Set current timer mode, don't change it.
    vMBMasterSetCurTimerMode(MB_TMODE_CONVERT_DELAY);

	// Configure the two 32-bit periodic timers.
//	 TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
//	 TimerLoadSet(TIMER0_BASE, TIMER_A, wTimerVal);		//g_ui32SysClock
//	Timer_setPeriodMicroSecs(ti_sysbios_family_arm_lm4_Timer0, wTimerVal);
	mbtimer_tick_cnt = wTimerVal;

	// Setup the interrupts for the timer timeouts.
//	 IntEnable(INT_TIMER0A);
//	 TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	// Enable the timers.
//	 TimerEnable(TIMER0_BASE, TIMER_A);
//	Timer_start(ti_sysbios_family_arm_lm4_Timer0);
	mbtimer_state = MBTIMER_START;

}

void vMBMasterPortTimersRespondTimeoutEnable()
{
	// timer count = (timerPeriod * Fosc)
//	wTimerVal = (MB_MASTER_TIMEOUT_MS_RESPOND * g_ui32SysClock / 1000);
	wTimerVal = MB_MASTER_TIMEOUT_MS_RESPOND;	//timer_tick_timeout;

	// Set current timer mode, don't change it.
    vMBMasterSetCurTimerMode(MB_TMODE_CONVERT_DELAY);

	// Configure the two 32-bit periodic timers.
//	 TimerLoadSet(TIMER0_BASE, TIMER_A, wTimerVal);		//g_ui32SysClock
//	Timer_setPeriodMicroSecs(ti_sysbios_family_arm_lm4_Timer0, wTimerVal);
	mbtimer_tick_cnt = wTimerVal;

	// Enable the timers.
//	 TimerEnable(TIMER0_BASE, TIMER_A);
//	Timer_start(ti_sysbios_family_arm_lm4_Timer0);
	mbtimer_state = MBTIMER_START;

}

void vMBMasterPortTimersDisable()
{
//	rt_timer_stop(&timer);
    /* Disable any pending timers. */
//	 TimerDisable(TIMER0_BASE, TIMER_A);
//	Timer_stop(ti_sysbios_family_arm_lm4_Timer0);
	mbtimer_state = MBTIMER_STOP;
}

#if 0
void prvvTIMERExpiredISR(void)
{
//	(void) pxMBMasterPortCBTimerExpired();
	(void) xMBMasterRTUTimerExpired();
}

static void timer_timeout_ind(void* parameter)
{
    prvvTIMERExpiredISR();
}
#endif

#endif
