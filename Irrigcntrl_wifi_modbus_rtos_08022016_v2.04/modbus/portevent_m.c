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
 * File: $Id: portevent_m.c v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include <stdio.h>

#include "mb.h"
#include "mb_m.h"
#include "mbport.h"
#include "port.h"
#include "mbconfig.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Defines ------------------------------------------*/
/* ----------------------- Variables ----------------------------------------*/
//static struct rt_semaphore xMasterRunRes;
BOOL			xMasterRunRes;

//static struct rt_event     xMasterOsEvent;

//static eMBMasterEventType	eMasterQueuedEvent;
uint16_t		eMasterQueuedEvent;
BOOL			xMasterEventInQueue;

uint16_t setEvent, recvedEvent;

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBMasterPortEventInit( void )
{
	xMasterEventInQueue = FALSE;
	eMasterQueuedEvent = 0;	//EV_MASTER_READY;

//    System_printf("PortEventInit-> %d\t", eMasterQueuedEvent);
	/* SysMin will only print to the console when you call flush or exit */
//	System_flush();

	return TRUE;
}

//BOOL xMBMasterPortEventPost( eMBMasterEventType eEvent )
BOOL xMBMasterPortEventPost( uint16_t eEvent )
{
	xMasterEventInQueue = TRUE;
//    eMasterQueuedEvent |= eEvent;
    eMasterQueuedEvent = eEvent;

//    System_printf("PortEventPost-> %d\t", eMasterQueuedEvent);
	/* SysMin will only print to the console when you call flush or exit */
//	System_flush();

    return TRUE;
}

//BOOL xMBMasterPortEventGet( eMBMasterEventType * eEvent )
BOOL xMBMasterPortEventGet( uint16_t * eEvent )
{
#if 0
BOOL xEventHappened = FALSE;

	if( xMasterEventInQueue )
	{
		xMasterEventInQueue = FALSE;
		*eEvent = eMasterQueuedEvent;
		xEventHappened = TRUE;
	}

	return xEventHappened;
#endif

//BOOL recvedEventFlag = TRUE;

//	setEvent = EV_MASTER_READY | EV_MASTER_FRAME_RECEIVED | EV_MASTER_EXECUTE | EV_MASTER_FRAME_SENT | EV_MASTER_ERROR_PROCESS;
	setEvent = 0x001F;

	// set received event
	recvedEvent = (eMasterQueuedEvent & setEvent);

    // As option = RT_EVENT_FLAG_OR
	if (recvedEvent)
	{
        // received event
//		eMasterQueuedEvent &= 0xFFE0;

		// the enum type couldn't convert to int type
		switch (recvedEvent)
		{
			case EV_MASTER_READY:
				*eEvent = EV_MASTER_READY;
				break;
			case EV_MASTER_FRAME_RECEIVED:
				*eEvent = EV_MASTER_FRAME_RECEIVED;
				break;
			case EV_MASTER_EXECUTE:
				*eEvent = EV_MASTER_EXECUTE;
				break;
			case EV_MASTER_FRAME_SENT:
				*eEvent = EV_MASTER_FRAME_SENT;
				break;
			case EV_MASTER_ERROR_PROCESS:
				*eEvent = EV_MASTER_ERROR_PROCESS;
				break;
//			default:
//				recvedEventFlag = FALSE;
//				break;
		}

//		if(recvedEventFlag)		eMasterQueuedEvent &= 0xFFE0;
		eMasterQueuedEvent &= 0xFFE0;

		xMasterEventInQueue = FALSE;

//		System_printf("PortEventGet-> %d\t", *eEvent);
		/* SysMin will only print to the console when you call flush or exit */
	//	System_flush();

		return TRUE;
	}

	return FALSE;
}
/**
 * This function is initialize the OS resource for modbus master.
 * Note:The resource is define by OS.If you not use OS this function can be empty.
 *
 */
void vMBMasterOsResInit( void )
{
//    rt_sem_init(&xMasterRunRes, "master res", 0x01 , RT_IPC_FLAG_PRIO);
	xMasterRunRes = FALSE;
}

/**
 * This function is take Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use OS this function can be just return TRUE.
 *
 * @param lTimeOut the waiting time.
 *
 * @return resource taked result
 */

BOOL xMBMasterRunResTake( LONG lTimeOut )
{
	/*If waiting time is -1 .It will wait forever */
//	return rt_sem_take(&xMasterRunRes, lTimeOut) ? FALSE : TRUE ;
	xMasterRunRes = TRUE;

	return xMasterRunRes;		//TRUE;
}

/**
 * This function is release Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use OS this function can be empty.
 *
 */
void vMBMasterRunResRelease( void )
{
	/* release resource */
//	rt_sem_release(&xMasterRunRes);
	xMasterRunRes = FALSE;
}

/**
 * This is modbus master respond timeout error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBRespondTimeout(UCHAR ucDestAddress, const UCHAR* pucPDUData,
        USHORT ucPDULength) {
	/**
	 * @note This code is use OS's event mechanism for modbus master protocol stack.
	 * If you don't use OS, you can change it.
	 */
	eMasterQueuedEvent |= EV_MASTER_ERROR_RESPOND_TIMEOUT;

//	System_printf("ErrorEventSet-> %d\t", eMasterQueuedEvent);
	/* SysMin will only print to the console when you call flush or exit */
//	System_flush();

	xMasterEventInQueue = TRUE;

    /* You can add your code under here. */

}

/**
 * This is modbus master receive data error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBReceiveData(UCHAR ucDestAddress, const UCHAR* pucPDUData,
        USHORT ucPDULength) {
	/**
	 * @note This code is use OS's event mechanism for modbus master protocol stack.
	 * If you don't use OS, you can change it.
	 */
	eMasterQueuedEvent |= EV_MASTER_ERROR_RECEIVE_DATA;

//	System_printf("ErrorEventSet-> %d\t", eMasterQueuedEvent);
	/* SysMin will only print to the console when you call flush or exit */
//	System_flush();

	xMasterEventInQueue = TRUE;

    /* You can add your code under here. */

}

/**
 * This is modbus master execute function error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBExecuteFunction(UCHAR ucDestAddress, const UCHAR* pucPDUData,
        USHORT ucPDULength) {
	/**
	 * @note This code is use OS's event mechanism for modbus master protocol stack.
	 * If you don't use OS, you can change it.
	 */
	eMasterQueuedEvent |= EV_MASTER_ERROR_EXECUTE_FUNCTION;

//	System_printf("ErrorEventSet-> %d\t", eMasterQueuedEvent);
	/* SysMin will only print to the console when you call flush or exit */
//	System_flush();

	xMasterEventInQueue = TRUE;

    /* You can add your code under here. */

}

/**
 * This is modbus master request process success callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 */
void vMBMasterCBRequestScuuess( void ) {
	/**
	 * @note This code is use OS's event mechanism for modbus master protocol stack.
	 * If you don't use OS, you can change it.
	 */
	eMasterQueuedEvent |= EV_MASTER_PROCESS_SUCESS;

//	System_printf("SuccessEventSet-> %d\t", eMasterQueuedEvent);
	/* SysMin will only print to the console when you call flush or exit */
//	System_flush();

	xMasterEventInQueue = TRUE;

    /* You can add your code under here. */

}

/**
 * This function is wait for modbus master request finish and return result.
 * Waiting result include request process success, request respond timeout,
 * receive data error and execute function error.You can use the above callback function.
 * @note If you are use OS, you can use OS's event mechanism. Otherwise you have to run
 * much user custom delay for waiting.
 *
 * @return request error code
 */
eMBMasterReqErrCode eMBMasterWaitRequestFinish( void ) {

eMBMasterReqErrCode    eErrStatus = MB_MRE_NO_ERR;

//	setEvent = EV_MASTER_PROCESS_SUCESS | EV_MASTER_ERROR_RESPOND_TIMEOUT
//					| EV_MASTER_ERROR_RECEIVE_DATA | EV_MASTER_ERROR_EXECUTE_FUNCTION;
	setEvent = 0x01E0;

	// As option = RT_EVENT_FLAG_OR
	if (eMasterQueuedEvent & setEvent)
	{
		// set received event
//		recvedEvent = (eMasterQueuedEvent & setEvent);
		recvedEvent = (eMasterQueuedEvent & 0xFE1F);

		// received event
		// As option = RT_EVENT_FLAG_CLEAR
		eMasterQueuedEvent &= ~setEvent;

		// the enum type couldn't convert to int type
		switch (recvedEvent)
		{
			case EV_MASTER_PROCESS_SUCESS:
					break;
			case EV_MASTER_ERROR_RESPOND_TIMEOUT:
				{
					eErrStatus = MB_MRE_TIMEDOUT;
					break;
				}
			case EV_MASTER_ERROR_RECEIVE_DATA:
				{
					eErrStatus = MB_MRE_REV_DATA;
					break;
				}
			case EV_MASTER_ERROR_EXECUTE_FUNCTION:
				{
					eErrStatus = MB_MRE_EXE_FUN;
					break;
				}
		}

//	    System_printf("ErrorEventGet-> %d\t", recvedEvent);
		/* SysMin will only print to the console when you call flush or exit */
	//	System_flush();

		xMasterEventInQueue = FALSE;
	}
	return eErrStatus;
}

#endif
