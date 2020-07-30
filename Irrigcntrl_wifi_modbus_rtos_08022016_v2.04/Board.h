/*
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __BOARD_H
#define __BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Irrigation_Controller.h"

#define Board_initEMAC              Irrigation_Controller_initEMAC
#define Board_initGeneral           Irrigation_Controller_initGeneral
#define Board_initGPIO              Irrigation_Controller_initGPIO
#define Board_initI2C               Irrigation_Controller_initI2C
#define Board_initSDSPI             Irrigation_Controller_initSDSPI
#define Board_initSPI               Irrigation_Controller_initSPI
#define Board_initUART              Irrigation_Controller_initUART
#define Board_initUSB               Irrigation_Controller_initUSB
#define Board_initUSBMSCHFatFs      Irrigation_Controller_initUSBMSCHFatFs
#define Board_initWatchdog          Irrigation_Controller_initWatchdog
#define Board_initWiFi              Irrigation_Controller_initWiFi

#define Board_LED_ON                Irrigation_Controller_LED_ON
#define Board_LED_OFF               Irrigation_Controller_LED_OFF
#define Board_PIN_HIGH				Irrigation_Controller_PIN_HIGH
#define Board_PIN_LOW				Irrigation_Controller_PIN_LOW
#define Board_LED0                  Irrigation_Controller_D1
#define Board_LED1                  Irrigation_Controller_D2
#define Board_LED2                  Irrigation_Controller_D2
#define Board_BUTTON0               Irrigation_Controller_USR_SW1
#define Board_BUTTON1               Irrigation_Controller_USR_SW2
#define Board_keypad_IRQ			Irrigation_Controller_keypad_IRQ
#define Board_Display_CS1			Irrigation_Controller_Display_CS1
#define Board_Display_CS2			Irrigation_Controller_Display_CS2
#define Board_Display_RST			Irrigation_Controller_Display_RST
#define Board_Display_E				Irrigation_Controller_Display_E
#define Board_Display_RW			Irrigation_Controller_Display_RW
#define Board_Display_RS			Irrigation_Controller_Display_RS
#define Board_Display_DB0			Irrigation_Controller_Display_DB0
#define Board_Display_DB1			Irrigation_Controller_Display_DB1
#define Board_Display_DB2			Irrigation_Controller_Display_DB2
#define Board_Display_DB3			Irrigation_Controller_Display_DB3
#define Board_Display_DB4			Irrigation_Controller_Display_DB4
#define Board_Display_DB5			Irrigation_Controller_Display_DB5
#define Board_Display_DB6			Irrigation_Controller_Display_DB6
#define Board_Display_DB7			Irrigation_Controller_Display_DB7

#define	Board_Display_BL			Irrigation_Controller_Display_BL
#define	Board_BUZZER				Irrigation_Controller_BUZZER

#define Board_Display_DI1			Irrigation_Controller_DI1
#define Board_Display_DI2			Irrigation_Controller_DI2
#define Board_Display_DI3			Irrigation_Controller_DI3
#define Board_Display_DI4			Irrigation_Controller_DI4

#define Board_Display_AI1			Irrigation_Controller_AI1
#define Board_Display_AI2			Irrigation_Controller_AI2
#define Board_Display_AI3			Irrigation_Controller_AI3
#define Board_Display_AI4			Irrigation_Controller_AI4

#define Board_DO1			Irrigation_Controller_DO1
#define Board_DO2			Irrigation_Controller_DO2
#define Board_DO3			Irrigation_Controller_DO3
#define Board_DO4			Irrigation_Controller_DO4
#define Board_DO5			Irrigation_Controller_DO5
#define Board_DO6			Irrigation_Controller_DO6
#define Board_DO7			Irrigation_Controller_DO7
#define Board_DO8			Irrigation_Controller_DO8
#define Board_DO9			Irrigation_Controller_DO9
#define Board_DO10			Irrigation_Controller_DO10
#define Board_DO11			Irrigation_Controller_DO11
#define Board_DO12			Irrigation_Controller_DO12
#define Board_DO13			Irrigation_Controller_DO13
#define Board_DO14			Irrigation_Controller_DO14
#define Board_DO15			Irrigation_Controller_DO15
#define Board_DO16			Irrigation_Controller_DO16

#define Board_GSMAN			Irrigation_Controller_GSMAN
#define Board_GSMRST		Irrigation_Controller_GSMRST

#define Board_RS485EN		Irrigation_Controller_RS485EN


#define Board_I2C0                  Irrigation_Controller_I2C7
#define Board_I2C1                  Irrigation_Controller_I2C8
#define Board_I2C2                  Irrigation_Controller_I2C0
#define Board_I2C3                  Irrigation_Controller_I2C2
#define Board_I2C_TMP               Irrigation_Controller_I2C7
#define Board_I2C_NFC               Irrigation_Controller_I2C7
#define Board_I2C_TPL0401           Irrigation_Controller_I2C7
#define Board_I2C_TCA8424           Irrigation_Controller_I2C7

#define Board_SDSPI0                Irrigation_Controller_SDSPI0
#define Board_SDSPI1                Irrigation_Controller_SDSPI1

#define Board_SPI0                  Irrigation_Controller_SPI2
#define Board_SPI1                  Irrigation_Controller_SPI3
#define Board_SPI_CC3100            Irrigation_Controller_SPI2

#define Board_USBMSCHFatFs0         Irrigation_Controller_USBMSCHFatFs0

#define Board_USBHOST               Irrigation_Controller_USBHOST
#define Board_USBDEVICE             Irrigation_Controller_USBDEVICE

#define Board_UART0                 Irrigation_Controller_UART0
#define Board_UART1                 Irrigation_Controller_UART6
#define Board_UART2                 Irrigation_Controller_UART2
#define Board_UART3                 Irrigation_Controller_UART3

#define Board_WATCHDOG0             Irrigation_Controller_WATCHDOG0

#define Board_WIFI                  Irrigation_Controller_WIFI

#define Board_keypad_callback       Irrigation_Controller_gpioPortPCallbacks
#define Board_gpioCallbacks1        Irrigation_Controller_gpioPortJCallbacks

/* Board specific I2C addresses */
#define Board_TMP006_ADDR           (0x40)
#define Board_RF430CL330_ADDR       (0x28)
#define Board_TPL0401_ADDR          (0x40)
#define Board_TCA8424_ADDR          (0x3b)

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */
