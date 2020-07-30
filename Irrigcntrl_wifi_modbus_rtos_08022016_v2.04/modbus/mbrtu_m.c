/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2013 China Beijing Armink <armink.ztl@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbrtu_m.c,v 1.60 2013/08/17 11:42:56 Armink Add Master Functions $
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include <stdint.h>

#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbrtu.h"
#include "mbframe.h"

#include "mbcrc.h"
#include "mbport.h"

#if MB_MASTER_RTU_ENABLED > 0
/* ----------------------- Defines ------------------------------------------*/
#define MB_SER_PDU_SIZE_MIN     4       /*!< Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_MAX     256     /*!< Maximum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_CRC     2       /*!< Size of CRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF     0       /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF      1       /*!< Offset of Modbus-PDU in Ser-PDU. */

/* ----------------------- Type definitions ---------------------------------*/
typedef enum
{
    STATE_M_RX_INIT,              /*!< Receiver is in initial state. */
    STATE_M_RX_IDLE,              /*!< Receiver is in idle state. */
    STATE_M_RX_RCV,               /*!< Frame is beeing received. */
    STATE_M_RX_ERROR,              /*!< If the frame is invalid. */
} eMBMasterRcvState;

typedef enum
{
    STATE_M_TX_IDLE,              /*!< Transmitter is in idle state. */
    STATE_M_TX_XMIT,              /*!< Transmitter is in transfer state. */
    STATE_M_TX_XFWR,              /*!< Transmitter is in transfer finish and wait receive state. */
} eMBMasterSndState;

/* ----------------------- Static variables ---------------------------------*/
static volatile eMBMasterSndState eSndState;
static volatile eMBMasterRcvState eRcvState;

static volatile UCHAR  ucMasterRTUSndBuf[MB_PDU_SIZE_MAX];
static volatile UCHAR  ucMasterRTURcvBuf[MB_SER_PDU_SIZE_MAX];
static volatile USHORT usMasterSendPDULength;

static volatile UCHAR *pucMasterSndBufferCur;
static volatile UCHAR usMasterSndBufferCount, usMasterSndBufferIndex;

//static volatile UCHAR usMasterRcvBufferPos, usMasterRcvBufferIndex;
static volatile UCHAR usMasterRcvBufferIndex;
static volatile BOOL   xFrameIsBroadcast = FALSE;

static volatile eMBMasterTimerMode eMasterCurTimerMode;

//testing purpose
#include <ti/drivers/UART.h>
extern UART_Handle uart2;

volatile uint8_t  ucMasterRTURcvBuf_test[50], ucMBRTURcvBuf[50];//$$NR$$//dated:6Feb16
volatile uint8_t  RTURcvBufIndex = 0;

extern volatile UCHAR tx_cnt, rx_cnt;

/* ----------------------- Start implementation -----------------------------*/

eMBErrorCode eMBMasterRTUInit( uint32_t g_ui32SysClock, ULONG ulBaudRate, ULONG eParity )
{
    eMBErrorCode    eStatus = MB_ENOERR;
//    ULONG           usTimerT35_50us;

    //ENTER_CRITICAL_SECTION(  );

    /* Modbus RTU uses 8 Databits. */
	if( xMBMasterPortSerialInit( g_ui32SysClock, ulBaudRate, 8, eParity ) != TRUE )
    {
        eStatus = MB_EPORTERR;
    }
    else
    {
        /* If baudrate > 19200 then we should use the fixed timer values
         * t35 = 1750us. Otherwise t35 must be 3.5 times the character time.
         */
        if( ulBaudRate > 19200 )
        {
//            usTimerT35_50us = 35;       /* 1800us. */
        }
        else
        {
            /* The timer reload value for a character is given by:
             *
             * ChTimeValue = Ticks_per_1s / ( Baudrate / 11 )
             *             = 11 * Ticks_per_1s / Baudrate
             *             = 220000 / Baudrate
             * The reload for t3.5 is 1.5 times this value and similary
             * for t3.5.
             */
//			usTimerT35_50us = ( 7UL * 220000UL ) / ( 2UL * ulBaudRate );
        }
//		if( xMBMasterPortTimersInit( ( USHORT ) usTimerT35_50us ) != TRUE )
		if( xMBMasterPortTimersInit(g_ui32SysClock) != TRUE )
        {
            eStatus = MB_EPORTERR;
        }
    }
   // EXIT_CRITICAL_SECTION(  );

    return eStatus;
}

void
eMBMasterRTUStart( void )
{
    //ENTER_CRITICAL_SECTION(  );
    /* Initially the receiver is in the state STATE_M_RX_INIT. we start
     * the timer and if no character is received within t3.5 we change
     * to STATE_M_RX_IDLE. This makes sure that we delay startup of the
     * modbus protocol stack until the bus is free.
     */
    eRcvState = STATE_M_RX_INIT;
	// set direction for data receive
	vMBMasterPortSerialEnable( TRUE, FALSE );       //$$NR$$//dated:6Feb16
//	GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_4, 0x10);
//  GPIO_write(Board_RS485EN,(~0));

    vMBMasterPortTimersT35Enable(  );

   // EXIT_CRITICAL_SECTION(  );
}

void
eMBMasterRTUStop( void )
{
    //ENTER_CRITICAL_SECTION(  );
	// set direction for data trasmit
	vMBMasterPortSerialEnable( FALSE, TRUE );       //$$NR$$//dated:6Feb16
//	GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_4, 0x00);
//	GPIO_write(Board_RS485EN,(0));

    vMBMasterPortTimersDisable(  );
   // EXIT_CRITICAL_SECTION(  );
}

eMBErrorCode
eMBMasterRTUReceive( UCHAR * pucRcvAddress, UCHAR ** pucFrame, USHORT * pusLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;

//	System_printf("Rxd Bytes-> %d\n", usMasterRcvBufferIndex);
//	System_flush();

    //ENTER_CRITICAL_SECTION(  );

    /* Length and CRC check */
//	if( ( usMasterRcvBufferPos >= MB_SER_PDU_SIZE_MIN )
//		&& ( usMBCRC16( ( UCHAR * ) ucMasterRTURcvBuf, usMasterRcvBufferPos ) == 0 ) )
	if( ( usMasterRcvBufferIndex >= MB_SER_PDU_SIZE_MIN )
		&& ( usMBCRC16( ( UCHAR * ) ucMasterRTURcvBuf, usMasterRcvBufferIndex ) == 0 ) )
//	if( ( usMasterRcvBufferIndex >= MB_SER_PDU_SIZE_MIN )
//		&& ( usMBCRC16( ( UCHAR * ) ucMasterRTURcvBuf, usMasterRcvBufferIndex ) ) )
    {
        /* Save the address field. All frames are passed to the upper layed
         * and the decision if a frame is used is done there.
         */
        *pucRcvAddress = ucMasterRTURcvBuf[MB_SER_PDU_ADDR_OFF];

        /* Total length of Modbus-PDU is Modbus-Serial-Line-PDU minus
         * size of address field and CRC checksum.
         */
//		*pusLength = ( USHORT )( usMasterRcvBufferPos - MB_SER_PDU_PDU_OFF - MB_SER_PDU_SIZE_CRC );
		*pusLength = ( USHORT )( usMasterRcvBufferIndex - MB_SER_PDU_PDU_OFF - MB_SER_PDU_SIZE_CRC );

        /* Return the start of the Modbus PDU to the caller. */
        *pucFrame = ( UCHAR * ) & ucMasterRTURcvBuf[MB_SER_PDU_PDU_OFF];
    }
    else
    {
        eStatus = MB_EIO;
    }

   // EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBMasterRTUSend( UCHAR ucSlaveAddress, const UCHAR * pucFrame, USHORT usLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          usCRC16;

    if ( ucSlaveAddress > MB_MASTER_TOTAL_SLAVE_NUM ) return MB_EINVAL;

    //ENTER_CRITICAL_SECTION(  );

    /* Check if the receiver is still in idle state. If not we where to
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     */
    if( eRcvState == STATE_M_RX_IDLE )
    {
        /* First byte before the Modbus-PDU is the slave address. */
        pucMasterSndBufferCur = ( UCHAR * ) pucFrame - 1;
        usMasterSndBufferCount = 1;

        /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */
        pucMasterSndBufferCur[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;
        usMasterSndBufferCount += usLength;

        /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
        usCRC16 = usMBCRC16( ( UCHAR * ) pucMasterSndBufferCur, usMasterSndBufferCount );
        ucMasterRTUSndBuf[usMasterSndBufferCount++] = ( UCHAR )( usCRC16 & 0xFF );
        ucMasterRTUSndBuf[usMasterSndBufferCount++] = ( UCHAR )( usCRC16 >> 8 );

        /* Activate the transmitter. */
        eSndState = STATE_M_TX_XMIT;

		//testing purpose
//		usMasterRcvBufferPos = 0;
//		usMasterRcvBufferIndex = 0;

//		for(index=0; index<15; index++)		ucMasterRTURcvBuf_test[index] = 0;
//		RTURcvBufIndex = 0;
        tx_cnt = 0;

//		UART_read(uart2, (uint8_t *)&ucMBRTURcvBuf, 1);
//		( void )xMBMasterPortSerialGetByte( (uint8_t *)&ucMBRTURcvBuf );

		// set direction for data trasmit
    	vMBMasterPortSerialEnable( FALSE, TRUE );//$$NR$$//dated:20Sep15
//		GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_4, 0x00);
//		GPIO_write(Board_RS485EN,(0));

//        while (usMasterSndBufferCount != 0)
//        {
//			usMasterSndBufferIndex = 0;
//    		usMasterSndBufferCount--;
    		xMBMasterPortSerialPutByte( (const uint8_t * )pucMasterSndBufferCur, usMasterSndBufferCount );
//    		xMBMasterPortSerialPutByte( ( const uint8_t * )&pucMasterSndBufferCur[usMasterSndBufferIndex++], 1 );

//			pucMasterSndBufferCur++;  // next byte in sendbuffer.
//        }
	}
    else
    {
        eStatus = MB_EIO;
    }
   // EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

//$$NR$$//dated:6Feb16
//BOOL
//xMBMasterRTUReceiveFSM( void )
//{
//BOOL xTaskNeedSwitch = FALSE;
//static uint8_t test_index = 0;
//
////    assert_param(( eSndState == STATE_M_TX_IDLE ) || ( eSndState == STATE_M_TX_XFWR ));
//
////	/* Always read the character. */
////	ucMasterRTURcvBuf_test[RTURcvBufIndex++] = ucMBRTURcvBuf;
//////	UART_read(uart2, (uint8_t *)&ucMBRTURcvBuf, 1);
////	( void )xMBMasterPortSerialGetByte( (uint8_t *)&ucMBRTURcvBuf );
//
//    switch ( eRcvState )
//    {
//        /* If we have received a character in the init state we have to
//         * wait until the frame is finished.
//         */
//    case STATE_M_RX_INIT:
//        vMBMasterPortTimersT35Enable( );
//        break;
//
//        /* In the error state we wait until all characters in the
//         * damaged frame are transmitted.
//         */
//    case STATE_M_RX_ERROR:
//        vMBMasterPortTimersT35Enable( );
//        break;
//
//        /* In the idle state we wait for a new character. If a character
//         * is received the t1.5 and t3.5 timers are started and the
//         * receiver is in the state STATE_RX_RECEIVCE and disable early
//         * the timer of respond timeout .
//         */
//    case STATE_M_RX_IDLE:
//    	/* In time of respond timeout,the receiver receive a frame.
//    	 * Disable timer of respond timeout and change the transmiter state to idle.
//    	 */
//    	vMBMasterPortTimersDisable( );
//    	eSndState = STATE_M_TX_IDLE;
//
//    	/* Always read the character. */
////		usMasterRcvBufferPos = 0;
////		ucMasterRTURcvBuf[usMasterRcvBufferPos++] = ucMBRTURcvBuf;
//
//    	usMasterRcvBufferIndex = 0;
////		ucMasterRTURcvBuf[usMasterRcvBufferIndex++] = ucMBRTURcvBuf;
//        ucMasterRTURcvBuf[usMasterRcvBufferIndex] = ucMBRTURcvBuf[usMasterRcvBufferIndex];  usMasterRcvBufferIndex++;
//
////    	ucMasterRTURcvBuf_test[RTURcvBufIndex++] = ucMBRTURcvBuf;
//
//		RTURcvBufIndex = 0;
//
//    	eRcvState = STATE_M_RX_RCV;
//
//        /* Enable t3.5 timers. */
//        vMBMasterPortTimersT35Enable( );
//        break;
//
//        /* We are currently receiving a frame. Reset the timer after
//         * every character received. If more than the maximum possible
//         * number of bytes in a modbus frame is received the frame is
//         * ignored.
//         */
//    case STATE_M_RX_RCV:
////      if( usMasterRcvBufferPos < MB_SER_PDU_SIZE_MAX )
//        if( usMasterRcvBufferIndex < MB_SER_PDU_SIZE_MAX )
//        {
//        	/* Always read the character. */
////			ucMasterRTURcvBuf[usMasterRcvBufferPos++] = ucMBRTURcvBuf;
////          ucMasterRTURcvBuf[usMasterRcvBufferIndex++] = ucMBRTURcvBuf;
//            ucMasterRTURcvBuf[usMasterRcvBufferIndex] = ucMBRTURcvBuf[usMasterRcvBufferIndex];  usMasterRcvBufferIndex++;
//
////        	ucMasterRTURcvBuf_test[RTURcvBufIndex++] = ucMBRTURcvBuf;
//        }
//        else
//        {
//            eRcvState = STATE_M_RX_ERROR;
//        }
//        vMBMasterPortTimersT35Enable();
//        break;
//    }
//
////  ucMasterRTURcvBuf_test[RTURcvBufIndex++] = ucMBRTURcvBuf;
//
////  ucMasterRTURcvBuf_test[test_index++] = ucMBRTURcvBuf;
//    for (test_index=0; test_index<30; test_index++)
//    {
//        ucMasterRTURcvBuf_test[test_index] = ucMBRTURcvBuf[test_index];
//        System_printf(" %02x", ucMasterRTURcvBuf_test[test_index]);
//    }
//    System_printf("\n");
//    System_flush();
//
////	ucMasterRTURcvBuf_test[usMasterRcvBufferIndex-1] = ucMBRTURcvBuf;
//
////	UART_read(uart2, (uint8_t *)&ucMBRTURcvBuf, 1);
////	( void )xMBMasterPortSerialGetByte( (uint8_t *)&ucMBRTURcvBuf );
//
////    if(UARTCharsAvail(UART2_BASE))
////        UART_readPolling(uart2, ucMasterRTURcvBuf_test, 10);
//
////    UART_read(uart2, ucMasterRTURcvBuf_test, 10);
//
//	return xTaskNeedSwitch;
//}

BOOL
xMBMasterRTUReceiveFSM( void )
{
BOOL xTaskNeedSwitch = FALSE;
uint8_t frame_length = 0;

//    assert_param(( eSndState == STATE_M_TX_IDLE ) || ( eSndState == STATE_M_TX_XFWR ));

    switch ( eRcvState )
    {
        /* If we have received a character in the init state we have to
         * wait until the frame is finished.
         */
    case STATE_M_RX_INIT:
        vMBMasterPortTimersT35Enable( );
        break;

        /* In the error state we wait until all characters in the
         * damaged frame are transmitted.
         */
    case STATE_M_RX_ERROR:
        vMBMasterPortTimersT35Enable( );
        break;

        /* In the idle state we wait for a new character. If a character
         * is received the t1.5 and t3.5 timers are started and the
         * receiver is in the state STATE_RX_RECEIVCE and disable early
         * the timer of respond timeout .
         */
    case STATE_M_RX_IDLE:
        /* In time of respond timeout,the receiver receive a frame.
         * Disable timer of respond timeout and change the transmiter state to idle.
         */
        vMBMasterPortTimersDisable( );
        eSndState = STATE_M_TX_IDLE;

        /* Always read the character. */
//      usMasterRcvBufferPos = 0;
//      ucMasterRTURcvBuf[usMasterRcvBufferPos++] = ucMBRTURcvBuf;

//      usMasterRcvBufferIndex = 0;
//      ucMasterRTURcvBuf[usMasterRcvBufferIndex++] = ucMBRTURcvBuf;

        frame_length = ucMBRTURcvBuf[2] + 5;
        for (usMasterRcvBufferIndex=0; usMasterRcvBufferIndex < frame_length; usMasterRcvBufferIndex++)
        {
            if( usMasterRcvBufferIndex < MB_SER_PDU_SIZE_MAX )
            {
                ucMasterRTURcvBuf[usMasterRcvBufferIndex] = ucMBRTURcvBuf[usMasterRcvBufferIndex];

                ucMasterRTURcvBuf_test[usMasterRcvBufferIndex] = ucMBRTURcvBuf[usMasterRcvBufferIndex];
              //  System_printf(" %02x", ucMasterRTURcvBuf_test[usMasterRcvBufferIndex]);
            }
            else
            {
                eRcvState = STATE_M_RX_ERROR;
            }
        }
      //  System_printf("\n");
      //  System_flush();

//      ucMasterRTURcvBuf_test[RTURcvBufIndex++] = ucMBRTURcvBuf;

        RTURcvBufIndex = 0;

        eRcvState = STATE_M_RX_RCV;

//        /* Enable t3.5 timers. */
//        vMBMasterPortTimersT35Enable( );
//        break;
//
//        /* We are currently receiving a frame. Reset the timer after
//         * every character received. If more than the maximum possible
//         * number of bytes in a modbus frame is received the frame is
//         * ignored.
//         */
//    case STATE_M_RX_RCV:

////      if( usMasterRcvBufferPos < MB_SER_PDU_SIZE_MAX )
//        if( usMasterRcvBufferIndex < MB_SER_PDU_SIZE_MAX )
//        {
//            /* Always read the character. */
////          ucMasterRTURcvBuf[usMasterRcvBufferPos++] = ucMBRTURcvBuf;
////          ucMasterRTURcvBuf[usMasterRcvBufferIndex++] = ucMBRTURcvBuf;
//            ucMasterRTURcvBuf[usMasterRcvBufferIndex] = ucMBRTURcvBuf[usMasterRcvBufferIndex];  usMasterRcvBufferIndex++;
//
////          ucMasterRTURcvBuf_test[RTURcvBufIndex++] = ucMBRTURcvBuf;
//        }
//        else
//        {
//            eRcvState = STATE_M_RX_ERROR;
//        }
        vMBMasterPortTimersT35Enable();
        break;
    }

//  ucMasterRTURcvBuf_test[RTURcvBufIndex++] = ucMBRTURcvBuf;
//  ucMasterRTURcvBuf_test[usMasterRcvBufferIndex-1] = ucMBRTURcvBuf;

    return xTaskNeedSwitch;
}

//$$NR$$//dated:6Feb16

BOOL
xMBMasterRTUTransmitFSM( void )
{
BOOL xNeedPoll = FALSE;
//uint8_t index;

//    assert_param( eRcvState == STATE_M_RX_IDLE );

    switch ( eSndState )
    {
        /* We should not get a transmitter event if the transmitter is in
         * idle state.  */
    case STATE_M_TX_IDLE:
		/* enable receiver/disable transmitter. */
		// set direction for data receive
//		GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_4, 0x10);
//		vMBMasterPortSerialEnable( TRUE, FALSE );//$$NR$$//dated:20Sep15
        break;

    case STATE_M_TX_XMIT:
        /* check if we are finished. */
//		if( usMasterSndBufferCount != 0 )
//		{
//			usMasterSndBufferCount--;
////			xMBMasterPortSerialPutByte( ( CHAR )*pucMasterSndBufferCur );
//			xMBMasterPortSerialPutByte( ( const uint8_t * )&pucMasterSndBufferCur[usMasterSndBufferIndex++], 1 );
////			pucMasterSndBufferCur++;  /* next byte in sendbuffer. */
//		}
//		else
		{
			usMasterRcvBufferIndex = 0;
////		for(index=0; index<15; index++)		ucMasterRTURcvBuf_test[index] = 0;
			RTURcvBufIndex = 0;

////		UART_read(uart2, (uint8_t *)&ucMBRTURcvBuf, 1);
			( void )xMBMasterPortSerialGetByte( (uint8_t *)&ucMBRTURcvBuf );
//			UART_read(uart2, (uint8_t *)&ucMasterRTURcvBuf_test, 30);

			xFrameIsBroadcast = ( ucMasterRTUSndBuf[MB_SER_PDU_ADDR_OFF] == MB_ADDRESS_BROADCAST ) ? TRUE : FALSE;
			/* Disable transmitter. This prevents another transmit buffer
			* empty interrupt. */
//			vMBMasterPortSerialEnable( TRUE, FALSE );//$$NR$$//dated:20Sep15
			eSndState = STATE_M_TX_XFWR;
			/* If the frame is broadcast ,master will enable timer of convert delay,
			* else master will enable timer of respond timeout. */
			if ( xFrameIsBroadcast == TRUE )
			{
				vMBMasterPortTimersConvertDelayEnable( );
			}
			else
			{
				vMBMasterPortTimersRespondTimeoutEnable( );
			}
		}
        break;
    }

    return xNeedPoll;
}

BOOL
xMBMasterRTUTimerExpired(void)
{
	BOOL xNeedPoll = FALSE;

	switch (eRcvState)
	{
		/* Timer t35 expired. Startup phase is finished. */
	case STATE_M_RX_INIT:
		xNeedPoll = xMBMasterPortEventPost(EV_MASTER_READY);
		break;

		/* A frame was received and t35 expired. Notify the listener that
		 * a new frame was received. */
	case STATE_M_RX_RCV:
		xNeedPoll = xMBMasterPortEventPost(EV_MASTER_FRAME_RECEIVED);
//		System_printf("EV_MASTER_FRAME_RECEIVED ...\n");
		/* SysMin will only print to the console when you call flush or exit */
//		System_flush();

		break;

		/* An error occured while receiving the frame. */
	case STATE_M_RX_ERROR:
		vMBMasterSetErrorType(EV_ERROR_RECEIVE_DATA);
		xNeedPoll = xMBMasterPortEventPost( EV_MASTER_ERROR_PROCESS );

//		System_printf("MasterSetErrorType-> EV_ERROR_RECEIVE_DATA Interrupt\n");
		/* SysMin will only print to the console when you call flush or exit */
//		System_flush();
		break;

		/* Function called in an illegal state. */
	default:
//		assert_param(
//				( eRcvState == STATE_M_RX_INIT ) || ( eRcvState == STATE_M_RX_RCV ) ||
//				( eRcvState == STATE_M_RX_ERROR ) || ( eRcvState == STATE_M_RX_IDLE ));
		break;
	}
	eRcvState = STATE_M_RX_IDLE;

	switch (eSndState)
	{
		/* A frame was send finish and convert delay or respond timeout expired.
		 * If the frame is broadcast,The master will idle,and if the frame is not
		 * broadcast.Notify the listener process error.*/
	case STATE_M_TX_XFWR:
		if ( xFrameIsBroadcast == FALSE ) {
			vMBMasterSetErrorType(EV_ERROR_RESPOND_TIMEOUT);
			xNeedPoll = xMBMasterPortEventPost(EV_MASTER_ERROR_PROCESS);

//			System_printf("MasterSetErrorType-> EV_ERROR_RESPOND_TIMEOUT Interrupt\n");
			/* SysMin will only print to the console when you call flush or exit */
//			System_flush();
		}
		break;
		/* Function called in an illegal state. */
	default:
//		assert_param(
//				( eSndState == STATE_M_TX_XFWR ) || ( eSndState == STATE_M_TX_IDLE ));
		break;
	}
	eSndState = STATE_M_TX_IDLE;

	vMBMasterPortTimersDisable( );
	/* If timer mode is convert delay, the master event then turns EV_MASTER_EXECUTE status. */
	if (eMasterCurTimerMode == MB_TMODE_CONVERT_DELAY) {
		xNeedPoll = xMBMasterPortEventPost( EV_MASTER_EXECUTE );
	}

	return xNeedPoll;
}

/* Get Modbus Master send RTU's buffer address pointer.*/
void vMBMasterGetRTUSndBuf( UCHAR ** pucFrame )
{
	*pucFrame = ( UCHAR * ) ucMasterRTUSndBuf;
}

/* Get Modbus Master send PDU's buffer address pointer.*/
void vMBMasterGetPDUSndBuf( UCHAR ** pucFrame )
{
	*pucFrame = ( UCHAR * ) &ucMasterRTUSndBuf[MB_SER_PDU_PDU_OFF];
}

/* Set Modbus Master send PDU's buffer length.*/
void vMBMasterSetPDUSndLength( USHORT SendPDULength )
{
	usMasterSendPDULength = SendPDULength;
}

/* Get Modbus Master send PDU's buffer length.*/
USHORT usMBMasterGetPDUSndLength( void )
{
	return usMasterSendPDULength;
}

/* Set Modbus Master current timer mode.*/
void vMBMasterSetCurTimerMode( eMBMasterTimerMode eMBTimerMode )
{
	eMasterCurTimerMode = eMBTimerMode;
}

/* The master request is broadcast? */
BOOL xMBMasterRequestIsBroadcast( void ){
	return xFrameIsBroadcast;
}
#endif

