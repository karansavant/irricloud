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

/*
 *  ======== Irrigation_Controller.c ========
 *  This file is responsible for setting up the board specific items for the
 *  Irrigation_Controller board.
 *
 *  The following defines are used to determine which TI-RTOS peripheral drivers
 *  to include:
 *     TI_DRIVERS_EMAC_INCLUDED
 *     TI_DRIVERS_GPIO_INCLUDED
 *     TI_DRIVERS_I2C_INCLUDED
 *     TI_DRIVERS_SDSPI_INCLUDED
 *     TI_DRIVERS_SPI_INCLUDED
 *     TI_DRIVERS_UART_INCLUDED
 *     TI_DRIVERS_USBMSCHFATFS_INCLUDED
 *     TI_DRIVERS_WATCHDOG_INCLUDED
 *     TI_DRIVERS_WIFI_INCLUDED
 *  These defines are created when a useModule is done on the driver in the
 *  application's .cfg file. The actual #define is in the application
 *  generated header file that is brought in via the xdc/cfg/global.h.
 *  For example the following in the .cfg file
 *     var GPIO = xdc.useModule('ti.drivers.GPIO');
 *  Generates the following
 *     #define TI_DRIVERS_GPIO_INCLUDED 1
 *  If there is no useModule of ti.drivers.GPIO, the constant is set to 0.
 *
 *  Note: a useModule is generated in the .cfg file via the graphical
 *  configuration tool when the "Add xxx to my configuration" is checked
 *  or "Use xxx" is selected.
 */

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_ints.h>
#include <inc/hw_gpio.h>

#include <driverlib/gpio.h>
#include <driverlib/flash.h>
#include <driverlib/sysctl.h>
#include <driverlib/i2c.h>
#include <driverlib/ssi.h>
#include <driverlib/udma.h>
#include <driverlib/pin_map.h>
#include <driverlib/adc.h>
#include <driverlib/uart.h>

#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>

#include "../Board.h"	//$$NR$$//dated:19Sep15
#include "Irrigation_Controller.h"

/*
 *  =============================== DMA ===============================
 */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(dmaControlTable, 1024)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=1024
#elif defined(__GNUC__)
__attribute__ ((aligned (1024)))
#endif
static tDMAControlTable dmaControlTable[32];
static Bool dmaInitialized = false;

/* Hwi_Struct used in the initDMA Hwi_construct call */
static Hwi_Struct dmaHwiStruct;

/* Hwi_Struct used in the usbBusFault Hwi_construct call */
static Hwi_Struct usbBusFaultHwiStruct;

/*
 *  ======== dmaErrorHwi ========
 */
static Void dmaErrorHwi(UArg arg)
{
   // System_printf("DMA error code: %d\n", uDMAErrorStatusGet());
    uDMAErrorStatusClear();
   // System_abort("DMA error!!");
}

/*
 *  ======== Irrigation_Controller_usbBusFaultHwi ========
 */
static Void Irrigation_Controller_usbBusFaultHwi(UArg arg)
{
    /*
     *  This function should be modified to appropriately manage handle
     *  a USB bus fault.
    */
  //  System_printf("USB bus fault detected.");
    Hwi_clearInterrupt(INT_GPIOQ4);
  //  System_abort("USB error!!");
}

/*
 *  ======== Irrigation_Controller_initDMA ========
 */
void Irrigation_Controller_initDMA(void)
{
    Error_Block eb;
    Hwi_Params  hwiParams;

    if (!dmaInitialized) {

        Error_init(&eb);

        Hwi_Params_init(&hwiParams);
        Hwi_construct(&(dmaHwiStruct), INT_UDMAERR, dmaErrorHwi,
                      &hwiParams, &eb);
        if (Error_check(&eb)) {
          //  System_abort("Couldn't construct DMA error hwi");
        }

        SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
        uDMAEnable();
        uDMAControlBaseSet(dmaControlTable);

        dmaInitialized = true;
    }
}

/*
 *  =============================== General ===============================
 */
/*
 *  ======== Irrigation_Controller_initGeneral ========
 */
void Irrigation_Controller_initGeneral(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOR);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOS);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOT);
}



/*
 *  =============================== EMAC ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(EMAC_config, ".const:EMAC_config")
#pragma DATA_SECTION(emacHWAttrs, ".const:emacHWAttrs")
#pragma DATA_SECTION(NIMUDeviceTable, ".data:NIMUDeviceTable")
#endif

#include <ti/drivers/EMAC.h>
#include <ti/drivers/emac/EMACSnow.h>

/*
 *  Required by the Networking Stack (NDK). This array must be NULL terminated.
 *  This can be removed if NDK is not used.
 *  Double curly braces are needed to avoid GCC bug #944572
 *  https://bugs.launchpad.net/gcc-linaro/+bug/944572
 */
#if TI_EXAMPLES_PPP
/* Use PPP driver for PPP example only */
NIMU_DEVICE_TABLE_ENTRY  NIMUDeviceTable[2] = {{USBSerialPPP_NIMUInit}, {NULL}};
#else
/* Default: use Ethernet driver */
NIMU_DEVICE_TABLE_ENTRY  NIMUDeviceTable[2] = {{EMACSnow_NIMUInit}, {NULL}};
#endif

/* EMAC objects */
EMACSnow_Object emacObjects[Irrigation_Controller_EMACCOUNT];

/*
 *  EMAC configuration structure
 *  Set user/company specific MAC octates. The following sets the address
 *  to ff-ff-ff-ff-ff-ff. Users need to change this to make the label on
 *  their boards.
 */
unsigned char macAddress[6] = {0x00, 0x1a, 0xb6, 0x02, 0xa0, 0x41};//unsigned char macAddress[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

const EMACSnow_HWAttrs emacHWAttrs[Irrigation_Controller_EMACCOUNT] = {
    {EMAC0_BASE, INT_EMAC0, ~0 /* Interrupt priority */, macAddress}
};

const EMAC_Config EMAC_config[] = {
    {
        &EMACSnow_fxnTable,
        &emacObjects[0],
        &emacHWAttrs[0]
    },
    {NULL, NULL, NULL}
};

/*
 *  ======== Irrigation_Controller_initEMAC ========
 */
void Irrigation_Controller_initEMAC(void)
{
    uint32_t ulUser0, ulUser1;

    ulUser0=11934208;
    ulUser1=4300802;
    /* Get the MAC address */
    FlashUserSet(ulUser0, ulUser1);
    FlashUserGet(&ulUser0, &ulUser1);
    if ((ulUser0 != 0xffffffff) && (ulUser1 != 0xffffffff)) {
     //   System_printf("Using MAC address in flash\n");
        /*
         *  Convert the 24/24 split MAC address from NV ram into a 32/16 split MAC
         *  address needed to program the hardware registers, then program the MAC
         *  address into the Ethernet Controller registers.
         */
        macAddress[0] = ((ulUser0 >>  0) & 0xff);
        macAddress[1] = ((ulUser0 >>  8) & 0xff);
        macAddress[2] = ((ulUser0 >> 16) & 0xff);
        macAddress[3] = ((ulUser1 >>  0) & 0xff);
        macAddress[4] = ((ulUser1 >>  8) & 0xff);
        macAddress[5] = ((ulUser1 >> 16) & 0xff);
    }
    else if (macAddress[0] == 0xff && macAddress[1] == 0xff &&
             macAddress[2] == 0xff && macAddress[3] == 0xff &&
             macAddress[4] == 0xff && macAddress[5] == 0xff) {
      //  System_abort("Change the macAddress variable to match your boards MAC sticker");
    }

    GPIOPinConfigure(GPIO_PK5_EN0LED2);
    GPIOPinConfigure(GPIO_PK6_EN0LED1);
    GPIOPinTypeEthernetLED(GPIO_PORTK_BASE, GPIO_PIN_5 | GPIO_PIN_6);


    /* Once EMAC_init is called, EMAC_config cannot be changed */
    EMAC_init();
}


/*
 *  =============================== GPIO ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(GPIOTiva_config, ".const:GPIOTiva_config")
#endif

#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOTiva.h>

void keypad_interrupt(void);
/*
 * Array of Pin configurations
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in Irrigation_Controller.h
 * NOTE: Pins not used for interrupts should be placed at the end of the
 *       array.  Callback entries can be omitted from callbacks array to
 *       reduce memory usage.
 */
GPIO_PinConfig gpioPinConfigs[] = {

	/* Irrigation_Controller_keypad_IRQ */
    GPIOTiva_PP_0 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING,
	/* Irrigation_Controller_Display_CS1 */
	GPIOTiva_PP_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_Display_CS2 */
	GPIOTiva_PK_4 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_Display_RST */
	GPIOTiva_PN_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_Display_E */
	GPIOTiva_PA_6 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_Display_RW */
	GPIOTiva_PB_4 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_Display_RS */
	GPIOTiva_PA_2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_Display_DB0 */
	GPIOTiva_PM_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_Display_DB1 */
	GPIOTiva_PM_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_Display_DB2 */
	GPIOTiva_PM_2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_Display_DB3 */
	GPIOTiva_PM_3 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_Display_DB4 */
	GPIOTiva_PM_4 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_Display_DB5 */
	GPIOTiva_PM_5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_Display_DB6 */
	GPIOTiva_PM_6 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_Display_DB7 */
	GPIOTiva_PM_7 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_AI3 */
	GPIOTiva_PK_1 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_NONE,
	/* Irrigation_Controller_AI4 */
	GPIOTiva_PK_0 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_NONE,
	/* Irrigation_Controller_DI1 */
	GPIOTiva_PL_5 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_NONE,
	/* Irrigation_Controller_DI2 */
	GPIOTiva_PL_4 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_NONE,
	/* Irrigation_Controller_DI3 */
	GPIOTiva_PL_3 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_NONE,
	/* Irrigation_Controller_DI4 */
	GPIOTiva_PL_2 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_NONE,
	/* Irrigation_Controller_DO8 */
	GPIOTiva_PH_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_DO7 */
	GPIOTiva_PF_3 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_DO6 */
	GPIOTiva_PF_4 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_DO5 */
	GPIOTiva_PG_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_DO4 */
	GPIOTiva_PG_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_DO3 */
	GPIOTiva_PK_7 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_DO2 */
	GPIOTiva_PL_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_DO1 */
	GPIOTiva_PL_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_D16 */
	GPIOTiva_PN_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_D16 */
	GPIOTiva_PE_2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_D16 */
	GPIOTiva_PE_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_D16 */
	GPIOTiva_PE_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_D16 */
	GPIOTiva_PE_3 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_D16 */
	GPIOTiva_PK_3 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_D16 */
	GPIOTiva_PK_2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_D16 */
	GPIOTiva_PH_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_GSMAN */
	GPIOTiva_PD_2 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_NONE,
	/* Irrigation_Controller_GSMRST */
	GPIOTiva_PP_5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	/* Irrigation_Controller_Display_BL */
	GPIOTiva_PB_5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
	 /* Irrigation_Controller_BUZZER */
	GPIOTiva_PF_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,
	/* Irrigation_Controller_RS485EN */
	GPIOTiva_PQ_4 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
};

/*
 * Array of callback function pointers
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in Irrigation_Controller.h
 * NOTE: Pins not used for interrupts can be omitted from callbacks array to
 *       reduce memory usage (if placed at end of gpioPinConfigs array).
 */
GPIO_CallbackFxn gpioCallbackFunctions[] = {
		NULL  /* Irrigation_Controller_keypad_IRQ */
};

/* The device-specific GPIO_config structure */
const GPIOTiva_Config GPIOTiva_config = {
    .pinConfigs = (GPIO_PinConfig *) gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *) gpioCallbackFunctions,
    .numberOfPinConfigs = sizeof(gpioPinConfigs) / sizeof(GPIO_PinConfig),
    .numberOfCallbacks = sizeof(gpioCallbackFunctions)/sizeof(GPIO_CallbackFxn),
    .intPriority = (~0)
};

/*
 *  ======== Irrigation_Controller_initGPIO ========
 */
void Irrigation_Controller_initGPIO(void)
{
    /* Initialize peripheral and pins */
    GPIO_init();
}



/*
 *  =============================== I2C ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(I2C_config, ".const:I2C_config")
#pragma DATA_SECTION(i2cTivaHWAttrs, ".const:i2cTivaHWAttrs")
#endif

#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CTiva.h>

/* I2C objects */
I2CTiva_Object i2cTivaObjects[Irrigation_Controller_I2CCOUNT];

/* I2C configuration structure, describing which pins are to be used */
const I2CTiva_HWAttrs i2cTivaHWAttrs[Irrigation_Controller_I2CCOUNT] = {
    {I2C7_BASE, INT_I2C7, ~0 /* Interrupt priority */}
};

const I2C_Config I2C_config[] = {
    {&I2CTiva_fxnTable, &i2cTivaObjects[0], &i2cTivaHWAttrs[0]},
    {NULL, NULL, NULL}
};

/*
 *  ======== Irrigation_Controller_initI2C ========
 */
void Irrigation_Controller_initI2C(void)
{
    /* I2C7 Init */
    /*
     * NOTE: TI-RTOS examples configure pins PD0 & PD1 for SSI2 or I2C7.  Thus,
     * a conflict occurs when the I2C & SPI drivers are used simultaneously in
     * an application.  Modify the pin mux settings in this file and resolve the
     * conflict before running your the application.
     */
	/* Enable the peripheral */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C7);

	/* Configure the appropriate pins to be I2C instead of GPIO. */
	GPIOPinConfigure(GPIO_PA4_I2C7SCL);
	GPIOPinConfigure(GPIO_PA5_I2C7SDA);
	GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_4);
	GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_5);

	I2C_init();
}



/*
 *  =============================== SPI ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(SPI_config, ".const:SPI_config")
#pragma DATA_SECTION(spiTivaDMAHWAttrs, ".const:spiTivaDMAHWAttrs")
#endif

#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPITivaDMA.h>

/* SPI objects */
SPITivaDMA_Object spiTivaDMAObjects[Irrigation_Controller_SPICOUNT];
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(spiTivaDMAscratchBuf, 32)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=32
#elif defined(__GNUC__)
__attribute__ ((aligned (32)))
#endif
uint32_t spiTivaDMAscratchBuf[Irrigation_Controller_SPICOUNT];

/* SPI configuration structure */
const SPITivaDMA_HWAttrs spiTivaDMAHWAttrs[Irrigation_Controller_SPICOUNT] = {
    {
        SSI2_BASE,
        INT_SSI2,
        ~0,         /* Interrupt priority */
        &spiTivaDMAscratchBuf[0],
        0,
        UDMA_SEC_CHANNEL_UART2RX_12,
        UDMA_SEC_CHANNEL_UART2TX_13,
        uDMAChannelAssign,
        UDMA_CH12_SSI2RX,
        UDMA_CH13_SSI2TX
    }
};

const SPI_Config SPI_config[] = {
    {&SPITivaDMA_fxnTable, &spiTivaDMAObjects[0], &spiTivaDMAHWAttrs[0]},
    {NULL, NULL, NULL}
};

/*
 *  ======== Irrigation_Controller_initSPI ========
 */
void Irrigation_Controller_initSPI(void)
{
    /* SSI2 */
    /*
     * NOTE: TI-RTOS examples configure pins PD0 & PD1 for SSI2 or I2C7.  Thus,
     * a conflict occurs when the I2C & SPI drivers are used simultaneously in
     * an application.  Modify the pin mux settings in this file and resolve the
     * conflict before running your the application.
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
    GPIOPinConfigure(GPIO_PD3_SSI2CLK);
    GPIOPinConfigure(GPIO_PD2_SSI2FSS);
    GPIOPinConfigure(GPIO_PD1_SSI2XDAT0);
    GPIOPinConfigure(GPIO_PD0_SSI2XDAT1);

    GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 |
                                    GPIO_PIN_2 | GPIO_PIN_3);

    //Irrigation_Controller_initDMA();
    SPI_init();
}



/*
 *  =============================== UART ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(UART_config, ".const:UART_config")
#pragma DATA_SECTION(uartTivaHWAttrs, ".const:uartTivaHWAttrs")
#endif

#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTTiva.h>
//#include <ti/drivers/uart/UARTTivaDMA.h>
//
///* UART objects using DMA */
//UARTTivaDMA_Object uartTivaObjects[Irrigation_Controller_UARTCOUNT];
//
///* UART DMA configuration structure */
//const UARTTivaDMA_HWAttrs uartTivaHWAttrs[Irrigation_Controller_UARTCOUNT] = {
//    {/* Irrigation_Controller_UART0 */
//        UART0_BASE,
//        INT_UART0,
//        ~0,        /* Interrupt priority */
//        UDMA_CH8_UART0RX,
//        UDMA_CH9_UART0TX,
//    }
//};
//
//const UART_Config UART_config[] = {
//    {
//        &UARTTivaDMA_fxnTable,
//        &uartTivaObjects[0],
//        &uartTivaHWAttrs[0]
//    },
//    {NULL, NULL, NULL}
//};




/* UART objects */
UARTTiva_Object uartTivaObjects[Irrigation_Controller_UARTCOUNT];

unsigned char uartTivaRingBuffer[32];
unsigned char buf[20000];

/* UART configuration structure */
const UARTTiva_HWAttrs uartTivaHWAttrs[Irrigation_Controller_UARTCOUNT] = {
    {/* Irrigation_Controller_UART3 */
        .baseAddr = UART3_BASE,
        .intNum = INT_UART3,
        .intPriority = ~0,
        .flowControl = UART_FLOWCONTROL_NONE,
        .ringBufPtr  = buf,
        .ringBufSize = sizeof(buf)
    },
	{/* Irrigation_Controller_UART2 */
		.baseAddr = UART2_BASE,
		.intNum = INT_UART2,
		.intPriority = ~0,
		.flowControl = UART_FLOWCONTROL_NONE,
		.ringBufPtr  = uartTivaRingBuffer,
		.ringBufSize = sizeof(uartTivaRingBuffer)
	}
};

const UART_Config UART_config[] = {
    {
        &UARTTiva_fxnTable,
        &uartTivaObjects[0],
        &uartTivaHWAttrs[0]
    },
	{
		&UARTTiva_fxnTable,
		&uartTivaObjects[1],
		&uartTivaHWAttrs[1]
	},
    {NULL, NULL, NULL}
};

/*
 *  ======== Irrigation_Controller_initUART ========
 */
void Irrigation_Controller_initUART(void)
{
    /* Enable and configure the peripherals used by the UART3 */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
	GPIOPinConfigure(GPIO_PJ0_U3RX);
	GPIOPinConfigure(GPIO_PJ1_U3TX);
	GPIOPinTypeUART(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	 /* Enable and configure the peripherals used by the UART2 */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
	GPIOPinConfigure(GPIO_PD4_U2RX);
	GPIOPinConfigure(GPIO_PD5_U2TX);
	GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    /* Initialize the UART driver */
//#if TI_DRIVERS_UART_DMA
//    Irrigation_Controller_initDMA();
//#endif
    UART_init();
}


/*
 *  ======== Irrigation_Controller_initUSB ========
 *  This function just turns on the USB
 */
void Irrigation_Controller_initUSB(Irrigation_Controller_USBMode usbMode)
{
    Error_Block eb;
    Hwi_Params  hwiParams;

    /* Enable the USB peripheral and PLL */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);
    SysCtlUSBPLLEnable();

    /* Setup pins for USB operation */
    GPIOPinTypeUSBAnalog(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeUSBAnalog(GPIO_PORTL_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    /* Additional configurations for Host mode */
    if (usbMode == Irrigation_Controller_USBHOST) {
        /* Configure the pins needed */
        HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
        HWREG(GPIO_PORTD_BASE + GPIO_O_CR) = 0xff;
        GPIOPinConfigure(GPIO_PD6_USB0EPEN);
        GPIOPinTypeUSBDigital(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);

        /*
         *  USB bus fault is routed to pin PQ4.  We create a Hwi to allow us
         *  to detect power faults and recover gracefully or terminate the
         *  program.  PQ4 is active low; set the pin as input with a weak
         *  pull-up.
         */
    //    GPIOPadConfigSet(GPIO_PORTQ_BASE, GPIO_PIN_4,
    //                     GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    //    GPIOIntTypeSet(GPIO_PORTQ_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);
    //    GPIOIntClear(GPIO_PORTQ_BASE, GPIO_PIN_4);

        /* Create a Hwi for PQ4 pin. */
        Error_init(&eb);
        Hwi_Params_init(&hwiParams);
        Hwi_construct(&(usbBusFaultHwiStruct), INT_GPIOQ4,
                      Irrigation_Controller_usbBusFaultHwi, &hwiParams, &eb);
        if (Error_check(&eb)) {
            //System_abort("Couldn't construct USB bus fault hwi");
        }
    }
}



/*
 *  =============================== USBMSCHFatFs ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(USBMSCHFatFs_config, ".const:USBMSCHFatFs_config")
#pragma DATA_SECTION(usbmschfatfstivaHWAttrs, ".const:usbmschfatfstivaHWAttrs")
#endif

#include <ti/drivers/USBMSCHFatFs.h>
#include <ti/drivers/usbmschfatfs/USBMSCHFatFsTiva.h>

/* USBMSCHFatFs objects */
USBMSCHFatFsTiva_Object usbmschfatfstivaObjects[Irrigation_Controller_USBMSCHFatFsCOUNT];

/* USBMSCHFatFs configuration structure, describing which pins are to be used */
const USBMSCHFatFsTiva_HWAttrs usbmschfatfstivaHWAttrs[Irrigation_Controller_USBMSCHFatFsCOUNT] = {
    {INT_USB0, ~0 /* Interrupt priority */}
};

const USBMSCHFatFs_Config USBMSCHFatFs_config[] = {
    {
        &USBMSCHFatFsTiva_fxnTable,
        &usbmschfatfstivaObjects[0],
        &usbmschfatfstivaHWAttrs[0]
    },
    {NULL, NULL, NULL}
};

/*
 *  ======== Irrigation_Controller_initUSBMSCHFatFs ========
 */
void Irrigation_Controller_initUSBMSCHFatFs(void)
{
    /* Initialize the DMA control table */
    Irrigation_Controller_initDMA();

    /* Call the USB initialization function for the USB Reference modules */
    Irrigation_Controller_initUSB(Irrigation_Controller_USBHOST);
    USBMSCHFatFs_init();
}





/*
 *  =============================== Watchdog ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(Watchdog_config, ".const:Watchdog_config")
#pragma DATA_SECTION(watchdogTivaHWAttrs, ".const:watchdogTivaHWAttrs")
#endif

#include <ti/drivers/Watchdog.h>
#include <ti/drivers/watchdog/WatchdogTiva.h>

/* Watchdog objects */
WatchdogTiva_Object watchdogTivaObjects[Irrigation_Controller_WATCHDOGCOUNT];

/* Watchdog configuration structure */
const WatchdogTiva_HWAttrs watchdogTivaHWAttrs[Irrigation_Controller_WATCHDOGCOUNT] = {
    /* Irrigation_Controller_WATCHDOG0 with 1 sec period at default CPU clock freq */
    {
        WATCHDOG0_BASE,
        INT_WATCHDOG,
        ~0,           /* Interrupt priority */
        80000000
    },
};

const Watchdog_Config Watchdog_config[] = {
    {&WatchdogTiva_fxnTable, &watchdogTivaObjects[0], &watchdogTivaHWAttrs[0]},
    {NULL, NULL, NULL},
};

/*
 *  ======== Irrigation_Controller_initWatchdog ========
 *
 * NOTE: To use the other watchdog timer with base address WATCHDOG1_BASE,
 *       an additional function call may need be made to enable PIOSC. Enabling
 *       WDOG1 does not do this. Enabling another peripheral that uses PIOSC
 *       such as ADC0 or SSI0, however, will do so. Example:
 *
 *       SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
 *       SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG1);
 *
 *       See the following forum post for more information:
 *       http://e2e.ti.com/support/microcontrollers/stellaris_arm_cortex-m3_microcontroller/f/471/p/176487/654390.aspx#654390
 */
void Irrigation_Controller_initWatchdog(void)
{
    /* Enable peripherals used by Watchdog */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);

    /* Initialize the Watchdog driver */
    Watchdog_init();
}


/*
 *  =============================== WiFi ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(WiFi_config, ".const:WiFi_config")
#pragma DATA_SECTION(wiFiCC3100HWAttrs, ".const:wiFiCC3100HWAttrs")
#endif

#include <ti/drivers/WiFi.h>
#include <ti/drivers/wifi/WiFiCC3100.h>

/* WiFi objects */
WiFiCC3100_Object wiFiCC3100Objects[Irrigation_Controller_WIFICOUNT];

/* WiFi configuration structure */
const WiFiCC3100_HWAttrs wiFiCC3100HWAttrs[Irrigation_Controller_WIFICOUNT] = {
    {
    		GPIO_PORTF_BASE, /* IRQ port */
			GPIO_PIN_0,      /* IRQ pin */
			INT_GPIOF,       /* IRQ port interrupt vector */

			GPIO_PORTH_BASE, /* CS port */
			GPIO_PIN_2,      /* CS pin */

			GPIO_PORTC_BASE, /* WLAN EN port */
			GPIO_PIN_6       /* WLAN EN pin */
    }
};

const WiFi_Config WiFi_config[] = {
    {
        &WiFiCC3100_fxnTable,
        &wiFiCC3100Objects[0],
        &wiFiCC3100HWAttrs[0]
    },
    {NULL,NULL, NULL},
};

/*
 *  ======== Irrigation_Controller_initWiFi ========
 */
void Irrigation_Controller_initWiFi(void)
{
	/* Configure EN & CS pins to disable CC3100 */
	GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_2);
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6);
	GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_2, GPIO_PIN_2);
	GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0);

	/* Configure SSI2 for CC3100 */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
	GPIOPinConfigure(GPIO_PD3_SSI2CLK);
	GPIOPinConfigure(GPIO_PD1_SSI2XDAT0);
	GPIOPinConfigure(GPIO_PD0_SSI2XDAT1);
	GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3);

	/* Configure IRQ pin */
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA,
					 GPIO_PIN_TYPE_STD_WPD);
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_RISING_EDGE);

    SPI_init();
    Irrigation_Controller_initDMA();

    WiFi_init();
}



void adc_init()
{
	/* Enable ADC0 for Power Good Monitoring on PK0*/
	// Enable clock to ADC0.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	GPIOPinTypeADC(GPIO_PORTK_BASE, GPIO_PIN_0);

	/* Enable ADC1 for Battery Monitoring on PK1*/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
	GPIOPinTypeADC(GPIO_PORTK_BASE, GPIO_PIN_1);

	//SysCtlADCSpeedSet(SYSCTL_ADCSPEED_250KSPS);
	//ADCClockConfigSet(ADC0_BASE, ADC_CLOCK_SRC_PLL | ADC_CLOCK_RATE_FULL, 24);

	// Configure ADC0 Sample Sequencer 3 for processor trigger operation.
//	ADCSequenceDisable(ADC0_BASE, 3);
	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	//ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0);
	//ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH1);
	//ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH2);

	// Configure ADC0 sequencer 0 for a single sample of Chanel 16
	ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH16 | ADC_CTL_IE | ADC_CTL_END);

	// Enable the sequencer.
	ADCSequenceEnable(ADC0_BASE, 3);

	// Clear the interrupt bit for sequencer 3 to make sure it is not set before the first sample is taken.
	ADCIntClear(ADC0_BASE, 3);
//	ADCIntEnable(ADC0_BASE, 0);
//	IntEnable(INT_ADC0SS0_TM4C129);

	// Configure ADC1 Sample Sequencer 0 for processor trigger operation.
	//ADCSequenceDisable(ADC1_BASE, 3);
	ADCSequenceConfigure(ADC1_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	//ADCSequenceStepConfigure(ADC1_BASE, 0, 0, ADC_CTL_CH8);
	//ADCSequenceStepConfigure(ADC1_BASE, 0, 1, ADC_CTL_CH1);
	//ADCSequenceStepConfigure(ADC1_BASE, 0, 2, ADC_CTL_CH2);

	// Configure ADC0 sequencer 0 for a single sample of Chanel 17
	ADCSequenceStepConfigure(ADC1_BASE, 3, 0, ADC_CTL_CH17 | ADC_CTL_IE | ADC_CTL_END);

	// Enable the sequencer.
	ADCSequenceEnable(ADC1_BASE, 3);

	// Clear the interrupt bit for sequencer 3 to make sure it is not set before the first sample is taken.
	ADCIntClear(ADC1_BASE, 3);
//	ADCIntEnable(ADC1_BASE, 0);
//	IntEnable(INT_ADC0SS0_TM4C129);tEnable(INT_ADC1);
//	IntMasterEnable();
}


uint32_t read_power_status()
{
	uint32_t value[1];
	uint32_t new_value = 0;

	ADCProcessorTrigger(ADC0_BASE, 3);
	while(!ADCIntStatus(ADC0_BASE, 3, false))
	{
	}
	ADCIntClear(ADC0_BASE, 3);
	ADCSequenceDataGet(ADC0_BASE, 3, value);
	new_value = value[0];// + value[1] + value[2] + value[3];
	//new_value = new_value/4;

	ADCSequenceEnable(ADC0_BASE, 3);
	return new_value;
}

uint32_t read_battery_status()
{
	uint32_t value_b[1];
	uint32_t new_value_b = 0;

	ADCProcessorTrigger(ADC1_BASE, 3);
	while(!ADCIntStatus(ADC1_BASE, 3, false))
	{
	}
	ADCIntClear(ADC1_BASE, 3);
	ADCSequenceDataGet(ADC1_BASE, 3, value_b);
	new_value_b = value_b[0];// + value_b[1] + value_b[2] + value_b[3];
	//new_value_b = new_value_b/4;

	ADCSequenceEnable(ADC1_BASE, 3);
	return new_value_b;
}

uint32_t pwr_good=0,batt_mon=0;

void ADCHandler()
{
//	ADCSequenceDisable(ADC0_BASE, 3);
	pwr_good = read_power_status();
//	ADCSequenceEnable(ADC0_BASE, 3);

//	ADCSequenceDisable(ADC1_BASE, 3);
	batt_mon = read_battery_status();
//	ADCSequenceEnable(ADC1_BASE, 3);
}
