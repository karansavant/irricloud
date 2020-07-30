/*
 * FreeModbus Libary: PIC Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
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
 * File: $Id: port.h,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#ifndef _PORT_H
#define _PORT_H

#include <stdint.h>
#include <stdbool.h>
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

//#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <xdc/runtime/Error.h>

// Definitions
#define _XTAL_FREQ  25000000	//16Mhz// this is used by the __delay_ms(xx) and __delay_us(xx) functions

//#define DIRECTION_485_PORT  GPIO_PORTQ_BASE
//#define TRANSMIT_485_BIT    GPIO_PIN_4

#define	INLINE                      inline
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }

//Comment out next line if not using NOVO RTOS
#define USING_NOVO 0

//#define TRANSMIT_ON_485()   GPIOPinWrite(DIRECTION_485_PORT, TRANSMIT_485_BIT, 0x00)
//#define RECEIVE_ON_485()    GPIOPinWrite(DIRECTION_485_PORT, TRANSMIT_485_BIT, 0x10)

#define ENTER_CRITICAL_SECTION()    Hwi_disable()	//IntMasterDisable()	//(INTCONbits.GIE = 0)
#define EXIT_CRITICAL_SECTION()     Hwi_enable()	//IntMasterEnable()	//(INTCONbits.GIE = 1)

/*
typedef enum
{
    STATE_ENABLED,
    STATE_DISABLED,
    STATE_NOT_INITIALIZED
}EMBSTATE;
*/

typedef char BOOL;

typedef unsigned char UCHAR;
typedef char CHAR;

typedef unsigned short USHORT;
typedef short SHORT;

typedef unsigned long ULONG;
typedef long LONG;

#ifndef TRUE
#define TRUE        1
#endif

#ifndef FALSE
#define FALSE       0
#endif

#endif
