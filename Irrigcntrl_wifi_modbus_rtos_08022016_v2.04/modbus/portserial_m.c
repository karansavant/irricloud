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
 * File: $Id: portserial_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */

#include <stdint.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

/* Example/Board Header files */
#include "../Board.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbconfig.h"
//#include "rtdevice.h"
//#include "bsp.h"

#if MB_MASTER_RTU_ENABLED == 1
#include "mbrtu.h"
#endif
#if MB_MASTER_ASCII_ENABLED == 1
#include "mbascii.h"
#endif
#if MB_MASTER_TCP_ENABLED == 1
#include "mbtcp.h"
#endif

//#define TRANSMIT_ON_485()   GPIO_write(Board_RS485EN, Board_RS485_TX_ON)
//#define RECEIVE_ON_485()    GPIO_write(Board_RS485EN, Board_RS485_RX_ON)
#define TRANSMIT_ON_485()	GPIO_write(Board_RS485EN,(0));	//LOGIC 0---TX ON
#define RECEIVE_ON_485()	GPIO_write(Board_RS485EN,(~0));	//LOGIC 1---RX ON


#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Static variables ---------------------------------*/
//ALIGN(RT_ALIGN_SIZE)

/* software simulation serial transmit IRQ handler thread stack */
//static rt_uint8_t serial_soft_trans_irq_stack[512];
/* software simulation serial transmit IRQ handler thread */
//static struct rt_thread thread_serial_soft_trans_irq;
/* serial event */
//static struct rt_event event_serial;
//static uint8_t event_serial=0;
/* modbus master serial device */
//static rt_serial_t *serial;

//static eMBParity  eCurrParity;
//static UCHAR eCurrParity;

/* ----------------------- Defines ------------------------------------------*/
/* serial transmit event */
#define EVENT_SERIAL_TRANS_START    (1<<0)

/* ----------------------- static functions ---------------------------------*/
//static void prvvUARTTxReadyISR(void);
//void prvvUARTTxReadyISR(UART_Handle handle, void *buffer, size_t num);
//static void prvvUARTRxISR(void);
//void prvvUARTRxISR(UART_Handle handle, void *buffer, size_t num);
//NR//static rt_err_t serial_rx_ind(rt_device_t dev, rt_size_t size);
//NR//static void serial_soft_trans_irq(void* parameter);

/* ----------------------- functions ---------------------------------*/
//UCHAR CalcParity( UCHAR ucData );
//extern BOOL xMBMasterRTUTransmitFSM( void );
//extern BOOL xMBMasterRTUReceiveFSM( void );
extern void func_write(UART_Handle handle, void *buffer, size_t num);
extern void func_read(UART_Handle handle, void *buffer, size_t num);

/* ----------------------- variables ---------------------------------*/

UART_Params uartParams2;
UART_Handle uart2;

extern volatile uint8_t  ucMasterRTURcvBuf_test[50], ucMBRTURcvBuf[50];//$$NR$$//dated:6Feb16

extern volatile UCHAR tx_cnt, rx_cnt;

/* -------------------------------------------------------------------*/

void m_delay_us (unsigned char count)
{
unsigned short int i, j;

	for(i=0; i<count; i++)
	{
		for(j=0; j<1000; j++);
	}
}

/* ----------------------- Start implementation -----------------------------*/

BOOL xMBMasterPortSerialInit( uint32_t g_ui32SysClock, ULONG ulBaudRate, ULONG ucDataBits, ULONG eParity )
{
//	if (ucDataBits == 8)
//		ucDataBits = UART_CONFIG_WLEN_8;
//	else if (ucDataBits == 7)
//		ucDataBits = UART_CONFIG_WLEN_7;
//
//	// Set up the UART
//
//	// Enable the peripherals used by this example.
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
//
//	// Enable processor interrupts.
//	IntMasterEnable();
//
//	// Set GPIO D4 and D5 as UART pins.
//	GPIOPinConfigure(GPIO_PD4_U2RX);
//	GPIOPinConfigure(GPIO_PD5_U2TX);
//	GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);
//
//	//Save parity since it is generated programatically
//	eCurrParity = eParity;
//
//	// Configure the UART for 115,200, 8-N-1 operation.
////	UARTConfigSetExpClk(UART2_BASE, g_ui32SysClock, ulBaudRate,
////							(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
//	if(eParity == UART_CONFIG_PAR_NONE)
//	{
////		UARTConfigSetExpClk(UART2_BASE, g_ui32SysClock, ulBaudRate,
////								(ucDataBits | UART_CONFIG_STOP_TWO | eParity));
//		UARTConfigSetExpClk(UART2_BASE, g_ui32SysClock, ulBaudRate,
//								(ucDataBits | UART_CONFIG_STOP_ONE | eParity));
//	}
//	else
//	{
//		UARTConfigSetExpClk(UART2_BASE, g_ui32SysClock, ulBaudRate,
//								(ucDataBits | UART_CONFIG_STOP_ONE | eParity));
//	}
//
//
//	//Leave transmit enabled
//////	set_bit(txsta, TXEN);
////	TXSTAbits.TXEN = 1;     // enable transmit
//
//////	RCSTAbits.CREN=1;       // receive enabled
//
//
//	// Enable the UART interrupt.
//	IntEnable(INT_UART2);
//
//    UARTIntEnable(UART2_BASE, UART_INT_RX | UART_INT_RT);
//	UARTIntEnable(UART2_BASE, UART_INT_RX | UART_INT_TX | UART_INT_RT);
//
//	UARTFIFODisable(UART2_BASE);
//	UARTFIFOEnable(UART2_BASE);
//	UARTTxIntModeSet(UART2_BASE, UART_TXINT_MODE_FIFO);
//	UARTTxIntModeSet(UART2_BASE, UART_TXINT_MODE_EOT);

	UART_Params_init(&uartParams2);

    uartParams2.readMode       = UART_MODE_CALLBACK;              /*!< Mode for all read calls */
    uartParams2.writeMode      = UART_MODE_CALLBACK;              /*!< Mode for all write calls */
	uartParams2.readTimeout    = 10;                              /*!< Timeout for read semaphore *///$$NR$$//dated:6Feb16
	uartParams2.writeTimeout   = 10;                              /*!< Timeout for write semaphore *///$$NR$$//dated:6Feb16
	uartParams2.readCallback   = &func_read;	//&prvvUARTRxISR;		/*!< Pointer to read callback */
	uartParams2.writeCallback  = &func_write;	//&prvvUARTTxReadyISR;	/*!< Pointer to write callback */
	uartParams2.readReturnMode = UART_RETURN_FULL;             	/*!< Receive return mode */
	uartParams2.readDataMode   = UART_DATA_BINARY;                /*!< Type of data being read */
	uartParams2.writeDataMode  = UART_DATA_BINARY;                /*!< Type of data being written */
	uartParams2.readEcho       = UART_ECHO_OFF;                   /*!< Echo received data back */
	uartParams2.baudRate       = 9600;              				/*!< Baud rate for UART */
	uartParams2.dataLength     = UART_LEN_8;                      /*!< Data length for UART */
	uartParams2.stopBits       = UART_STOP_ONE;                   /*!< Stop bits for UART */
	uartParams2.parityType     = UART_PAR_NONE;

	uart2 = UART_open(Board_UART2, &uartParams2);

	if (uart2 == NULL) {
		//System_abort("Error opening the UART");
	}

	// set direction for data receive
//	GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_4, 0x00);
//	GPIO_write(Board_RS485EN,(~0));			//$$NR$$//dated:20Sep15
	vMBMasterPortSerialEnable( TRUE, FALSE);//$$NR$$//dated:6Feb16
//	RECEIVE_ON_485();						//$$NR$$//dated:20Sep15

//	rt_thread_startup(&thread_serial_soft_trans_irq);
//	rt_event_init(&event_serial, "master event", RT_IPC_FLAG_PRIO);
//	event_serial = 0;

	return TRUE;
}

void vMBMasterPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
//rt_uint32_t recved_event;
//uint8_t recved_event;

	// If xRXEnable enable serial receive interrupts. If xTxENable enable
	// transmitter empty interrupts.

    if(xRxEnable == TRUE)
    {
		/* enable RX interrupt */
//		serial->ops->control(serial, RT_DEVICE_CTRL_SET_INT, (void *)RT_DEVICE_FLAG_INT_RX);
		/* switch 485 to receive mode */
//		rt_pin_write(MODBUS_MASTER_RT_CONTROL_PIN_INDEX, PIN_LOW);

//		UARTIntEnable(UART2_BASE, UART_INT_RX);

		//485 status determined by xTxEnable state
		//Bad way to do this, but wait for TX complete
//		RECEIVE_ON_485();
//		m_delay_us(10);
    }
	else
	{
		/* switch 485 to transmit mode */
//		rt_pin_write(MODBUS_MASTER_RT_CONTROL_PIN_INDEX, PIN_HIGH);
		/* disable RX interrupt */
//		serial->ops->control(serial, RT_DEVICE_CTRL_CLR_INT, (void *)RT_DEVICE_FLAG_INT_RX);

//		UARTIntDisable(UART2_BASE, UART_INT_RX);

		//485 status determined by xTxEnable state
//		TRANSMIT_ON_485();

		//Allow time for bus to turn around.
//      delay_10us(1);
//		m_delay_us(10);
	}

	if (xTxEnable)
	{
		/* start serial transmit */
//		rt_event_send(&event_serial, EVENT_SERIAL_TRANS_START);
//		event_serial |= EVENT_SERIAL_TRANS_START;

//		UARTIntEnable(UART2_BASE, UART_INT_TX);
		TRANSMIT_ON_485();

		//Allow time for bus to turn around.
//		delay_10us(10);
//		m_delay_us(10);
	}
	else
	{
		/* stop serial transmit */
//		rt_event_recv(&event_serial, EVENT_SERIAL_TRANS_START,
//		RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, 0, &recved_event);
//		event_serial &= ~EVENT_SERIAL_TRANS_START;
//		recved_event = event_serial;

//		UARTIntDisable(UART2_BASE, UART_INT_TX);

		//Bad way to do this, but wait for TX complete
		RECEIVE_ON_485();
	}

//    IntMasterEnable( );
}


void vMBMasterPortClose(void)
{
 //   serial->parent.close(&(serial->parent));
}

//BOOL xMBMasterPortSerialPutByte(uint8_t * ucByte,  uint8_t ucByteCnt )
BOOL xMBMasterPortSerialPutByte( const uint8_t *ucByte, uint8_t ucByteCnt )
{
//	uint8_t num;

//	serial->parent.write(&(serial->parent), 0, &ucByte, 1);
//	return TRUE;

	/* Put a byte in the UARTs transmit buffer. This function is called
	* by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
	* called. */

	/* We need to generate TX parity, if enabled */
/*
	switch(eCurrParity)
	{
		case UART_CONFIG_PAR_EVEN:
//			if(CalcParity(ucByte) == 0)
//				TXSTAbits.TX9D = 0;
//			else
//				TXSTAbits.TX9D = 1;

			break;

		case UART_CONFIG_PAR_ODD:
//			if(CalcParity(ucByte) == 0 )
//				TXSTAbits.TX9D = 1;
//			else
//				TXSTAbits.TX9D = 0;

			break;
	}
*/

//	UARTCharNonBlockingPut(UART2_BASE, ucByte);
//	UARTCharPut(UART2_BASE, ucByte);
//	num = (uint8_t) UART_write(uart2, ucByte, ucByteCnt);
	UART_write(uart2, ucByte, ucByteCnt);
//	( void )UART_writePolling(uart2, ucByte, ucByteCnt);

//	tx_cnt += num;

	return TRUE;
}

BOOL xMBMasterPortSerialGetByte(uint8_t *pucByte)
{
//    serial->parent.read(&(serial->parent), 0, pucByte, 1);
//    return TRUE;

	/* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack
     */
//	*pucByte = UARTCharGetNonBlocking(UART2_BASE);
//	UART_read(uart2, (uint8_t *)pucByte, 1);

//  UART_read(uart2, pucByte, 1);   //$$NR$$//dated:6Feb16
	UART_read(uart2, pucByte, 30);  //$$NR$$//dated:6Feb16

    return TRUE;
}

#if 0

/**
 * Software simulation serial transmit IRQ handler.
 *
 * @param parameter parameter
 */
static void serial_soft_trans_irq(void* parameter) {
    rt_uint32_t recved_event;
    while (1)
    {
        /* waiting for serial transmit start */
        rt_event_recv(&event_serial, EVENT_SERIAL_TRANS_START, RT_EVENT_FLAG_OR,
                RT_WAITING_FOREVER, &recved_event);
        /* execute modbus callback */
        prvvUARTTxReadyISR();
    }
}

/**
 * This function is serial receive callback function
 *
 * @param dev the device of serial
 * @param size the data size that receive
 *
 * @return return RT_EOK
 */
static rt_err_t serial_rx_ind(rt_device_t dev, rt_size_t size) {
    prvvUARTRxISR();
    return RT_EOK;
}
#endif

#endif
