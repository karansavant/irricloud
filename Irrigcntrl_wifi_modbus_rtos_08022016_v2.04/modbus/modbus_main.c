/*
 * FreeModbus Libary: BARE Demo Application
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * File: $Id: demo.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */
//*****************************************************************************
//
// demo.c - Example for reading data from and writing data to the UART in
//          an interrupt driven fashion.
//
// This is part of revision 2.1.0.12573 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************
//
//! The first UART will be configured in 115,200 baud, 8-n-1 mode.
//
//*****************************************************************************

/*
Use RTU protocol, 9600 as default Baud rate,8 bit. no parity, one stop bit

Following parameters need to be kept programmable in respective MODBUS register (suggested register map):

Input Register (Read Only) 30001 onwards

30001		P1
30002		P2
30003		dP ( P1-P2 with sign)
30004		Temp
30005		RH
30006		Error register Bit wise errors to be defined, for ADC OR/UR/Non response / P1 OR /  P2 OR/

Holding Registers (Read/Write) 40001 onwards

40001		Device ID
40002		Baud Rate
40003		Pressure 1 Zero ADC count
40004		Pressure 1 FS ADC count corresponding to FS engg Value
40005		Pressure 1 FS the engg Value
40006		Pressure 2 Zero ADC count
40007		Pressure 2 FS ADC count corresponding to FS engg Value
40008		Pressure2 FS the engg Value
40009		Limit Value for dP
40010		Damping in second

Input Status (Read Only) 10001 onwards

10001		RAIN status
10002		error
10003		Limit status on dP
10004

*/

/* ----------------------- System includes ----------------------------------*/

/*
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "inc/hw_types.h"

#include "driverlib/debug.h"

#include "driverlib/fpu.h"

#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include <ti/drivers/UART.h>
//#include "utils/uartstdio.h"
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


/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"
#include "mbconfig.h"

#include "mbframe.h"	//NR//

/* ----------------------- Defines ------------------------------------------*/

// Timer Defines

#define TIMER1_CLOCKS_PER_US	((_XTAL_FREQ*4.8)/1000000)
#define TIMER1_1MS_LOAD 		(TIMER1_CLOCKS_PER_US * 1000)

#define MSB(x)		(((x) >> 8) & 0xFF)
#define LSB(x)		((x) & 0xFF)

/* ----------------------- Global functions ---------------------------------*/

void func_write(UART_Handle handle, void *buffer, size_t num);
void func_read(UART_Handle handle, void *buffer, size_t num);

extern BOOL xMBMasterRTUTransmitFSM( void );
extern BOOL xMBMasterRTUReceiveFSM( void );
extern BOOL xMBMasterRTUTimerExpired( void );

//extern void eMBMasterPoll();

/* ----------------------- Static variables ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/

//*****************************************************************************
// Flags that contain the current value of the interrupt indicator as displayed
// on the UART.
//*****************************************************************************
uint32_t g_ui32Flags;

//*****************************************************************************
// for testing purpose
extern UART_Handle uart2;

extern volatile uint8_t  ucMasterRTURcvBuf_test[50], ucMBRTURcvBuf[50];//$$NR$$//dated:6Feb16

//extern volatile uint16_t Mod_Req_Timecnt;
volatile UCHAR test_point = 0, tx_cnt = 0, rx_cnt = 0;

//*****************************************************************************
// The error routine that is called if the driver library encounters an error.
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
// Receive / Send a string to the UART.
//*****************************************************************************
//UART_read() function calling is not allowed in below UART Receive Callback function

void func_read(UART_Handle handle, void *buffer, size_t num)
{
	xMBMasterRTUReceiveFSM();

//	test_point = 1;
}

void func_write(UART_Handle handle, void *buffer, size_t num)
{
	tx_cnt += num;

    xMBMasterRTUTransmitFSM();

    test_point = 2;
}

//*****************************************************************************
// The interrupt handler for the first timer interrupt.
//*****************************************************************************
void periodic_1msec()
{
	if ((mbtimer_state) && (mbtimer_tick_cnt))
	{
		mbtimer_tick_cnt--;

		if (mbtimer_tick_cnt == 0)
		{
			xMBMasterRTUTimerExpired();
//		    UART_readCancel(uart2);         //$$NR$$//dated:7Feb16

//			Timer_setPeriodMicroSecs(ti_sysbios_family_arm_lm4_Timer0, wTimerVal); /* change 5s to 1s */
			mbtimer_tick_cnt = wTimerVal;
		}
		else if (mbtimer_tick_cnt == 10)
        {
            UART_readCancel(uart2);         //$$NR$$//dated:7Feb16
        }
		else if ((mbtimer_tick_cnt == (MB_MASTER_TIMEOUT_MS_RESPOND - 10)) && (wTimerVal == MB_MASTER_TIMEOUT_MS_RESPOND))
		{
			// set direction for data receive
//			GPIO_write(Board_RS485EN,(~0));			//$$NR$$//dated:20Sep15
//			RECEIVE_ON_485();						//$$NR$$//dated:20Sep15
			vMBMasterPortSerialEnable( TRUE, FALSE );//$$NR$$//dated:20Sep15
		}
	}
}

#if 0
void Timer0IntHandler()
{
	xMBMasterRTUTimerExpired();

//	//	Clear the timer interrupt.
//	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

//	// Update the interrupt status.
//	IntMasterDisable();

	/* Turn on user LED */
	GPIO_write(Board_LED0, Board_LED_OFF);

//	// Enable the timer with the timeout passed to xMBPortTimersInit( )
//	TimerLoadSet(TIMER0_BASE, TIMER_A, wTimerVal);

	Timer_setPeriodMicroSecs(ti_sysbios_family_arm_lm4_Timer0, wTimerVal); /* change 5s to 1s */

//	IntMasterEnable();

	test_point = 3;
}
#endif

//*************************************************************************************
//
//*************************************************************************************
//$$NR$$//dated:07Feb16

typedef struct Menu_Tree_Struct
{
    uint8_t
        slaveaddr;
    uint16_t
        usModbusUserData[10];
}sensor_data;

sensor_data   sensor_update;

BOOL sensor_config_query_flag = false;

void SysMonitor(void)
{
eMBMasterReqErrCode    errorCode = MB_MRE_NO_ERR;
uint16_t errorCount = 0;
static uint8_t cur_slaveaddr = 0, request_type = 4, index;

    for(index=0; index<30; index++)
        ucMasterRTURcvBuf_test[index]=0;

    if (sensor_config_query_flag)   // Write/Read holding register data
    {
        if (sensor_update.slaveaddr == 1)     // Temp_Humidity_Rain Sensor board
        {
//        errorCode = eMBMasterReqWriteHoldingRegister(sensor_update.slaveaddr,3,sensor_update.usModbusUserData[0],-1);
//        errorCode = eMBMasterReqReadHoldingRegister( sensor_update.slaveaddr, 0, 2, -1 );
        }
        else if (sensor_update.slaveaddr == 2)// Pressure Sensor board
        {
//        errorCode = eMBMasterReqWriteHoldingRegister(sensor_update.slaveaddr,3,sensor_update.usModbusUserData[0],-1);
//        errorCode = eMBMasterReqReadHoldingRegister(sensor_update.slaveaddr, 0, 10, -1 );
        }
    }
    else                            // Read Discrete input / input register data
    {
        cur_slaveaddr++;
        if (cur_slaveaddr >= 4)
        {
            cur_slaveaddr = 1;

            if (request_type == 2)
                request_type = 4;       // send request for read input registers
            else if (request_type == 4)
                request_type = 2;      // send request for read discrete input
        }

        if (request_type == 2)          // send request for read discrete input
        {
            if (cur_slaveaddr == 1)     // Temp_Humidity_Rain Sensor board
            {
                // read parameters, RAIN status & error
                errorCode = eMBMasterReqReadDiscreteInputs(cur_slaveaddr,0,2,-1);//NR//RT_WAITING_FOREVER);
            }
            else if (cur_slaveaddr == 2)// Pressure Sensor board
            {
                // read parameters, error & Limit status on dP
                errorCode = eMBMasterReqReadDiscreteInputs(cur_slaveaddr,1,2,-1);//NR//RT_WAITING_FOREVER);
            }
        }
        else if (request_type == 4)     // send request for read input registers
        {
            if (cur_slaveaddr == 1)     // Temp_Humidity_Rain Sensor board
            {
                // read parameters, Temp, RH & "Error register Bit wise errors to be defined, for ADC OR/UR/Non response / P1 OR /  P2 OR/"
//                errorCode = eMBMasterReqReadInputRegister(cur_slaveaddr,3,3,-1);//NR//RT_WAITING_FOREVER);
                errorCode = eMBMasterReqReadInputRegister(cur_slaveaddr,0,6,-1);//NR//RT_WAITING_FOREVER);
            }
            else if (cur_slaveaddr == 2)// Pressure Sensor board
            {
                // read parameters, P1, P2, dP ( P1-P2 with sign), Temp, RH & "Error register Bit wise errors to be defined, for ADC OR/UR/Non response / P1 OR /  P2 OR/"
                errorCode = eMBMasterReqReadInputRegister(cur_slaveaddr,0,6,-1);//NR//RT_WAITING_FOREVER);
            }
        }
    }

	if (errorCode != MB_MRE_NO_ERR) {
		errorCount++;
	}
}
//$$NR$$//dated:07Feb16

//*************************************************************************************
