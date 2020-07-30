/********************************************************************************/
//main.c
/********************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <inc/hw_memmap.h>
//#include <inc/hw_nvic.h>
//#include <inc/hw_types.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include "driverlib/UART.h"
#include <ti/drivers/I2C.h>
#include <ti/drivers/Watchdog.h>
#include <ti/drivers/USBMSCHFatFs.h>
/* Example/Board Header files */
#include "Board.h"
#include "keypad.h"

//#include "PLL.h"
#include "IRQ.h"
#include "GLCD.h"

#include "GlobalConst.h"		/* Global Constants prototypes     	  */
#include "keycodes.h"			/* Key codes Constants           	  */
#include "GLCDConst.h"			/* Global Constants prototypes     	  */
#include "database_struct.h"

/* SimpleLink Wi-Fi Host Driver Header files */
#include <simplelink.h>
/* Local Platform Specific Header file */
#include "sockets.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include <modbus\mb.h>
#include <modbus\mb_m.h>

/********************************************************************************/

#define panel_initialsetup_done	1
#define RS485_BRATE     	9600    //19200

#define SENSOR_SCANNING_FREQ    10      //1 tick = 100msec      //$$NR$$//dated:6Feb16

/* Port number for listening for TCP packets */

#define LOGFILE_BUFF_SIZE       20000

/******************************************************************************/
// Functions used for Global purpose //
extern void ShowInitialSetupScreen();
extern void SetInitialSetup();

extern void ShowHomeMode(void);
extern void HomeStatus(uint8_t key_code);
extern void RefreshHomeScreen(void);

extern void ShowPasswordScreen();
extern void GetPassword(uint8_t key_code);

extern void AccessMode(uint8_t key_code);				// defined in Menubar.c

extern void rtc_init(void);
extern void get_updated_datetime(void);

extern void eeprom_init(void);
extern void eeprom_test(void);
extern bool panel_setup_status(void);
extern void eeprom_read_all_data(void);

extern void auto_mode();								// defined in Program.c
extern void manual_mode();								// defined in Program.c
//extern void Backflush_program();

extern void message_store();
extern int GSM_Activate();
extern int GSM_Configure();

extern void GSM_EEPROM_UPDATE();
extern void echoFxn(int port);
extern void GSM_Main();
extern void init_gsm();

extern void WIFI_Main();

extern void Write_log();
extern void debug_logdata();					//$$NR$$//dated:23Aug15
extern void Read_log();
extern void USBlog_init();
extern void Remaintime_update();
extern void Starttime_Shift();
//Modbus Externs
extern void SysMonitor(void);

/********************************************************/
// Functions used for Local purpose //
Void GUI_Task(UArg arg0, UArg arg1);
Void Program_Task(UArg arg0, UArg arg1);
Void GSM_Task(UArg arg0, UArg arg1);
Void Modbus_Task(UArg arg0, UArg arg1);
void Board_init(void);
void Driver_init(void);
void wtcdog_init(void);

void KeyboardHandler(void);
void Mode_Action(void);

/********************************************************************************/
extern char wireless_mode;								// 0=Local, 1=Internet
extern const int SPAWN_TASK_PRI;/* Spawn Task Priority */
extern char deviceid[11];
extern uint8_t localaccessflag;
extern volatile bool CheckRTC;
extern userdef_tm	CurTime, SetTime;
extern int32_t Menu_Index;

extern volatile uint16_t Mod_Req_Timecnt;		//$$NR$$//dated:19Sep15
extern BOOL	xMasterRunRes;						//$$NR$$//dated:19Sep15

// Variables used for Keyboard scanning
extern volatile uint8_t					//variable for Keyboard
key_scan_code,								// defined in main.c
		key_press_detected;							// defined in main.c

extern volatile uint8_t
	activity_timecnt_gsm_task,					//$$NR$$//dated:23Aug15
	activity_timecnt_gui_task,					//$$NR$$//dated:23Aug15
	activity_timecnt_program_task;				//$$NR$$//dated:23Aug15

extern volatile bool typingdelay_elapsed_flag;

extern char message[8][30];	//for temporary storing the message
extern char temp_ip[30];
extern char input_buffer[1000];
extern char ouput_buffer[20000];
extern UART_Handle uart;

extern bool editing_text_alpha_flag,
	   numkey_pressed_flag;

//$$NR$$//dated:21Aug15
extern volatile bool cursorflag;					//$$NR$$//dated:21Aug15

extern char edit_tempstr[22];

extern uint8_t
	fun_line_addr,
	fun_col_addr,
	GetFunFlag,
	//	textbox_char_count,			//$$NR$$//dated:07Nov15
	tool_type;			// 1=listbox, 2=textbox_numeric, 3=textbox_alphanumeric
//$$NR$$//dated:21Aug15

//$$NR$$//dated:21Aug15
extern volatile uint16_t
	typingdelay_count;

//$$NR$$//dated:07Nov15
extern uint8_t
	textbox_char_encoded_trancated;		// bit0 - 0/1 - text string trancating = no / yes
										// bit1 - 0/1 - char encoding = yes / no
										// bit2 - 0/1 - check data validity = no / yes
										// bit3 - 0/1 - cursor position = start / end
										// bit4 - 0/1 - data flush at editing start = no / yes
extern uint8_t test_in_progress;
//$$NR$$//dated:07Nov15


//$$NR$$//dated:23Aug15
extern uint8_t
	textbox_char_count,
	textbox_size;

extern uint8_t
	prev_key_code,
	numkey_press_cnt;
//$$NR$$//dated:23Aug15

//$$NR$$//dated:21Aug15
extern char portal_ip[4];
extern char dial1[200];//socket dial

extern char cpy_buff[20000];
extern bool deviceConnected;
extern bool ipAcquired;
extern char fileusb[50];

extern char wifi_module; // 0=Disconnected, 1=Connected
extern char gsm_module; // 0=Disconnected, 1=Connected

extern char strmsglog[50];	//$$NR$$//dated:23Aug15

extern char currentday;

extern char bf_valonoff[4];

extern int shift;//time shift due to power failure
extern char interrupted_prog;//0 is no prog interrupted else the prog no value
uint8_t selected_ps_keycode = 0;

bool
	water_on_off_keystate = false,		//ON/OFF - Water OFF / Water ON
	cursorstate = false;		//$$NR$$//dated:21Aug15

//$$NR$$//dated:6Feb16
// variable defined in modbus
extern USHORT  mb_ippressure_curvalue;
extern USHORT  mb_oppressure_curvalue;
extern USHORT  mb_diffpressure_curvalue;
extern USHORT  mb_temperature_curvalue;
extern USHORT  mb_humidity_curvalue;
extern USHORT  mb_sensor_errors;

extern UCHAR  mb_rain_curvalue;
extern UCHAR  mb_sensor_error_bit;
extern UCHAR  mb_limit_status_on_dP;

// variable defined in menubar
extern uint16_t ippressure_curvalue;
extern uint16_t oppressure_curvalue;
extern uint16_t diffpressure_curvalue;
extern uint16_t temperature_curvalue;
extern uint16_t humidity_curvalue;
extern uint16_t sensor_errors;

extern uint8_t rain_curvalue;
extern uint8_t sensor_error_bit;
extern uint8_t limit_status_on_dP;

//$$NR$$//dated:6Feb16

/********************************************************************************/

volatile uint8_t prev_access_level = 1, access_level = 0,// by default, Normal Mode
		PanelState = 0;								// by default, Home

uint8_t key_code = 0;								//variable for Keyboard

int flag_30s = 1;

int backlight_count = 1;

uint32_t tempmsgid = 0;
uint32_t noofmsg = 0;
int gsm_activ = 0;//gsm connection active to send data ie connected to internet
int gsm_config = 0;//gsm connection configured
int wifi_activ = 0;//wifi connection active to send data ie connected to internet
int wifi_config = 0;//wifi connection configured

uint8_t remote_data_recevied_flag = 0;
void *netIF;
UART_Params uartParams;

int taskactive = 0;
int logflag = 0;
int logsend = 0;

char str[20] = { 0 }, len[10] = { 0 }, temp[100] = { 0 };

int PrevPanelState = 1;
char synccount = 1;
char syncsubcount = 0;
char commu_on = 0;							//1 if GSM Module in connect mode
char sendack = 0;
char system_rst = 0;
int smartphone_access=0;

char msgtoportal[200]={0};
char logtoportal[200]={0};
char msgfromportal[200]={0};
char msgfromportal1[200]={0};

//****************************************************************************
// System clock rate in Hz.
//****************************************************************************
uint32_t g_ui32SysClock;

/*
 *  ======== callback ========
 *  Watchdog interrupt callback function. It clears the watchdog interrupt flag.
 */
Void Watchdog_isr(UArg handle) {
	Watchdog_clear((Watchdog_Handle) handle);
}
/********************************************************************************/
// ======== main ========
/********************************************************************************/
 int main(void) {

	char ip[50]={0};

	Board_init();

	Board_initkeypad();

	wtcdog_init();

	eeprom_read_all_data();

	//create msg to and from portal
	sprintf (dial1,"AT#SD=1,0,80,\"%d.%d.%d.%d\",0,0,0\r",portal_ip[3],portal_ip[2],portal_ip[1],portal_ip[0]);
	sprintf (ip,"%d.%d.%d.%d",portal_ip[3],portal_ip[2],portal_ip[1],portal_ip[0]);

	memset(msgtoportal,0, sizeof(msgtoportal));
	strcat (msgtoportal,"POST http://");
	strcat (msgtoportal,ip);
	strcat (msgtoportal,"/IOT_Local/recivedata.php HTTP/1.0\r\nContent-Type: application/x-www-form-urlencoded;charset=utf-8\r\nContent-Length: ");

	memset(logtoportal,0, sizeof(logtoportal));
	strcat (logtoportal,"POST http://");
	strcat (logtoportal,ip);
	strcat (logtoportal,"/IOT_Local/recivelogsdata.php HTTP/1.0\r\nContent-Type: application/x-www-form-urlencoded;charset=utf-8\r\nContent-Length: ");

	memset(msgfromportal,0, sizeof(msgfromportal));
	strcat (msgfromportal,"POST http://");
	strcat (msgfromportal,ip);
	strcat (msgfromportal,"/IOT_Local/sendmessage_wifi.php HTTP/1.0\r\nContent-Type: application/x-www-form-urlencoded;charset=utf-8\r\nContent-Length: ");

	memset(msgfromportal1,0, sizeof(msgfromportal1));
	strcat (msgfromportal1,"POST http://");
	strcat (msgfromportal1,ip);
	strcat (msgfromportal1,"/IOT_Local/sendmessage.php HTTP/1.0\r\nContent-Type: application/x-www-form-urlencoded;charset=utf-8\r\nContent-Length: ");

	//wireless_mode = 0;
	/* Init Wifi Task*/
	if (wireless_mode != 2) {
		/*Reset The GSM Module*/
	//	GPIO_write(Board_GSMRST, (~0));					//LOGIC 0---RST ON
		VStartSimpleLinkSpawnTask(SPAWN_TASK_PRI);
	}
	/* Start BIOS */
	BIOS_start();

	return (0);
}

/********************************************************************************/
/*  							Initialisation									*/
/********************************************************************************
 * Function Name  : Board_init
 * Description    : Intialisation of Processor and all peripherals.
 * input_buffer          : None
 * Output         : None
 * Return         : None
 *********************************************************************************/

void wtcdog_init(void) {
	Watchdog_Params params;
	Watchdog_Handle watchdog;

	/* Create and enable a Watchdog with resets enabled */
	Watchdog_Params_init(&params);
	params.callbackFxn = Watchdog_isr;
	params.resetMode = Watchdog_RESET_ON;

	watchdog = Watchdog_open(Board_WATCHDOG0, &params);
	if (watchdog == NULL) {
		//System_abort("Error opening Watchdog!\n");
	}
}

/********************************************************************************/
/*  							Initialisation									*/
/********************************************************************************
 * Function Name  : Board_init
 * Description    : Intialisation of Processor and all peripherals.
 * input_buffer          : None
 * Output         : None
 * Return         : None
 *********************************************************************************/

void Board_init(void) {
	/* Call board init functions */
	Board_initGeneral();
	Board_initGPIO();
	Board_initI2C();
	Board_initUART();
	Board_initWatchdog();
	Board_initWiFi();
	Board_initUSBMSCHFatFs();
	rtc_init();
	eeprom_init();
	adc_init();
//	eeprom_test();
	//System_printf("\nStarting Irrigation Controller Program...\n");
	/* SysMin will only print to the console when you call flush or exit */
	//System_flush();
}

/********************************************************************************/
/*  							Initialisation									*/
/********************************************************************************
 * Function Name  : Init_Processor
 * Description    : Intialisation of all peripherals.
 * input_buffer          : None
 * Output         : None
 * Return         : None
 *********************************************************************************/

void Driver_init(void) {
	bool panel_eeprom_status = false;

	keypad_read_status();

	GPIO_write(Board_Display_BL, (~0));								//LOGIC 1
	//GPIO_write(Board_BUZZER,(~0));//LOGIC 1



	GLCD_Init();
	set_start_line(3, 0);

	GLCD_clear_screen(false);	// clear the lcd display

	// Display Welcome Screen
	GLCD_write_titlebar("WELCOME");
	goto_screen_map_yz(0, SCRN_MAP_CUR_Z + 10);
	GLCD_WriteString("    INITIALIZING    ", false);
	goto_screen_map_yz(0, SCRN_MAP_CUR_Z);
	GLCD_WriteString("  PLEASE WAIT . . . ", false);

	GLCD_Screen_Update();

	// Delay for a bit.
	//delay_sec(2);								// approximately 500 ms
	Task_sleep(2000);

	led_control(LED_POWER, ON);		// Turn on the Power LED
	// set flags to by default values
	access_level = alNorm;						// by default, Normal Mode
	prev_access_level = alNoMode;				// by default, No Mode

	panel_eeprom_status = panel_setup_status();

	if (panel_eeprom_status) {
		eeprom_read_all_data();

		PanelState = psHome;					// by default, Home
		CheckRTC = 1;
	} else
		PanelState = psInitSetup;				// by default, InitSetup

}

/********************************************************************************/
/*  							Backlight Control								*/
/********************************************************************************
 * Function Name  : backlight_control
 * Description    : Control of LCD Backlight
 * input_buffer   : None
 * Output         : None
 * Return         : None
 *********************************************************************************/
void backlight_control() {
	if (backlight_count <= 3000)				//30Sec timeout
			{
		GPIO_write(Board_Display_BL, (~0));		//LOGIC 1//switch on backlight
	} else {
		backlight_count = 13000;
		GPIO_write(Board_Display_BL, (0));		//LOGIC 0//switch off backlight
	}
}

bool keypad_init_flag = true;

/******************************************************************
 * Function Name  : GUI_Task
 * Description    : RTOS Task that handles Display and EEPROM related
 * 				   Functions
 * input_buffer   : None
 * Output         : None
 * Return         : None
 *******************************************************************/
Void GUI_Task(UArg arg0, UArg arg1)
{
	//Task_sleep(2000);
	Task_sleep(500);

	Driver_init();

	if (PanelState == psInitSetup) {
		ShowInitialSetupScreen();
		eeprom_read_all_data();
	}
	while (1) {
		//GPIO_write(Board_DO1, (0));	//on
		backlight_control();
		get_updated_datetime();
		if (smartphone_access == 0) {
			KeyboardHandler();			// Function used for action on Key press
			RefreshHomeScreen();// Function used for Refreshing Date, Time, Mode and status
		} else {
			GPIO_write(Board_Display_BL, (~0));					//LOGIC 1
			//GPIO_write(Board_BUZZER,(~0));//LOGIC 1

			led_control(LED_POWER, ON);		// Turn on the Power LED

			GLCD_Init();
			set_start_line(3, 0);

			GLCD_clear_screen(false);	// clear the lcd display

			// Display Welcome Screen
			GLCD_write_titlebar("SMARTPHONE ACCESS");
			goto_screen_map_yz(0, SCRN_MAP_CUR_Z + 10);
			GLCD_WriteString("  DISCONNECT PHONE  ", false);
			goto_screen_map_yz(0, SCRN_MAP_CUR_Z);
			GLCD_WriteString(" TO ACTIVATE KEYPAD ", false);

			GLCD_Screen_Update();

			while (smartphone_access == 1)
			{
				get_updated_datetime();
				Task_sleep(20);
			}
		}

	//	GPIO_write(Board_DO1, (~0));	//off
		Task_sleep(20);
	}
}

extern void eeprom_write_progstatus(uint8_t parameter_index);
extern void eeprom_read_progstatus(uint8_t parameter_index);
extern uint32_t pwr_good,batt_mon;
/******************************************************************
 * Function Name  : Program_Task
 * Description    : Task to implement irrigation program functionality
 * 				   and data logging to USB
 * input_buffer   : None
 * Output         : None
 * Return         : None
 *******************************************************************/
Void Program_Task(UArg arg0, UArg arg1) {
	int power_flag=1;
	USBlog_init();				//USB Flash Drive Initialization
//	Remaintime_update();
//	eeprom_write_progstatus(0xff);//store program status to eeprom
	eeprom_read_progstatus(0xff);//read program status from eeprom??
	while (1)
	{
		//GPIO_write(Board_DO2, (0));	//on

		get_updated_datetime();

		if(CurTime.tm_mday!=currentday)
		{///at day change logic
			currentday = CurTime.tm_mday;
			Remaintime_update();
			eeprom_write_progstatus(0xff);//store program status to eeprom
			eeprom_read_progstatus(0xff);//read program status from eeprom??
			logsend = 0;
		}

		ADCHandler();		//Read ADC Values

		if(pwr_good < 900)//ie mains power not ok
		{
			if(power_flag == 0)
			{
				eeprom_write_progstatus(0xff);//store program status to eeprom
				eeprom_read_progstatus(0xff);//read program status from eeprom??
				power_flag = 1;
				bf_valonoff[0]=0;	bf_valonoff[1]=0;	bf_valonoff[2]=0;	bf_valonoff[3]=0;
			}
			//hibernate
		}
		else
		{
			if(power_flag == 1)
			{
				eeprom_read_progstatus(0xff);//read program status from eeprom
				power_flag = 0;
				Starttime_Shift();
			}
			auto_mode();		//Auto Mode Implementation

			manual_mode();		//Manual Mode Implementation

			shift=0;

			if((interrupted_prog!=0)&&(pwr_good > 1100))
			{
				interrupted_prog =0;
				eeprom_write_progstatus(0xff);//store program status to eeprom
				eeprom_read_progstatus(0xff);//read program status from eeprom??
			}


			//Backflush_program();

			//GSM_EEPROM_UPDATE();		// Func used to update the values in eeprom

		}


		if (logflag == 0) {
			Write_log();
			logflag = 1;
		}
		GSM_EEPROM_UPDATE();		// Func used to update the values in eeprom
	//	GPIO_write(Board_DO2, (~0));	//off
		Task_sleep(20);
	}
}
/******************************************************************************/

/************************************************************************
 * Function Name  : GSM_Task
 * Description    : Task to implement GSM based communication to Portal
 * input_buffer   : None
 * Output         : None
 * Return         : None
 *************************************************************************/

Void GSM_Task(UArg arg0, UArg arg1)
{
	Task_sleep(2000);
	if (wireless_mode == 2) {
		init_gsm();
	} else {
//		/*Reset The Module*/
		GPIO_write(Board_GSMRST, (~0));					//LOGIC 0---RST ON
		/* Open WiFi and await a connection */
//		for(c=0;c<10;c++)
//		{
//			Task_sleep(2000);
//		}

		netIF = socketsStartUp();
	}
	while (1) {
		//GPIO_write(Board_DO3, (0));	//on
		if (wireless_mode == 2) {
			GSM_Main();
		} else {
			WIFI_Main();
			Task_sleep(1);
		}
		//in program task if the task is stuck executing a program values were not updated. so eeprom update is called here also
		GSM_EEPROM_UPDATE();		// Func used to update the values in eeprom

		//GPIO_write(Board_DO3, (~0));	//off
	}
}
/******************************************************************************/

/************************************************************************
 * Function Name  : Modbus_Task
 * Description    : Task to implement Modbus Commmunication
 * input_buffer   : None
 * Output         : None
 * Return         : None
 *************************************************************************/

Void Modbus_Task(UArg arg0, UArg arg1)
{
	// Initialize protocol stack in RTU mode for a Master
	eMBMasterInit(g_ui32SysClock, RS485_BRATE,  UART_CONFIG_PAR_NONE);//UART_CONFIG_PAR_ODD );//MB_PAR_EVEN);

    /* Enable the Modbus Protocol Stack. */
	eMBMasterEnable(  );

    // Enable processor interrupts.
	IntMasterEnable();

	while(1)
	{
		get_updated_datetime();
		if (Mod_Req_Timecnt == 0)
		{
			Mod_Req_Timecnt = SENSOR_SCANNING_FREQ;

			SysMonitor();
		}
//$$NR$$//dated:6Feb16
		else if (SENSOR_SCANNING_FREQ - 3)  //after sending request, wait for 300msec & then get data
		{
		    //update sensor data status
		    ippressure_curvalue = mb_ippressure_curvalue;
		    oppressure_curvalue = mb_oppressure_curvalue;
		    diffpressure_curvalue = mb_diffpressure_curvalue;
		    temperature_curvalue = mb_temperature_curvalue;
		    humidity_curvalue = mb_humidity_curvalue;
		    sensor_errors = mb_sensor_errors;

		    rain_curvalue = mb_rain_curvalue;
		    sensor_error_bit = mb_sensor_error_bit;
		    limit_status_on_dP = mb_limit_status_on_dP;
		}
//$$NR$$//dated:6Feb16

		// Call the main polling loop of the Modbus Master protocol stack.
		eMBMasterPoll();

		if (xMasterRunRes)
 			Task_sleep(2);
 		else
			Task_sleep(20);
	}
}


/*******************************************************************************
 * Function Name  : KeyboardHandler
 * Description    : Get the keycode for pressed key and then take the action
 Also check timeouts of different functions
 * input_buffer          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/

void KeyboardHandler(void) {
//declared as global variables before defining main() function
//uint16_t tempsec = 0;

	/*
	 // check if access level time out
	 if ((access_level_timeout & 0x01) == 0x01)
	 {
	 access_level_timeout &= 0xFE;

	 if (access_level != alNorm)
	 {
	 access_level = alNorm;									// if Menubar Access Timer over, set Access Level to Normal state
	 BuzzerStatus = 0x01;									// Enable Buzzer

	 numkey_pressed_flag = false;	editing_text_alpha_flag = false;	typingdelay_elapsed_flag = false;
	 }
	 else if (PanelState == psPassword)
	 {
	 key_scan_code = kbEsc;									// send Auto 'Esc' Key press event, for processing
	 key_press_detected = 1;
	 }
	 }
	 */
	/*
	 // check if is in character editing mode, then change cursor position
	 if ((key_press_detected == 0) && (numkey_pressed_flag) && (editing_text_alpha_flag) && (typingdelay_elapsed_flag))
	 {
	 key_scan_code = kbRight;									// send Auto Right Key press event, for processing
	 key_press_detected = 1;
	 }
	 */

	//$$NR$$//dated:30Aug15
		// display cursor
	//	if ((PanelState >= psSetup) && (PanelState <= psDiagnostic))
		if ((PanelState >= psPassword) && (PanelState <= psDiagnostic))
		{
			if ((GetFunFlag) && ((tool_type == 2) || (tool_type == 3)) && (cursorflag))
			{
				//move to char position after typing delay timeout
				if ((tool_type == 3) && (typingdelay_elapsed_flag))
				{
	//				if (key_press_detected & 0x02)
					{
						typingdelay_elapsed_flag = false;	typingdelay_count = 0;

						numkey_press_cnt = 0;		prev_key_code = 0;
						if ((numkey_pressed_flag) && ((textbox_char_count + 1) < textbox_size))
						{
							goto_screen_map_yz(fun_col_addr+2+(textbox_char_count*6), fun_line_addr);

							if ((textbox_char_encoded_trancated & 0x02) && (edit_tempstr[textbox_char_count] != ' '))
								GLCD_writesymbol((uint8_t)'*', true);
							else
								GLCD_writesymbol((uint8_t)(edit_tempstr[textbox_char_count]), true);

	//						GLCD_MsgLine_Update(fun_col_addr+2, fun_line_addr, fun_col_addr+8, fun_line_addr+10);

							textbox_char_count += 1;
						}

						numkey_pressed_flag = false;
					}
				}

				cursorflag = false;

				goto_screen_map_yz(fun_col_addr+2+(textbox_char_count*6), fun_line_addr);

				if ((textbox_char_encoded_trancated & 0x02) && (edit_tempstr[textbox_char_count] != ' '))
					GLCD_writesymbol((uint8_t)'*', cursorstate);
				else
					GLCD_writesymbol((uint8_t)(edit_tempstr[textbox_char_count]), cursorstate);

	//			GLCD_MsgLine_Update(fun_col_addr+2, fun_line_addr, fun_col_addr+8, fun_line_addr+10);
				GLCD_MsgLine_Update(fun_col_addr+2, fun_line_addr, 126, fun_line_addr+10);

				if (cursorstate)
					cursorstate = false;
				else
					cursorstate = true;
			}
		}
		//$$NR$$//dated:30Aug15

		//$$NR$$//dated:07Nov15
		if (key_press_detected & 0x10)
		{
			if (test_in_progress == 1)
				key_scan_code = kbVirtualTest;
		}
		//$$NR$$//dated:07Nov15

	// check flag of key press detected, else exit
	if ((key_press_detected & 0x06) == 0)
		return;

//	key_press_detected = 0x10;
	key_press_detected <<= 0x04;
//	System_printf("\nScan Code = %d\n\r",key_scan_code);
//	System_flush();

	key_code = key_scan_code;					// Get Keycode of pressed key

	//$$NR$$//dated:23Aug15
//	sprintf(strmsglog, "KeyHandle- %d\r\n", key_code);
//	debug_logdata();
	//$$NR$$//dated:23Aug15

	switch (key_code) {
	case kbManual:		// System Operating Keys
	case kbProgram:
	case kbLogs:
	case kbDiagnostic:
	case kbHome:
	case kbSetup:

	case kbLeft:
	case kbRight:
	case kbUp:
	case kbDown:
	case kbEnter:
	case kbDot:
	case kbClear:
		break;

	case kbWaterOnOff:
		water_on_off_keystate = ~(water_on_off_keystate);
		//todo remove it from here
		//use to reset the controller using software
		//HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
		break;

	case kbNum0:
		key_code = 0x30;
		break;
	case kbNum1:
		key_code = 0x31;
		break;
	case kbNum2:
		key_code = 0x32;
		break;
	case kbNum3:
		key_code = 0x33;
		break;
	case kbNum4:
		key_code = 0x34;
		break;
	case kbNum5:
		key_code = 0x35;
		break;
	case kbNum6:
		key_code = 0x36;
		break;
	case kbNum7:
		key_code = 0x37;
		break;
	case kbNum8:
		key_code = 0x38;
		break;
	case kbNum9:
		key_code = 0x39;
		break;
	default:
		return;
	}

	if (key_code != kbNoKey)
		Mode_Action();						// Function for Action

	key_press_detected = 0;
}

/********************************************************************************/

/******************************************************************
 * Function Name  : Mode_action
 * Description    : Take the Actions as per selected mode
 * input_buffer   : None
 * Output         : None
 * Return         : None
 *******************************************************************/

void Mode_Action(void) {
	// check if num key is pressed of Password Entry for accessing higher mode
	if ((access_level == alNorm) && (PanelState == psHome)
			&& ((key_code == kbSetup) || (key_code == kbManual)
					|| (key_code == kbProgram) || (key_code == kbLogs)
					|| (key_code == kbDiagnostic))) {
		selected_ps_keycode = key_code;

		ShowPasswordScreen();
	}
//	else if ((PanelState == psHome) && (EvtPriorityState) && ((key_code == kbDown) || (key_code == kbUp)))	// check if Panel is in normal mode..
//	{																										// check if Up/Down key pressed and there are Current events
//		ShowHomeMessages ();
//	}
	else if ((PanelState == psHome)
			&& ((key_code == kbDown) || (key_code == kbUp) || (key_code == kbHome)
					|| (key_code == kbLeft) || (key_code == kbRight)))// check if Panel is in normal mode..
			{						// check if Up/Down/Left/Right key pressed
		HomeStatus(key_code);
	}

	// check if Panel is in password entry mode..
	if (PanelState == psPassword) {
		//$$NR$$//dated:15Aug15
		if (key_code == kbHome)
		{
			GetFunFlag = 0;		//$$NR$$//dated:30Aug15
			ShowHomeMode();
		}
		else
			GetPassword(key_code);
		//$$NR$$//dated:15Aug15
	}

	// check if Panel is in program mode..
	if (((PanelState >= psSetup) && (PanelState <= psDiagnostic))
			|| (PanelState == psInitSetup)) {
		if (PanelState == psInitSetup)
			SetInitialSetup(key_code);
		else if (((PanelState >= psSetup) && (PanelState <= psDiagnostic))
				&& (key_code == kbHome))
		{
			GetFunFlag = 0;		//$$NR$$//dated:30Aug15
			ShowHomeMode();
		}
		else
			AccessMode(key_code);
	}
}
