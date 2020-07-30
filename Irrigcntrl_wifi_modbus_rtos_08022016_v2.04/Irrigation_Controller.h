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
/** ============================================================================
 *  @file       Irrigation_Controller.h
 *
 *  @brief      Irrigation_Controller Board Specific APIs
 *
 *  The Irrigation_Controller header file should be included in an application as follows:
 *  @code
 *  #include <Irrigation_Controller.h>
 *  @endcode
 *
 *  ============================================================================
 */

#ifndef __Irrigation_Controller_H
#define __Irrigation_Controller_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/drivers/GPIO.h>

/* LEDs on Irrigation_Controller are active high. */
#define Irrigation_Controller_LED_OFF ( 0)
#define Irrigation_Controller_LED_ON  (~0)

/* Pins on Irrigation_Controller. */
#define Irrigation_Controller_PIN_LOW ( 0)
#define Irrigation_Controller_PIN_HIGH  (~0)



/*!
 *  @def    Irrigation_Controller_EMACName
 *  @brief  Enum of EMAC names on the Irrigation_Controller dev board
 */
typedef enum Irrigation_Controller_EMACName {
    Irrigation_Controller_EMAC0 = 0,

    Irrigation_Controller_EMACCOUNT
} Irrigation_Controller_EMACName;

/*!
 *  @def    Irrigation_Controller_GPIOName
 *  @brief  Enum of LED names on the Irrigation_Controller dev board
 */
typedef enum Irrigation_Controller_GPIOName {
	Irrigation_Controller_keypad_IRQ = 0,
	Irrigation_Controller_Display_CS1,
	Irrigation_Controller_Display_CS2,
	Irrigation_Controller_Display_RST,
	Irrigation_Controller_Display_E,
	Irrigation_Controller_Display_RW,
	Irrigation_Controller_Display_RS,
	Irrigation_Controller_Display_DB0,
	Irrigation_Controller_Display_DB1,
	Irrigation_Controller_Display_DB2,
	Irrigation_Controller_Display_DB3,
	Irrigation_Controller_Display_DB4,
	Irrigation_Controller_Display_DB5,
	Irrigation_Controller_Display_DB6,
	Irrigation_Controller_Display_DB7,
	Irrigation_Controller_DI1,
	Irrigation_Controller_DI2,
	Irrigation_Controller_DI3,
	Irrigation_Controller_DI4,
	Irrigation_Controller_AI3,
	Irrigation_Controller_AI4,
	Irrigation_Controller_DO1,
	Irrigation_Controller_DO2,
	Irrigation_Controller_DO3,
	Irrigation_Controller_DO4,
	Irrigation_Controller_DO5,
	Irrigation_Controller_DO6,
	Irrigation_Controller_DO7,
	Irrigation_Controller_DO8,
	Irrigation_Controller_DO9,
	Irrigation_Controller_DO10,
	Irrigation_Controller_DO11,
	Irrigation_Controller_DO12,
	Irrigation_Controller_DO13,
	Irrigation_Controller_DO14,
	Irrigation_Controller_DO15,
	Irrigation_Controller_DO16,
	Irrigation_Controller_GSMAN,
	Irrigation_Controller_GSMRST,
	Irrigation_Controller_Display_BL,
	Irrigation_Controller_BUZZER,
	Irrigation_Controller_RS485EN,
    Irrigation_Controller_GPIOCOUNT
} Irrigation_Controller_GPIOName;

/*!
 *  @def    Irrigation_Controller_I2CName
 *  @brief  Enum of I2C names on the Irrigation_Controller dev board
 */
typedef enum Irrigation_Controller_I2CName {
	Irrigation_Controller_I2C7 = 0,

    Irrigation_Controller_I2CCOUNT
} EIrrigation_Controller_I2CName;

/*!
 *  @def    Irrigation_Controller_SDSPIName
 *  @brief  Enum of SDSPI names on the Irrigation_Controller dev board
 */
typedef enum Irrigation_Controller_SDSPIName {
    Irrigation_Controller_SDSPI0 = 0,
    Irrigation_Controller_SDSPI1,

    Irrigation_Controller_SDSPICOUNT
} Irrigation_Controller_SDSPIName;

/*!
 *  @def    Irrigation_Controller_SPIName
 *  @brief  Enum of SPI names on the Irrigation_Controller dev board
 */
typedef enum Irrigation_Controller_SPIName {
    Irrigation_Controller_SPI2 = 0,
    Irrigation_Controller_SPI3,

    Irrigation_Controller_SPICOUNT
} Irrigation_Controller_SPIName;

/*!
 *  @def    Irrigation_Controller_UARTName
 *  @brief  Enum of UARTs on the Irrigation_Controller dev board
 */
typedef enum Irrigation_Controller_UARTName {
    Irrigation_Controller_UART3 = 0,
    Irrigation_Controller_UART2,
    Irrigation_Controller_UARTCOUNT
} Irrigation_Controller_UARTName;

/*!
 *  @def    Irrigation_Controller_USBMode
 *  @brief  Enum of USB setup function on the Irrigation_Controller dev board
 */
typedef enum Irrigation_Controller_USBMode {
    Irrigation_Controller_USBDEVICE,
    Irrigation_Controller_USBHOST
} Irrigation_Controller_USBMode;

/*!
 *  @def    Irrigation_Controller_USBMSCHFatFsName
 *  @brief  Enum of USBMSCHFatFs names on the Irrigation_Controller dev board
 */
typedef enum Irrigation_Controller_USBMSCHFatFsName {
    Irrigation_Controller_USBMSCHFatFs0 = 0,

    Irrigation_Controller_USBMSCHFatFsCOUNT
} Irrigation_Controller_USBMSCHFatFsName;

/*
 *  @def    Irrigation_Controller_WatchdogName
 *  @brief  Enum of Watchdogs on the Irrigation_Controller dev board
 */
typedef enum Irrigation_Controller_WatchdogName {
    Irrigation_Controller_WATCHDOG0 = 0,

    Irrigation_Controller_WATCHDOGCOUNT
} Irrigation_Controller_WatchdogName;

/*!
 *  @def    Irrigation_Controller_WiFiName
 *  @brief  Enum of WiFi names on the Irrigation_Controller dev board
 */
typedef enum Irrigation_Controller_WiFiName {
    Irrigation_Controller_WIFI = 0,

    Irrigation_Controller_WIFICOUNT
} Irrigation_Controller_WiFiName;

/*!
 *  @brief  Initialize the general board specific settings
 *
 *  This function initializes the general board specific settings. This include
 *     - Enable clock sources for peripherals
 */
extern void Irrigation_Controller_initGeneral(void);

/*!
 *  @brief Initialize board specific EMAC settings
 *
 *  This function initializes the board specific EMAC settings and
 *  then calls the EMAC_init API to initialize the EMAC module.
 *
 *  The EMAC address is programmed as part of this call.
 *
 */
extern void Irrigation_Controller_initEMAC(void);

/*!
 *  @brief  Initialize board specific GPIO settings
 *
 *  This function initializes the board specific GPIO settings and
 *  then calls the GPIO_init API to initialize the GPIO module.
 *
 *  The GPIOs controlled by the GPIO module are determined by the GPIO_config
 *  variable.
 */
extern void Irrigation_Controller_initGPIO(void);

/*!
 *  @brief  Initialize board specific I2C settings
 *
 *  This function initializes the board specific I2C settings and then calls
 *  the I2C_init API to initialize the I2C module.
 *
 *  The I2C peripherals controlled by the I2C module are determined by the
 *  I2C_config variable.
 */
extern void Irrigation_Controller_initI2C(void);

/*!
 *  @brief  Initialize board specific SDSPI settings
 *
 *  This function initializes the board specific SDSPI settings and then calls
 *  the SDSPI_init API to initialize the SDSPI module.
 *
 *  The SDSPI peripherals controlled by the SDSPI module are determined by the
 *  SDSPI_config variable.
 */
extern void Irrigation_Controller_initSDSPI(void);

/*!
 *  @brief  Initialize board specific SPI settings
 *
 *  This function initializes the board specific SPI settings and then calls
 *  the SPI_init API to initialize the SPI module.
 *
 *  The SPI peripherals controlled by the SPI module are determined by the
 *  SPI_config variable.
 */
extern void Irrigation_Controller_initSPI(void);

/*!
 *  @brief  Initialize board specific UART settings
 *
 *  This function initializes the board specific UART settings and then calls
 *  the UART_init API to initialize the UART module.
 *
 *  The UART peripherals controlled by the UART module are determined by the
 *  UART_config variable.
 */
extern void Irrigation_Controller_initUART(void);

/*!
 *  @brief  Initialize board specific USB settings
 *
 *  This function initializes the board specific USB settings and pins based on
 *  the USB mode of operation.
 *
 *  @param      usbMode    USB mode of operation
 */
extern void Irrigation_Controller_initUSB(Irrigation_Controller_USBMode usbMode);

/*!
 *  @brief  Initialize board specific USBMSCHFatFs settings
 *
 *  This function initializes the board specific USBMSCHFatFs settings and then
 *  calls the USBMSCHFatFs_init API to initialize the USBMSCHFatFs module.
 *
 *  The USBMSCHFatFs peripherals controlled by the USBMSCHFatFs module are
 *  determined by the USBMSCHFatFs_config variable.
 */
extern void Irrigation_Controller_initUSBMSCHFatFs(void);

/*!
 *  @brief  Initialize board specific Watchdog settings
 *
 *  This function initializes the board specific Watchdog settings and then
 *  calls the Watchdog_init API to initialize the Watchdog module.
 *
 *  The Watchdog peripherals controlled by the Watchdog module are determined
 *  by the Watchdog_config variable.
 */
extern void Irrigation_Controller_initWatchdog(void);

/*!
 *  @brief  Initialize board specific WiFi settings
 *
 *  This function initializes the board specific WiFi settings and then calls
 *  the WiFi_init API to initialize the WiFi module.
 *
 *  The hardware resources controlled by the WiFi module are determined by the
 *  WiFi_config variable.
 */
extern void Irrigation_Controller_initWiFi(void);


extern void ADCHandler();
extern void adc_init();

#ifdef __cplusplus
}
#endif

#endif /* __Irrigation_Controller_H */
