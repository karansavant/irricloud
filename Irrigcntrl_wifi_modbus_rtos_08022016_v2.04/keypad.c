/********************************************************************************/
//keypad.c
/********************************************************************************/

/* TI-RTOS Header files */
#include <stdio.h>				//$$NR$$//dated:23Aug15
#include <ti/drivers/I2C.h>
#include <xdc/runtime/System.h>

/* Example/Board Header files */
#include "Board.h"

/*******************************************************************/
extern void debug_logdata();			//$$NR$$//dated:23Aug15

extern volatile uint8_t	key_press_time_count;
extern int backlight_count;

extern char strmsglog[50];	//$$NR$$//dated:23Aug15
/*******************************************************************/
//
/*******************************************************************/
//For LEDs
uint8_t LED_Status = 0x00;

//For Keyboard

I2C_Handle i2c;
I2C_Params i2cParams;
I2C_Transaction i2cTransaction;

uint8_t
	txBuffer[10] = {0},
	rxBuffer[25] = {0};

volatile uint8_t							//variable for Keyboard
	key_scan_code = 0,
	key_press_detected = 0;


void keypad_interrupt(void);
/*******************************************************************/
//
/*******************************************************************/

void keypadi2c_callback(I2C_Handle p1, I2C_Transaction *p2, bool p3){

}



void set_write_mode(void)
{
	i2cTransaction.slaveAddress = Board_TCA8424_ADDR;
	i2cTransaction.writeBuf = txBuffer;
	i2cTransaction.writeCount = 5;
	i2cTransaction.readBuf = 0;
	i2cTransaction.readCount = 0;
}

/*******************************************************************/
//
/*******************************************************************/

void set_read_mode(void)
{
	i2cTransaction.slaveAddress = Board_TCA8424_ADDR;
	i2cTransaction.writeBuf = 0;
	i2cTransaction.writeCount = 0;
	i2cTransaction.readBuf = rxBuffer;
	i2cTransaction.readCount = 11;
}

/*******************************************************************/
//
/*******************************************************************/

void keypad_read_status(void)
{
	I2C_transfer(i2c, &i2cTransaction);
}

/*******************************************************************/
// TO DO: need to modify
/*******************************************************************/
/*
void keypad_send_command(uint8_t data_byte)
{
	txBuffer[0] = 0x00;
	txBuffer[1] = 0x05;
	txBuffer[2] = 0x03;
	txBuffer[3] = 0x00;
	txBuffer[4] = data_byte;

	set_write_mode(txBuffer);	//for changing LED State, set keyboard to write mode

	I2C_transfer(i2c, &i2cTransaction);

	set_read_mode();	//by default, set keyboard to read mode
}
*/

/*******************************************************************/
//
/*******************************************************************/

void Board_initkeypad (void)
{
//	Board_initI2C();

	/* Initialize interrupts for keypad IRQ (PP0) */
//	GPIO_setupCallbacks(&Board_keypad_callback);

	/* Enable interrupts */
	//GPIO_enableInt(Board_keypad_IRQ, GPIO_INT_FALLING);
	  /* install Button callback */
	    GPIO_setCallback(Board_keypad_IRQ, keypad_interrupt);

	    /* Enable interrupts */
	    GPIO_enableInt(Board_keypad_IRQ);
	//Create I2C for usage
	I2C_Params_init(&i2cParams);
	i2cParams.bitRate = I2C_100kHz;
	i2cParams.transferMode = I2C_MODE_CALLBACK;
	i2cParams.transferCallbackFxn = keypadi2c_callback;

	i2c = I2C_open(Board_I2C_TMP, &i2cParams);

	if (i2c == NULL) {
	//	System_abort("Error Initializing I2C\n");
	} else {
		//System_printf("I2C Initialized!\n");
	}

	set_read_mode();
}

/*******************************************************************/
//
/*******************************************************************/
//led_control(LED1, ON);

void led_control(uint8_t led_no, uint8_t led_state)
{
	LED_Status &= ~(1 << led_no);
	if (led_state)	LED_Status |= (1 << led_no);;

	txBuffer[0] = 0x00;
	txBuffer[1] = 0x05;
	txBuffer[2] = 0x03;
	txBuffer[3] = 0x00;
	txBuffer[4] = LED_Status;

	set_write_mode();			// for changing LED State, set keyboard to write mode

	I2C_transfer(i2c, &i2cTransaction);

//	System_printf("\nLED State Changed for LED%d = %d\n", led_no, led_state);

	/* SysMin will only print to the console when you call flush or exit */
//	System_flush();

	set_read_mode();			// by default, set keyboard to read mode
}

/*******************************************************************/
//
/*******************************************************************/
int i=0;
void keypad_interrupt(void)
{
	GPIO_clearInt(Board_keypad_IRQ);
//	GPIO_write(Board_DO4, (0));	//on
	keypad_read_status();

	if (key_press_detected <= 0x01)
	{
		if (rxBuffer[5] == 0)
		{
			key_press_detected = 0x01;
			//$$NR$$//dated:23Aug15
			key_press_time_count = 10;			// time count of 1 sec for long key press (lower to upper case convertion)
			//$$NR$$//dated:23Aug15
		}
		else
		{
			key_scan_code = rxBuffer[5];
			key_press_detected = 0x02;
			if (key_press_time_count == 0)		key_press_detected |= 0x04;
		}
	}
	backlight_count=1;
//	GPIO_write(Board_DO4, (~0));	//off
	//$$NR$$//dated:23Aug15
//	sprintf(strmsglog, "KeyISR- %d, %d\r\n", key_press_detected, rxBuffer[5]);
//	debug_logdata();
	//$$NR$$//dated:23Aug15
/*
	if ((key_press_detected == 0) && (rxBuffer[5]))
	{
		key_scan_code = rxBuffer[5];
//		i++;//
//		if (key_scan_code)
			key_press_detected = 1;
	}
*/
}

/********************************************************************************/
