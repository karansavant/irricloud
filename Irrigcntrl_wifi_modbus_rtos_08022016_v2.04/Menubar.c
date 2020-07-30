
/********************************************************************/
/*						Menu Operations Routines					*/
/********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "GlobalConst.h"
#include "keycodes.h"
#include "IRQ.h"                      	/*	IRQ function prototypes	*/
#include "MenuDefs.h"
#include "GLCD.h"
#include "GLCDConst.h"
#include "keypad.h"
#include "database_struct.h"
#include "database.h"
#include "utils/ustdlib.h"
#include <inc/hw_nvic.h>
#include <inc/hw_types.h>
#include <ti/sysbios/knl/Task.h>
/********************************************************************/
#define LOGFILE_BUFF_SIZE       20000
#define MIN_LIMIT				0		//$$NR$$//dated:19Sep15
#define MAX_LIMIT				10000	//$$NR$$//dated:19Sep15
/********************************************************************/
// Declare Global Functions
extern uint8_t GetDaysInMonth(uint8_t ui8Year, uint8_t ui8Mon);
extern bool DateTimeStrToDec(char *temp_str, uint8_t type);
//extern void DelayMs(int32_t j);
void ViewDateTimeMode (bool screen_refresh);

extern void get_default_value_fertiprog(uint8_t index);
extern void get_default_value_zoneprog(uint8_t index);

extern void eeprom_read_contactinfo(uint8_t parameter_index);
extern void eeprom_write_contactinfo(uint8_t parameter_index);
extern void eeprom_read_timedate(uint8_t parameter_index);
extern void eeprom_write_timedate(uint8_t parameter_index);
extern void eeprom_read_hardwareconfig(uint8_t parameter_index);
extern void eeprom_write_hardwareconfig(uint8_t parameter_index);
extern void eeprom_read_systemsetup(uint8_t parameter_index);
extern void eeprom_write_systemsetup(uint8_t parameter_index);

extern void eeprom_read_output(uint8_t index);
extern void eeprom_write_output(uint8_t index);
extern void eeprom_read_input(uint8_t index);
extern void eeprom_write_input(uint8_t index);
extern void eeprom_read_sensor(uint8_t index);
extern void eeprom_write_sensor(uint8_t index);

//extern void eeprom_read_network(uint8_t parameter_index);
//extern void eeprom_write_network(uint8_t parameter_index);
extern void eeprom_read_network_gsm(uint8_t parameter_index);
extern void eeprom_write_network_gsm(uint8_t parameter_index);
//extern void eeprom_read_network1(uint8_t parameter_index);
//extern void eeprom_write_network1(uint8_t parameter_index);
extern void eeprom_read_network_wifi(uint8_t parameter_index);
extern void eeprom_write_network_wifi(uint8_t parameter_index);

extern void eeprom_read_backflush(uint8_t parameter_index);
extern void eeprom_write_backflush(uint8_t parameter_index);
extern void eeprom_read_authentication(uint8_t index);
extern void eeprom_write_authentication(uint8_t index);
extern void eeprom_read_firmware(uint8_t parameter_index);
extern void eeprom_write_firmware(uint8_t parameter_index);

//extern void eeprom_read_fertiprog(uint8_t index);
extern void eeprom_read_fertiprog1(uint8_t progid,uint8_t parameter_index);
extern void eeprom_read_fertiprog2(uint8_t progid,uint8_t parameter_index);
extern void eeprom_read_fertiprog3(uint8_t progid,uint8_t ioid,uint8_t parameter_index);

//void eeprom_write_fertiprog(uint8_t index);
extern void eeprom_write_fertiprog1(uint8_t progid,uint8_t parameter_index);
extern void eeprom_write_fertiprog2(uint8_t progid,uint8_t parameter_index);
extern void eeprom_write_fertiprog3(uint8_t progid,uint8_t ioid,uint8_t parameter_index);

//void eeprom_read_zoneprog(uint8_t index);
extern void eeprom_read_zoneprog1(uint8_t progid,uint8_t parameter_index);
extern void eeprom_read_zoneprog2(uint8_t progid,uint8_t parameter_index);
extern void eeprom_read_zoneprog3(uint8_t progid,uint8_t parameter_index);
extern void eeprom_read_zoneprog4(uint8_t progid,uint8_t ioid,uint8_t parameter_index);

//void eeprom_write_zoneprog(uint8_t index);
extern void eeprom_write_zoneprog1(uint8_t progid,uint8_t parameter_index);
extern void eeprom_write_zoneprog2(uint8_t progid,uint8_t parameter_index);
extern void eeprom_write_zoneprog3(uint8_t progid,uint8_t parameter_index);
extern void eeprom_write_zoneprog4(uint8_t progid,uint8_t ioid,uint8_t parameter_index);

extern void eeprom_factory_reset(void);
extern void eeprom_write_progstatus(uint8_t parameter_index);
extern void eeprom_read_progstatus(uint8_t parameter_index);

extern uint8_t Read_log(uint16_t read_startloc);
extern uint8_t usb_test_write(char *write_data);	//$$NR$$//dated:08Nov15
extern uint8_t usb_test_read(char *read_data);		//$$NR$$//dated:08Nov15
//$$ Modified by Karan Start$$//
extern void OP_Update(uint8_t opid,uint32_t on_off,uint8_t log);

extern void Remaintime_update1(uint8_t x);//x is the prog no
extern void Remaintime_clear(uint8_t x);//x is the prog no
extern void nextprogstart();

//$$ Modified by Karan End$$//
extern uint8_t	cntrl_mode;						// 0-Auto / 1-Manual /2-Program
uint8_t	cntrl_mode_prev=0;							// 0-Auto / 1-Manual /2-Program

extern char logfile_buff[LOGFILE_BUFF_SIZE];
extern uint16_t logfile_size, logfile_rddata_size;

extern int cycle;//current cycle of the program

/********************************************************************/
// Declare Local Functions

void ShowHomeMode(void);
void ViewHomeScreen(void);

void Init_Menu_Screen (void);
void AccessMode (uint8_t key_code);
void InitFunctionScreen (void);
void RefreshMenu (void);

uint8_t select_list_option(uint8_t key_code);
uint8_t edit_text_numeric(uint8_t key_code);
uint8_t edit_text_alphanumeric(uint8_t key_code);
void start_typing_delay(void);
void textbox_set_initial_cursrpos(void);

void CallFunction (uint8_t FunctNo, uint8_t key_code);
void DisplayMenuLabel(void);
void Show(uint8_t FunNumber, uint8_t key_code);

void ContactInfo(uint8_t key_code);
void SetTimeDate(uint8_t key_code);
void LoadfromUSB(uint8_t key_code);
void SavetoUSB(uint8_t key_code);
void HardwareConfig(uint8_t key_code);
void SystemSetup(uint8_t key_code);

void OP_Assignment(uint8_t key_code);
void IP_Assignment(uint8_t key_code);
void Sensor_Assignment(uint8_t key_code);
void Backflush(uint8_t key_code);
void FirmwareInfo(uint8_t key_code);
void FactoryReset(uint8_t key_code);

void Network_Wireless_Mode(uint8_t key_code);
void Network_GSM_Settings(uint8_t key_code);
void Network_Wifi_Settings(uint8_t key_code);

void Authentication(uint8_t fun_no, uint8_t key_code);

void FertiProgMethod(uint8_t key_code);
void FertiProgDelays(uint8_t key_code);
void FertiProgDetails(uint8_t key_code);
void FertiClearProgram(uint8_t key_code);

void ZoneProgStatus(uint8_t key_code);
void ZoneProgRunTime(uint8_t key_code);
void ZoneProgPrecondition(uint8_t fun_no, uint8_t key_code);
void ZoneProgRunDays(uint8_t key_code);
void ZoneProgStartTime(uint8_t key_code);
uint16_t GetOutputSortedList(uint8_t tmp_optype, uint8_t zoneindex);
void ZoneProgDetails(uint8_t key_code);
//void ZoneProgDetailsTime(uint8_t key_code);
//void ZoneProgDetailsFlow(uint8_t key_code);
void ZoneProgDelays(uint8_t key_code);
void ZoneClearProgram(uint8_t key_code);

void AutoMan_SelectMode(uint8_t key_code);

void ManualProgram(uint8_t key_code);
void ManualOutput(uint8_t key_code);

void ViewIrrigationLog(uint8_t key_code);
void ViewEventLog(uint8_t key_code);
void ViewSensorLog(uint8_t key_code);
void ViewSystemLog(uint8_t key_code);

void OutputTest(uint8_t key_code);
void DiagnosticAlerts(uint8_t fun_no, uint8_t key_code);


//$$NR$$//dated:01Nov15
void Keypad_Test(uint8_t key_code);
void Display_Test(uint8_t key_code);
void Buzzer_Test(uint8_t key_code);
void Output_Test(uint8_t key_code);
void Input_Test(uint8_t key_code);
void Sensor_Test(uint8_t key_code);
void USB_Test(uint8_t key_code);
void WiFi_Local_Test(uint8_t key_code);
void WiFi_Internet_Test(uint8_t key_code);
void GSM_Test(uint8_t key_code);
//$$NR$$//dated:01Nov15

void ShowPasswordScreen();  //$$NR$$//dated:08Feb16

void HomeStatus(uint8_t key_code);
void HomeDateTimeStatus(uint8_t key_code);
void HomeProgramStatus(uint8_t key_code);
void HomeSensorStatus(uint8_t key_code);
void HomeOutputStatus(uint8_t key_code);

void alert_status_update(uint8_t index, uint8_t state);

/********************************************************************/
// local variables related to date and time
//char
//	disp_rtcdate[9] = "00/00/00",
//	disp_rtctime[6] = "00:00";

/********************************************************************/
// Variables used for Global purpose
extern userdef_tm CurTime, SetTime;
extern volatile uint8_t g_bSetDate;

extern volatile uint8_t
	prev_access_level,
	access_level,								// Defined in Main.c
	access_level_timeout,						// defined in main.c
	PanelState,									// Defined in Main.c
	EvtPriorityState,							// Defined in Main.c
	EvtScreenState;								// Defined in Main.c

// Variables used for Keyboard
extern volatile uint8_t		key_press_detected;
extern volatile uint16_t	typingdelay_count;
extern volatile bool		typingdelay_elapsed_flag;

extern uint8_t selected_ps_keycode;

// variables related to textbox
bool
	editing_text_alpha_flag = false,
	numkey_pressed_flag = false;

//$$ Modified by Karan Start$$//
//Variables related to Program.c
extern char on_zones;//No of on zones for auto mode
extern char on_op;//No of on op for manual mode
extern int zone_onoff[10];//0=zone is currently off 1=zone is currently on
extern int op_onoff[16];//0=op is currently off 1=op is currently on
extern char temp1[20000];
extern int gsm_activ;
extern int gsm_config;
extern int wifi_activ;
extern int wifi_config;
extern char log_buffer[1000];
extern char log_temp[100];
extern int logflag;
extern int shift;//time shift due to power failure
//$$ Modified by Karan End$$//

//extern uint8_t
//	BuzzerStatus;								// Defined in IRQ.c

/********************************************************************/
// Variables used for menu Handling

//char
//	sel_menu_label[15];

uint8_t
	cur_menu_fun_no = 0,
	GetFunFlag = 0,
//	menutree_member_max,						// Max. No of counts for Main Menus
//	MaxFunNo,									// The largest function number in the menu trees array.
	menutree_menu_base_index,
//	menutree_sel_menu_index,
	menuoptions_max_count = 0,
	menuoptions_selected_count = 0,
	menu_cursor_pos = 0;
//$$NR$$//dated:07Feb16
//	//[access_level][PanelState]
//						//psHome/psPassword/psSetup/psManual/psProgram/psLogs/psDiagnostic/psInitSetup
//	MaxMainMenu[5][8] = {{0, 	0, 			2, 		2, 		2, 			3, 		2, 			0},		// alNorm
//						 {0, 	0, 			2, 		2, 		2, 			3, 		2, 			0},		// alAdmin
//						 {0, 	0, 			2, 		2, 		2, 			3, 		2, 			0},		// alEngr
//						 {0, 	0, 			2, 		2, 		2, 			3, 		2, 			0},		// alOwner
//						 {0, 	0, 			2, 		2, 		2, 			3, 		2, 			0}};	// alOperator
//$$NR$$//dated:07Feb16

uint32_t
	Menu_Index = 0,
	Prv_Menu_Index = 0,
	menuoptions_selected = 0;

uint8_t
	prog_index = 0;

/********************************************************************/
// Variables used for data edit Handling

uint8_t
	fun_stage = 0,
//	fun_err_stage = 0,
	fun_line_addr = 0,
	fun_col_addr = 0,
//	fun_cursor_pos = 0,
	fun_result1 = 0,
	fun_result2 = 0,
	test_in_progress = 0;			//$$NR$$//dated:07Nov15

uint8_t
	sub_fun_state = 0,		// 1=init, 2=process, 3=save
	tool_type = 0;			// 1=listbox, 2=textbox_numeric, 3=textbox_alphanumeric

char
	disp_tempstr[22],
	edit_data_str[22];

// variables related to listbox
char
	*(*listbox)[];

int8_t
	listbox_baseval = 0;

uint8_t
	listbox_type = 0,		//0=const str, 1=int8_t, 2=uint8_t
	listbox_index = 0,
	listbox_size = 0;

uint32_t
	listbox_selected = 0;

// variables related to textbox
char
	edit_tempstr[22];

//$$NR$$//dated:19Sep15
uint8_t
	textbox_char_encoded_trancated = 0;		// bit0 - 0/1 - text string trancating = no / yes
											// bit1 - 0/1 - char encoding = no / yes
											// bit2 - 0/1 - check data validity = no / yes

float
	editvalue_min_limit,					// min limit
	editvalue_max_limit;					// max limit

//$$NR$$//dated:19Sep15

uint8_t
	textbox_char_count = 0,
	textbox_size = 0;

uint8_t
	prev_key_code = 0,
	numkey_press_cnt = 0;

uint8_t	 prog_t_updated=0;

/********************************************************************/
// Variables used for others

typedef struct edit_date
{
	uint8_t tm_mday;	// day of the month           - [1,31]
	uint8_t tm_mon;		// months since January       - [0,11]
	uint8_t tm_year;	// years since 1900

}edit_dt;

typedef struct edit_time
{
	uint8_t tm_hour;	// hours after the midnight   - [0,23]
	uint8_t tm_min;		// minutes after the hour     - [0,59]
	uint8_t tm_sec;		// seconds after the minute   - [0,59]
	uint8_t tm_slot;	// time slot          		  - [0,1]

}edit_tm;

edit_tm disp_time = {0}, rd_time = {0}, wr_time = {0};
edit_dt rd_date = {0}, log_from_date = {0}, log_to_date = {0};
char msg_send[200]={0};
//uint8_t log_start_date[3] = {0}, log_stop_date[3] = {0};

volatile bool CheckRTC;

/********************************************************************/
// Variables used for Testing Purpose

/********************************************************************************/
/* 						Routines For Menubar Handling							*/
/********************************************************************************/
// Function used Initilisation of Menubar
/********************************************************************/
//$$NR$$//dated:08Feb16

void Init_Menu_Screen(void)
{
	if (access_level == alNorm)
	{
		PanelState = psHome;
	}
	else
	{
//		GLCD_clear_screen(false);					// clear the lcd display

		if (selected_ps_keycode == kbSetup)			// set pointer menu group & menu tree for Setup
		{
			menu = (menu_data *)(&menu_Setup);
			menu_tree = (menu_tree_data *)menu_tree_Setup;
		}
		else if (selected_ps_keycode == kbManual)	// set pointer menu group & menu tree for Manual
		{
//			led_control(LED_MANUAL, ON);					// Turn on the MANUAL LED

			menu = (menu_data *)(&menu_Manual);
			menu_tree = (menu_tree_data *)menu_tree_Manual;
		}
		else if (selected_ps_keycode == kbProgram)	// set pointer menu group & menu tree for Program
		{
//			led_control(LED_PROGRAM, ON);					// Turn on the PROGRAM LED
//			cntrl_mode_prev=cntrl_mode;
//			cntrl_mode=2;
			menu = (menu_data *)(&menu_Program);
			menu_tree = (menu_tree_data *)menu_tree_Program;
		}
		else if (selected_ps_keycode == kbLogs)		// set pointer menu group & menu tree for Logs
		{
			menu = (menu_data *)(&menu_Logs);
			menu_tree = (menu_tree_data *)menu_tree_Logs;
		}
		else if (selected_ps_keycode == kbDiagnostic)// set pointer menu group & menu tree for Diagnostic
		{
			menu = (menu_data *)(&menu_Diagnostic);
			menu_tree = (menu_tree_data *)menu_tree_Diagnostic;
//
//			// testing purpose
//			uint8_t i;
//			for(i=0; i<ALERT_MAX; i++)
//			{
//				alert[i].status = 1;
//			}
		}
	}

	if ((menu->AccessRight & (0x01 << access_level)) == 0)   //check menu access right for current user
    {
        goto_screen_map_yz(3, fun_line_addr - 10);
        GLCD_WriteString("  No Access Right  ", false);
        goto_screen_map_yz(3, fun_line_addr);
        GLCD_WriteString("   to this menu    ", false);

        GLCD_MsgLine_Update(0, fun_line_addr - 11, 126, fun_line_addr + 9);

        //delay_sec(2);
        Task_sleep(2000);
        ShowPasswordScreen();
        return;
    }

	PanelState =  menu->MenuState;					// Set Panel State
	if (PanelState == psProgram)
	{
	    led_control(LED_PROGRAM, ON);               // Turn on the PROGRAM LED
	    cntrl_mode_prev=cntrl_mode;
	    cntrl_mode=2;
	}

//	menutree_member_max = menu->MenuCount;			// Max. No of counts for Main Menus
//	MaxFunNo = menu->FunCount;						// The largest function number in the menu items array.
//	strcpy(sel_menu_label, menu->MenuTitle);

	Menu_Index = 0;		Prv_Menu_Index = 0;
	prog_index = 0;

//	menuoptions_max_count = MaxMainMenu[access_level][PanelState];
	menutree_menu_base_index = 0xFF;
	menu_cursor_pos = 0;

	cur_menu_fun_no = 0;	GetFunFlag = 0;

	GLCD_write_titlebar(menu->MenuTitle);
}
//$$NR$$//dated:08Feb16

/********************************************************************/
// Function used for Control of different Mode from Menu tree
/********************************************************************/
uint8_t FunErrNo = 0;

void AccessMode(uint8_t key_code)
{
//$$ Modified by Karan Start$$//
int i;
//$$ Modified by Karan End$$//
uint8_t temp_index;

	if (GetFunFlag == 0)
	{
		if ((key_code == kbUp)  || (key_code == kbDown))	// Check if Up / Down Key pressed, scroll up the Menubar
		{
			if (key_code == kbUp)							// Check if Up Key pressed, scroll up the Menubar
			{
				if ((menu_cursor_pos == 0) && (menuoptions_selected_count <= 5))	return;

				if (menu_cursor_pos)		menu_cursor_pos--;

				do
				{
					Menu_Index--;		temp_index = (uint8_t)(Menu_Index % 0x10);
					if(temp_index == 0)		Menu_Index += menuoptions_max_count;
					temp_index = (uint8_t)(Menu_Index % 0x10);

				}while ((menuoptions_selected & (0x01 << temp_index)) == 0);
			}
			else if (key_code == kbDown)					// Check if Down Key pressed, scroll down the Menubar
			{
				if (((menu_cursor_pos+1) >= menuoptions_selected_count))	return;

				if (menu_cursor_pos < 4)	menu_cursor_pos++;

				do
				{
					Menu_Index++;		temp_index = (uint8_t)(Menu_Index % 0x10);
					if (temp_index > menuoptions_max_count)		Menu_Index -= menuoptions_max_count;
					temp_index = (uint8_t)(Menu_Index % 0x10);

				}while ((menuoptions_selected & (0x01 << temp_index)) == 0);
			}
		}
		else if ((key_code == kbNext) || (key_code == kbEnter))	// Check if Enter/ Key pressed, go to sub menu (or function)
		{
			if (cur_menu_fun_no == 0)		// check if menu option is not followed by function
			{
/*
				if (PanelState == psSetup)
				else if (PanelState == psManual)
				else if (PanelState == psProgram)
				else if (PanelState == psLogs)
				else if (PanelState == psDiagnostic)
*/
				if ((PanelState == psProgram) && (Menu_Index >= 0x00000011) && (Menu_Index <= 0x00000026))
				{
					prog_index = (uint8_t)((Menu_Index & 0x0000000F) - 0x00000001);
					Menu_Index = ((Menu_Index & 0x000000F0) << 4) + 0x11;	// 0x00000111 / 0x00000211
				}
				else if (PanelState == psManual)		// check if auto / manual mode
				{
					if (Menu_Index == 0)
					{
						Menu_Index = (manual_mode_enabled + 1);
					}
					else if (Menu_Index == 0x00000001)
					{
						Menu_Index = 0;		//manual_mode_enabled = 0;

						AutoMan_SelectMode(kbEnter);		// clear flag of manual mode

						led_control(LED_MANUAL, OFF);		// Turn off the MANUAL LED
						//$$ Modified by Karan Start$$//
						cntrl_mode=0;	//toggle mode
						cntrl_mode_prev=cntrl_mode;
						on_zones=0;//No of on zones for auto/manual mode
						on_op=0;//No of on op for manual mode
						for(i=0;i<10;i++)
						{
							zone_onoff[i]=0;//0=zone is currently off 1=zone is currently on
						}
						for(i=0;i<16;i++)
						{
							op_onoff[i]=0;
							OP_Update(i+1,(~0),0);//switch off all outputs
						}

						//$$ Modified by Karan End$$//
					}
					else if (Menu_Index == 0x00000002)
					{
						Menu_Index = (Menu_Index * 0x10) + 1;
						manual_mode_enabled = 1;			// set flag of manual mode

						led_control(LED_MANUAL, ON);		// Turn on the MANUAL LED
						//$$ Modified by Karan Start$$//
						cntrl_mode=1;	//toggle mode
						cntrl_mode_prev=cntrl_mode;
						on_zones=0;//No of on zones for auto/manual mode
						on_op=0;//No of on op for manual mode
						for(i=0;i<10;i++)
						{
							zone_onoff[i]=0;//0=zone is currently off 1=zone is currently on
						}
						for(i=0;i<16;i++)
						{
							op_onoff[i]=0;
							OP_Update(i+1,(~0),0);//switch off all outputs
						}
						//$$ Modified by Karan End$$//
					}
				}
				else
				{
					Menu_Index = (Menu_Index * 0x10) + 1;
				}

				menu_cursor_pos = 0;	menutree_menu_base_index = 0xFF;
			}
			else							// check if menu option is followed by function
			{
				InitFunctionScreen ();					// Initialise Function screen
			}

			if (FunErrNo == 1)
			{
				FunErrNo = 0;
			}
		}
		else if (key_code == kbBack)					// Check if Back Key pressed, go back to prev menu
		{
			Menu_Index = (Menu_Index / 0x10);

			if ((PanelState == psProgram) && ((Menu_Index == 0x00000011) || (Menu_Index == 0x00000021)))
				Menu_Index += prog_index;
			else if ((PanelState == psManual) && (Menu_Index == 0x00000002))	// check if auto / manual mode
				Menu_Index = 0;

			menu_cursor_pos = 0;	menutree_menu_base_index = 0xFF;
		}

		// Update the label for selected Menu Option
		if (GetFunFlag == 0)	RefreshMenu();
	}
//	else if (key_code == kbVirtual)					// Check if kbVirtual Key pressed, go back to same menu
	else if (key_code == kbVirtualViewLog)			// Check if kbVirtual Key pressed, go back to same menu
	{
//		// check if virtual key for ViewLogs function from Supervisor Mode
//		if ((access_level == alOwner) && ((cur_menu_fun_no >= 13) && (cur_menu_fun_no <= 16)))
		// check if virtual key for ViewLogs function from Supervisor & Engineer Mode
//		if (((access_level == alOwner) || (access_level == alEngr)) && ((cur_menu_fun_no >= 5) && (cur_menu_fun_no <= 8)))
//			InitFunctionScreen ();
	}

	// go for executing the function
	if (GetFunFlag)		CallFunction(cur_menu_fun_no, key_code);
}

/********************************************************************/
// Function used for Refresh Menubar(2nd and 3rd Line)
/********************************************************************/
//$$NR$$//dated:07Feb16

void RefreshMenu(void)
{
uint8_t i = 0, j = 0, usertype = 0;

	// Check if index of displayed menu option changed
	if (Menu_Index == Prv_Menu_Index) return;

	// Get index of selected menu option
	Prv_Menu_Index = Menu_Index;

	if (Menu_Index == 0)						// Quit from Program Mode
	{
		ShowHomeMode();
	}
	else
	{
		// get base index of currently selected sub menu list
		if (menutree_menu_base_index != 0xFF)	i = menutree_menu_base_index;

		do
		{
			// Check if index of selected menu option is in list of Menu options
			if (menu_tree[i].MenuIndex == Menu_Index)
			{
				cur_menu_fun_no = menu_tree[i].FunNo;						// Get Function No for selected Menu Option
				menuoptions_max_count = menu_tree[i].Max_Index;

//				// Check if Main MenuOption
//				if (Menu_Index < 0x10)	menuoptions_max_count = MaxMainMenu[access_level][PanelState];

				// get base index of currently selected sub menu list
				if (menutree_menu_base_index == 0xFF)
				{
					menutree_menu_base_index = i - ((Menu_Index % 0x10) - 1);

					menuoptions_selected = 0;	menuoptions_selected_count = 0;		j = 1;
					usertype = 0x01 << access_level;

					while (j <= menuoptions_max_count)
					{
//						if ((PanelState == psDiagnostic) && (Menu_Index >= 0x00000011) && (Menu_Index <= 0x0000001B))
						if ((PanelState == psDiagnostic) && (Menu_Index == 0x00000011))
						{
							if (j == 1)							// "clear all" menu option
							{
								menuoptions_selected |= (0x01 << j);
								menuoptions_selected_count++;
							}
							else if (alert[j-2].status)			// check if alert is activated
							{
								menuoptions_selected |= (0x01 << j);
								menuoptions_selected_count++;
							}
						}
						else if (menu_tree[i+j-1].AccessRight & usertype)   //check menu access right for current user
						{
							menuoptions_selected |= (0x01 << j);
							menuoptions_selected_count++;
						}

						j++;
					}
				}

//				menutree_sel_menu_index = i;

				// Display Menu Label on line 2 to 6
				DisplayMenuLabel ();

				break;
			}

			i++;

		} while (i < menu->MenuCount);

		// Reload the Menu Index
		Menu_Index = Prv_Menu_Index;

		if (i >= menu->MenuCount)
		{
	 		Menu_Index = (Menu_Index / 0x10);
			Prv_Menu_Index = Menu_Index;
		}
	}
}
//$$NR$$//dated:07Feb16

/********************************************************/
// Function used for Display Menu Label on line 2 & 3
/********************************************************/

void DisplayMenuLabel (void)
{
uint8_t z_addr = 0, i = 0, j = 0;//, k = 0;
uint8_t temp_index;
bool cursor_mark;

	z_addr = MIDDLE_PAGE_TOP + 1;

	// clear part of screen
	GLCD_clear_part_of_screen(MIDDLE_PAGE_TOP, 0, 63, 127, false);				// clear the lcd display

	// get index of menu option @ first line
	temp_index = (uint8_t)(Menu_Index % 0x10);

	for (j=menu_cursor_pos; j > 0; j--)
	{
		do
		{
			temp_index--;
			if(temp_index == 0)		temp_index += menuoptions_max_count;

		}while ((menuoptions_selected & (0x01 << temp_index)) == 0);
	}

    i = menutree_menu_base_index + (temp_index - 1);


	for (j=0; j<5; j++)
	{
		if (j < menuoptions_selected_count)
		{
		    cursor_mark = false;
			if (j == menu_cursor_pos)		cursor_mark = true;

			goto_screen_map_yz(5, (z_addr + j*GLCD_LINE_HEIGHT));
		    GLCD_WriteString(menu_tree[i].MenuLabel, cursor_mark);	// Load data into buffer of LCD display

		    do
		    {
			    i++;
				if ((i - menutree_menu_base_index) >= menuoptions_max_count)		i = menutree_menu_base_index;
			}while ((menuoptions_selected & (0x01 << (i - menutree_menu_base_index + 1))) == 0);
		}
	}

	// display down arrow key
	if (menuoptions_selected_count > 5)
	{
		goto_screen_map_yz(0, (z_addr + 4*GLCD_LINE_HEIGHT));
		GLCD_writesymbol((uint8_t)(96+32), false);
	}

//	GLCD_write_messagebar("");

	GLCD_Screen_Update(0, 0, GLCD_SCREEN_WIDTH-1, GLCD_SCREEN_HEIGHT-1);
}

/********************************************************************/
// Function used for initialisation of Function Screen
/********************************************************************/

void InitFunctionScreen(void)
{
	if (PanelState != psHome)	GetFunFlag = 1;

	fun_stage = 0;	//fun_err_stage = 0;

	fun_line_addr = 0;	fun_col_addr = 0;
//	fun_cursor_pos = 0;
	fun_result1 = 0;	fun_result2 = 0;

	sub_fun_state = 0;	tool_type = 0;

	textbox_char_encoded_trancated = 0;
}

/********************************************************************/
// Function used for Calling Different Funs for selected Menu Option
/********************************************************************/

void CallFunction(uint8_t FunctNo, uint8_t key_code)
{
	char temp_wm;
//uint8_t MenuSet_As_Fun = 0;

//	GetFunFlag = 1;
	if ((FunctNo >= 82) && (FunctNo <= 91))		test_in_progress = (FunctNo - 81);	//$$NR$$//dated:07Nov15

	if (access_level <= alOperator)												// Normal / User / Supervisor / Engineer
	{
		switch (FunctNo)
		{
// Setup
			case 1:
					ContactInfo(key_code);
					break;

			case 2:
					SetTimeDate(key_code);
					break;

			case 3:
					LoadfromUSB(key_code);
					break;

			case 4:
					SavetoUSB(key_code);
					break;

			case 5:
					HardwareConfig(key_code);
					break;

			case 6:
					SystemSetup(key_code);
					break;

			case 7:
					OP_Assignment(key_code);
					break;

			case 8:
					IP_Assignment(key_code);
					break;

			case 9:
					Sensor_Assignment(key_code);
					break;

			case 10:
					Backflush(key_code);
					break;

			case 11:
					FirmwareInfo(key_code);
					break;

			case 12:
					FactoryReset(key_code);
					break;

			case 13:
					temp_wm = wireless_mode;
					Network_Wireless_Mode(key_code);
					if(temp_wm != wireless_mode)
					{
						//used to reset the controller using software
						HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
					}
					break;

			case 14:
					Network_GSM_Settings(key_code);
					break;

			case 15:
					Network_Wifi_Settings(key_code);
					break;

//			case 16:
//					Network_Zigbee(key_code);
//					break;

//			case 17:
//					Network_CAN(key_code);
//					break;

			case 20:
			case 21:
			case 22:
			case 23:
					Authentication(FunctNo-20, key_code);
					break;
//Program - Ferti
			case 31:
					FertiProgMethod(key_code);
					break;

			case 32:
					FertiProgDelays(key_code);
					break;

			case 33:
					FertiProgDetails(key_code);
					break;

			case 34:
					FertiClearProgram(key_code);
					break;

//Program - Zone
			case 35:
					ZoneProgStatus(key_code);
					break;

			case 36:
					ZoneProgRunTime(key_code);
					break;

			case 37:
					ZoneProgRunDays(key_code);
					break;

			case 38:
					ZoneProgStartTime(key_code);
					break;

			case 39:
					ZoneProgDetails(key_code);
					break;

			case 40:
					ZoneProgDelays(key_code);
					break;

			case 41:
					ZoneClearProgram(key_code);
					break;

			case 42:
			case 43:
			case 44:
			case 45:
			case 46:
			case 47:
					ZoneProgPrecondition(FunctNo-42, key_code);
					break;

			case 50:
					AutoMan_SelectMode(key_code);
					break;

			case 51:
					ManualProgram(key_code);
					break;

			case 52:
					ManualOutput(key_code);
					break;

			case 60:
				ViewIrrigationLog(key_code);
				break;

			case 61:
				ViewEventLog(key_code);
				break;

			case 62:
				ViewSensorLog(key_code);
				break;

			case 63:
				ViewSystemLog(key_code);
				break;

			case 70:
				OutputTest(key_code);
				break;

			case 71:
			case 72:
			case 73:
			case 74:
			case 75:
			case 76:
			case 77:
			case 78:
			case 79:
			case 80:
			case 81:
					DiagnosticAlerts(FunctNo-71, key_code);
					break;

			//$$NR$$//dated:01Nov15
			case 82:
					Keypad_Test(key_code);
					break;

			case 83:
					Display_Test(key_code);
					break;

			case 84:
					Buzzer_Test(key_code);
					break;

			case 85:
					Output_Test(key_code);
					break;

			case 86:
					Input_Test(key_code);
					break;

			case 87:
					Sensor_Test(key_code);
					break;

			case 88:
					USB_Test(key_code);
					break;

			case 89:
					WiFi_Local_Test(key_code);
					break;

			case 90:
					WiFi_Internet_Test(key_code);
					break;

			case 91:
					GSM_Test(key_code);
					break;

					//$$NR$$//dated:01Nov15

			// Function 0 indicates all submenus:-
			default:
					Show(FunctNo, key_code);
					break;
		}
	}

	if(prog_t_updated==1)//if program time parameters are updated update the remtime
	{
		prog_t_updated=0;
		if(CurTime.tm_24hr == zoneprog[prog_index].starttime[0])//update remain time on program paramaters change
		{
			if(CurTime.tm_min < zoneprog[prog_index].starttime[1])
			{
				Remaintime_update1(prog_index);
			}
		}
		else if(CurTime.tm_24hr < zoneprog[prog_index].starttime[0])
		{
			Remaintime_update1(prog_index);
		}
		else
		{
			//give alert program will start from tomm
		};
	}

	if (GetFunFlag == 0)
	{
/*		CheckFlags = (CheckFlags & 0xFD);

		// check if menu used as function is selected or all current events are removed
		if ((MenuSet_As_Fun) || ((access_level == alNorm) && (EvtPriorityState == 0)))
		{
			if ((access_level == alNorm) && (EvtPriorityState == 0))
				Menu_Index = 0;					// Quit from View Event Mode

			AccessMode (kbEsc);
		}
		else
*/
		test_in_progress = 0;			//$$NR$$//dated:07Nov15

		if ((PanelState == psDiagnostic) && (Menu_Index != Prv_Menu_Index))
		{
			// Update the label for selected Menu Option
			RefreshMenu();
		}
		else
		{
			GLCD_write_titlebar(menu->MenuTitle);

			// Display Menu Label on line 2 to 6
			DisplayMenuLabel ();

//			DelayMs (10);
		}

	}

}

/**************************************************************/
// Function used for Displaying Function, Yet to implement....
// for selected menu option
/**************************************************************/

void Show(uint8_t FunNumber, uint8_t key_code)
{
uint8_t z_addr;

	if (fun_stage == 0)
	{
		z_addr = MIDDLE_PAGE_TOP + 10;

		GLCD_clear_part_of_screen(MIDDLE_PAGE_TOP, 0, 63, 127, false);				// clear the lcd display

		goto_screen_map_yz(3, z_addr);
	    GLCD_WriteString("Yet to implement....", false);

	    goto_screen_map_yz(3, (z_addr + 10));
    	usnprintf(disp_tempstr, 21, "  Function no. %2u   ", FunNumber);
	    GLCD_WriteString(disp_tempstr, false);

	    GLCD_Screen_Update();
		fun_stage = 1;
	}
	else if (fun_stage == 1)
	{
		if (key_code == kbEsc)					// Check if Esc Key pressed, go back to prev menu
			GetFunFlag = 0;
	}
}

/********************************************************************************/
/* 							Routines For Setup Menu								*/
/********************************************************************************/
//
/********************************************************************/
/*
uint8_t strtodec(char *tempstring)
{
uint8_t tempval = 0, tempstrlen = 0;

	tempstrlen = strlen(tempstring);

	while (tempstrlen)
	{
		tempstrlen--;
		tempval += (tempstring[0] - '0') * (10 ^ (tempstrlen));
	}

//	tempval = (tempstring[0] - '0') * 100;
//	tempval += (tempstring[1] - '0') * 10;
//	tempval += (tempstring[2] - '0');

	return tempval;
}
*/
/********************************************************************/
//
/********************************************************************/

uint32_t strtoint(char *tempstring, uint8_t noofdigit)
{
uint32_t tempval = 0;//tempval1 = 0, tempval2 = 1;

	while (*tempstring)
	{
		tempval *= 10;
		tempval += (*tempstring - '0');//(tempstring[index++] - '0');
		tempstring++;	//noofdigit--;
	}

	return tempval;
}

/********************************************************************/
//
/********************************************************************/

float strtofloat(char *tempstring)
{
uint8_t dec_flag = 0;
float tempval = 0, tempval2 = 1;

	while (*tempstring)
	{
		if (*tempstring == '.')
			dec_flag = 1;
		else
		{
			if (dec_flag == 0)
				tempval *= 10;
			else if (dec_flag == 1)
				tempval2 /= 10;

			tempval += ((*tempstring - '0') * tempval2);
		}

		tempstring++;
	}

	return tempval;
}

/********************************************************************/
//
/********************************************************************/

void ftoa(char *buf, float f)
{
unsigned int i;

	i = (unsigned int)((float)f*10);

    usnprintf(buf, 5, "%02u.%01u", (i / 10), (i % 10));
}

//*****************************************************************************
// This function converts 24 hour time interval into editable format.
// Paramter -	timedata[0] = hr, timedata[1] = min
//				if clkformat_enable = true, then check for disp_clockformat
//*****************************************************************************

void set_disp_time(uint8_t *timedata, bool clkformat_enable)
{
    // set edit time
    disp_time.tm_hour = *(&timedata[0]);
    disp_time.tm_min = *(&timedata[1]);
    disp_time.tm_slot = 0;
	if ((clkformat_enable) && (disp_clockformat == 0))
	{
//		disp_time.tm_slot = 1;
//		if (disp_time.tm_hour < 12)	disp_time.tm_slot = 0;
		if (disp_time.tm_hour >= 12)	disp_time.tm_slot = 1;
		if (disp_time.tm_hour > 12)	disp_time.tm_hour -= 12;
	}
}

//*****************************************************************************
// This function converts 24 hour time interval into editable format.
// Paramter -	timedata[0] = hr, timedata[1] = min
//				if clkformat_enable = true, then check for disp_clockformat
//*****************************************************************************

void set_edit_time(uint8_t *timedata, bool clkformat_enable)
{
    // set edit time
    rd_time.tm_hour = *(&timedata[0]);
    rd_time.tm_min = *(&timedata[1]);
    rd_time.tm_slot = 0;
	if ((clkformat_enable) && (disp_clockformat == 0))
	{
//		rd_time.tm_slot = 1;
//		if (rd_time.tm_hour < 12)	rd_time.tm_slot = 0;
		if (rd_time.tm_hour >= 12)	rd_time.tm_slot = 1;
		if (rd_time.tm_hour > 12)	rd_time.tm_hour -= 12;
	}

	// set write time
	wr_time = rd_time;
}

//*****************************************************************************
// This function converts editable time into 24 hour time interval format.
// Paramter -	timedata[0] = hr, timedata[1] = min
//				if clkformat_enable = true, then check for disp_clockformat
//*****************************************************************************

bool get_edit_time1(char *temp_str, uint8_t *timedata, uint8_t datatype, bool clkformat_enable)
{
uint8_t temp_data[3] = {0, 0, 0}, type = 0;

	if (*(temp_str+1) == 'M')	type = 1;

	if (type == 0)			// time (hh:mm)
	{
		temp_data[0]	= (*temp_str - '0')*10;		temp_str++;			//hh
		temp_data[0]	+= (*temp_str - '0');		temp_str+=2;
		temp_data[1]	= (*temp_str - '0')*10;		temp_str++;			//mm
		temp_data[1]	+= (*temp_str - '0');

		// Is valid data?
		if((temp_data[0] > 23) || (temp_data[1] > 59))       return false;

		// Is valid data?
		if ((clkformat_enable) && (disp_clockformat == 0) && (temp_data[0] > 12))	return false;

		// set valid time
		wr_time.tm_hour	= temp_data[0];
		wr_time.tm_min	= temp_data[1];
	}
	else if (type == 1)			// time (AM/PM)
	{
		if (*temp_str == 'P')	wr_time.tm_slot = 1;
	}



	// get edit time
	*(&timedata[0]) = wr_time.tm_hour;
	if ((clkformat_enable) && (wr_time.tm_slot == 1) && (wr_time.tm_hour < 12))	*(&timedata[0]) += 12;

	*(&timedata[1]) = wr_time.tm_min;

	return true;
}

//*****************************************************************************
// This function converts editable time into 24 hour time interval format.
// Paramter -	timedata[0] = hr, timedata[1] = min
//				if clkformat_enable = true, then check for disp_clockformat
//*****************************************************************************

void get_edit_time(uint8_t *timedata, bool clkformat_enable)
{
    // get edit time
	*(&timedata[0]) = wr_time.tm_hour;
	if ((clkformat_enable) && (wr_time.tm_slot == 1) && (wr_time.tm_hour < 12))	*(&timedata[0]) += 12;

    *(&timedata[1]) = wr_time.tm_min;
}

//*****************************************************************************
// This function converts time interval from string to decimal format.
//*****************************************************************************

bool time_strtodec(char *temp_str, uint8_t type, bool clkformat_enable)
{
uint8_t temp_data[3] = {0, 0, 0};

	if (type == 1)			// time (hh:mm)
	{
		temp_data[0]	= (*temp_str - '0')*10;		temp_str++;			//hh
		temp_data[0]	+= (*temp_str - '0');		temp_str+=2;
		temp_data[1]	= (*temp_str - '0')*10;		temp_str++;			//mm
		temp_data[1]	+= (*temp_str - '0');

	    // Is valid data?
		if((temp_data[0] > 23) || (temp_data[1] > 59))       return false;

	    // Is valid data?
	    if ((clkformat_enable) && (disp_clockformat == 0) && (temp_data[0] > 12))	return false;

	    // set valid time
		wr_time.tm_hour	= temp_data[0];
		wr_time.tm_min	= temp_data[1];

	    // Return that new data is available so that it can be displayed.
	    return true;
	}
	else if (type == 2)			// time (AM/PM)
	{
	    if (*temp_str == 'P')	wr_time.tm_slot = 1;

	    // Return that new data is available so that it can be displayed.
	    return true;
	}

	return false;
}

//*****************************************************************************
// This function converts date from string to decimal format.
//*****************************************************************************

bool date_strtodec(char *temp_str, edit_dt cmp_date, uint8_t limit)
{
uint8_t temp_data[3] = {0, 0, 0}, ui8MonthDays;

	if (dateformat == 0)
	{
		temp_data[0]	= (*temp_str - '0')*10;		temp_str++;		//dd
		temp_data[0]	+= (*temp_str - '0');		temp_str+=2;
		temp_data[1]	= (*temp_str - '0')*10;		temp_str++;		//mm
		temp_data[1]	+= (*temp_str - '0');		temp_str+=2;
	}
	else
	{
		temp_data[1]	= (*temp_str - '0')*10;		temp_str++;		//mm
		temp_data[1]	+= (*temp_str - '0');		temp_str+=2;
		temp_data[0]	= (*temp_str - '0')*10;		temp_str++;		//dd
		temp_data[0]	+= (*temp_str - '0');		temp_str+=2;
	}

	temp_data[2]	= (*temp_str - '0')*10;		temp_str++;			//yy
	temp_data[2]	+= (*temp_str - '0');

    // Is valid data?
	if((temp_data[0] > 31) || (temp_data[1] > 12) || (temp_data[2] > 99))	return false;

    // Since different months have varying number of days, get the number of
    // days for the current month and year.
    ui8MonthDays = GetDaysInMonth(temp_data[2], temp_data[1]-1);

    if(temp_data[0] > ui8MonthDays)		return false;

    if (limit == 1)			// check for max limit
    {
    	if (temp_data[2] > cmp_date.tm_year)	return false;
    	if ((temp_data[2] == cmp_date.tm_year) && (temp_data[1] > cmp_date.tm_mon))	return false;
    	if ((temp_data[2] == cmp_date.tm_year) && (temp_data[1] == cmp_date.tm_mon) && (temp_data[0] > cmp_date.tm_mday))	return false;
    }
    else if (limit == 2)	// check for min limit
    {
    	if (temp_data[2] < cmp_date.tm_year)	return false;
    	if ((temp_data[2] == cmp_date.tm_year) && (temp_data[1] < cmp_date.tm_mon))	return false;
    	if ((temp_data[2] == cmp_date.tm_year) && (temp_data[1] == cmp_date.tm_mon) && (temp_data[0] < cmp_date.tm_mday))	return false;
    }

    // set valid time
	rd_date.tm_mday	= temp_data[0];
	rd_date.tm_mon	= temp_data[1];
	rd_date.tm_year = temp_data[2];

    // Return that new data is available so that it can be displayed.
    return true;
}


//*****************************************************************************
// This function compare date with min and max limit.
//*****************************************************************************

bool date_compare(uint8_t *temp_date, edit_dt min_date_limit, edit_dt max_date_limit)
{
    // check for max limit
	if (*(temp_date + 2) > max_date_limit.tm_year)	return false;
	if ((*(temp_date + 2) == max_date_limit.tm_year) && (*(temp_date + 1) > max_date_limit.tm_mon))	return false;
	if ((*(temp_date + 2) == max_date_limit.tm_year) && (*(temp_date + 1) == max_date_limit.tm_mon) && (*(temp_date) > max_date_limit.tm_mday))	return false;

    // check for min limit
	if (*(temp_date + 2) < min_date_limit.tm_year)	return false;
	if ((*(temp_date + 2) == min_date_limit.tm_year) && (*(temp_date + 1) < min_date_limit.tm_mon))	return false;
	if ((*(temp_date + 2) == min_date_limit.tm_year) && (*(temp_date + 1) == min_date_limit.tm_mon) && (*(temp_date) < min_date_limit.tm_mday))	return false;

    // Return that new data is available so that it can be displayed.
    return true;
}


//bool date_compare(edit_dt temp_date, edit_dt min_date_limit, edit_dt max_date_limit)
//{
//    // check for max limit
//	if (temp_date.tm_year > max_date_limit.tm_year)	return false;
//	if ((temp_date.tm_year == max_date_limit.tm_year) && (temp_date.tm_mon > max_date_limit.tm_mon))	return false;
//	if ((temp_date.tm_year == max_date_limit.tm_year) && (temp_date.tm_mon == max_date_limit.tm_mon) && (temp_date.tm_mday > max_date_limit.tm_mday))	return false;
//
//    // check for min limit
//	if (temp_date.tm_year < min_date_limit.tm_year)	return false;
//	if ((temp_date.tm_year == min_date_limit.tm_year) && (temp_date.tm_mon < min_date_limit.tm_mon))	return false;
//	if ((temp_date.tm_year == min_date_limit.tm_year) && (temp_date.tm_mon == min_date_limit.tm_mon) && (temp_date.tm_mday < min_date_limit.tm_mday))	return false;
//
//    // Return that new data is available so that it can be displayed.
//    return true;
//}

/*
bool IntervalStrToDec(char *temp_str, uint8_t *tempInterval, uint8_t type)
{
uint8_t temp_data[3] = {0,0,0};

	if (type == 1)			// time
	{
		temp_data[0]	= (*temp_str - '0')*10;		temp_str++;			//hh
		temp_data[0]	+= (*temp_str - '0');		temp_str+=2;
		temp_data[1]	= (*temp_str - '0')*10;		temp_str++;			//mm
		temp_data[1]	+= (*temp_str - '0');

	    // Is valid data?
		if((temp_data[0] > 23) || (temp_data[1] > 59))       return false;

	    // set valid time
	    *(&tempInterval[0])	= temp_data[0];
	    *(&tempInterval[1])	= temp_data[1];

	    // Return that new data is available so that it can be displayed.
	    return true;
	}
	else if (type == 2)			// time
	{
		temp_data[0]	= (*temp_str - '0')*10;		temp_str++;			//hh
		temp_data[0]	+= (*temp_str - '0');		temp_str+=2;
		temp_data[1]	= (*temp_str - '0')*10;		temp_str++;			//mm
		temp_data[1]	+= (*temp_str - '0');	    temp_str+=2;
	    if (*temp_str == 'P')	temp_data[2] = 1;

	    // Is valid data?
		if((temp_data[0] > 23) || (temp_data[1] > 59) || (temp_data[2] > 59))       return false;

	    if (disp_clockformat == 0)
	    {
		    if (temp_data[0] > 12)		return false;

//			strcat(edit_tempstr, (1 ? " PM":" AM"));
//		    temp_str+=2;
//		    if (*temp_str == 'A')	temp_data[2] = 1;

		    if ((temp_data[2]) && (temp_data[0] < 12))	temp_data[0] += 12;
	    }

	    // set valid time
	    *(&tempInterval[0])	= temp_data[0];
	    *(&tempInterval[1])	= temp_data[1];

	    // Return that new data is available so that it can be displayed.
	    return true;
	}

	return false;
}
*/
/********************************************************************/
//
/********************************************************************/

void ContactInfo(uint8_t key_code)
{
//uint8_t z_addr;

	if (fun_stage == 0)
	{
		// read data into eeprom
		eeprom_read_contactinfo(0x0F);

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen - title
		GLCD_write_titlebar("CONTACT INFO");
//		z_addr = SCRN_MAP_CUR_Z;

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    GLCD_WriteString("For Support, Call us", false);

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    GLCD_WriteString(contact_phone[0], false);

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    GLCD_WriteString(contact_phone[1], false);

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    GLCD_WriteString("OR Email us :       ", false);

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    GLCD_WriteString(contact_email, false);

		fun_stage = 1;

		GLCD_Screen_Update();
	}
//	else if (fun_stage == 1)
//	{
//		if (key_code == kbEnter)
//		{
//			GetFunFlag = 0;
//		}
//	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}
}

/********************************************************/

/********************************************************/

void SetTimeDate(uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data into eeprom
		eeprom_read_timedate(0x01);

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen - title
		GLCD_write_titlebar("SET TIME/DATE");
		z_addr = SCRN_MAP_CUR_Z;

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    strcpy(disp_tempstr, "SOURCE        ");	    GLCD_WriteString(disp_tempstr, false);

	    if (datetimesource == 0)	// Manual (local controller date & time)
	    	SetTime = CurTime;
	    else						// Computer / server date & time
	    	SetTime = server_time;

	    // Display date and time of selected source
	    goto_screen_map_yz(3, z_addr + 10);
	    usnprintf(disp_tempstr, 18, "TIME        %02u:%02u", SetTime.tm_hour, SetTime.tm_min);
	    if (disp_clockformat == 0)	strcat(disp_tempstr, (SetTime.tm_slot ? " PM":" AM"));
	    GLCD_WriteString(disp_tempstr, false);

	    goto_screen_map_yz(3, z_addr + 20);
	    if (dateformat == 0)
			usnprintf(disp_tempstr, 21, "DATE        %02u/%02u/%02u", SetTime.tm_mday, SetTime.tm_mon, SetTime.tm_year);
	    else
	    	usnprintf(disp_tempstr, 21, "DATE        %02u/%02u/%02u", SetTime.tm_mon, SetTime.tm_mday, SetTime.tm_year);
	    GLCD_WriteString(disp_tempstr, false);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
	    listbox_type = 0;	listbox_index = datetimesource;		listbox_size = 3;    listbox_selected = 0;
		listbox = &datetimesource_list;		listbox_baseval = 0;
		strcpy(disp_tempstr, (*listbox)[listbox_index]);

		fun_col_addr = 64;		fun_line_addr = z_addr;
		y_start = 0;	z_start = 0;	y_end = 127;	z_end = 63;
	}
	else if (fun_stage == 1)	// Source
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[6] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			datetimesource = listbox_index;

		    if (datetimesource == 0)		// Manual (local controller date & time)
		    	SetTime = CurTime;
		    else							// Computer / server date & time
		    	SetTime = server_time;


			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|10||1||%d||1||%s||0||0|*\r\n",datetimesource,deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			// store data into eeprom
			eeprom_write_timedate(0x01);
			eeprom_read_timedate(0x01);

			if (listbox_index == 0)
			{
				// set fun_stage = value for next stage
				// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			    fun_stage = 2;		tool_type = 2;	    sub_fun_state = 1;

				//init variables for textbox_num / textbox_alphanum
			    textbox_char_encoded_trancated = 0;	textbox_size = 5;
			    usnprintf((char *)edit_tempstr, 6, "%02u:%02u", SetTime.tm_hour, SetTime.tm_min);

				fun_col_addr = 76;		fun_line_addr += 10;
				y_start = 0;	z_start = fun_line_addr-10;		y_end = 127;	z_end = fun_line_addr+20;
			}
			else
			{
				// set fun_stage = value for next stage & tool_type = 0
				fun_stage = 5;	    tool_type = 0;

//				SetTime = server_time;

				g_bSetDate = 0x07;//true;

				GLCD_MsgLine_Update(0, fun_line_addr, 127, fun_line_addr+40);
			}
		}
	}
	else if (fun_stage == 2)	// time
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[6] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			if (DateTimeStrToDec(edit_tempstr, 1) == false)
			{
				sub_fun_state = 1;

				usnprintf((char *)edit_tempstr, 6, "%02u:%02u", SetTime.tm_hour, SetTime.tm_min);
				y_start = 76;	z_start = fun_line_addr;		y_end = 127;	z_end = fun_line_addr+9;
			}
			else
			{
				if (disp_clockformat == 0)
				{
					fun_stage = 3;	    tool_type = 1;		sub_fun_state = 1;

					listbox_type = 0;	listbox_index = CurTime.tm_slot;	listbox_size = 2;    listbox_selected = 0;
					listbox = &timeslot_list;
					strcpy(disp_tempstr, (*listbox)[listbox_index]);

					fun_line_addr = fun_line_addr;		fun_col_addr = 112;
					y_start = 76;	z_start = fun_line_addr;	y_end = 126;	z_end = fun_line_addr+9;
				}
				else
				{
					// set fun_stage = value for next stage
					// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
				    fun_stage = 4;		tool_type = 2;	    sub_fun_state = 1;

					//init variables for textbox_num / textbox_alphanum
				    textbox_char_encoded_trancated = 0;	textbox_size = 8;
	//				strcpy((char *)edit_tempstr, disp_rtcdate);

				    if (dateformat == 0)
						usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", SetTime.tm_mday, SetTime.tm_mon, SetTime.tm_year);
				    else
				    	usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", SetTime.tm_mon, SetTime.tm_mday, SetTime.tm_year);

					fun_line_addr += 10;		fun_col_addr = 76;
					y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
				}

			    g_bSetDate = 0x01;//true;
			}
		}
	}
	else if (fun_stage == 3)	// time slot
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[6] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

//			rtctimeslot = listbox_index;
			SetTime.tm_slot = listbox_index;

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		    fun_stage = 4;		tool_type = 2;	    sub_fun_state = 1;

			//init variables for textbox_num / textbox_alphanum
		    textbox_char_encoded_trancated = 0;	textbox_size = 8;
//			strcpy((char *)edit_tempstr, disp_rtcdate);

		    if (dateformat == 0)
				usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", SetTime.tm_mday, SetTime.tm_mon, SetTime.tm_year);
		    else
		    	usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", SetTime.tm_mon, SetTime.tm_mday, SetTime.tm_year);

			fun_line_addr += 10;		fun_col_addr = 76;
			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;

		    g_bSetDate = 0x02;//true;
		}
	}
	else if (fun_stage == 4)	// date
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[6] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

//			strcpy(disp_rtcdate, edit_tempstr);
			if (DateTimeStrToDec(edit_tempstr, 2) == false)
			{
				sub_fun_state = 1;

			    if (dateformat == 0)
					usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", SetTime.tm_mday, SetTime.tm_mon, SetTime.tm_year);
			    else
			    	usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", SetTime.tm_mon, SetTime.tm_mday, SetTime.tm_year);

			    y_start = 76;	z_start = fun_line_addr;		y_end = 127;	z_end = fun_line_addr+9;
			}
			else
			{
				fun_stage = 5;	    tool_type = 0;

				g_bSetDate = 0x04;//true;

				GetFunFlag = 0;
			}
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if (sub_fun_state == 1)	// init
	{
		sub_fun_state = 2;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(disp_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void LoadfromUSB(uint8_t key_code)
{
uint8_t z_addr;

	if (fun_stage == 0)
	{
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		GLCD_write_titlebar("LOAD FROM USB");
		z_addr = SCRN_MAP_CUR_Z + 10;

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
	    GLCD_WriteString(" Do You Want to Load", false);
	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    GLCD_WriteString("  Setting from USB? ", false);

	    fun_line_addr = z_addr;
		fun_stage = 1;

		GLCD_Screen_Update();
	}
	else if (fun_stage == 1)
	{
		if (key_code == kbEnter)
		{
		    goto_screen_map_yz(3, fun_line_addr);
		    GLCD_WriteString("   System Storing   ", false);
		    goto_screen_map_yz(3, fun_line_addr+10);
		    GLCD_WriteString("  to USB Setting..  ", false);

		    GLCD_MsgLine_Update(0, fun_line_addr, 126, fun_line_addr + 20);

		    //delay_sec(2);
		    Task_sleep(2000);

			GetFunFlag = 0;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}
}

/********************************************************/

/********************************************************/

void SavetoUSB(uint8_t key_code)
{
uint8_t z_addr;

	if (fun_stage == 0)
	{
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		GLCD_write_titlebar("SAVE TO USB");
		z_addr = SCRN_MAP_CUR_Z + 10;

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
	    GLCD_WriteString(" Do You Want to Save", false);
	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    GLCD_WriteString("   Setting to USB?  ", false);

	    fun_line_addr = z_addr;
		fun_stage = 1;

		GLCD_Screen_Update();
	}
	else if (fun_stage == 1)
	{
		if (key_code == kbEnter)
		{
		    goto_screen_map_yz(3, fun_line_addr);
		    GLCD_WriteString("   System Storing   ", false);
		    goto_screen_map_yz(3, fun_line_addr+10);
		    GLCD_WriteString("  Setting to USB..  ", false);

		    GLCD_MsgLine_Update(0, fun_line_addr, 126, fun_line_addr + 20);

		    //delay_sec(2);
		    Task_sleep(2000);

			GetFunFlag = 0;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}
}

/********************************************************/

/********************************************************/

void HardwareConfig(uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;
//int8_t temp_val;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)	// modules
	{
		// read data into eeprom
		eeprom_read_hardwareconfig(0xFF);

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen - title
		GLCD_write_titlebar("HARDWARE CONFIG");
		z_addr = SCRN_MAP_CUR_Z;

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
		GLCD_WriteString("MODULES     ", false);

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    usnprintf(disp_tempstr, 21, "FERTIGATION %s", status_list[fertigation]);
 	    GLCD_WriteString(disp_tempstr, false);

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
		usnprintf(disp_tempstr, 21, "FLOW/PULSE  %05u", flowperpulse);
	    GLCD_WriteString(disp_tempstr, false);

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
		strcpy(disp_tempstr, "MIN OPERAT PRES ");	ftoa(disp_tempstr+16, minop);
	    GLCD_WriteString(disp_tempstr, false);

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    strcpy(disp_tempstr, "MAX DELTA PRESR ");	ftoa(disp_tempstr+16, maxdp);
	    GLCD_WriteString(disp_tempstr, false);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
	    listbox_type = 2;	listbox_index = noopmodules;		listbox_size = 2;    listbox_selected = 0;
		listbox = &virtual_list;	listbox_baseval = 1;
		usnprintf(disp_tempstr, 3, "%02u", (listbox_baseval + listbox_index));

		fun_line_addr = z_addr;		fun_col_addr = 76;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// modules
	{
		if (key_code == kbEnter)
		{
			//for sync logic
						table_changed[6] = 1;
						eeprom_write_progstatus(0x40);
						eeprom_read_progstatus(0x40);


			noopmodules = listbox_index;

			// store data into eeprom
			eeprom_write_hardwareconfig(0x01);
			eeprom_read_hardwareconfig(0x01);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|10||2||%d||1||%s||0||0|*\r\n",(noopmodules+1),deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 2;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 0;	listbox_index = fertigation;		listbox_size = 2;    listbox_selected = 0;
			listbox = &status_list;		listbox_baseval = 0;
			strcpy(disp_tempstr, (*listbox)[listbox_index]);

			fun_line_addr += 10;	//fun_col_addr = 76;
			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 2)	// fertigation
	{
		if (key_code == kbEnter)
		{
			//for sync logic
						table_changed[6] = 1;
						eeprom_write_progstatus(0x40);
						eeprom_read_progstatus(0x40);

			fertigation = listbox_index;

			// store data into eeprom
			eeprom_write_hardwareconfig(0x02);
			eeprom_read_hardwareconfig(0x02);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|10||3||%d||1||%s||0||0|*\r\n",fertigation,deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 3;	    tool_type = 2;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			//$$NR$$//dated:19Sep15
			textbox_char_encoded_trancated = 4;			textbox_size = 5;
			editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			//$$NR$$//dated:19Sep15
			//			textbox_char_encoded_trancated = 0;	textbox_size = 5;	//$$NR$$//dated:07Nov15
			usnprintf(edit_tempstr, 6, "%05u", flowperpulse);

			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15
			fun_line_addr += 10;	//fun_col_addr = 76;
			y_start = 76;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 3)	// flow / pulse
	{
		if (key_code == kbEnter)
		{
			//for sync logic
						table_changed[6] = 1;
						eeprom_write_progstatus(0x40);
						eeprom_read_progstatus(0x40);

			flowperpulse = (uint32_t)strtoint(edit_tempstr, 5);//strtofloat(edit_tempstr);

			// store data into eeprom
			eeprom_write_hardwareconfig(0x04);
			eeprom_read_hardwareconfig(0x04);
			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|10||4||%d||1||%s||0||0|*\r\n",flowperpulse,deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 4;	    tool_type = 2;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			//$$NR$$//dated:19Sep15
			textbox_char_encoded_trancated = 4;			textbox_size = 4;
			editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			//$$NR$$//dated:19Sep15

			ftoa(edit_tempstr, minop);
			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			fun_line_addr += 10;	fun_col_addr = 100;
			y_start = 76;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 4)	// min operating pressure
	{
		if (key_code == kbEnter)
		{
			//for sync logic
						table_changed[6] = 1;
						eeprom_write_progstatus(0x40);
						eeprom_read_progstatus(0x40);


			minop = strtofloat(edit_tempstr);

			// store data into eeprom
			eeprom_write_hardwareconfig(0x08);
			eeprom_read_hardwareconfig(0x08);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|10||5||%.2lf||1||%s||0||0|*\r\n",minop,deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 5;	    tool_type = 2;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			//$$NR$$//dated:19Sep15
			textbox_char_encoded_trancated = 4;			textbox_size = 4;
			editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			//$$NR$$//dated:19Sep15

			ftoa(edit_tempstr, maxdp);
			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			fun_line_addr += 10;	fun_col_addr = 100;
			y_start = 76;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 5)	// max delta pressure
	{
		if (key_code == kbEnter)
		{
			//for sync logic
						table_changed[6] = 1;
						eeprom_write_progstatus(0x40);
						eeprom_read_progstatus(0x40);


			maxdp = strtofloat(edit_tempstr);

			// store data into eeprom
			eeprom_write_hardwareconfig(0x10);
			eeprom_read_hardwareconfig(0x10);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|10||6||%.2lf||1||%s||0||0|*\r\n",maxdp,deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

//			fun_stage = 6;	    tool_type = 0;

//			GetFunFlag = 0;
			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
		    listbox_type = 2;	listbox_index = noopmodules;		listbox_size = 2;    listbox_selected = 0;
			listbox = &virtual_list;	listbox_baseval = 1;
			usnprintf(disp_tempstr, 3, "%02u", (listbox_baseval + listbox_index));

			fun_line_addr -= 40;		fun_col_addr = 76;
			y_start = 76;	z_start = fun_line_addr;		y_end = 126;	z_end = 63;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if (sub_fun_state == 1)	// init
	{
		sub_fun_state = 2;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
		{
			GLCD_WriteString(disp_tempstr, true);
		}
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void SystemSetup(uint8_t key_code)
{
uint8_t z_addr;
int8_t temp_val;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)	//languages
	{
		// read data into eeprom
		eeprom_read_systemsetup(0xFF);

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen - title
		GLCD_write_titlebar("SYSTEM SETUP");
		z_addr = SCRN_MAP_CUR_Z;

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
		GLCD_WriteString("LANGUAGE      ", false);	//GLCD_WriteString("LANGUAGE     ENGLISH", false);

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
    	usnprintf(disp_tempstr, 21, ((contrast<15)?"CONTRAST         %03d":"CONTRAST         +%02d"), (contrast - 15));
	    GLCD_WriteString(disp_tempstr, false);

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    strcpy(disp_tempstr, "CLOCK FORMAT ");
	    strcat(disp_tempstr, disp_clockformat_list[disp_clockformat]);
	    GLCD_WriteString(disp_tempstr, false);

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    strcpy(disp_tempstr, "DATE FORMAT ");
	    strcat(disp_tempstr, dateformat_list[dateformat]);
	    GLCD_WriteString(disp_tempstr, false);

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    strcpy(disp_tempstr, "BEGIN WEEK ");
	    strcat(disp_tempstr, beginweek_list[beginweek]);
	    GLCD_WriteString(disp_tempstr, false);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
	    listbox_type = 0;	listbox_index = language;		listbox_size = 1;    listbox_selected = 0;
		listbox = &language_list;	listbox_baseval = 0;
		strcpy(disp_tempstr, (*listbox)[listbox_index]);

		fun_col_addr = 82;		fun_line_addr = z_addr;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	//languages
	{
		if (key_code == kbEnter)
		{
			//for sync logic
						table_changed[6] = 1;
						eeprom_write_progstatus(0x40);
						eeprom_read_progstatus(0x40);

			language = listbox_index;

			// store data into eeprom
			eeprom_write_systemsetup(0x01);
			eeprom_read_systemsetup(0x01);
			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|10||7||%d||1||%s||0||0|*\r\n",language,deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 2;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 1;	listbox_index = contrast;		listbox_size = 31;    listbox_selected = 0;
			listbox = &virtual_list;	listbox_baseval = -15;
			temp_val = listbox_baseval + listbox_index;
			usnprintf(disp_tempstr, 4, ((temp_val<0)?"%03d":"+%02d"), temp_val);

			fun_line_addr += 10;	fun_col_addr = 106;
			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 2)	//Contrast
	{
		if (key_code == kbEnter)
		{
			//for sync logic
						table_changed[6] = 1;
						eeprom_write_progstatus(0x40);
						eeprom_read_progstatus(0x40);

			contrast = listbox_index;

			// store data into eeprom
			eeprom_write_systemsetup(0x02);
			eeprom_read_systemsetup(0x02);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|10||8||%d||1||%s||0||0|*\r\n",contrast,deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}
			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 3;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 0;	listbox_index = disp_clockformat;		listbox_size = 2;    listbox_selected = 0;
			listbox = &disp_clockformat_list;
			strcpy(disp_tempstr, (*listbox)[listbox_index]);

			fun_line_addr += 10;	fun_col_addr = 82;
			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 3)	// clock format
	{

		if (key_code == kbEnter)
		{
			//for sync logic
								table_changed[6] = 1;
								eeprom_write_progstatus(0x40);
								eeprom_read_progstatus(0x40);

			disp_clockformat = listbox_index;

			// store data into eeprom
			eeprom_write_systemsetup(0x04);
			eeprom_read_systemsetup(0x04);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|10||9||%d||1||%s||0||0|*\r\n",disp_clockformat,deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 4;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 0;	listbox_index = dateformat;		listbox_size = 2;    listbox_selected = 0;
			listbox = &dateformat_list;
			strcpy(disp_tempstr, (*listbox)[listbox_index]);

			fun_line_addr += 10;	fun_col_addr = 76;
			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 4)	//date format
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[6] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			dateformat = listbox_index;

			// store data into eeprom
			eeprom_write_systemsetup(0x08);
			eeprom_read_systemsetup(0x08);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|10||10||%d||1||%s||0||0|*\r\n",dateformat,deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}
			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 5;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 0;	listbox_index = beginweek;		listbox_size = 7;    listbox_selected = 0;
			listbox = &beginweek_list;
			strcpy(disp_tempstr, (*listbox)[listbox_index]);

			fun_line_addr += 10;	fun_col_addr = 70;
			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = 63;
		}
	}
	else if (fun_stage == 5)	// begin week
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[6] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			beginweek = listbox_index;

			// store data into eeprom
			eeprom_write_systemsetup(0x10);
			eeprom_read_systemsetup(0x10);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|10||11||%d||1||%s||0||0|*\r\n",beginweek,deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}
//			fun_stage = 6;	    tool_type = 0;

//			GetFunFlag = 0;

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
		    listbox_type = 0;	listbox_index = language;		listbox_size = 1;    listbox_selected = 0;
			listbox = &language_list;	listbox_baseval = 0;
			strcpy(disp_tempstr, (*listbox)[listbox_index]);

			fun_col_addr = 82;		fun_line_addr -= 40;
			y_start = 70;	z_start = fun_line_addr;		y_end = 126;	z_end = 63;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if (sub_fun_state == 1)	// init
	{
		sub_fun_state = 2;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
		{
			GLCD_WriteString(disp_tempstr, true);
		}
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/
void OP_TypeID_Update(void);

void OP_Assignment(uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;
uint8_t i;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		GLCD_clear_screen(false);						// clear the lcd display

		// update o/p type id
		OP_TypeID_Update();

		// Display Screen - title
		GLCD_write_titlebar("OUTPUT ASSIGNMENT");
		z_addr = SCRN_MAP_CUR_Z;

	    goto_screen_map_yz(3, z_addr);
	    strcpy(disp_tempstr, "PORT NO  ");	//strcat(disp_tempstr, oppartno);
	    GLCD_WriteString(disp_tempstr, false);

	    goto_screen_map_yz(3, z_addr + 10);
	    usnprintf(disp_tempstr, 21, "TYPE     %s", optype_list[output[fun_result1].optype]);
	    GLCD_WriteString(disp_tempstr, false);

	    goto_screen_map_yz(3, z_addr + 20);
		usnprintf(disp_tempstr, 21, "TYPE ID  %02u", output[fun_result1].typeid);
	    GLCD_WriteString(disp_tempstr, false);

	    goto_screen_map_yz(3, z_addr + 30);
	    usnprintf(disp_tempstr, 21, "NAME     %s", output[fun_result1].opname);
	    GLCD_WriteString(disp_tempstr, false);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t - all) / 3(uint8_t - selected)
	    listbox_type = 2;	listbox_index = fun_result1;		listbox_size = ((noopmodules + 1) * 8);	    listbox_selected = 0;
		listbox = &virtual_list;	listbox_baseval = 1;
		usnprintf(disp_tempstr, 3, "%02u", (listbox_baseval + listbox_index));

		fun_col_addr = 58;		fun_line_addr = z_addr;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// o/p port no
	{
		if (key_code == kbEnter)
		{
			fun_result1 = listbox_index;

		    goto_screen_map_yz(3, fun_line_addr + 10);
		    usnprintf(disp_tempstr, 21, "TYPE                ");
		    GLCD_WriteString(disp_tempstr, false);

//		    goto_screen_map_yz(58, fun_line_addr + 10);
//		    strcpy(disp_tempstr, optype_list[output[fun_result1].optype]);
//		    GLCD_WriteString(disp_tempstr, false);

			goto_screen_map_yz(3, fun_line_addr + 20);
			usnprintf(disp_tempstr, 21, "TYPE ID  %02u", output[fun_result1].typeid);
		    GLCD_WriteString(disp_tempstr, false);

		    goto_screen_map_yz(3, fun_line_addr + 30);
		    usnprintf(disp_tempstr, 21, "NAME     %10s", output[fun_result1].opname);
		    GLCD_WriteString(disp_tempstr, false);

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 2;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 0;	listbox_index = output[fun_result1].optype;		listbox_size = 6;	listbox_selected = 0;

			//get list of selected o/p types
			for (i=0; i<((noopmodules + 1) * 8); i++)
			{
				//check if o/p type is master pump / ferti pump
				if ((output[i].optype == 1) || (output[i].optype == 4))
				{
					if (i != fun_result1)		listbox_selected |= (0x01 << output[i].optype);
				}
			}

			listbox = &optype_list;
			strcpy(disp_tempstr, (*listbox)[listbox_index]);

			fun_line_addr += 10;		fun_col_addr = 58;
//			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
	}
	else if (fun_stage == 2)	// op type
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[1] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			output[fun_result1].optype = listbox_index;

			eeprom_write_output(fun_result1);
			eeprom_read_output(fun_result1);

			// update o/p type id
			OP_TypeID_Update();

			goto_screen_map_yz(3, fun_line_addr + 10);
			usnprintf(disp_tempstr, 21, "TYPE ID  %02u", output[fun_result1].typeid);
		    GLCD_WriteString(disp_tempstr, false);

			// set fun_stage = value for next stage
			fun_stage = 3;	    tool_type = 3;		sub_fun_state = 1;

			textbox_size = 10;
			strcpy((char *)edit_tempstr, output[fun_result1].opname);
			if (strlen(edit_tempstr) == 0)
				strcpy((char *)edit_tempstr, "          ");

			fun_line_addr += 20;		//fun_col_addr = 58;
			y_start = fun_col_addr;	z_start = fun_line_addr-20;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 3)	// op name
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[1] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);
			strcpy(output[fun_result1].opname, edit_tempstr);

			// store data into eeprom
			eeprom_write_output(fun_result1);
			eeprom_read_output(fun_result1);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|1||1||%d||1||%s||0||%d|*\r\n",output[fun_result1].optype,deviceid,(fun_result1+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
				sprintf (msg_send,"*|1||3||%s||1||%s||0||%d|*\r\n",output[fun_result1].opname,deviceid,(fun_result1+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
				sprintf (msg_send,"*|1||2||%d||1||%s||0||%d|*\r\n",output[fun_result1].typeid,deviceid,(fun_result1+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			fun_stage = 0;

			fun_result1++;
			if (fun_result1 >= ((noopmodules + 1) * 8))		fun_result1 = 0;

			OP_Assignment(kbVirtualEdit);

			return;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if (sub_fun_state == 1)	// init
	{
		sub_fun_state = 2;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(disp_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15

			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/*****************************************/

/*****************************************/

void OP_TypeID_Update(void)
{
uint8_t i, typeid_cnt[6] = {0, 0, 0, 0, 0, 0};

	//get list of selected o/p types
	for (i=0; i<((noopmodules + 1) * 8); i++)
	{
		// read data into eeprom
		eeprom_read_output(i);

		typeid_cnt[output[i].optype]++;

		//set o/p type id for o/p type
		output[i].typeid = typeid_cnt[output[i].optype];
	}
}

/*****************************************/

/*****************************************/
//$$NR$$//dated:23Aug15

uint8_t get_output_id(uint8_t tmp_optype, uint8_t tmp_typeid)
{
uint8_t tmp_opid = 0;

	//get o/p id
	while(tmp_opid < OUTPUT_MAX)
	{
		// read data into eeprom
		eeprom_read_output(tmp_opid);

		if ((output[tmp_opid].optype == tmp_optype) &&
			(output[tmp_opid].typeid == tmp_typeid))
		{
			return(tmp_opid);
		}

		tmp_opid++;
	}

	return(0xFF);
}
//$$NR$$//dated:23Aug15

/********************************************************/

/********************************************************/

void IP_Assignment(uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;
//char tempstr1[5], tempstr2[5];

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data into eeprom
		eeprom_read_input(fun_result1);

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		GLCD_write_titlebar("INPUT ASSIGNMENT");
		z_addr = SCRN_MAP_CUR_Z+10;

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
	    strcpy(disp_tempstr, "PORT NO  ");
	    GLCD_WriteString(disp_tempstr, false);

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z);

//$$NR$$//dated:16Aug15
//		strcpy(disp_tempstr, "NAME     ");	strcat(disp_tempstr, input[fun_result1].ipname);

		strcpy(disp_tempstr, "NAME  ");
		if (fun_result1 <= 2)			//$$NR$$//dated:23Aug15
			strcat(disp_tempstr, ipname_pump_list[input[fun_result1].iptype]);
		else
			strcat(disp_tempstr, ipname_fmeter_list[input[fun_result1].iptype]);
//$$NR$$//dated:16Aug15

		GLCD_WriteString(disp_tempstr, false);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t - all) / 3(uint8_t - selected)
	    listbox_type = 2;	listbox_index = fun_result1;		listbox_size = INPUT_MAX;	    listbox_selected = 0;
		listbox = &virtual_list;	listbox_baseval = 1;
		usnprintf(disp_tempstr, 3, "%02u", (listbox_baseval + listbox_index));

		fun_col_addr = 58;		fun_line_addr = z_addr;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// i/p port no
	{
		if (key_code == kbEnter)
		{

			fun_result1 = listbox_index;


//$$NR$$//dated:16Aug15
/*
			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 2;	    tool_type = 3;		sub_fun_state = 1;

			textbox_size = 10;
			strcpy((char *)edit_tempstr, input[fun_result1].ipname);
			if (strlen(edit_tempstr) == 0)
				strcpy((char *)edit_tempstr, "          ");

			fun_line_addr += 10;		fun_col_addr = 58;
			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
*/
			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 2;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t - all) / 3(uint8_t - selected)
		    listbox_type = 0;	listbox_index = input[fun_result1].iptype;		listbox_size = 3;	    listbox_selected = 0;

			if (fun_result1 <= 2)				//$$NR$$//dated:23Aug15
				listbox = &ipname_pump_list;
			else
				listbox = &ipname_fmeter_list;

			strcpy(disp_tempstr, (*listbox)[listbox_index]);

			fun_line_addr += 10;		fun_col_addr = 40;
			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;

//$$NR$$//dated:16Aug15
		}
	}
	else if (fun_stage == 2)	// ip name
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[1] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);
//$$NR$$//dated:16Aug15
//			strcpy(input[fun_result1].ipname, edit_tempstr);
			input[fun_result1].iptype = listbox_index;

//$$NR$$//dated:16Aug15

			// store data into eeprom
			eeprom_write_input(fun_result1);
			eeprom_read_input(fun_result1);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
//$$NR$$//dated:16Aug15
//				sprintf (msg_send,"*|2||1||%s||1||%s||0||%d|*\r\n",input[fun_result1].ipname,deviceid,(fun_result1+1));
				//sprintf (msg_send,"*|2||1||%s||1||%s||0||%d|*\r\n",(*listbox)[listbox_index],deviceid,(fun_result1+1));
				sprintf (msg_send,"*|2||1||%d||1||%s||0||%d|*\r\n",input[fun_result1].iptype,deviceid,(fun_result1+1));
//$$NR$$//dated:16Aug15

				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			fun_stage = 0;

			fun_result1++;
			if (fun_result1 >= INPUT_MAX)	fun_result1 = 0;

			IP_Assignment(kbVirtualEdit);

			return;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if (sub_fun_state == 1)	// init
	{
		sub_fun_state = 2;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(disp_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void Sensor_Assignment(uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;
char tempstr1[5], tempstr2[5];

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data into eeprom
		eeprom_read_sensor(fun_result1);

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		GLCD_write_titlebar("SENSOR ASSIGNMENT");
		z_addr = SCRN_MAP_CUR_Z;

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
//		usnprintf(disp_tempstr, 21, "SENSOR   %s", sensortype_list[sensor[fun_result1].sensortype]);
		usnprintf(disp_tempstr, 21, "SENSOR   ");
		GLCD_WriteString(disp_tempstr, false);

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
		usnprintf(disp_tempstr, 21, "SERIAL ID%02u", sensor[fun_result1].typeid);
		GLCD_WriteString(disp_tempstr, false);

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
//		snprintf(disp_tempstr, 21, "MIN  %04.1f  MAX  %04.1f", input[0].min, input[0].max); //5//16
	    ftoa(tempstr1, sensor[fun_result1].min);	    ftoa(tempstr2, sensor[fun_result1].max);
		usnprintf(disp_tempstr, 21, "MIN  %s  MAX  %s", tempstr1, tempstr2);
		GLCD_WriteString(disp_tempstr, false);

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z);

//$$NR$$//dated:16Aug15
//		usnprintf(disp_tempstr, 21, "UNIT     %s", sensor[fun_result1].unit);

		if (sensor[fun_result1].sensortype == 1)
			usnprintf(disp_tempstr, 21, "UNIT     %s", unitname_rain_list[sensor[fun_result1].unittype]);
		else if (sensor[fun_result1].sensortype == 2)
			usnprintf(disp_tempstr, 21, "UNIT     %s", unitname_temp_list[sensor[fun_result1].unittype]);
		else if (sensor[fun_result1].sensortype == 3)
			usnprintf(disp_tempstr, 21, "UNIT     %s", unitname_humid_list[sensor[fun_result1].unittype]);
		else if (sensor[fun_result1].sensortype == 4)
			usnprintf(disp_tempstr, 21, "UNIT     %s", unitname_flow_list[sensor[fun_result1].unittype]);
		else if (sensor[fun_result1].sensortype >= 5)
			usnprintf(disp_tempstr, 21, "UNIT     %s", unitname_presr_list[sensor[fun_result1].unittype]);
//$$NR$$//dated:16Aug15

		GLCD_WriteString(disp_tempstr, false);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
		listbox_type = 0;	listbox_index = fun_result1;/*sensor[fun_result1].sensortype;*/		listbox_size = 6;    listbox_selected = 0;
		listbox = &sensortype_list;
		strcpy(disp_tempstr, (*listbox)[listbox_index]);

		fun_col_addr = 58;		fun_line_addr = z_addr;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// sensor type
	{
		if (key_code == kbEnter)
		{
			fun_result1 = listbox_index;
			sensor[fun_result1].sensortype = fun_result1+1;

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 2;	    tool_type = 2;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			//$$NR$$//dated:19Sep15
			textbox_char_encoded_trancated = 4;			textbox_size = 2;
			editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			//$$NR$$//dated:19Sep15

			usnprintf(edit_tempstr, 3, "%02u", sensor[fun_result1].typeid);
			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			fun_col_addr = 58;		fun_line_addr += 10;
			y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 2)	// sensor type id
	{
		if (key_code == kbEnter)
		{
			sensor[fun_result1].typeid = (uint8_t)strtoint(edit_tempstr, 2);

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 3;	    tool_type = 2;		sub_fun_state = 1;

			//$$NR$$//dated:19Sep15
			textbox_char_encoded_trancated = 4;			textbox_size = 4;
			editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			//$$NR$$//dated:19Sep15

//			snprintf(edit_tempstr, 5, "%04.1f", input[fun_result1].min);
			ftoa(edit_tempstr, sensor[fun_result1].min);
			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			fun_line_addr += 10;		fun_col_addr = 34;
			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 3)	// min
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[2] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			sensor[fun_result1].min = strtofloat(edit_tempstr);

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 4;	    tool_type = 2;		sub_fun_state = 1;

			//$$NR$$//dated:19Sep15
			textbox_char_encoded_trancated = 4;			textbox_size = 4;
			editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			//$$NR$$//dated:19Sep15

//			snprintf(edit_tempstr, 5, "%04.1f", input[fun_result1].max);
			ftoa(edit_tempstr, sensor[fun_result1].max);
			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			fun_col_addr = 100;		//fun_line_addr += 10;
			y_start = 34;	z_start = fun_line_addr-1;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 4)	// max
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[2] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			sensor[fun_result1].max = strtofloat(edit_tempstr);

//$$NR$$//dated:16Aug15
/*
			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 5;	    tool_type = 3;		sub_fun_state = 1;

			textbox_char_encoded_trancated = 0;	textbox_size = 10;
			usnprintf(edit_tempstr, 11, "%10s", sensor[fun_result1].unit);
			if (strlen(edit_tempstr) == 0)
				strcpy((char *)edit_tempstr, "          ");

			fun_line_addr += 10;		fun_col_addr = 58;
			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
*/
			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 5;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 0;	listbox_index = sensor[fun_result1].unittype;		listbox_size = 2;    listbox_selected = 0;

			if (sensor[fun_result1].sensortype == 1)
				listbox = &unitname_rain_list;
			else if (sensor[fun_result1].sensortype == 2)
			{
				listbox = &unitname_temp_list;	listbox_size = 3;
			}
			else if (sensor[fun_result1].sensortype == 3)
				listbox = &unitname_humid_list;
			else if (sensor[fun_result1].sensortype == 4)
			{
				listbox = &unitname_flow_list;	listbox_size = 5;
			}
			else if (sensor[fun_result1].sensortype >= 5)
				listbox = &unitname_presr_list;			listbox_size = 3;

			strcpy(disp_tempstr, (*listbox)[listbox_index]);

			fun_line_addr += 10;		fun_col_addr = 58;
			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
//$$NR$$//dated:16Aug15
		}
	}
	else if (fun_stage == 5)	// unit
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[2] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

//			strcpy((char *)sensor[fun_result1].unit, edit_tempstr);		//$$NR$$//dated:16Aug15
			sensor[fun_result1].unittype = listbox_index;				//$$NR$$//dated:16Aug15

//			// set fun_stage = value for next stage
//			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
//			fun_stage = 6;	    tool_type = 3;		sub_fun_state = 1;
//
//			textbox_size = 10;
//			strcpy((char *)edit_tempstr, sensor[fun_result1].sensorname);
//			if (strlen(edit_tempstr) == 0)
//				strcpy((char *)edit_tempstr, "          ");
//
//			fun_line_addr += 10;		//fun_col_addr = 58;
//			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
//		}
//	}
//	else if (fun_stage == 6)	// ip name
//	{
//		if (key_code == kbEnter)
//		{
//			strcpy(sensor[fun_result1].sensorname, edit_tempstr);

			// store data into eeprom
			eeprom_write_sensor(fun_result1);
			eeprom_read_sensor(fun_result1);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|3||1||%d||1||%s||0||%d|*\r\n",(sensor[fun_result1].sensortype),deviceid,(fun_result1+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
				sprintf (msg_send,"*|3||2||%d||1||%s||0||%d|*\r\n",sensor[fun_result1].serialid,deviceid,(fun_result1+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
				sprintf (msg_send,"*|3||3||%d||1||%s||0||%d|*\r\n",sensor[fun_result1].typeid,deviceid,(fun_result1+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|3||6||%s||1||%s||0||%d|*\r\n",sensor[fun_result1].unit,deviceid,(fun_result1+1));		//$$NR$$//dated:16Aug15
				sprintf (msg_send,"*|3||6||%d||1||%s||0||%d|*\r\n",sensor[fun_result1].unittype,deviceid,(fun_result1+1));	//$$NR$$//dated:16Aug15
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
				sprintf (msg_send,"*|3||5||%.2lf||1||%s||0||%d|*\r\n",sensor[fun_result1].max,deviceid,(fun_result1+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
				sprintf (msg_send,"*|3||4||%.2lf||1||%s||0||%d|*\r\n",sensor[fun_result1].min,deviceid,(fun_result1+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			fun_stage = 0;

			fun_result1++;
			if (fun_result1 >= 6)	fun_result1 = 0;

			Sensor_Assignment(kbVirtualEdit);

			return;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if (sub_fun_state == 1)	// init
	{
		sub_fun_state = 2;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(disp_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}
//void Sensor_Assignment(uint8_t key_code)
//{
//uint8_t z_addr;
//uint8_t y_start, z_start, y_end, z_end;
//char tempstr1[5], tempstr2[5];
//
//	if (tool_type == 1)
//		key_code = select_list_option(key_code);
//	else if (tool_type == 2)
//		key_code = edit_text_numeric(key_code);
//	else if (tool_type == 3)
//		key_code = edit_text_alphanumeric(key_code);
//
//	if (key_code == 0)	return;
//
//	if (fun_stage == 0)
//	{
//		// read data into eeprom
//		eeprom_read_sensor(fun_result1);
//
//		GLCD_clear_screen(false);						// clear the lcd display
//
//		// Display Screen
//		GLCD_write_titlebar("SENSOR ASSIGNMENT");
//		z_addr = SCRN_MAP_CUR_Z;
//
//		goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
////		usnprintf(disp_tempstr, 21, "SENSOR   %s", sensortype_list[sensor[fun_result1].sensortype]);
//		usnprintf(disp_tempstr, 21, "SENSOR   ");
//		GLCD_WriteString(disp_tempstr, false);
//
//		goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
//		usnprintf(disp_tempstr, 21, "TYPE ID  %02u", sensor[fun_result1].typeid);
//		GLCD_WriteString(disp_tempstr, false);
//
//		goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
////		snprintf(disp_tempstr, 21, "MIN  %04.1f  MAX  %04.1f", input[0].min, input[0].max); //5//16
//	    ftoa(tempstr1, sensor[fun_result1].min);	    ftoa(tempstr2, sensor[fun_result1].max);
//		usnprintf(disp_tempstr, 21, "MIN  %s  MAX  %s", tempstr1, tempstr2);
//		GLCD_WriteString(disp_tempstr, false);
//
//		goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
//
////$$NR$$//dated:16Aug15
////		usnprintf(disp_tempstr, 21, "UNIT     %s", sensor[fun_result1].unit);
//
//		if (sensor[fun_result1].sensortype == 0)
//			usnprintf(disp_tempstr, 21, "UNIT     %s", unitname_rain_list[sensor[fun_result1].unittype]);
//		else if (sensor[fun_result1].sensortype == 1)
//			usnprintf(disp_tempstr, 21, "UNIT     %s", unitname_temp_list[sensor[fun_result1].unittype]);
//		else if (sensor[fun_result1].sensortype == 2)
//			usnprintf(disp_tempstr, 21, "UNIT     %s", unitname_humid_list[sensor[fun_result1].unittype]);
//		else if (sensor[fun_result1].sensortype == 3)
//			usnprintf(disp_tempstr, 21, "UNIT     %s", unitname_flow_list[sensor[fun_result1].unittype]);
//		else if (sensor[fun_result1].sensortype >= 4)
//			usnprintf(disp_tempstr, 21, "UNIT     %s", unitname_presr_list[sensor[fun_result1].unittype]);
////$$NR$$//dated:16Aug15
//
//		GLCD_WriteString(disp_tempstr, false);
//
//		// set fun_stage = value for next stage
//		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
//		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;
//
//		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
//		listbox_type = 0;	listbox_index = fun_result1;/*sensor[fun_result1].sensortype;*/		listbox_size = 6;    listbox_selected = 0;
//		listbox = &sensortype_list;
//		strcpy(disp_tempstr, (*listbox)[listbox_index]);
//
//		fun_col_addr = 58;		fun_line_addr = z_addr;
//		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
//	}
//	else if (fun_stage == 1)	// sensor type
//	{
//		if (key_code == kbEnter)
//		{
//			fun_result1 = listbox_index;
//			sensor[fun_result1].sensortype = fun_result1;
//
//			// set fun_stage = value for next stage
//			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
//			fun_stage = 2;	    tool_type = 2;		sub_fun_state = 1;
//
//			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
//			textbox_char_encoded_trancated = 0;	textbox_size = 2;
//
//			usnprintf(edit_tempstr, 3, "%02u", sensor[fun_result1].typeid);
//
//			fun_col_addr = 58;		fun_line_addr += 10;
//			y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
//		}
//	}
//	else if (fun_stage == 2)	// sensor type id
//	{
//		if (key_code == kbEnter)
//		{
//			sensor[fun_result1].typeid = (uint8_t)strtoint(edit_tempstr, 2);
//
//			// set fun_stage = value for next stage
//			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
//			fun_stage = 3;	    tool_type = 2;		sub_fun_state = 1;
//
//			textbox_char_encoded_trancated = 0;	textbox_size = 4;
////			snprintf(edit_tempstr, 5, "%04.1f", input[fun_result1].min);
//		    ftoa(edit_tempstr, sensor[fun_result1].min);
//
//			fun_line_addr += 10;		fun_col_addr = 34;
//			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
//		}
//	}
//	else if (fun_stage == 3)	// min
//	{
//		if (key_code == kbEnter)
//		{
//			sensor[fun_result1].min = strtofloat(edit_tempstr);
//
//			// set fun_stage = value for next stage
//			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
//			fun_stage = 4;	    tool_type = 2;		sub_fun_state = 1;
//
//			textbox_char_encoded_trancated = 0;	textbox_size = 4;
////			snprintf(edit_tempstr, 5, "%04.1f", input[fun_result1].max);
//		    ftoa(edit_tempstr, sensor[fun_result1].max);
//
//			fun_col_addr = 100;		//fun_line_addr += 10;
//			y_start = 34;	z_start = fun_line_addr-1;		y_end = 126;	z_end = fun_line_addr+9;
//		}
//	}
//	else if (fun_stage == 4)	// max
//	{
//		if (key_code == kbEnter)
//		{
//			sensor[fun_result1].max = strtofloat(edit_tempstr);
//
////$$NR$$//dated:16Aug15
///*
//			// set fun_stage = value for next stage
//			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
//			fun_stage = 5;	    tool_type = 3;		sub_fun_state = 1;
//
//			textbox_char_encoded_trancated = 0;	textbox_size = 10;
//			usnprintf(edit_tempstr, 11, "%10s", sensor[fun_result1].unit);
//			if (strlen(edit_tempstr) == 0)
//				strcpy((char *)edit_tempstr, "          ");
//
//			fun_line_addr += 10;		fun_col_addr = 58;
//			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
//*/
//			// set fun_stage = value for next stage
//			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
//			fun_stage = 5;	    tool_type = 1;		sub_fun_state = 1;
//
//			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
//			listbox_type = 0;	listbox_index = sensor[fun_result1].unittype;		listbox_size = 2;    listbox_selected = 0;
//
//
//			if (sensor[fun_result1].sensortype == 1)
//				listbox = &unitname_rain_list;
//			else if (sensor[fun_result1].sensortype == 2)
//			{
//				listbox_size = 3;
//				listbox = &unitname_temp_list;
//			}
//			else if (sensor[fun_result1].sensortype == 3)
//				listbox = &unitname_humid_list;
//			else if (sensor[fun_result1].sensortype == 4)
//			{
//				listbox_size = 5;
//				listbox = &unitname_flow_list;
//			}
//			else if (sensor[fun_result1].sensortype >= 5)
//			{
//				listbox = &unitname_presr_list;			listbox_size = 3;
//			}
//
//			strcpy(disp_tempstr, (*listbox)[listbox_index]);
//
//			fun_line_addr += 10;		fun_col_addr = 58;
//			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
////$$NR$$//dated:16Aug15
//		}
//	}
//	else if (fun_stage == 5)	// unit
//	{
//		if (key_code == kbEnter)
//		{
////			strcpy((char *)sensor[fun_result1].unit, edit_tempstr);		//$$NR$$//dated:16Aug15
//			sensor[fun_result1].unittype = listbox_index;				//$$NR$$//dated:16Aug15
//
////			// set fun_stage = value for next stage
////			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
////			fun_stage = 6;	    tool_type = 3;		sub_fun_state = 1;
////
////			textbox_size = 10;
////			strcpy((char *)edit_tempstr, sensor[fun_result1].sensorname);
////			if (strlen(edit_tempstr) == 0)
////				strcpy((char *)edit_tempstr, "          ");
////
////			fun_line_addr += 10;		//fun_col_addr = 58;
////			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
////		}
////	}
////	else if (fun_stage == 6)	// ip name
////	{
////		if (key_code == kbEnter)
////		{
////			strcpy(sensor[fun_result1].sensorname, edit_tempstr);
//
//			// store data into eeprom
//			eeprom_write_sensor(fun_result1);
//			eeprom_read_sensor(fun_result1);
//
//			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
//			{
//				sprintf (msg_send,"*|3||1||%d||1||%s||0||%d|*\r\n",sensor[fun_result1].sensortype,deviceid,(fun_result1+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|3||2||%d||1||%s||0||%d|*\r\n",sensor[fun_result1].serialid,deviceid,(fun_result1+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|3||3||%d||1||%s||0||%d|*\r\n",sensor[fun_result1].typeid,deviceid,(fun_result1+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
////				sprintf (msg_send,"*|3||6||%s||1||%s||0||%d|*\r\n",sensor[fun_result1].unit,deviceid,(fun_result1+1));		//$$NR$$//dated:16Aug15
//				sprintf (msg_send,"*|3||6||%d||1||%s||0||%d|*\r\n",sensor[fun_result1].unittype,deviceid,(fun_result1+1));	//$$NR$$//dated:16Aug15
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|3||5||%.2lf||1||%s||0||%d|*\r\n",sensor[fun_result1].max,deviceid,(fun_result1+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|3||4||%.2lf||1||%s||0||%d|*\r\n",sensor[fun_result1].min,deviceid,(fun_result1+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//			}
//
//			fun_stage = 0;
//
//			fun_result1++;
//			if (fun_result1 >= 6)	fun_result1 = 0;
//
//			Sensor_Assignment(kbVirtualEdit);
//
//			return;
//		}
//	}
//
//	if (key_code == kbBack)
//	{
//		GetFunFlag = 0;
//	}
//
//	if (sub_fun_state == 1)	// init
//	{
//		sub_fun_state = 2;
//
//		goto_screen_map_yz(fun_col_addr, fun_line_addr);
//
//		if (tool_type == 1)
//			GLCD_WriteString(disp_tempstr, true);
//		else if (tool_type >= 2)
//		{
//			// Check if first char of the editing mode
//			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
//			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
//			typingdelay_elapsed_flag = false;	typingdelay_count = 0;
//
//			GLCD_WriteString(edit_tempstr, true);
//		}
//
//		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
//	}
//}

/********************************************************/

/********************************************************/

void Backflush(uint8_t key_code)
{
uint8_t z_addr, fun_para_no;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data into eeprom
		eeprom_read_backflush(0xFF);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
		listbox_type = 0;	listbox_index = backflushstatus;		listbox_size = 2;    listbox_selected = 0;
		listbox = &status_list;
		strcpy(edit_tempstr, (*listbox)[listbox_index]);

		fun_col_addr = 66;		fun_line_addr = MIDDLE_PAGE_TOP;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// status
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[6] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			backflushstatus = listbox_index;

			// store data into eeprom
			eeprom_write_backflush(0x01);
			eeprom_read_backflush(0x01);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|11||1||%d||1||%s||0||0|*\r\n",backflushstatus,deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 2;	    tool_type = 2;		sub_fun_state = 2;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			textbox_char_encoded_trancated = 0;	textbox_size = 5;

//			usnprintf(edit_tempstr, 6, "%02u:%02u", backflushinterval[0], backflushinterval[1]);
		    // set time value of editing
		    set_edit_time(backflushinterval, false);
			usnprintf(edit_tempstr, 6, "%02u:%02u", rd_time.tm_hour, rd_time.tm_min);

			fun_col_addr = 66;		fun_line_addr += 10;
			y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+10;
		}
	}
	else if (fun_stage == 2)	// interval
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[6] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

//			if (IntervalStrToDec(edit_tempstr, backflushinterval, 1) == false)
			if (time_strtodec(edit_tempstr, 1, false) == false)
			{
				sub_fun_state = 2;

//				usnprintf(edit_tempstr, 6, "%02u:%02u", backflushinterval[0], backflushinterval[1]);
				usnprintf(edit_tempstr, 6, "%02u:%02u", rd_time.tm_hour, rd_time.tm_min);
				y_start = fun_col_addr;	z_start = fun_line_addr;		y_end = 127;	z_end = fun_line_addr+9;
			}
			else
			{
			    // get time value of editing
			    get_edit_time(backflushinterval, false);

			    // store data into eeprom
				eeprom_write_backflush(0x02);
				eeprom_read_backflush(0x02);
				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|11||2||%d:%d||1||%s||0||0|*\r\n",backflushinterval[0],backflushinterval[1],deviceid);
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}
				// set fun_stage = value for next stage
				// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
				fun_stage = 3;	    tool_type = 2;		sub_fun_state = 2;

				// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
				//$$NR$$//dated:19Sep15
				textbox_char_encoded_trancated = 4;			textbox_size = 3;
				editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
				//$$NR$$//dated:19Sep15

				usnprintf(edit_tempstr, 4, "%03u", backflushduration);
				strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

				fun_col_addr = 66;		fun_line_addr += 10;
				y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
			}
		}
	}
	else if (fun_stage == 3)	// Duration
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[6] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			backflushduration = (uint16_t)strtoint(edit_tempstr, 3);

			// store data into eeprom
			eeprom_write_backflush(0x04);
			eeprom_read_backflush(0x04);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|11||3||%d||1||%s||0||0|*\r\n",backflushduration,deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 4;	    tool_type = 2;		sub_fun_state = 2;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)


			//$$NR$$//dated:19Sep15
			textbox_char_encoded_trancated = 4;			textbox_size = 3;
			editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			//$$NR$$//dated:19Sep15

			usnprintf(edit_tempstr, 4, "%03u", backflushdelay);
			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			fun_col_addr = 66;		fun_line_addr += 10;
			y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 4)	//Delay
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[6] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			backflushdelay = (uint16_t)strtoint(edit_tempstr, 3);

			// store data into eeprom
			eeprom_write_backflush(0x08);
			eeprom_read_backflush(0x08);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|11||4||%d||1||%s||0||0|*\r\n",backflushdelay,deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 5;	    tool_type = 1;		sub_fun_state = 2;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 0;	listbox_index = deltapres;		listbox_size = 2;    listbox_selected = 0;
			listbox = &status_list;
			strcpy(edit_tempstr, (*listbox)[listbox_index]);

			fun_col_addr = 84;		fun_line_addr += 10;
			y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 5)	// delta pressure
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[6] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			deltapres = listbox_index;

			// store data into eeprom
			eeprom_write_backflush(0x10);
			eeprom_read_backflush(0x10);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|11||5||%d||1||%s||0||0|*\r\n",deltapres,deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 6;	    tool_type = 2;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			//$$NR$$//dated:19Sep15
			textbox_char_encoded_trancated = 4;			textbox_size = 3;
			editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			//$$NR$$//dated:19Sep15

			usnprintf(edit_tempstr, 4, "%03u", deltadelay);
			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			fun_col_addr = 90;		//fun_line_addr = MIDDLE_PAGE_TOP+40;
//			y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
	}
	else if (fun_stage == 6)	// delta delay
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[6] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			deltadelay = (uint16_t)strtoint(edit_tempstr, 3);

			// store data into eeprom
			eeprom_write_backflush(0x20);
			eeprom_read_backflush(0x20);
			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|11||6||%d||1||%s||0||0|*\r\n",deltadelay,deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}
			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 7;	    tool_type = 2;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			//$$NR$$//dated:19Sep15
			textbox_char_encoded_trancated = 4;			textbox_size = 2;
			editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			//$$NR$$//dated:19Sep15

			usnprintf(edit_tempstr, 3, "%02u", deltaitera);
			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			fun_col_addr = 102;		//fun_line_addr = MIDDLE_PAGE_TOP+40;
//			y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
	}
	else if (fun_stage == 7)	// delta iteration
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[6] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			deltaitera = (uint8_t)strtoint(edit_tempstr, 2);

			// store data into eeprom
			eeprom_write_backflush(0x40);
			eeprom_read_backflush(0x40);
			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|11||7||%d||1||%s||0||0|*\r\n",deltaitera,deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}
			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 8;	    tool_type = 2;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			//$$NR$$//dated:19Sep15
			textbox_char_encoded_trancated = 4;			textbox_size = 3;
			editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			//$$NR$$//dated:19Sep15

			usnprintf(edit_tempstr, 4, "%03u", dwelltime);
			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			fun_col_addr = 90;		//fun_line_addr = MIDDLE_PAGE_TOP+40;
//			y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
	}
	else if (fun_stage == 8)	// dwell time
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[6] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			dwelltime = (uint16_t)strtoint(edit_tempstr, 3);

			// store data into eeprom
			eeprom_write_backflush(0x80);
			eeprom_read_backflush(0x80);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|11||8||%d||1||%s||0||0|*\r\n",dwelltime,deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}
			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 0;	listbox_index = backflushstatus;		listbox_size = 2;    listbox_selected = 0;
			listbox = &status_list;
			strcpy(edit_tempstr, (*listbox)[listbox_index]);

			fun_col_addr = 66;		fun_line_addr = MIDDLE_PAGE_TOP;
			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
	}


	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if ((sub_fun_state == 1) || (sub_fun_state == 2))	// init screen / line
	{
		if (sub_fun_state == 1)	// init screen
		{
			if (fun_stage <= 5)
				fun_para_no = 0;
			else
				fun_para_no = fun_stage - 5;

			GLCD_clear_screen(false);						// clear the lcd display

			// Display Screen - Title
			usnprintf(disp_tempstr, 21, "BACK FLUSH");
			GLCD_write_titlebar(disp_tempstr);

//			fun_line_addr = SCRN_MAP_CUR_Z;

			for (z_addr=0; z_addr<5; z_addr++)
			{
				goto_screen_map_yz(5, SCRN_MAP_CUR_Z);

				if (fun_para_no == 0)
					usnprintf(disp_tempstr, 21, "STATUS    %s", status_list[deltapres]);
				else if (fun_para_no == 1)
					usnprintf(disp_tempstr, 21, "INTERVAL  %02u:%02u", backflushinterval[0], backflushinterval[1]);
				else if (fun_para_no == 2)
					usnprintf(disp_tempstr, 21, "DURATION  %03u mins", backflushduration);
				else if (fun_para_no == 3)
					usnprintf(disp_tempstr, 21, "DELAY     %03u mins", backflushdelay);
				else if (fun_para_no == 4)
					usnprintf(disp_tempstr, 21, "DELTA PRESUR %s", status_list[deltapres]);
				else if (fun_para_no == 5)
					usnprintf(disp_tempstr, 21, "DELTA DELAY   %03u", deltadelay);
				else if (fun_para_no == 6)
					usnprintf(disp_tempstr, 21, "DELTA ITERATION %02u", deltaitera);
				else if (fun_para_no == 7)
					usnprintf(disp_tempstr, 21, "DWELL TIME    %03u", dwelltime);

				GLCD_WriteString(disp_tempstr, false);

				fun_para_no++;
			}

			// display down arrow key
			goto_screen_map_yz(0, (MIDDLE_PAGE_TOP + 4*GLCD_LINE_HEIGHT + 1));
			GLCD_writesymbol((uint8_t)(96+32), false);
		}

		sub_fun_state = 3;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(edit_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void FirmwareInfo(uint8_t key_code)
{
//uint8_t z_addr;

	if (fun_stage == 0)
	{
		// read data into eeprom
		eeprom_read_firmware(0xFF);

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		goto_screen_map_yz(3, 1);
		GLCD_WriteString("      FIRMWARE      ", true);
//		z_addr = SCRN_MAP_CUR_Z;

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
//		strcpy(disp_tempstr, "MODEL     ");		strcat(disp_tempstr, model);
		strcpy(disp_tempstr, "MODEL   ");		strcat(disp_tempstr, model);
		GLCD_WriteString(disp_tempstr, false);

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z - 1);
		strcpy(disp_tempstr, "DEVICE ID ");		strcat(disp_tempstr, deviceid);
		GLCD_WriteString(disp_tempstr, false);

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z - 1);
//		snprintf(disp_tempstr, 21, "FIRMWARE VERSION %04.1f", firmwareversion);
	    strcpy(disp_tempstr, "FIRMWARE VER  ");	ftoa(disp_tempstr+17, maxdp);
		GLCD_WriteString(disp_tempstr, false);

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z - 1);
		strcpy(disp_tempstr, "E-NET MAC ");		strcat(disp_tempstr, enetmacid);
		GLCD_WriteString(disp_tempstr, false);

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z - 1);
		strcpy(disp_tempstr, "WIFI MAC  ");		strcat(disp_tempstr, wifimacid);
		GLCD_WriteString(disp_tempstr, false);

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z - 1);
		strcpy(disp_tempstr, "IMEI      ");		strcat(disp_tempstr, imeino);
		GLCD_WriteString(disp_tempstr, false);

		fun_stage = 1;

		GLCD_Screen_Update();
	}
//	else if (fun_stage == 1)
//	{
//		if (key_code == kbEnter)
//		{
//			GetFunFlag = 0;
//		}
//	}

	if (key_code == kbBack)
	{
		GLCD_clear_page(0, 15, false);	// clear the lcd display
		GetFunFlag = 0;
	}
}

/********************************************************/

/********************************************************/

void FactoryReset(uint8_t key_code)
{
uint8_t z_addr,k;

	if (fun_stage == 0)
	{
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		GLCD_write_titlebar("FACTORY RESET");
		z_addr = SCRN_MAP_CUR_Z + 10;

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
	    GLCD_WriteString("   Do You Want to   ", false);
	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    GLCD_WriteString("  Reset The System? ", false);

	    fun_line_addr = z_addr;
		fun_stage = 1;

		GLCD_Screen_Update();
	}
	else if (fun_stage == 1)
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			for(k=0;k<=9;k++)
			{
				table_changed[k] = 1;
			}
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

		    goto_screen_map_yz(3, fun_line_addr);
		    GLCD_WriteString("  System Restoring  ", false);
		    goto_screen_map_yz(3, fun_line_addr+10);
		    GLCD_WriteString("to Factory Default..", false);

		    GLCD_MsgLine_Update(0, fun_line_addr, 126, fun_line_addr + 20);

		    // get default values and store data into eeprom
		    eeprom_factory_reset();

		    if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|10||14||1||1||%s||0||0|*\r\n",deviceid);
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

		    //delay_sec(2);
		    Task_sleep(2000);

			GetFunFlag = 0;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}
}

/********************************************************/

/********************************************************/

//$$NR$$//dated:07Nov15

//extern uint8_t portal_ip[4];
//char portal_name[21] = "portal.avkon.in";

/********************************************************/
// This function converts ip from string to decimal format.
/********************************************************/

void IPAddrStrToDec(char *temp_str)
{
uint8_t temp_data[4];

	temp_data[0]	 = (*temp_str - '0')*100;	temp_str++;		//byte 1
	temp_data[0]	+= (*temp_str - '0')*10;	temp_str++;
	temp_data[0]	+= (*temp_str - '0');		temp_str+=2;
	temp_data[1]	 = (*temp_str - '0')*100;	temp_str++;		//byte 2
	temp_data[1]	+= (*temp_str - '0')*10;	temp_str++;
	temp_data[1]	+= (*temp_str - '0');		temp_str+=2;
	temp_data[2]	 = (*temp_str - '0')*100;	temp_str++;		//byte 3
	temp_data[2]	+= (*temp_str - '0')*10;	temp_str++;
	temp_data[2]	+= (*temp_str - '0');		temp_str+=2;
	temp_data[3]	 = (*temp_str - '0')*100;	temp_str++;		//byte 4
	temp_data[3]	+= (*temp_str - '0')*10;	temp_str++;
	temp_data[3]	+= (*temp_str - '0');

	portal_ip[0] = temp_data[3];//temp_data[0];
	portal_ip[1] = temp_data[2];//temp_data[1];
	portal_ip[2] = temp_data[1];//temp_data[2];
	portal_ip[3] = temp_data[0];//temp_data[3];
}

/********************************************************/
//
/********************************************************/

void Network_Wireless_Mode(uint8_t key_code)
{
uint8_t z_addr, fun_para_no;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if ((fun_stage == 0) || (fun_stage == 3))
	{
		if (fun_stage == 3)
		{
			strcpy(portal_name, edit_tempstr);

			// store data into eeprom
//			eeprom_write_network_gsm(0x04);
//			eeprom_read_network_gsm(0x04);
		}
		//for sync logic
		table_changed[7] = 1;
		eeprom_write_progstatus(0x40);
		eeprom_read_progstatus(0x40);

		eeprom_write_network_wifi(0x40);
		eeprom_read_network_wifi(0x40);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
		listbox_type = 0;	listbox_index = wireless_mode;		listbox_size = 3;    listbox_selected = 0;
		listbox = &wireless_mode_list;
		strcpy(edit_tempstr, (*listbox)[listbox_index]);

		fun_col_addr = 46;		fun_line_addr = MIDDLE_PAGE_TOP;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// mode
	{
		if (key_code == kbEnter)
		{
			wireless_mode = listbox_index;

			//for sync logic
			table_changed[7] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			// write data into eeprom
			eeprom_write_network_gsm(0x01);
			eeprom_read_network_gsm(0x01);

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 2;	    tool_type = 2;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			textbox_char_encoded_trancated = 0;	textbox_size = 15;

			usnprintf(edit_tempstr, 16, "%03d.%03d.%03d.%03d", portal_ip[3], portal_ip[2], portal_ip[1], portal_ip[0]);

			fun_col_addr = 35;		fun_line_addr += 10;
//			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 2)	// portal ip
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[7] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			IPAddrStrToDec(edit_tempstr);

			// store data into eeprom
			eeprom_write_network_wifi(0x20);
			eeprom_read_network_wifi(0x20);

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 3;	    tool_type = 3;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			textbox_char_encoded_trancated = 1;	textbox_size = 20;

			usnprintf(edit_tempstr, 21, "%020s", portal_name);

			fun_col_addr = 6;		fun_line_addr += 20;
			y_start = fun_col_addr;		z_start = fun_line_addr-20;		y_end = 126;	z_end = fun_line_addr+10;
//			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
	}


	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if ((sub_fun_state == 1) || (sub_fun_state == 2))	// init screen / line
	{
		if (sub_fun_state == 1)	// init screen
		{
			fun_para_no = 0;

			GLCD_clear_screen(false);						// clear the lcd display

			// Display Screen - Title
			usnprintf(disp_tempstr, 21, "WIRELESS");
			GLCD_write_titlebar(disp_tempstr);

//			fun_line_addr = SCRN_MAP_CUR_Z;

			for (z_addr=0; z_addr<4; z_addr++)
			{
				goto_screen_map_yz(5, SCRN_MAP_CUR_Z);

				if (fun_para_no == 0)
					usnprintf(disp_tempstr, 21, "MODE   %s", wireless_mode_list[wireless_mode]);
				else if (fun_para_no == 1)
//					usnprintf(disp_tempstr, 21, "IP   %015s", );
					usnprintf(disp_tempstr, 21, "IP   %03d.%03d.%03d.%03d", portal_ip[3], portal_ip[2], portal_ip[1], portal_ip[0]);
				else if (fun_para_no == 2)
					usnprintf(disp_tempstr, 21, "PORTAL NAME");
				else if (fun_para_no == 3)
					usnprintf(disp_tempstr, 21, "%020s", portal_name);

				GLCD_WriteString(disp_tempstr, false);

				fun_para_no++;
			}
		}

		sub_fun_state = 3;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(edit_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}
//$$NR$$//dated:07Nov15


/********************************************************/

/********************************************************/

//$$NR$$//dated:24Aug15

void Network_GSM_Settings(uint8_t key_code)
{
uint8_t z_addr, fun_para_no;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data into eeprom
		eeprom_read_network_gsm(0x1E);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
		listbox_type = 0;	listbox_index = gsm_module;		listbox_size = 2;    listbox_selected = 0;
		listbox = &wireless_module_state_list;
		strcpy(edit_tempstr, (*listbox)[listbox_index]);

		fun_col_addr = 54;		fun_line_addr = MIDDLE_PAGE_TOP;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// status
	{
		if (key_code == kbEnter)
		{
			gsm_module = listbox_index;

			// store data into eeprom
			eeprom_write_network_gsm(0x02);
			eeprom_read_network_gsm(0x02);

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 2;	    tool_type = 3;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			textbox_char_encoded_trancated = 1;	textbox_size = 20;

			usnprintf(edit_tempstr, 21, "%020s", gsm_apn);		//$$NR$$//dated:23Aug15

			fun_col_addr = 6;		fun_line_addr += 10;
//			y_start = fun_col_addr;		z_start = fun_line_addr-20;		y_end = 126;	z_end = fun_line_addr+10;
			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
	}
	else if (fun_stage == 2)	// gsm apn
	{
		if (key_code == kbEnter)
		{
			strcpy(gsm_apn, edit_tempstr);

			// store data into eeprom
			eeprom_write_network_gsm(0x04);
			eeprom_read_network_gsm(0x04);

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 0;	listbox_index = gsm_module;		listbox_size = 2;    listbox_selected = 0;
			listbox = &wireless_module_state_list;
			strcpy(edit_tempstr, (*listbox)[listbox_index]);

			fun_col_addr = 54;		fun_line_addr = MIDDLE_PAGE_TOP;
			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
	}


	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if ((sub_fun_state == 1) || (sub_fun_state == 2))	// init screen / line
	{
		if (sub_fun_state == 1)	// init screen
		{
			if (fun_stage <= 1)
				fun_para_no = 0;
			else
				fun_para_no = fun_stage - 1;

			GLCD_clear_screen(false);						// clear the lcd display

			// Display Screen - Title
			usnprintf(disp_tempstr, 21, "GSM SETTINGS");
			GLCD_write_titlebar(disp_tempstr);

//			fun_line_addr = SCRN_MAP_CUR_Z;

			for (z_addr=0; z_addr<5; z_addr++)
			{
				goto_screen_map_yz(5, SCRN_MAP_CUR_Z);

				if (fun_para_no == 0)
					usnprintf(disp_tempstr, 21, "STATUS   %s", wireless_module_state_list[gsm_module]);
				else if (fun_para_no == 1)
//					usnprintf(disp_tempstr, 21, "ACCESS POINT NAME ");
					usnprintf(disp_tempstr, 21, "APN ");
				else if (fun_para_no == 2)
					usnprintf(disp_tempstr, 21, "%020s", gsm_apn);			//$$NR$$//dated:23Aug15
				else if (fun_para_no == 3)
					usnprintf(disp_tempstr, 21, "OPERATOR ");
				else if (fun_para_no == 4)
					usnprintf(disp_tempstr, 21, "%020s", gsm_operator);	//$$NR$$//dated:23Aug15
				else if (fun_para_no == 5)
					usnprintf(disp_tempstr, 21, "SIGNAL   %01u", sig_strength);

				GLCD_WriteString(disp_tempstr, false);

				fun_para_no++;
			}

			// display down arrow key
			goto_screen_map_yz(0, (MIDDLE_PAGE_TOP + 4*GLCD_LINE_HEIGHT + 1));
			GLCD_writesymbol((uint8_t)(96+32), false);
		}

		sub_fun_state = 3;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(edit_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void Network_Wifi_Settings(uint8_t key_code)
{
uint8_t z_addr, fun_para_no;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data into eeprom
		eeprom_read_network_wifi(0x1F);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
		listbox_type = 0;	listbox_index = wifi_module;		listbox_size = 2;    listbox_selected = 0;
		listbox = &wireless_module_state_list;
		strcpy(edit_tempstr, (*listbox)[listbox_index]);

		fun_col_addr = 53;		fun_line_addr = MIDDLE_PAGE_TOP;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// status
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[7] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			wifi_module = listbox_index;

			// store data into eeprom
			eeprom_write_network_wifi(0x01);
			eeprom_read_network_wifi(0x01);

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 2;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 0;	listbox_index = wifi_mode;		listbox_size = 2;    listbox_selected = 0;
			listbox = &wifi_mode_list;
			strcpy(edit_tempstr, (*listbox)[listbox_index]);

			fun_col_addr = 53;		fun_line_addr = MIDDLE_PAGE_TOP;
			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
	}
	else if (fun_stage == 2)	// wifi mode
	{
		if ((key_code == kbUp) || (key_code == kbDown))
		{
			wifi_mode = listbox_index;
			strcpy(edit_tempstr, (*listbox)[listbox_index]);

			sub_fun_state = 1;

			fun_col_addr = 53;		fun_line_addr = MIDDLE_PAGE_TOP;
			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
		else if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[7] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			wifi_mode = listbox_index;

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 3;	    tool_type = 3;		sub_fun_state = 2;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			textbox_char_encoded_trancated = 1;	textbox_size = 20;

			if (wifi_mode == 0)
				usnprintf(edit_tempstr, 21, "%020s", wifi_name);	//$$NR$$//dated:23Aug15
			else
				usnprintf(edit_tempstr, 21, "%020s", ext_apn);		//$$NR$$//dated:23Aug15

			fun_col_addr = 5;		fun_line_addr += 20;
//			y_start = fun_col_addr;		z_start = fun_line_addr-20;		y_end = 126;	z_end = fun_line_addr+10;
			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
	}
	else if (fun_stage == 3)	// wifi name / ext apn
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[7] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			// store data into eeprom
			if (wifi_mode == 0)
			{
				strcpy(wifi_name, edit_tempstr);

				eeprom_write_network_wifi(0x02);
				eeprom_read_network_wifi(0x02);
			}
			else
			{
				strcpy(ext_apn, edit_tempstr);

				eeprom_write_network_wifi(0x08);
				eeprom_read_network_wifi(0x08);
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 4;	    tool_type = 3;		sub_fun_state = 2;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			textbox_char_encoded_trancated = 1;	textbox_size = 20;

			if (wifi_mode == 0)
				usnprintf(edit_tempstr, 21, "%020s", wifi_pass);	//$$NR$$//dated:23Aug15
			else
				usnprintf(edit_tempstr, 21, "%020s", ext_pass);	//$$NR$$//dated:23Aug15

			fun_col_addr = 5;		fun_line_addr += 20;
			y_start = fun_col_addr;		z_start = fun_line_addr-20;		y_end = 126;	z_end = fun_line_addr+10;
		}
	}
	else if (fun_stage == 4)	// signal strength
	{
		if (key_code == kbEnter)
		{
			// store data into eeprom
			if (wifi_mode == 0)
			{
				strcpy(wifi_pass, edit_tempstr);

				eeprom_write_network_wifi(0x04);
				eeprom_read_network_wifi(0x04);
			}
			else
			{
				strcpy(ext_pass, edit_tempstr);

				eeprom_write_network_wifi(0x10);
				eeprom_read_network_wifi(0x10);
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 0;	listbox_index = wifi_module;		listbox_size = 2;    listbox_selected = 0;
			listbox = &wireless_module_state_list;
			strcpy(edit_tempstr, (*listbox)[listbox_index]);

			fun_col_addr = 54;		fun_line_addr = MIDDLE_PAGE_TOP;
			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
	}


	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if ((sub_fun_state == 1) || (sub_fun_state == 2))	// init screen / line
	{
		if (sub_fun_state == 1)	// init screen
		{
			if (fun_stage <= 1)
				fun_para_no = 0;
			else
				fun_para_no = fun_stage - 1;

			GLCD_clear_screen(false);						// clear the lcd display

			// Display Screen - Title
			usnprintf(disp_tempstr, 21, "WIFI SETTINGS");
			GLCD_write_titlebar(disp_tempstr);

//			fun_line_addr = SCRN_MAP_CUR_Z;

			for (z_addr=0; z_addr<5; z_addr++)
			{
				goto_screen_map_yz(5, SCRN_MAP_CUR_Z);
				//$$NR$$//dated:23Aug15
				if (fun_para_no == 0)
					usnprintf(disp_tempstr, 21, "STATUS  %012s", wireless_module_state_list[wifi_module]);
				else if (fun_para_no == 1)
					usnprintf(disp_tempstr, 21, "MODE    %08s", wifi_mode_list[wifi_mode]);
				else if (fun_para_no == 2)
				{
//					usnprintf(disp_tempstr, 21, "ACCESS POINT NAME ");
					if (wifi_mode == 0)
						usnprintf(disp_tempstr, 21, "SSID                ");
					else
						usnprintf(disp_tempstr, 21, "APN                 ");
				}
				else if (fun_para_no == 3)
				{
					if (wifi_mode == 0)
						usnprintf(disp_tempstr, 21, "%020s", wifi_name);
					else
						usnprintf(disp_tempstr, 21, "%020s", ext_apn);
				}
				else if (fun_para_no == 4)
					usnprintf(disp_tempstr, 21, "PASSWORD            ");
				else if (fun_para_no == 5)
				{
					if (wifi_mode == 0)
						usnprintf(disp_tempstr, 21, "%020s", wifi_pass);
					else
						usnprintf(disp_tempstr, 21, "%020s", ext_pass);
				}

				GLCD_WriteString(disp_tempstr, false);

				fun_para_no++;
				//$$NR$$//dated:23Aug15
			}

			// display down arrow key
			goto_screen_map_yz(0, (MIDDLE_PAGE_TOP + 4*GLCD_LINE_HEIGHT + 1));
			GLCD_writesymbol((uint8_t)(96+32), false);
		}

		sub_fun_state = 3;
		if (fun_stage == 2)		sub_fun_state = 4;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(edit_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}
//$$NR$$//dated:24Aug15



/********************************************************/

/********************************************************/

void Authentication(uint8_t fun_no, uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		GLCD_write_titlebar("CHANGE PASSWORD");
		z_addr = SCRN_MAP_CUR_Z;

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
		strcpy(disp_tempstr, "USER ID  ");	//strcat(disp_tempstr, ippartno);
		GLCD_WriteString(disp_tempstr, false);

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
		strcpy(disp_tempstr, "CURRENT  ");
		GLCD_WriteString(disp_tempstr, false);

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
		strcpy(disp_tempstr, "NEW      ");
		GLCD_WriteString(disp_tempstr, false);

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
		strcpy(disp_tempstr, "CONFIRM  ");
		GLCD_WriteString(disp_tempstr, false);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
		listbox_type = 0;	listbox_index = fun_no;		listbox_size = 4;    listbox_selected = 0;
		fun_col_addr = 58;		fun_line_addr = z_addr;
		listbox = &loginuser_list;
		strcpy(disp_tempstr, (*listbox)[listbox_index]);

		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// user id
	{
		if (key_code == kbEnter)
		{
			fun_result1 = listbox_index;

			// read data into eeprom
			eeprom_read_authentication(fun_result1);

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 2;	    tool_type = 3;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			textbox_char_encoded_trancated = 3;	textbox_size = 10;
			strcpy((char *)edit_tempstr, "          ");

			fun_line_addr += 10;		//fun_col_addr = 58;
			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 2)	// current
	{
		if (key_code == kbEnter)
		{

			if (strcmp((char *)edit_tempstr, (const char *)loginpasswords[fun_result1]) == 0)
			{
//				strcpy(, edit_tempstr);

				// set fun_stage = value for next stage
				// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
				fun_stage = 3;	    tool_type = 3;		sub_fun_state = 1;

				// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
				textbox_char_encoded_trancated = 3;	textbox_size = 10;
				strcpy((char *)edit_tempstr, "          ");

				fun_line_addr += 10;		//fun_col_addr = 58;
				y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
			}
			else
			{
				goto_screen_map_yz(3, fun_line_addr - 10);
			    GLCD_WriteString("  Wrong Password   ", false);
			    goto_screen_map_yz(3, fun_line_addr);
			    GLCD_WriteString("Please, Re - Enter ", false);

			    GLCD_MsgLine_Update(0, fun_line_addr - 11, 126, fun_line_addr + 9);

			    //delay_sec(2);
			    Task_sleep(2000);

			    fun_stage = 0;	    tool_type = 0;

			    Authentication(fun_no, key_code);
				return;
			}
		}
	}
	else if (fun_stage == 3)	// New
	{
		if (key_code == kbEnter)
		{
			strcpy(edit_data_str, edit_tempstr);

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 4;	    tool_type = 3;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			textbox_char_encoded_trancated = 3;	textbox_size = 10;
			strcpy((char *)edit_tempstr, "          ");

			fun_line_addr += 10;		//fun_col_addr = 58;
			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 4)	// Confirm
	{
		if (key_code == kbEnter)
		{
			if (strcmp((char *)edit_tempstr, (const char *)edit_data_str) == 0)
			{
				strcpy((char *)loginpasswords[fun_result1], (const char *)edit_tempstr);

				// store data into eeprom
				eeprom_write_authentication(fun_result1);
				eeprom_read_authentication(fun_result1);

				fun_stage = 5;	    tool_type = 0;

				GetFunFlag = 0;

				GLCD_MsgLine_Update(fun_col_addr, fun_line_addr-10, 126, fun_line_addr + 10);
			}
			else
			{
				goto_screen_map_yz(3, fun_line_addr - 10);
			    GLCD_WriteString("  Wrong Password!  ", false);
			    goto_screen_map_yz(3, fun_line_addr);
			    GLCD_WriteString("Please, Re - Enter ", false);

			    GLCD_MsgLine_Update(0, fun_line_addr - 11, 126, fun_line_addr + 9);

			    //delay_sec(2);
			    Task_sleep(2000);

			    fun_stage = 0;	    tool_type = 0;

			    Authentication(fun_no, key_code);
				return;
			}
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if (sub_fun_state == 1)	// init
	{
		sub_fun_state = 2;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(disp_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void FertiProgMethod(uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data into eeprom
		//eeprom_read_fertiprog(prog_index);
		eeprom_read_fertiprog1(prog_index,0x04);
		eeprom_read_fertiprog1(prog_index,0x08);

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		usnprintf(disp_tempstr, 21, "FERTI PROG %01d- METHOD", (prog_index+1));
		GLCD_write_titlebar(disp_tempstr);
		z_addr = SCRN_MAP_CUR_Z+10;

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
		strcpy(disp_tempstr, "METHOD  ");
		GLCD_WriteString(disp_tempstr, false);

		if (fertiprog[prog_index].method == 1)
		{
			goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
			usnprintf(disp_tempstr, 21, "PROP CYCLES %02u", fertiprog[prog_index].propcycles);
			GLCD_WriteString(disp_tempstr, false);
		}

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
		listbox_type = 0;	listbox_index = fertiprog[prog_index].method;		listbox_size = 2;    listbox_selected = 0;
		listbox = &method_list;
		strcpy(disp_tempstr, (*listbox)[listbox_index]);

		fun_col_addr = 52;		fun_line_addr = z_addr;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// method
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[5] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			fertiprog[prog_index].method = listbox_index;

			// write data into eeprom
			//eeprom_write_fertiprog(prog_index);
			eeprom_write_fertiprog1(prog_index,0x04);
			eeprom_read_fertiprog1(prog_index,0x04);
			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|6||2||%d||1||%s||%d||0|*\r\n",fertiprog[prog_index].method,deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			goto_screen_map_yz(3, fun_line_addr+10);
			if (listbox_index == 0)			// Bulk
			{
				strcpy(disp_tempstr, "                  ");
				GLCD_WriteString(disp_tempstr, false);

				strcpy(disp_tempstr, (*listbox)[listbox_index]);

				sub_fun_state = 1;
				y_start = 0;	z_start = fun_line_addr;		y_end = 126;	z_end = fun_line_addr+19;
			}
			else if (listbox_index == 1)	// proportional
			{
				strcpy(disp_tempstr, "PROP CYCLES ");
				GLCD_WriteString(disp_tempstr, false);

				// set fun_stage = value for next stage
				// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
				fun_stage = 2;	    tool_type = 2;		sub_fun_state = 1;

				// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
				//$$NR$$//dated:19Sep15
				textbox_char_encoded_trancated = 4;			textbox_size = 2;
				editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
				//$$NR$$//dated:19Sep15

				usnprintf(edit_tempstr, 3, "%02u", fertiprog[prog_index].propcycles);
				strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

				fun_col_addr = 76;		fun_line_addr += 10;
				y_start = 0;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
			}
		}
	}
	else if (fun_stage == 2)	// proportional cycles
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[5] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			fertiprog[prog_index].propcycles = (uint8_t)strtoint(edit_tempstr, 2);

			// write data into eeprom
			//eeprom_write_fertiprog(prog_index);
			eeprom_write_fertiprog1(prog_index,0x04);
			eeprom_read_fertiprog1(prog_index,0x04);
			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|6||3||%d||1||%s||%d||0|*\r\n",fertiprog[prog_index].propcycles,deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 0;	listbox_index = fertiprog[prog_index].method;		listbox_size = 2;    listbox_selected = 0;
			listbox = &method_list;
			strcpy(disp_tempstr, (*listbox)[listbox_index]);

			fun_col_addr = 52;		fun_line_addr -= 10;
			y_start = 52;	z_start = fun_line_addr;		y_end = 126;	z_end = fun_line_addr+19;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if (sub_fun_state == 1)	// init
	{
		sub_fun_state = 2;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(disp_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void FertiProgDelays(uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data into eeprom
		//eeprom_read_fertiprog(prog_index);
		eeprom_read_fertiprog1(prog_index,0x10);
		eeprom_read_fertiprog1(prog_index,0x20);

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		usnprintf(disp_tempstr, 21, "FERTI PROG %d- DELAYS", (prog_index+1));
		GLCD_write_titlebar(disp_tempstr);

		z_addr = SCRN_MAP_CUR_Z+10;

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
	    strcpy(disp_tempstr, "START DELAY     Mins");
	    GLCD_WriteString(disp_tempstr, false);

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
		usnprintf(disp_tempstr, 21, "STOP DELAY  %03d Mins", fertiprog[prog_index].stopdelay);
	    GLCD_WriteString(disp_tempstr, false);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 2;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
		//$$NR$$//dated:19Sep15
		textbox_char_encoded_trancated = 4;			textbox_size = 3;
		editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
		//$$NR$$//dated:19Sep15

		usnprintf(edit_tempstr, 4, "%03d", fertiprog[prog_index].startdelay);
		strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

		fun_line_addr = z_addr;		fun_col_addr = 76;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// start delay
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[5] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			fertiprog[prog_index].startdelay = (uint16_t)strtoint(edit_tempstr, 3);

			// write data into eeprom
			//eeprom_write_fertiprog(prog_index);
			eeprom_write_fertiprog1(prog_index,0x10);
			eeprom_read_fertiprog1(prog_index,0x10);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|6||4||%d||1||%s||%d||0|*\r\n",fertiprog[prog_index].startdelay,deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 2;	    tool_type = 2;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			//$$NR$$//dated:19Sep15
			textbox_char_encoded_trancated = 4;			textbox_size = 3;
			editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			//$$NR$$//dated:19Sep15

			usnprintf(edit_tempstr, 4, "%03d", fertiprog[prog_index].stopdelay);
			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			fun_line_addr += 10;		//fun_col_addr = 76;
			y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 2)	//stop delay
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[5] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			fertiprog[prog_index].stopdelay = (uint16_t)strtoint(edit_tempstr, 3);

			// write data into eeprom
			//eeprom_write_fertiprog(prog_index);
			eeprom_write_fertiprog1(prog_index,0x20);
			eeprom_read_fertiprog1(prog_index,0x20);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|6||5||%d||1||%s||%d||0|*\r\n",fertiprog[prog_index].stopdelay,deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}
			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 1;	    tool_type = 2;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			//$$NR$$//dated:19Sep15
			textbox_char_encoded_trancated = 4;			textbox_size = 3;
			editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			//$$NR$$//dated:19Sep15

			usnprintf(edit_tempstr, 4, "%03d", fertiprog[prog_index].startdelay);
			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			fun_line_addr -= 10;		//fun_col_addr = 76;
			y_start = fun_col_addr;	z_start = fun_line_addr;		y_end = 126;	z_end = fun_line_addr+19;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if (sub_fun_state == 1)	// init
	{
		sub_fun_state = 2;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(disp_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/


void GetTotalTime_ferti(char prog_index)
{
	uint8_t temp,i;

	temp = fertiprog[prog_index].valve[0].stoptime;

	for(i=0;i<4;i++)
	{
		if(fertiprog[prog_index].valve[i].stoptime >= temp)
		{
			temp = fertiprog[prog_index].valve[i].stoptime;
		}
	}

	fertiprog[prog_index].totaltime = temp;
	eeprom_write_fertiprog2(prog_index,0x02);
	eeprom_read_fertiprog2(prog_index,0x02);
}






void FertiProgDetails(uint8_t key_code)
{
uint8_t z_addr, opindex = 0;
uint8_t y_start, z_start, y_end, z_end;
uint16_t valvecnt = 0;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data into eeprom
		//eeprom_read_fertiprog(prog_index);
		eeprom_read_fertiprog1(prog_index,0x02);

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		usnprintf(disp_tempstr, 21, "FERTI PROG%d- DETAILS", (prog_index+1));
		GLCD_write_titlebar(disp_tempstr);

		z_addr = SCRN_MAP_CUR_Z+10;

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
	    strcpy(disp_tempstr, "TYPE    ");
	    GLCD_WriteString(disp_tempstr, false);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 2;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
		listbox_type = 0;	listbox_index = fertiprog[prog_index].type;		listbox_size = 2;    listbox_selected = 0;
		listbox = &progtype_list;
		strcpy(edit_tempstr, (*listbox)[listbox_index]);

		fun_col_addr = 52;		fun_line_addr = z_addr;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// program type
	{
		if (key_code == kbEnter)
		{
			fertiprog[prog_index].type = listbox_index;

			// write data into eeprom
			//eeprom_write_fertiprog(prog_index);
			eeprom_write_fertiprog1(prog_index,0x02);
			eeprom_read_fertiprog1(prog_index,0x02);
			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|6||1||%d||1||%s||%d||0|*\r\n",fertiprog[prog_index].type,deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			fun_result1 = 0;
		}

		//get list of selected o/p types (o/p type != ferti valve)
		valvecnt = GetOutputSortedList(5, fun_result1);
		opindex = (uint8_t)((valvecnt & 0xFF00) >> 8);
		valvecnt = (uint8_t)(valvecnt & 0xFF);

		// set valve index & number of available valves
		/*fun_result1 = 0;*/	fun_result2 = valvecnt;

		fertiprog[prog_index].valvecount = valvecnt;

		// display error message if no zones are available
		if (valvecnt == 0)
		{
		    goto_screen_map_yz(3, fun_line_addr);
		    GLCD_WriteString("    No Valves Are   ", false);
		    goto_screen_map_yz(3, fun_line_addr+10);
		    GLCD_WriteString("     Configured     ", false);

			GLCD_Screen_Update();

		   //delay_sec(2);
		    Task_sleep(2000);

			GetFunFlag = 0;
			return;
		}

		fun_line_addr = TITLEBAR_TOP_SPACE + TITLEBAR_HEIGHT;

		// Display Screen
	    goto_screen_map_yz(3, fun_line_addr);
	    strcpy(disp_tempstr, "VALVE               ");
	    GLCD_WriteString(disp_tempstr, false);

	    if (fertiprog[prog_index].type == 0)	// time based
	    {
	    	// calculate total time
	    	GetTotalTime_ferti(prog_index);

	    	eeprom_read_fertiprog2(prog_index,0x02);
	    	eeprom_read_fertiprog3(prog_index,fun_result1,0x02);
	    	eeprom_read_fertiprog3(prog_index,fun_result1,0x04);

		    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
			usnprintf(disp_tempstr, 21, "TOTAL TIME      MIN ");
		    GLCD_WriteString(disp_tempstr, false);

		    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
			usnprintf(disp_tempstr, 21, "START TIME      MIN ");
		    GLCD_WriteString(disp_tempstr, false);

		    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
			usnprintf(disp_tempstr, 21, "STOP TIME       MIN ");
		    GLCD_WriteString(disp_tempstr, false);
	    }
	    else					// flow based
	    {
	    	eeprom_read_fertiprog2(prog_index,0x04);
			eeprom_read_fertiprog3(prog_index,fun_result1,0x08);
			eeprom_read_fertiprog3(prog_index,fun_result1,0x10);

		    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    	usnprintf(disp_tempstr, 21, "TOTAL FLOW      MIN ");
			GLCD_WriteString(disp_tempstr, false);

		    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    	usnprintf(disp_tempstr, 21, "START FLOW      MIN ");
			GLCD_WriteString(disp_tempstr, false);

		    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    	usnprintf(disp_tempstr, 21, "STOP FLOW       MIN ");
			GLCD_WriteString(disp_tempstr, false);
	    }

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 2;	    tool_type = 1;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t) / 3(valve/zone)
		listbox_type = 3;	listbox_index = opindex;/*fertiprog[prog_index].valve[fun_result1].oppartno;*/		listbox_size = ((noopmodules + 1) * 8);    //listbox_selected = 0;
		listbox = &virtual_list;	listbox_baseval = 0;

		usnprintf(edit_tempstr, 14, "%01u. %s", output[listbox_index].typeid, output[listbox_index].opname);
		SCRN_MAP_CUR_Z = fun_line_addr+10;

		fun_col_addr = 40;		//fun_line_addr = fun_line_addr;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 2)	// valve
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[5] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);
			//fertiprog[prog_index].valve[fun_result1].oppartno = listbox_index;

			// write data into eeprom
			//eeprom_write_fertiprog(prog_index);

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 3;	    tool_type = 2;		sub_fun_state = 1;

			if (fertiprog[prog_index].type == 0)		// time based
			{
				//$$NR$$//dated:19Sep15
				textbox_char_encoded_trancated = 4;			textbox_size = 3;
				editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			    //$$NR$$//dated:19Sep15

//				usnprintf(edit_tempstr, 4, "%03d", fertiprog[prog_index].totaltime);
				usnprintf(edit_tempstr, 4, "%03d", fertiprog[prog_index].valve[fun_result1].starttime);
			}
			else if (fertiprog[prog_index].type == 1)	// flow based
			{
				//$$NR$$//dated:19Sep15
				textbox_char_encoded_trancated = 4;			textbox_size = 5;
				editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			    //$$NR$$//dated:19Sep15

//				usnprintf(edit_tempstr, 6, "%05d", fertiprog[prog_index].totalflow);
				usnprintf(edit_tempstr, 6, "%05d", fertiprog[prog_index].valve[fun_result1].startflow);
			}
			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			SCRN_MAP_CUR_Z = fun_line_addr+10;

			fun_line_addr += 20;		fun_col_addr = 76;
//			y_start = fun_col_addr;		z_start = fun_line_addr-20;		y_end = 126;	z_end = fun_line_addr+10;
			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
	}
	else if (fun_stage == 3)	// start time/flow
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[5] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 4;	    tool_type = 2;		sub_fun_state = 1;

			if (fertiprog[prog_index].type == 0)		// time based
			{
				fertiprog[prog_index].valve[fun_result1].starttime = (uint16_t)strtoint(edit_tempstr, 3);
				eeprom_write_fertiprog3(prog_index,fun_result1,0x02);
				eeprom_read_fertiprog3(prog_index,fun_result1,0x02);
				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|7||2||%d||1||%s||%d||%d|*\r\n",fertiprog[prog_index].valve[fun_result1].starttime,deviceid,(prog_index+1),(fun_result1+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}

				//$$NR$$//dated:19Sep15
				textbox_char_encoded_trancated = 4;			textbox_size = 3;
				editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
				//$$NR$$//dated:19Sep15

				usnprintf(edit_tempstr, 4, "%03d", fertiprog[prog_index].valve[fun_result1].stoptime);
			}
			else if (fertiprog[prog_index].type == 1)	// flow based
			{
				fertiprog[prog_index].valve[fun_result1].startflow = (uint32_t)strtoint(edit_tempstr, 5);
				eeprom_write_fertiprog3(prog_index,fun_result1,0x08);
				eeprom_read_fertiprog3(prog_index,fun_result1,0x08);
				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|7||4||%d||1||%s||%d||%d|*\r\n",fertiprog[prog_index].valve[fun_result1].startflow,deviceid,(prog_index+1),(fun_result1+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}

				//$$NR$$//dated:19Sep15
				textbox_char_encoded_trancated = 4;			textbox_size = 5;
				editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
				//$$NR$$//dated:19Sep15

				usnprintf(edit_tempstr, 6, "%05d", fertiprog[prog_index].valve[fun_result1].stopflow);
			}

			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			SCRN_MAP_CUR_Z = fun_line_addr-10;
			// write data into eeprom
			//eeprom_write_fertiprog(prog_index);

			sub_fun_state = 1;

			fun_line_addr += 10;		fun_col_addr = 76;
			y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 4)	// stop time/flow
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[5] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			if (fertiprog[prog_index].type == 0)		// time based
			{
				fertiprog[prog_index].valve[fun_result1].stoptime = (uint16_t)strtoint(edit_tempstr, 3);
				eeprom_write_fertiprog3(prog_index,fun_result1,0x04);
				eeprom_read_fertiprog3(prog_index,fun_result1,0x04);
				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|7||3||%d||1||%s||%d||%d|*\r\n",fertiprog[prog_index].valve[fun_result1].stoptime,deviceid,(prog_index+1),(fun_result1+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}
			}
			else if (fertiprog[prog_index].type == 1)	// flow based
			{
				fertiprog[prog_index].valve[fun_result1].stopflow = (uint32_t)strtoint(edit_tempstr, 5);
				eeprom_write_fertiprog3(prog_index,fun_result1,0x10);
				eeprom_read_fertiprog3(prog_index,fun_result1,0x10);
				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|7||5||%d||1||%s||%d||%d|*\r\n",fertiprog[prog_index].valve[fun_result1].stopflow,deviceid,(prog_index+1),(fun_result1+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}
			}

			// write data into eeprom
			//eeprom_write_fertiprog(prog_index);

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 1;	    //tool_type = 1;		sub_fun_state = 1;

			fun_result1++;
			if (fun_result1 >= fun_result2)	fun_result1 = 0;
/*
			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 3;	listbox_index = fertiprog[prog_index].valve[fun_result1].oppartno;		listbox_size = ((noopmodules + 1) * 8);    listbox_selected = 0;
			listbox = &virtual_list;	listbox_baseval = 0;

			//get list of selected o/p types
			for (i=0; i<((noopmodules + 1) * 8); i++)
			{
				//check if o/p type is ferti valve
				if (output[i].optype != 5)	listbox_selected |= (0x01 << i);//output[i].optype);
			}

			usnprintf(edit_tempstr, 14, "%01u. %s", output[listbox_index].typeid, output[listbox_index].opname);
			SCRN_MAP_CUR_Z = fun_line_addr-20;

			fun_col_addr = 40;		fun_line_addr -= 30;
			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
*/
			FertiProgDetails(kbVirtualEdit);
			return;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if ((sub_fun_state == 1) || (sub_fun_state == 2))	// init screen / line
	{
		if (sub_fun_state == 1)	// init screen
		{
			if (fertiprog[prog_index].type == 0)		// time based
		    {
			    goto_screen_map_yz(76, SCRN_MAP_CUR_Z);
				usnprintf(disp_tempstr, 4, "%03d", fertiprog[prog_index].totaltime);
			    GLCD_WriteString(disp_tempstr, false);

			    goto_screen_map_yz(76, SCRN_MAP_CUR_Z);
				usnprintf(disp_tempstr, 4, "%03d", fertiprog[prog_index].valve[fun_result1].starttime);
			    GLCD_WriteString(disp_tempstr, false);

			    goto_screen_map_yz(76, SCRN_MAP_CUR_Z);
				usnprintf(disp_tempstr, 4, "%03d", fertiprog[prog_index].valve[fun_result1].stoptime);
			    GLCD_WriteString(disp_tempstr, false);
		    }
			else if (fertiprog[prog_index].type == 1)	// flow based
		    {
			    goto_screen_map_yz(76, SCRN_MAP_CUR_Z);
		    	usnprintf(disp_tempstr, 6, "%05d", fertiprog[prog_index].totalflow);
				GLCD_WriteString(disp_tempstr, false);

			    goto_screen_map_yz(76, SCRN_MAP_CUR_Z);
		    	usnprintf(disp_tempstr, 6, "%05d", fertiprog[prog_index].valve[fun_result1].startflow);
				GLCD_WriteString(disp_tempstr, false);

			    goto_screen_map_yz(76, SCRN_MAP_CUR_Z);
		    	usnprintf(disp_tempstr, 6, "%05d", fertiprog[prog_index].valve[fun_result1].stopflow);
				GLCD_WriteString(disp_tempstr, false);
		    }
		}
		sub_fun_state = 3;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(edit_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void FertiClearProgram(uint8_t key_code)
{
uint8_t z_addr,j;

	if (fun_stage == 0)
	{
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		usnprintf(disp_tempstr, 21, "PROG%d- CLEAR PROGRAM", (prog_index+1));
		GLCD_write_titlebar(disp_tempstr);
		z_addr = SCRN_MAP_CUR_Z + 10;

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
	    GLCD_WriteString("   Do You Want to   ", false);
	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    GLCD_WriteString("   Clear Program?   ", false);

	    fun_line_addr = z_addr;
		fun_stage = 1;

		GLCD_Screen_Update();
	}
	else if (fun_stage == 1)
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[5] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

		    goto_screen_map_yz(3, fun_line_addr);
		    GLCD_WriteString("   System Clearing  ", false);
		    goto_screen_map_yz(3, fun_line_addr+10);
		    GLCD_WriteString("      Program..     ", false);

		    GLCD_MsgLine_Update(0, fun_line_addr, 126, fun_line_addr + 20);

		    // get default value
		    get_default_value_fertiprog(prog_index);

			// write data into eeprom
			//eeprom_write_fertiprog(prog_index);
			eeprom_write_fertiprog1(prog_index,0XFF);
			eeprom_write_fertiprog2(prog_index,0XFF);
			eeprom_read_fertiprog1(prog_index,0XFF);
			eeprom_read_fertiprog2(prog_index,0XFF);
			for(j=0;j<4;j++)
			{
				eeprom_write_fertiprog3(prog_index,j,0XFF);
				eeprom_read_fertiprog3(prog_index,j,0XFF);
			}

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|6||9||1||1||%s||%d||0|*\r\n",deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
////				sprintf (msg_send,"*|6||id||%d||1||%s||%d||0|*\r\n",fertiprog[prog_index].id,deviceid,(prog_index+1));
////				strcat (temp1,msg_send);
////				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|6||2||%d||1||%s||%d||0|*\r\n",fertiprog[prog_index].method,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|6||3||%d||1||%s||%d||0|*\r\n",fertiprog[prog_index].propcycles,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|6||1||%d||1||%s||%d||0|*\r\n",fertiprog[prog_index].type,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|6||6||%d||1||%s||%d||0|*\r\n",fertiprog[prog_index].valvecount,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|6||4||%d||1||%s||%d||0|*\r\n",fertiprog[prog_index].startdelay,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|6||5||%d||1||%s||%d||0|*\r\n",fertiprog[prog_index].stopdelay,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|6||8||%d||1||%s||%d||0|*\r\n",fertiprog[prog_index].totalflow,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|6||7||%d||1||%s||%d||0|*\r\n",fertiprog[prog_index].totaltime,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//
//				for(j=0;j<4;j++)
//				{
//					sprintf (msg_send,"*|7||1||%d||1||%s||%d||%d|*\r\n",fertiprog[prog_index].valve[j].oppartno,deviceid,(prog_index+1),(j));
//					strcat (temp1,msg_send);
//					memset(msg_send,0, sizeof(msg_send));
//					sprintf (msg_send,"*|7||2||%d||1||%s||%d||%d|*\r\n",fertiprog[prog_index].valve[j].starttime,deviceid,(prog_index+1),(j));
//					strcat (temp1,msg_send);
//					memset(msg_send,0, sizeof(msg_send));
//					sprintf (msg_send,"*|7||3||%d||1||%s||%d||%d|*\r\n",fertiprog[prog_index].valve[j].stoptime,deviceid,(prog_index+1),(j));
//					strcat (temp1,msg_send);
//					memset(msg_send,0, sizeof(msg_send));
//					sprintf (msg_send,"*|7||4||%d||1||%s||%d||%d|*\r\n",fertiprog[prog_index].valve[j].startflow,deviceid,(prog_index+1),(j));
//					strcat (temp1,msg_send);
//					memset(msg_send,0, sizeof(msg_send));
//					sprintf (msg_send,"*|7||5||%d||1||%s||%d||%d|*\r\n",fertiprog[prog_index].valve[j].stopflow,deviceid,(prog_index+1),(j));
//					strcat (temp1,msg_send);
//					memset(msg_send,0, sizeof(msg_send));
//				}
			}

		    //delay_sec(2);
			Task_sleep(2000);
			GetFunFlag = 0;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}
}

/********************************************************/

/********************************************************/

void ZoneProgStatus(uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data from eeprom
		eeprom_read_zoneprog1(prog_index,0x04);
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		usnprintf(disp_tempstr, 21, "PROG %d- STATUS", (prog_index+1));
		GLCD_write_titlebar(disp_tempstr);
		z_addr = SCRN_MAP_CUR_Z+10;

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
		strcpy(disp_tempstr, "STATUS   ");
		GLCD_WriteString(disp_tempstr, false);

		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

		listbox_type = 0;	listbox_index = zoneprog[prog_index].status;		listbox_size = 2;    listbox_selected = 0;
		listbox = &status_list;
		strcpy(disp_tempstr, (*listbox)[listbox_index]);

		fun_col_addr = 58;		fun_line_addr = z_addr;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// status
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[3] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			zoneprog[prog_index].status = listbox_index;

			// write data into eeprom
			eeprom_write_zoneprog1(prog_index,0x04);
			eeprom_read_zoneprog1(prog_index,0x04);
			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|5||2||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].status,deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}
			sub_fun_state = 1;
			y_start = fun_col_addr;	z_start = fun_line_addr;		y_end = 126;	z_end = fun_line_addr+9;

			prog_t_updated=1;

		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if (sub_fun_state == 1)	// init
	{
		sub_fun_state = 2;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(disp_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void ZoneProgRunTime(uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data into eeprom
		eeprom_read_zoneprog1(prog_index,0x08);
		eeprom_read_zoneprog1(prog_index,0x10);
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		usnprintf(disp_tempstr, 21, "PROG %d- RUN TIME", (prog_index+1));
		GLCD_write_titlebar(disp_tempstr);

		z_addr = SCRN_MAP_CUR_Z+10;

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
	    strcpy(disp_tempstr, "REPEAT CYCLES  ");
	    GLCD_WriteString(disp_tempstr, false);

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
		usnprintf(disp_tempstr, 21, "CYCLE DELAY %03d Mins", zoneprog[prog_index].cycledelay);
	    GLCD_WriteString(disp_tempstr, false);

		fun_stage = 1;	    tool_type = 2;		sub_fun_state = 1;

		//$$NR$$//dated:19Sep15
		textbox_char_encoded_trancated = 4;			textbox_size = 2;
		editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
		//$$NR$$//dated:19Sep15

		usnprintf(edit_tempstr, 3, "%02d", zoneprog[prog_index].repeatcycles);
		strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

		fun_line_addr = z_addr;		fun_col_addr = 94;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// Repeat Cycles
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[3] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			zoneprog[prog_index].repeatcycles = (uint8_t)strtoint(edit_tempstr, 2);

			// write data into eeprom
			eeprom_write_zoneprog1(prog_index,0x08);
			eeprom_read_zoneprog1(prog_index,0x08);
			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|5||3||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].repeatcycles,deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}
			prog_t_updated=1;

			fun_stage = 2;	    tool_type = 2;		sub_fun_state = 1;

			//$$NR$$//dated:19Sep15
			textbox_char_encoded_trancated = 4;			textbox_size = 3;
			editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			//$$NR$$//dated:19Sep15

			usnprintf(edit_tempstr, 4, "%03d", zoneprog[prog_index].cycledelay);
			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			fun_line_addr += 10;		fun_col_addr = 76;
			y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;


		}
	}
	else if (fun_stage == 2)	//Cycle Delay
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[3] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			zoneprog[prog_index].cycledelay = (uint16_t)strtoint(edit_tempstr, 3);

			// write data into eeprom
			eeprom_write_zoneprog1(prog_index,0x10);
			eeprom_read_zoneprog1(prog_index,0x10);
			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|5||4||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].cycledelay,deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			prog_t_updated=1;

			fun_stage = 1;	    tool_type = 2;		sub_fun_state = 1;

			//$$NR$$//dated:19Sep15
			textbox_char_encoded_trancated = 4;			textbox_size = 2;
			editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			//$$NR$$//dated:19Sep15

			usnprintf(edit_tempstr, 3, "%02d", zoneprog[prog_index].repeatcycles);
			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			fun_line_addr -= 10;		fun_col_addr = 94;
			y_start = 76;	z_start = fun_line_addr;		y_end = 126;	z_end = fun_line_addr+19;

		    goto_screen_map_yz(fun_col_addr, fun_line_addr);

		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if (sub_fun_state == 1)	// init
	{
		sub_fun_state = 2;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(disp_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void ZoneProgPrecondition(uint8_t fun_no, uint8_t key_code)
{
uint8_t z_addr, i;
uint8_t y_start, z_start, y_end, z_end;
char tempstr1[5], tempstr2[5];

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data into eeprom
		eeprom_read_zoneprog3(prog_index,0xFF);

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		usnprintf(disp_tempstr, 21, "PROG %d- PRECONDITION", (prog_index+1));
		GLCD_write_titlebar(disp_tempstr);
		z_addr = SCRN_MAP_CUR_Z+10;

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
		strcpy(disp_tempstr, "SENSOR   ");
		GLCD_WriteString(disp_tempstr, false);

		if (fun_no == 0)
		{
			i = zoneprog[prog_index].rain.condition;
		}
		else if (fun_no == 1)
		{
			i = zoneprog[prog_index].temperature.condition;
			ftoa(tempstr1, zoneprog[prog_index].temperature.min);
			ftoa(tempstr2, zoneprog[prog_index].temperature.max);
		}
		else if (fun_no == 2)
		{
			i = zoneprog[prog_index].humidity.condition;
			ftoa(tempstr1, zoneprog[prog_index].humidity.min);
			ftoa(tempstr2, zoneprog[prog_index].humidity.max);
		}
		else if (fun_no == 3)
		{
			i = zoneprog[prog_index].flow.condition;
		}
		else if (fun_no == 4)
		{
			i = zoneprog[prog_index].oppressure.condition;
			ftoa(tempstr1, zoneprog[prog_index].oppressure.min);
			ftoa(tempstr2, zoneprog[prog_index].oppressure.max);
		}
		else if (fun_no == 5)
		{
			i = zoneprog[prog_index].ippressure.condition;
			ftoa(tempstr1, zoneprog[prog_index].ippressure.min);
			ftoa(tempstr2, zoneprog[prog_index].ippressure.max);
		}

		if ((fun_no == 1) || (fun_no == 2) || (fun_no == 4)|| (fun_no == 5))
		{
			goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
			usnprintf(disp_tempstr, 21, "MINIMUM  %s", tempstr1);
			GLCD_WriteString(disp_tempstr, false);

			goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
			usnprintf(disp_tempstr, 21, "MAXIMUM  %s", tempstr2);
			GLCD_WriteString(disp_tempstr, false);
		}

		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

		listbox_type = 0;	listbox_index = i;		listbox_size = 2;    listbox_selected = 0;
		listbox = &precond_list;
		strcpy(disp_tempstr, (*listbox)[listbox_index]);

		fun_col_addr = 58;		fun_line_addr = z_addr;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	//sensor status
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[3] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			if (fun_no == 0)
			{
				zoneprog[prog_index].rain.condition = listbox_index;
				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|5||5||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].rain.condition,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
					sprintf (msg_send,"*|5||6||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].rain.currentval,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}
			}
			else if (fun_no == 1)
			{
				zoneprog[prog_index].temperature.condition = listbox_index;
				ftoa(tempstr1, zoneprog[prog_index].temperature.min);
				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|5||9||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].temperature.condition,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
					sprintf (msg_send,"*|5||10||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].temperature.currentval,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}
			}
			else if (fun_no == 2)
			{
				zoneprog[prog_index].humidity.condition = listbox_index;
				ftoa(tempstr1, zoneprog[prog_index].humidity.min);

				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|5||13||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].humidity.condition,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
					sprintf (msg_send,"*|5||14||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].humidity.currentval,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}
			}
			else if (fun_no == 3)
			{
				zoneprog[prog_index].flow.condition = listbox_index;
				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|5||7||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].flow.condition,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
					sprintf (msg_send,"*|5||8||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].flow.currentval,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}
			}
			else if (fun_no == 4)
			{
				zoneprog[prog_index].oppressure.condition = listbox_index;
				ftoa(tempstr1, zoneprog[prog_index].oppressure.min);

				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|5||21||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].oppressure.condition,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
					sprintf (msg_send,"*|5||22||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].oppressure.currentval,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}
			}
			else if (fun_no == 5)
			{
				zoneprog[prog_index].ippressure.condition = listbox_index;
				ftoa(tempstr1, zoneprog[prog_index].ippressure.min);

				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|5||17||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].ippressure.condition,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
					sprintf (msg_send,"*|5||18||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].ippressure.currentval,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
				}
			}

			// write data into eeprom
			eeprom_write_zoneprog3(prog_index,0xFF);
			eeprom_read_zoneprog3(prog_index,0xFF);

			if ((fun_no == 0) || (fun_no == 3))
			{
				sub_fun_state = 1;
				strcpy(disp_tempstr, (*listbox)[listbox_index]);

				y_start = fun_col_addr;	z_start = fun_line_addr;		y_end = 126;	z_end = fun_line_addr+9;
			}
			else if ((fun_no == 1) || (fun_no == 2) || (fun_no == 4))
			{
				fun_stage = 2;	    tool_type = 2;		sub_fun_state = 1;

				//$$NR$$//dated:19Sep15
				textbox_char_encoded_trancated = 4;			textbox_size = 4;
				editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
				//$$NR$$//dated:19Sep15

				usnprintf(edit_tempstr, 5, "%s", tempstr1);
				strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

				fun_line_addr += 10;
				y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
			}
		}
	}
	else if (fun_stage == 2)	//min
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[3] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			if (fun_no == 1)
			{
				zoneprog[prog_index].temperature.min = strtofloat(edit_tempstr);

				ftoa(tempstr2, zoneprog[prog_index].temperature.max);

				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|5||11||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].temperature.min,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}
			}
			else if (fun_no == 2)
			{
				zoneprog[prog_index].humidity.min = strtofloat(edit_tempstr);

				ftoa(tempstr2, zoneprog[prog_index].humidity.max);

				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|5||15||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].humidity.min,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}
			}
			else if (fun_no == 4)
			{
				zoneprog[prog_index].oppressure.min = strtofloat(edit_tempstr);

				ftoa(tempstr2, zoneprog[prog_index].oppressure.max);
				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|5||23||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].oppressure.min,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}
			}
			else if (fun_no == 5)
			{
				zoneprog[prog_index].ippressure.min = strtofloat(edit_tempstr);

				ftoa(tempstr2, zoneprog[prog_index].ippressure.max);
				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|5||19||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].ippressure.min,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}
			}

			// write data into eeprom
			eeprom_write_zoneprog3(prog_index,0xFF);
			eeprom_read_zoneprog3(prog_index,0xFF);

			fun_stage = 3;	    tool_type = 2;		sub_fun_state = 1;

			//$$NR$$//dated:19Sep15
			textbox_char_encoded_trancated = 4;			textbox_size = 4;
			editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			//$$NR$$//dated:19Sep15

			usnprintf(edit_tempstr, 5, "%s", tempstr2);
			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			fun_line_addr += 10;
			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;

		}
	}
	else if (fun_stage == 3)	//max
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[3] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			if (fun_no == 1)
			{
				zoneprog[prog_index].temperature.max = strtofloat(edit_tempstr);

				i = zoneprog[prog_index].temperature.condition;

				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|5||12||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].temperature.max,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}
			}
			else if (fun_no == 2)
			{
				zoneprog[prog_index].humidity.max = strtofloat(edit_tempstr);

				i = zoneprog[prog_index].humidity.condition;

				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|5||16||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].humidity.max,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}
			}
			else if (fun_no == 4)
			{
				zoneprog[prog_index].oppressure.max = strtofloat(edit_tempstr);

				i = zoneprog[prog_index].oppressure.condition;
				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|5||24||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].oppressure.max,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}
			}
			else if (fun_no == 5)
			{
				zoneprog[prog_index].ippressure.max = strtofloat(edit_tempstr);

				i = zoneprog[prog_index].ippressure.condition;

				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					sprintf (msg_send,"*|5||20||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].ippressure.max,deviceid,(prog_index+1));
					strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}
			}

			// write data into eeprom
			eeprom_write_zoneprog3(prog_index,0xFF);
			eeprom_read_zoneprog3(prog_index,0xFF);
			fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

			listbox_type = 0;	listbox_index = i;		listbox_size = 2;    listbox_selected = 0;
			listbox = &precond_list;
			strcpy(disp_tempstr, (*listbox)[listbox_index]);

			fun_line_addr -= 20;	fun_col_addr = 58;
			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if (sub_fun_state == 1)	// init
	{
		sub_fun_state = 2;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(disp_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}
		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void ZoneProgRunDays(uint8_t key_code)
{
uint8_t z_addr, temp_val=0,shift=0;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data into eeprom
		eeprom_read_zoneprog1(prog_index,0x20);
		eeprom_read_zoneprog1(prog_index,0x40);

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		usnprintf(disp_tempstr, 21, "PROG %d- RUN DAYS", (prog_index+1));
		GLCD_write_titlebar(disp_tempstr);

		z_addr = SCRN_MAP_CUR_Z;

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    strcpy(disp_tempstr, "MODE     ");	    GLCD_WriteString(disp_tempstr, false);

		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

	    listbox_type = 0;	listbox_index = zoneprog[prog_index].rundaysmode;		listbox_size = 3;    listbox_selected = 0;
		listbox = &rundays_list;
		strcpy(disp_tempstr, (*listbox)[listbox_index]);

		fun_col_addr = 58;		fun_line_addr = z_addr;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// Source
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[3] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			zoneprog[prog_index].rundaysmode = listbox_index;

			// write data into eeprom
			eeprom_write_zoneprog1(prog_index,0x20);
			eeprom_read_zoneprog1(prog_index,0x20);
			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|5||25||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].rundaysmode,deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			prog_t_updated=1;

			temp_val = zoneprog[prog_index].rundaysval[listbox_index];

			if (listbox_index == 0)
			{
			    goto_screen_map_yz(3, fun_line_addr+20);
				GLCD_WriteString("S  M  T  W  T  F  S ", false);

				strcpy(disp_tempstr, "                    ");
			    for(z_addr=0; z_addr<7; z_addr++)
			    {
			    	if (temp_val & (0x01<<(z_addr)))	disp_tempstr[z_addr*3] = '!';
			    }

				goto_screen_map_yz(3, fun_line_addr+30);
				GLCD_WriteString(disp_tempstr, false);

				fun_stage = 2;	    tool_type = 1;		sub_fun_state = 1;

				listbox_type = 0;	listbox_index = (temp_val  & 0x01);	listbox_size = 2;    listbox_selected = 0;
				listbox = &rundays_daywise_list;
				strcpy(disp_tempstr, (*listbox)[listbox_index]);

				fun_line_addr += 30;		fun_col_addr = 4;
			}
			else if (listbox_index == 1)
			{
			    strcpy(disp_tempstr, "EVERY         DAYS");

			    goto_screen_map_yz(3, fun_line_addr+20);
				GLCD_WriteString(disp_tempstr, false);

				fun_stage = 3;	    tool_type = 1;		sub_fun_state = 1;

				listbox_type = 0;	listbox_index = temp_val;		listbox_size = 2;    listbox_selected = 0;
				listbox = &rundays_oddeven_list;
				strcpy(disp_tempstr, (*listbox)[listbox_index]);

				fun_line_addr += 20;		fun_col_addr = 52;
			}
			else if (listbox_index == 2)
			{
				if (temp_val == 1)
					strcpy(disp_tempstr, "EVERY       st  DAYS");
				else if (temp_val == 2)
					strcpy(disp_tempstr, "EVERY       nd  DAYS");
				else if (temp_val == 3)
					strcpy(disp_tempstr, "EVERY       rd  DAYS");
				else
					strcpy(disp_tempstr, "EVERY       th  DAYS");

			    goto_screen_map_yz(3, fun_line_addr+20);
				GLCD_WriteString(disp_tempstr, false);

				fun_stage = 3;	    tool_type = 1;		sub_fun_state = 1;

				listbox_type = 2;	listbox_index = temp_val;		listbox_size = 7;    listbox_selected = 0;
				listbox = &virtual_list;	listbox_baseval = 0;
				usnprintf(disp_tempstr, 3, "%02u", (listbox_baseval + listbox_index));

				fun_line_addr += 20;		fun_col_addr = 52;
			}

			y_start = 0;	z_start = fun_line_addr-30;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 2)
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[3] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			z_addr = (fun_col_addr - 4) / 18;
			shift = z_addr;//By Karan
			temp_val = zoneprog[prog_index].rundaysval[0];
			if (listbox_index == 0)
				temp_val &= ~(0x01<<shift);
			else if (listbox_index == 1)
				temp_val |= (0x01<<shift);

			zoneprog[prog_index].rundaysval[0] = temp_val;
			// write data into eeprom
			eeprom_write_zoneprog1(prog_index,0x40);
			eeprom_read_zoneprog1(prog_index,0x40);
			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|5||26[0]||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].rundaysval[0],deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}
			prog_t_updated=1;

			if (z_addr >= 6)
			{
				fun_stage = 0;
				ZoneProgRunDays(kbVirtualEdit);

				return;
			}
			else
			{
				z_addr++;
				sub_fun_state = 1;

				listbox_index = ((temp_val & (0x01 << (z_addr)))?1:0);

				fun_line_addr = fun_line_addr;		fun_col_addr += 18;
				y_start = 0;	z_start = fun_line_addr;		y_end = 126;	z_end = fun_line_addr+9;
			}
		}
	}
	else if (fun_stage == 3)
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[3] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			zoneprog[prog_index].rundaysval[zoneprog[prog_index].rundaysmode] = listbox_index;

			// write data into eeprom
			eeprom_write_zoneprog1(prog_index,0x40);
			eeprom_read_zoneprog1(prog_index,0x40);
			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|5||26[1]||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].rundaysval[1],deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
				sprintf (msg_send,"*|5||26[2]||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].rundaysval[2],deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}
			prog_t_updated=1;
			fun_stage = 0;
			ZoneProgRunDays(kbVirtualEdit);

			return;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if (sub_fun_state == 1)	// init
	{
		sub_fun_state = 2;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(disp_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void ZoneProgStartTime(uint8_t key_code)
{
uint8_t z_addr;//, temp_tm_hour;
uint8_t y_start, z_start, y_end, z_end;
char m;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data from eeprom
		eeprom_read_zoneprog2(prog_index,0x01);
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		usnprintf(disp_tempstr, 21, "PROG %d- START TIME", (prog_index+1));
		GLCD_write_titlebar(disp_tempstr);

		z_addr = SCRN_MAP_CUR_Z+10;

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
		strcpy(disp_tempstr, "START TIME       ");

	    // set time value of editing
	    set_edit_time(zoneprog[prog_index].starttime, true);

	    if (disp_clockformat == 0)	strcat(disp_tempstr, (rd_time.tm_slot ? " PM":" AM"));

	    GLCD_WriteString(disp_tempstr, false);

		fun_stage = 1;	    tool_type = 2;		sub_fun_state = 1;

		textbox_char_encoded_trancated = 0;	textbox_size = 5;

		usnprintf(edit_tempstr, 6, "%02u:%02u", rd_time.tm_hour, rd_time.tm_min);

		fun_col_addr = 76;		fun_line_addr = z_addr;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// time (hh:mm)
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[3] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			if (time_strtodec(edit_tempstr, 1, true) == false)
			{
				fun_stage = 0;

				ZoneProgStartTime(kbVirtualEdit);
				return;
			}
			else
			{
			    // get time value of editing
			    get_edit_time(zoneprog[prog_index].starttime, true);


				if (disp_clockformat == 0)
				{
				    fun_stage = 2;	    tool_type = 1;		sub_fun_state = 1;

					listbox_type = 0;	listbox_index = rd_time.tm_slot;	listbox_size = 2;    listbox_selected = 0;
					listbox = &timeslot_list;
					strcpy(disp_tempstr, (*listbox)[listbox_index]);

					fun_line_addr = fun_line_addr;		fun_col_addr = 112;
				}
				else
				{
					sub_fun_state = 1;
				}

				y_start = 76;	z_start = fun_line_addr;		y_end = 126;	z_end = fun_line_addr+9;
			}
		}
	}
	else if (fun_stage == 2)	// time slot
	{
		if (key_code == kbEnter)
		{

			//for sync logic
			table_changed[3] = 1;
			table_changed[4] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			wr_time.tm_slot = listbox_index;

		    // get time value of editing
		    get_edit_time(zoneprog[prog_index].starttime, true);
		    // write data into eeprom
			eeprom_write_zoneprog2(prog_index,0x01);
			eeprom_read_zoneprog2(prog_index,0x01);

			for(m=0;m<10;m++)
			{
				zoneprog[prog_index].zone[m].starttime[0]=zoneprog[prog_index].starttime[0];
				zoneprog[prog_index].zone[m].starttime[1]=zoneprog[prog_index].starttime[1];
				eeprom_write_zoneprog4(prog_index,m,0x02);
				eeprom_read_zoneprog4(prog_index,m,0x02);

				zoneprog[prog_index].zone[m].stoptime[0]=zoneprog[prog_index].starttime[0];
				zoneprog[prog_index].zone[m].stoptime[1]=zoneprog[prog_index].starttime[1];
				eeprom_write_zoneprog4(prog_index,m,0x04);
				eeprom_read_zoneprog4(prog_index,m,0x04);
			}

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|5||27||%d:%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].starttime[0],zoneprog[prog_index].starttime[1],deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}
			prog_t_updated=1;

			fun_stage = 0;

			ZoneProgStartTime (kbVirtualEdit);
			return;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if (sub_fun_state == 1)	// init
	{
		sub_fun_state = 2;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(disp_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

uint16_t GetOutputSortedList(uint8_t tmp_optype, uint8_t zoneindex)
{
uint8_t index=0, opindex = 0;
uint16_t zonecnt=0;

	listbox_selected = 0;
	//get list of selected o/p types
	for (index=0; index<((noopmodules + 1) * 8); index++)
	{
		//check if o/p type is zone(3) / ferti valve(5)
		if (output[index].optype != tmp_optype)
			listbox_selected |= (0x01 << index);
		else
		{
			zonecnt++;
			if (zonecnt == (zoneindex + 1))	opindex = index;
		}
	}

	zonecnt |= (uint16_t)(opindex << 8);

	return zonecnt;
}

/********************************************************/

/********************************************************/
/*
uint8_t GetFirstZoneIndex(void)
{
uint8_t i=0, firstzoneindex = 0;

	//get list of selected o/p types
	for (i=0; i<((noopmodules + 1) * 8); i++)
	{
		//check if o/p type is zone
		if ((listbox_selected & (0x01 << i)) == 0)
		{
			firstzoneindex = i;
			break;
		}
	}

	return firstzoneindex;
}
*/
/********************************************************/

/********************************************************/
/*
void GetTotalTimeFlow(uint8_t prog_index, uint8_t tempzonecnt)
{
uint8_t index=0, zonecnt=0;//, firstzoneindex = 0;

	if (zoneprog[prog_index].zonecount != tempzonecnt)

	//get list of selected o/p types
	for (index=0; index<tempzonecnt; index++)
	{
		//check if o/p type is zone
		zoneprog[prog_index].zone[fun_result1].starttime[0];
	}

	if (zoneprog[prog_index].type == 0)				//time based
	else if (zoneprog[prog_index].type == 1)		//flow based
}
*/

void GetTotalTimeFlow(void)
{
	uint8_t temp0,temp1,i;

	if (zoneprog[prog_index].type == 0)				//time based
	{
		//Total time Calculation
		temp0=zoneprog[prog_index].zone[0].stoptime[0];
		temp1=zoneprog[prog_index].zone[0].stoptime[1];

		for(i=0;i<10;i++)
		{
//			if(zoneprog[prog_index].zone[i].stoptime[0] < zoneprog[prog_index].starttime[0])
//			{
//				//add 24 hrs
//			}

			if(zoneprog[prog_index].zone[i].stoptime[0]>temp0)
			{
				temp0=zoneprog[prog_index].zone[i].stoptime[0];
				temp1=zoneprog[prog_index].zone[i].stoptime[1];
			}
		}
		for(i=0;i<10;i++)
		{
			if(zoneprog[prog_index].zone[i].stoptime[0]==temp0)
			{
				if(zoneprog[prog_index].zone[i].stoptime[1]>temp1)
				{
					temp1=zoneprog[prog_index].zone[i].stoptime[1];
				}
			}
		}

		if(temp1>=zoneprog[prog_index].starttime[1])
		{
			zoneprog[prog_index].totaltime[1]=temp1-zoneprog[prog_index].starttime[1];
			if(temp0>=zoneprog[prog_index].starttime[0])
			{
				zoneprog[prog_index].totaltime[0]=temp0-zoneprog[prog_index].starttime[0];
			}
		}
		else
		{
			if(temp0>zoneprog[prog_index].starttime[0])
			{
				zoneprog[prog_index].totaltime[1]=zoneprog[prog_index].starttime[1]-temp1;
				zoneprog[prog_index].totaltime[1]=60-zoneprog[prog_index].totaltime[1];
				zoneprog[prog_index].totaltime[0]=(temp0-zoneprog[prog_index].starttime[0])-1;
			}
		};
		eeprom_write_zoneprog2(prog_index,0x04);
		eeprom_read_zoneprog2(prog_index,0x04);
	}
}

/*

bool GetTotalTimeFlow(void)
{
uint8_t index=0;
uint32_t tmp_value1 = 0, tmp_value2 = 0, tmp_total = 0;

	//get list of selected o/p types
	for (index=0; index<zoneprog[prog_index].zonecount; index++)
	{
		if (zoneprog[prog_index].type == 0)				//time based
		{
			tmp_value1	= ((zoneprog[prog_index].zone[index].starttime[0] * 60) +  zoneprog[prog_index].zone[index].starttime[1]);
			tmp_value2	= ((zoneprog[prog_index].zone[index].stoptime[0] * 60) +  zoneprog[prog_index].zone[index].stoptime[1]);

			tmp_total	+= (tmp_value2 - tmp_value1);
		}
		else if (zoneprog[prog_index].type == 1)		//flow based
		{
			tmp_value1	= zoneprog[prog_index].zone[index].startflow;
			tmp_value2	= zoneprog[prog_index].zone[index].stopflow;

			tmp_total	+= (tmp_value2 - tmp_value1);
		}
	}

	if (zoneprog[prog_index].type == 0)				//time based
	{
		if (tmp_total >= 1440)	return false;

		zoneprog[prog_index].totaltime[0] = (tmp_total / 60);
		zoneprog[prog_index].totaltime[1] = (tmp_total % 60);
	}
	else if (zoneprog[prog_index].type == 1)		//flow based
	{
		if (tmp_total >= 60000)	return false;

		zoneprog[prog_index].totalflow = tmp_total;
	}

	return true;
}
 */
/********************************************************/

/********************************************************/

void ZoneProgDetails(uint8_t key_code)
{
uint8_t z_addr, opindex = 0;
uint8_t y_start, z_start, y_end, z_end;
uint16_t zonecnt = 0;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data into eeprom
		//eeprom_read_zoneprog(prog_index);
		eeprom_read_zoneprog1(prog_index,0x02);

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		usnprintf(disp_tempstr, 21, "ZONE PROG%d- DETAILS", (prog_index+1));
		GLCD_write_titlebar(disp_tempstr);

		z_addr = SCRN_MAP_CUR_Z+10;

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
		strcpy(disp_tempstr, "TYPE    ");
		GLCD_WriteString(disp_tempstr, false);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 2;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
		listbox_type = 0;	listbox_index = zoneprog[prog_index].type;		listbox_size = 2;    listbox_selected = 0;
		listbox = &progtype_list;
		strcpy(edit_tempstr, (*listbox)[listbox_index]);

		fun_col_addr = 52;		fun_line_addr = z_addr;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// program type
	{
		if (key_code == kbEnter)
		{
			zoneprog[prog_index].type = listbox_index;

			// write data into eeprom
			//eeprom_write_zoneprog(prog_index);
			eeprom_write_zoneprog1(prog_index,0x02);
			eeprom_read_zoneprog1(prog_index,0x02);
			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|5||1||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].type,deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			fun_result1 = 0;
		}

		//get list of selected o/p types (o/p type != zone)
		zonecnt = GetOutputSortedList(3, fun_result1);
		opindex = (uint8_t)((zonecnt & 0xFF00) >> 8);
		zonecnt = (uint8_t)(zonecnt & 0xFF);

		// set zone index & number of available zones
		/*fun_result1 = 0;*/	fun_result2 = zonecnt;

		zoneprog[prog_index].zonecount = zonecnt;

		// display error message if no zones are available
		if (zonecnt == 0)
		{
		    goto_screen_map_yz(3, fun_line_addr);
		    GLCD_WriteString("    No Zones Are    ", false);
		    goto_screen_map_yz(3, fun_line_addr+10);
		    GLCD_WriteString("     Configured     ", false);

			GLCD_Screen_Update();

		    //delay_sec(2);
			Task_sleep(2000);
			GetFunFlag = 0;
			return;
		}

		fun_line_addr = TITLEBAR_TOP_SPACE + TITLEBAR_HEIGHT;

		// Display Screen
		goto_screen_map_yz(3, fun_line_addr);
		strcpy(disp_tempstr, "ZONE                ");
		GLCD_WriteString(disp_tempstr, false);

		if (zoneprog[prog_index].type == 0)	// time based
		{
			eeprom_read_zoneprog2(prog_index,0x04);
			eeprom_read_zoneprog4(prog_index,fun_result1,0x02);
			eeprom_read_zoneprog4(prog_index,fun_result1,0x04);
			goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
//			usnprintf(disp_tempstr, 21, "TOTAL TIME       Hrs");
			usnprintf(disp_tempstr, 21, "TOTAL TIME          ");
			GLCD_WriteString(disp_tempstr, false);

			goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
//			usnprintf(disp_tempstr, 21, "START TIME       Hrs");
			usnprintf(disp_tempstr, 21, "START TIME          ");
			GLCD_WriteString(disp_tempstr, false);

			goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
//			usnprintf(disp_tempstr, 21, "STOP TIME        Hrs");
			usnprintf(disp_tempstr, 21, "STOP TIME           ");
			GLCD_WriteString(disp_tempstr, false);
		}
		else					// flow based
		{
			eeprom_read_zoneprog2(prog_index,0x08);
			eeprom_read_zoneprog4(prog_index,fun_result1,0x08);
			eeprom_read_zoneprog4(prog_index,fun_result1,0x10);

			goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
			usnprintf(disp_tempstr, 21, "TOTAL FLOW        Lt");
			GLCD_WriteString(disp_tempstr, false);

			goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
			usnprintf(disp_tempstr, 21, "START FLOW        Lt");
			GLCD_WriteString(disp_tempstr, false);

			goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
			usnprintf(disp_tempstr, 21, "STOP FLOW         Lt");
			GLCD_WriteString(disp_tempstr, false);
		}

		eeprom_read_zoneprog4(prog_index,fun_result1,0x20);
		goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
		usnprintf(disp_tempstr, 21, "FERTI PROG ");
		GLCD_WriteString(disp_tempstr, false);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 2;	    tool_type = 1;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t) / 3(valve/zone)
		listbox_type = 3;	listbox_index = opindex;/*zoneprog[prog_index].zone[fun_result1].oppartno;*/		listbox_size = ((noopmodules + 1) * 8);    //listbox_selected = 0;
		listbox = &virtual_list;	listbox_baseval = 0;

		usnprintf(edit_tempstr, 14, "%01u. %s", output[listbox_index].typeid, output[listbox_index].opname);
		SCRN_MAP_CUR_Z = fun_line_addr+10;

		fun_col_addr = 40;		//fun_line_addr = fun_line_addr;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 2)	// zone
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[4] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);
			//zoneprog[prog_index].zone[fun_result1].oppartno = listbox_index;

			// write data into eeprom
			//eeprom_write_zoneprog(prog_index);//todo

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 3;	    tool_type = 2;		sub_fun_state = 1;

			if (zoneprog[prog_index].type == 0)		// time based
			{
				textbox_char_encoded_trancated = 0;	textbox_size = 5;

			    // set time value of editing
			    set_edit_time(zoneprog[prog_index].zone[fun_result1].starttime, true);
				usnprintf(edit_tempstr, 6, "%02u:%02u", rd_time.tm_hour, rd_time.tm_min);
			}
			else if (zoneprog[prog_index].type == 1)	// flow based
			{
				//$$NR$$//dated:19Sep15
				textbox_char_encoded_trancated = 4;			textbox_size = 5;
				editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
				//$$NR$$//dated:19Sep15

				usnprintf(edit_tempstr, 6, "%05d", zoneprog[prog_index].zone[fun_result1].startflow);
				strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15
			}
			SCRN_MAP_CUR_Z = fun_line_addr+10;

			fun_line_addr += 20;		fun_col_addr = 70;
//			y_start = fun_col_addr;		z_start = fun_line_addr-20;		y_end = 126;	z_end = fun_line_addr+9;
			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
	}
	else if (fun_stage == 3)	// start time(hh:mm)/flow
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[4] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);
			if (zoneprog[prog_index].type == 0)		// time based
			{
				if (time_strtodec(edit_tempstr, 1, true) == false)
				{
					sub_fun_state = 2;

					usnprintf(edit_tempstr, 6, "%02u:%02u", rd_time.tm_hour, rd_time.tm_min);
					y_start = fun_col_addr;	z_start = fun_line_addr;		y_end = 127;	z_end = fun_line_addr+9;
				}
				else
				{
				    // get time value of editing
				    get_edit_time(zoneprog[prog_index].zone[fun_result1].starttime, true);

					// write data into eeprom
					//eeprom_write_zoneprog(prog_index);
				    eeprom_write_zoneprog4(prog_index,fun_result1,0x02);
				    eeprom_read_zoneprog4(prog_index,fun_result1,0x02);
				    if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
					{
				    	sprintf (msg_send,"*|4||3||%d:%d||1||%s||%d||%d|*\r\n",zoneprog[prog_index].zone[fun_result1].starttime[0],zoneprog[prog_index].zone[fun_result1].starttime[1],deviceid,(prog_index+1),(fun_result1+1));
				    	strcat (temp1,msg_send);
						memset(msg_send,0, sizeof(msg_send));
					}

				    prog_t_updated=1;


					if (disp_clockformat == 0)
					{
						fun_stage = 4;	    tool_type = 1;		sub_fun_state = 1;

						listbox_type = 0;	listbox_index = rd_time.tm_slot;	listbox_size = 2;    listbox_selected = 0;
						listbox = &timeslot_list;
						strcpy(edit_tempstr, (*listbox)[listbox_index]);

						fun_line_addr = fun_line_addr;		fun_col_addr = 106;
						y_start = 76;	z_start = fun_line_addr;		y_end = 126;	z_end = fun_line_addr+9;
					}
					else
					{
						// set fun_stage = value for next stage
						// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
						fun_stage = 5;	    tool_type = 2;		sub_fun_state = 1;

						textbox_char_encoded_trancated = 0;	textbox_size = 5;

						// set time value of editing
					    set_edit_time(zoneprog[prog_index].zone[fun_result1].stoptime, true);
						usnprintf(edit_tempstr, 6, "%02u:%02u", rd_time.tm_hour, rd_time.tm_min);

						SCRN_MAP_CUR_Z = fun_line_addr-10;

						fun_line_addr += 10;		fun_col_addr = 70;
						y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
					}
				}
			}
			else if (zoneprog[prog_index].type == 1)	// flow based
			{
				//for sync logic
				table_changed[4] = 1;
				eeprom_write_progstatus(0x40);
				eeprom_read_progstatus(0x40);

				zoneprog[prog_index].zone[fun_result1].startflow = (uint32_t)strtoint(edit_tempstr, 5);

				// write data into eeprom
				//eeprom_write_zoneprog(prog_index);
				eeprom_write_zoneprog4(prog_index,fun_result1,0x08);
				eeprom_read_zoneprog4(prog_index,fun_result1,0x08);
				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					 sprintf (msg_send,"*|4||5||%d||1||%s||%d||%d|*\r\n",zoneprog[prog_index].zone[fun_result1].startflow,deviceid,(prog_index+1),(fun_result1+1));
					 strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}

				//$$NR$$//dated:19Sep15
				textbox_char_encoded_trancated = 4;			textbox_size = 5;
				editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
				//$$NR$$//dated:19Sep15

				usnprintf(edit_tempstr, 6, "%05d", zoneprog[prog_index].zone[fun_result1].stopflow);
				strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

				SCRN_MAP_CUR_Z = fun_line_addr-10;

				// set fun_stage = value for next stage
				// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
				fun_stage = 5;	    tool_type = 2;		sub_fun_state = 1;

				fun_line_addr += 10;		fun_col_addr = 70;
				y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
			}

//			if (sub_fun_state == 1)
//			{
//				SCRN_MAP_CUR_Z = fun_line_addr-10;
//
//				// set fun_stage = value for next stage
//				// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
//				fun_stage = 4;	    tool_type = 2;		sub_fun_state = 1;
//
//				fun_line_addr += 10;		fun_col_addr = 70;
//				y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
//			}
		}
	}
	else if (fun_stage == 4)	// time slot
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[4] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			wr_time.tm_slot = listbox_index;

		    // get time value of editing
		    get_edit_time(zoneprog[prog_index].zone[fun_result1].starttime, true);

			// write data into eeprom
		//	eeprom_write_zoneprog(prog_index);
		    eeprom_write_zoneprog4(prog_index,fun_result1,0x02);
		    eeprom_read_zoneprog4(prog_index,fun_result1,0x02);
		    if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|4||3||%d:%d||1||%s||%d||%d|*\r\n",zoneprog[prog_index].zone[fun_result1].starttime[0],zoneprog[prog_index].zone[fun_result1].starttime[1],deviceid,(prog_index+1),(fun_result1+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}
		    prog_t_updated=1;

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 5;	    tool_type = 2;		sub_fun_state = 1;

			textbox_char_encoded_trancated = 0;	textbox_size = 5;

			// set time value of editing
		    set_edit_time(zoneprog[prog_index].zone[fun_result1].stoptime, true);
			usnprintf(edit_tempstr, 6, "%02u:%02u", rd_time.tm_hour, rd_time.tm_min);

			SCRN_MAP_CUR_Z = fun_line_addr-10;

			fun_line_addr += 10;		fun_col_addr = 70;
			y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 5)	// stop time(hh:mm)/flow
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[4] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			if (zoneprog[prog_index].type == 0)		// time based
			{
				if (time_strtodec(edit_tempstr, 1, true) == false)
				{
					sub_fun_state = 2;

					usnprintf(edit_tempstr, 6, "%02u:%02u", rd_time.tm_hour, rd_time.tm_min);
					y_start = fun_col_addr;	z_start = fun_line_addr;		y_end = 127;	z_end = fun_line_addr+9;
				}
				else
				{
				    // get time value of editing
				    get_edit_time(zoneprog[prog_index].zone[fun_result1].stoptime, true);

					// write data into eeprom
					//eeprom_write_zoneprog(prog_index);
				    eeprom_write_zoneprog4(prog_index,fun_result1,0x04);
				    eeprom_read_zoneprog4(prog_index,fun_result1,0x04);
				    if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
					{
						sprintf (msg_send,"*|4||4||%d:%d||1||%s||%d||%d|*\r\n",zoneprog[prog_index].zone[fun_result1].stoptime[0],zoneprog[prog_index].zone[fun_result1].stoptime[1],deviceid,(prog_index+1),(fun_result1+1));
						strcat (temp1,msg_send);
						memset(msg_send,0, sizeof(msg_send));
					}
				    prog_t_updated=1;

					if (disp_clockformat == 0)
					{
						fun_stage = 6;	    tool_type = 1;		sub_fun_state = 1;

						listbox_type = 0;	listbox_index = rd_time.tm_slot;	listbox_size = 2;    listbox_selected = 0;
						listbox = &timeslot_list;
						strcpy(edit_tempstr, (*listbox)[listbox_index]);

						fun_line_addr = fun_line_addr;		fun_col_addr = 106;
						y_start = 76;	z_start = fun_line_addr;		y_end = 126;	z_end = fun_line_addr+9;
					}
					else
					{
						// set fun_stage = value for next stage
						// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
						fun_stage = 7;	    tool_type = 1;		sub_fun_state = 1;

						// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
						listbox_type = 2;	listbox_index = zoneprog[prog_index].zone[fun_result1].fertprogid;		listbox_size = 7;    listbox_selected = 0;
						listbox = &virtual_list;	listbox_baseval = 0;

						usnprintf(edit_tempstr, 3, "%02u", (listbox_baseval + listbox_index));
						SCRN_MAP_CUR_Z = fun_line_addr-20;

						fun_col_addr = 70;		fun_line_addr += 10;
						y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
					}
				}
			}
			else if (zoneprog[prog_index].type == 1)	// flow based
			{
				zoneprog[prog_index].zone[fun_result1].stopflow = (uint32_t)strtoint(edit_tempstr, 5);

				// write data into eeprom
				//eeprom_write_zoneprog(prog_index);
				eeprom_write_zoneprog4(prog_index,fun_result1,0x10);
				eeprom_read_zoneprog4(prog_index,fun_result1,0x10);
				if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
				{
					 sprintf (msg_send,"*|4||6||%d||1||%s||%d||%d|*\r\n",zoneprog[prog_index].zone[fun_result1].stopflow,deviceid,(prog_index+1),(fun_result1+1));
					 strcat (temp1,msg_send);
					memset(msg_send,0, sizeof(msg_send));
				}

				// set fun_stage = value for next stage
				// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
				fun_stage = 7;	    tool_type = 1;		sub_fun_state = 1;

				// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
				listbox_type = 2;	listbox_index = zoneprog[prog_index].zone[fun_result1].fertprogid;		listbox_size = 7;    listbox_selected = 0;
				listbox = &virtual_list;	listbox_baseval = 0;

				usnprintf(edit_tempstr, 3, "%02u", (listbox_baseval + listbox_index));
				SCRN_MAP_CUR_Z = fun_line_addr-20;

				fun_col_addr = 70;		fun_line_addr += 10;
				y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
			}

//			if (sub_fun_state == 1)
//			{
//				SCRN_MAP_CUR_Z = fun_line_addr-10;
//
//				// set fun_stage = value for next stage
//				// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
//				fun_stage = 4;	    tool_type = 2;		sub_fun_state = 1;
//
//				fun_line_addr += 10;		fun_col_addr = 70;
//				y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
//			}
		}
	}
	else if (fun_stage == 6)	// time slot
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[4] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			wr_time.tm_slot = listbox_index;

		    // get time value of editing
		    get_edit_time(zoneprog[prog_index].zone[fun_result1].stoptime, true);

			// write data into eeprom
		//	eeprom_write_zoneprog(prog_index);
		    eeprom_write_zoneprog4(prog_index,fun_result1,0x04);
		    eeprom_read_zoneprog4(prog_index,fun_result1,0x04);
		    if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|4||4||%d:%d||1||%s||%d||%d|*\r\n",zoneprog[prog_index].zone[fun_result1].stoptime[0],zoneprog[prog_index].zone[fun_result1].stoptime[1],deviceid,(prog_index+1),(fun_result1+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

		    prog_t_updated=1;

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 7;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 2;	listbox_index = zoneprog[prog_index].zone[fun_result1].fertprogid;		listbox_size = 7;    listbox_selected = 0;
			listbox = &virtual_list;	listbox_baseval = 0;

			usnprintf(edit_tempstr, 3, "%02u", (listbox_baseval + listbox_index));
			SCRN_MAP_CUR_Z = fun_line_addr-20;

			fun_col_addr = 70;		fun_line_addr += 10;
			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
	}
	else if (fun_stage == 7)	// ferti prog id
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[4] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			zoneprog[prog_index].zone[fun_result1].fertprogid = listbox_index;

			// write data into eeprom
			//eeprom_write_zoneprog(prog_index);
			eeprom_write_zoneprog4(prog_index,fun_result1,0x20);
			eeprom_read_zoneprog4(prog_index,fun_result1,0x20);
			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|4||2||%d||1||%s||%d||%d|*\r\n",zoneprog[prog_index].zone[fun_result1].fertprogid,deviceid,(prog_index+1),(fun_result1+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 1;	    //tool_type = 1;		sub_fun_state = 1;

			fun_result1++;
			if (fun_result1 >= fun_result2)	fun_result1 = 0;

/*
			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 3;	listbox_index = zoneprog[prog_index].zone[fun_result1].oppartno;		listbox_size = ((noopmodules + 1) * 8);    listbox_selected = 0;
			listbox = &virtual_list;	listbox_baseval = 0;

			//get list of selected o/p types
			for (i=0; i<((noopmodules + 1) * 8); i++)
			{
				//check if o/p type is ferti zone
				if (output[i].optype != 3)	listbox_selected |= (0x01 << i);//output[i].optype);
			}

//			usnprintf(disp_tempstr, 14, "%01u. %s", output[listbox_index].typeid, output[listbox_index].opname);
			usnprintf(edit_tempstr, 14, "%01u. %s", output[listbox_index].typeid, output[listbox_index].opname);
			SCRN_MAP_CUR_Z = fun_line_addr-30;

			fun_col_addr = 40;		fun_line_addr -= 40;
			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
*/
			ZoneProgDetails(kbVirtualEdit);
			return;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if ((sub_fun_state == 1) || (sub_fun_state == 2))	// init screen / line
	{
		if (sub_fun_state == 1)	// init screen
		{
		    // calculate total time/flow
		    GetTotalTimeFlow();

			SCRN_MAP_CUR_Z = MIDDLE_PAGE_TOP+10;

			if (zoneprog[prog_index].type == 0)		// time based
			{
				goto_screen_map_yz(70, SCRN_MAP_CUR_Z);
				usnprintf(disp_tempstr, 6, "%02u:%02u", zoneprog[prog_index].totaltime[0], zoneprog[prog_index].totaltime[1]);
//			    if (disp_clockformat == 0)	strcat(disp_tempstr, (CurTime.tm_slot ? " PM":" AM"));
				GLCD_WriteString(disp_tempstr, false);

				goto_screen_map_yz(70, SCRN_MAP_CUR_Z);
//				usnprintf(disp_tempstr, 6, "%02u:%02u", zoneprog[prog_index].zone[fun_result1].starttime[0], zoneprog[prog_index].zone[fun_result1].starttime[1]);
//			    if (disp_clockformat == 0)	strcat(disp_tempstr, (CurTime.tm_slot ? " PM":" AM"));
				set_disp_time(zoneprog[prog_index].zone[fun_result1].starttime, true);
				usnprintf(disp_tempstr, 6, "%02u:%02u", disp_time.tm_hour, disp_time.tm_min);
			    if (disp_clockformat == 0)	strcat(disp_tempstr, (disp_time.tm_slot ? " PM":" AM"));
				GLCD_WriteString(disp_tempstr, false);

				goto_screen_map_yz(70, SCRN_MAP_CUR_Z);
//				usnprintf(disp_tempstr, 6, "%02u:%02u", zoneprog[prog_index].zone[fun_result1].stoptime[0], zoneprog[prog_index].zone[fun_result1].stoptime[1]);
//			    if (disp_clockformat == 0)	strcat(disp_tempstr, (CurTime.tm_slot ? " PM":" AM"));
				set_disp_time(zoneprog[prog_index].zone[fun_result1].stoptime, true);
				usnprintf(disp_tempstr, 6, "%02u:%02u", disp_time.tm_hour, disp_time.tm_min);
			    if (disp_clockformat == 0)	strcat(disp_tempstr, (disp_time.tm_slot ? " PM":" AM"));
				GLCD_WriteString(disp_tempstr, false);
			}
			else if (fertiprog[prog_index].type == 1)	// flow based
			{
				goto_screen_map_yz(70, SCRN_MAP_CUR_Z);
				usnprintf(disp_tempstr, 6, "%05d", zoneprog[prog_index].totalflow);
				GLCD_WriteString(disp_tempstr, false);

				goto_screen_map_yz(70, SCRN_MAP_CUR_Z);
				usnprintf(disp_tempstr, 6, "%05d", zoneprog[prog_index].zone[fun_result1].startflow);
				GLCD_WriteString(disp_tempstr, false);

				goto_screen_map_yz(70, SCRN_MAP_CUR_Z);
				usnprintf(disp_tempstr, 6, "%05d", zoneprog[prog_index].zone[fun_result1].stopflow);
				GLCD_WriteString(disp_tempstr, false);
			}

			goto_screen_map_yz(70, SCRN_MAP_CUR_Z);
			usnprintf(disp_tempstr, 3, "%02u", (zoneprog[prog_index].zone[fun_result1].fertprogid));
			GLCD_WriteString(disp_tempstr, false);
		}

		sub_fun_state = 3;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(edit_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void ZoneProgDelays(uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data into eeprom
		//eeprom_read_zoneprog(prog_index);
		eeprom_read_zoneprog2(prog_index,0x10);
		eeprom_read_zoneprog2(prog_index,0x20);

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		usnprintf(disp_tempstr, 21, "PROG %d- DELAYS", (prog_index+1));
		GLCD_write_titlebar(disp_tempstr);

		z_addr = SCRN_MAP_CUR_Z+10;

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
	    strcpy(disp_tempstr, "PUMP DELAY      Secs");
	    GLCD_WriteString(disp_tempstr, false);

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
		usnprintf(disp_tempstr, 21, "ZONE DELAY  %03d Secs", zoneprog[prog_index].zonedelay);
	    GLCD_WriteString(disp_tempstr, false);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 2;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
		//$$NR$$//dated:19Sep15
		textbox_char_encoded_trancated = 4;			textbox_size = 3;
		editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
		//$$NR$$//dated:19Sep15

		usnprintf(edit_tempstr, 4, "%03d", zoneprog[prog_index].pumpdelay);
		strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

		fun_line_addr = z_addr;		fun_col_addr = 76;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// Pump delay
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[3] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			zoneprog[prog_index].pumpdelay = (uint16_t)strtoint(edit_tempstr, 3);

			// write data into eeprom
			//eeprom_write_zoneprog(prog_index);
			eeprom_write_zoneprog2(prog_index,0x10);
			eeprom_read_zoneprog2(prog_index,0x10);
			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|5||31||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].pumpdelay,deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 2;	    tool_type = 2;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			//$$NR$$//dated:19Sep15
			textbox_char_encoded_trancated = 4;			textbox_size = 3;
			editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			//$$NR$$//dated:19Sep15

			usnprintf(edit_tempstr, 4, "%03d", zoneprog[prog_index].zonedelay);
			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			fun_line_addr += 10;		//fun_col_addr = 76;
			y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 2)	//Zone Delay
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[3] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			zoneprog[prog_index].zonedelay = (uint16_t)strtoint(edit_tempstr, 3);

			// write data into eeprom
			//eeprom_write_zoneprog(prog_index);
			eeprom_write_zoneprog2(prog_index,0x20);
			eeprom_read_zoneprog2(prog_index,0x20);
			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|5||32||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].zonedelay,deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 1;	    tool_type = 2;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			//$$NR$$//dated:19Sep15
			textbox_char_encoded_trancated = 4;			textbox_size = 3;
			editvalue_min_limit = MIN_LIMIT;			editvalue_max_limit = MAX_LIMIT;
			//$$NR$$//dated:19Sep15

			usnprintf(edit_tempstr, 4, "%03d", zoneprog[prog_index].pumpdelay);
			strcpy(edit_data_str, edit_tempstr);		//$$NR$$//dated:19Sep15

			fun_line_addr -= 10;		//fun_col_addr = 76;
			y_start = fun_col_addr;	z_start = fun_line_addr;		y_end = 126;	z_end = fun_line_addr+19;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if (sub_fun_state == 1)	// init
	{
		sub_fun_state = 2;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(disp_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void ZoneClearProgram(uint8_t key_code)
{
uint8_t z_addr,j;

	if (fun_stage == 0)
	{
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		usnprintf(disp_tempstr, 21, "PROG%d- CLEAR PROGRAM", (prog_index+1));
		GLCD_write_titlebar(disp_tempstr);
		z_addr = SCRN_MAP_CUR_Z + 10;

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
	    GLCD_WriteString("   Do You Want to   ", false);
	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
	    GLCD_WriteString("   Clear Program?   ", false);

	    fun_line_addr = z_addr;
		fun_stage = 1;

		GLCD_Screen_Update();
	}
	else if (fun_stage == 1)
	{
		if (key_code == kbEnter)
		{
			//for sync logic
			table_changed[3] = 1;
			table_changed[4] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

		    goto_screen_map_yz(3, fun_line_addr);
		    GLCD_WriteString("   System Clearing  ", false);
		    goto_screen_map_yz(3, fun_line_addr+10);
		    GLCD_WriteString("      Program..     ", false);

		    GLCD_MsgLine_Update(0, fun_line_addr, 126, fun_line_addr + 20);

		    // get default value
		    get_default_value_zoneprog(prog_index);

			// write data into eeprom
			//eeprom_write_zoneprog(prog_index);
			eeprom_write_zoneprog1(prog_index,0XFF);
			eeprom_write_zoneprog2(prog_index,0XFF);
			eeprom_write_zoneprog3(prog_index,0XFF);
			eeprom_read_zoneprog1(prog_index,0XFF);
			eeprom_read_zoneprog2(prog_index,0XFF);
			eeprom_read_zoneprog3(prog_index,0XFF);
			for(j=0;j<10;j++)
			{
				eeprom_write_zoneprog4(prog_index,j,0XFF);
				eeprom_read_zoneprog4(prog_index,j,0XFF);
			}

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|5||33||1||1||%s||%d||0|*\r\n",deviceid,(prog_index+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||id||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].id,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||1||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].type,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||2||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].status,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||3||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].repeatcycles,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||4||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].cycledelay,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||5||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].rain.condition,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||6||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].rain.currentval,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||7||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].flow.condition,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||8||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].flow.currentval,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||9||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].temperature.condition,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||10||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].temperature.currentval,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||11||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].temperature.min,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||12||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].temperature.max,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||13||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].humidity.condition,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||14||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].humidity.currentval,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||15||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].humidity.min,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||16||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].humidity.max,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||17||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].ippressure.condition,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||18||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].ippressure.currentval,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||19||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].ippressure.min,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||20||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].ippressure.max,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||21||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].oppressure.condition,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||22||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].oppressure.currentval,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||23||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].oppressure.min,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||24||%.2lf||1||%s||%d||0|*\r\n",zoneprog[prog_index].oppressure.max,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||25||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].rundaysmode,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||26[0]||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].rundaysval[0],deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||26[1]||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].rundaysval[1],deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||26[2]||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].rundaysval[2],deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||27||%d:%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].starttime[0],zoneprog[prog_index].starttime[1],deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||28||%d:%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].totaltime[0],zoneprog[prog_index].totaltime[1],deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||29||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].zonecount,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||30||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].totalflow,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||31||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].pumpdelay,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//				sprintf (msg_send,"*|5||32||%d||1||%s||%d||0|*\r\n",zoneprog[prog_index].zonedelay,deviceid,(prog_index+1));
//				strcat (temp1,msg_send);
//				memset(msg_send,0, sizeof(msg_send));
//
//				for(j=0;j<10;j++)
//				{
//					sprintf (msg_send,"*|4||1||%d||1||%s||%d||%d|*\r\n",zoneprog[prog_index].zone[j].oppartno,deviceid,(prog_index+1),(j));
//					strcat (temp1,msg_send);
//					memset(msg_send,0, sizeof(msg_send));
//					sprintf (msg_send,"*|4||2||%d||1||%s||%d||%d|*\r\n",zoneprog[prog_index].zone[j].fertprogid,deviceid,(prog_index+1),(j));
//					strcat (temp1,msg_send);
//					memset(msg_send,0, sizeof(msg_send));
//					//time_minutes=(zoneprog[prog_index].zone[ioid].starttime[0]*60)+zoneprog[prog_index].zone[ioid].starttime[1];
//					sprintf (msg_send,"*|4||3||%d:%d||1||%s||%d||%d|*\r\n",zoneprog[prog_index].zone[j].starttime[0],zoneprog[prog_index].zone[j].starttime[1],deviceid,(prog_index+1),(j));
//					strcat (temp1,msg_send);
//					memset(msg_send,0, sizeof(msg_send));
//					//time_minutes=(zoneprog[prog_index].zone[ioid].stoptime[0]*60)+zoneprog[prog_index].zone[ioid].stoptime[1];
//					sprintf (msg_send,"*|4||4||%d:%d||1||%s||%d||%d|*\r\n",zoneprog[prog_index].zone[j].stoptime[0],zoneprog[prog_index].zone[j].stoptime[1],deviceid,(prog_index+1),(j));
//					strcat (temp1,msg_send);
//					memset(msg_send,0, sizeof(msg_send));
//					sprintf (msg_send,"*|4||5||%d||1||%s||%d||%d|*\r\n",zoneprog[prog_index].zone[j].startflow,deviceid,(prog_index+1),(j));
//					strcat (temp1,msg_send);
//					memset(msg_send,0, sizeof(msg_send));
//					sprintf (msg_send,"*|4||6||%d||1||%s||%d||%d|*\r\n",zoneprog[prog_index].zone[j].stopflow,deviceid,(prog_index+1),(j));
//					strcat (temp1,msg_send);
//					memset(msg_send,0, sizeof(msg_send));
//
//				}


			}

			 prog_t_updated=1;
			 Remaintime_clear(prog_index);


		    //delay_sec(2);
		    Task_sleep(2000);

			GetFunFlag = 0;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}
}

/********************************************************/

/********************************************************/

void AutoMan_SelectMode(uint8_t key_code)
{
uint8_t i=0;

//	if (fun_stage == 0)
//	{
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen - Title
		GLCD_write_titlebar("AUTO MODE");

		// clear flag of manual mode
		manual_mode_enabled = 0;

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z+20);
	    GLCD_WriteString(" Set to AUTO Mode! ", false);

		GLCD_Screen_Update();

//	    // get default values and store data into eeprom
//	    get_default_value_man_program();

		for (i=0; i<MAN_PROG_MAX; i++)
			manprog[i].state = 0;

		for (i=0; i<OUTPUT_MAX; i++)
			manoutput[i].state = 0;

//		eeprom_write_man_program(0xFF);//(i);
//		eeprom_read_man_program(0xFF);//(i);

//	    // get default values and store data into eeprom
//	    get_default_value_man_output();



//		eeprom_write_man_output(0xFF);//(i);
//		eeprom_read_man_output(0xFF);//(i);

	    //delay_sec(2);
		Task_sleep(2000);

//		GetFunFlag = 0;
//	}
}

/********************************************************/

/********************************************************/

void AutoMan_SelectMode1(uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen - Title
		GLCD_write_titlebar("MODE SELECT");

		z_addr = SCRN_MAP_CUR_Z+10;

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
	    strcpy(disp_tempstr, "MODE    ");
	    GLCD_WriteString(disp_tempstr, false);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
		listbox_type = 0;	listbox_index = manual_mode_enabled;		listbox_size = 2;    listbox_selected = 0;
		listbox = &automanmode_list;
		strcpy(edit_tempstr, (*listbox)[listbox_index]);

		fun_col_addr = 52;		fun_line_addr = z_addr;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// manual/auto mode
	{
		if (key_code == kbEnter)
		{
			manual_mode_enabled = listbox_index;

			GetFunFlag = 0;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if (sub_fun_state == 1)		// init screen / line
	{
		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(edit_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void ManualProgram(uint8_t key_code)
{
uint8_t z_addr, fun_para_no;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data into eeprom
//	    for (fun_para_no=0; fun_para_no<MAN_PROG_MAX; fun_para_no++)
//	    	eeprom_read_man_program(0xFF);//(fun_para_no);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
		listbox_type = 0;	listbox_index = manprog[fun_result1].state;		listbox_size = 2;    listbox_selected = 0;
		listbox = &state_list;
		strcpy(edit_tempstr, (*listbox)[listbox_index]);

		fun_col_addr = 78;		fun_line_addr = MIDDLE_PAGE_TOP;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// status
	{
		if (key_code == kbEnter)
		{
			manprog[fun_result1].state = listbox_index;

			// store data into eeprom
//			eeprom_write_man_program(fun_result1);
//			eeprom_read_man_program(fun_result1);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|8||1||%d||1||%s||0||%d|*\r\n",manprog[fun_result1].state,deviceid,(fun_result1+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 1;	    tool_type = 1;		sub_fun_state = 2;

			fun_col_addr = 78;

			fun_result1++;
			if (fun_result1 < 5)
			{
				fun_line_addr += 10;
				y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+10;
			}
			else if (fun_result1 >= 5)
			{
				sub_fun_state = 1;
//				fun_line_addr += 10;

				if (fun_result1 >= MAN_PROG_MAX)
				{
					fun_result1 = 0;
					fun_line_addr = MIDDLE_PAGE_TOP;
				}

				y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
			}

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 0;	listbox_index = manprog[fun_result1].state;		listbox_size = 2;    listbox_selected = 0;
			listbox = &state_list;
			strcpy(edit_tempstr, (*listbox)[listbox_index]);
		}
	}


	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if ((sub_fun_state == 1) || (sub_fun_state == 2))	// init screen / line
	{
		if (sub_fun_state == 1)	// init screen
		{
			if (fun_result1 < 5)
				fun_para_no = 0;
			else
				fun_para_no = fun_result1 - 4;

			GLCD_clear_screen(false);						// clear the lcd display

			// Display Screen - Title
			usnprintf(disp_tempstr, 21, "MANAUL PROGRAM");
			GLCD_write_titlebar(disp_tempstr);

//			fun_line_addr = SCRN_MAP_CUR_Z;

			for (z_addr=0; z_addr<5; z_addr++)
			{
				goto_screen_map_yz(5, SCRN_MAP_CUR_Z);
				usnprintf(disp_tempstr, 21, "PROGRAM %01u   %s", (fun_para_no+1), state_list[manprog[fun_para_no].state]);
				GLCD_WriteString(disp_tempstr, false);

				fun_para_no++;
			}

			// display down arrow key
			goto_screen_map_yz(0, (MIDDLE_PAGE_TOP + 4*GLCD_LINE_HEIGHT + 1));
			GLCD_writesymbol((uint8_t)(96+32), false);
		}

		sub_fun_state = 3;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(edit_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}


/********************************************************/

/********************************************************/

void ManualOutput(uint8_t key_code)
{
uint8_t z_addr, fun_para_no;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// read data into eeprom
//	    for (fun_para_no=0; fun_para_no<OUTPUT_MAX; fun_para_no++)
//	    	eeprom_read_man_output(0xFF);//(fun_para_no);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
		listbox_type = 0;	listbox_index = manoutput[fun_result1].state;		listbox_size = 2;    listbox_selected = 0;
		listbox = &state_list;
		strcpy(edit_tempstr, (*listbox)[listbox_index]);

		fun_col_addr = 102;		fun_line_addr = MIDDLE_PAGE_TOP;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// status
	{
		if (key_code == kbEnter)
		{
			manoutput[fun_result1].state = listbox_index;

			// store data into eeprom
//			eeprom_write_man_output(fun_result1);
//			eeprom_read_man_output(fun_result1);

			if(((gsm_activ==1)&&(gsm_config==1))||((wifi_activ==1)&&(wifi_config==1)))
			{
				sprintf (msg_send,"*|9||1||%d||1||%s||0||%d|*\r\n",manoutput[fun_result1].state,deviceid,(fun_result1+1));
				strcat (temp1,msg_send);
				memset(msg_send,0, sizeof(msg_send));
			}
			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 1;	    tool_type = 1;		sub_fun_state = 2;

			fun_col_addr = 102;

			fun_result1++;
			if (fun_result1 < 5)
			{
				fun_line_addr += 10;
				y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+10;
			}
			else if (fun_result1 >= 5)
			{
				sub_fun_state = 1;
//				fun_line_addr += 10;

				if (fun_result1 >= OUTPUT_MAX)
				{
					fun_result1 = 0;
					fun_line_addr = MIDDLE_PAGE_TOP;
				}

				y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
			}

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 0;	listbox_index = manoutput[fun_result1].state;		listbox_size = 2;    listbox_selected = 0;
			listbox = &state_list;
			strcpy(edit_tempstr, (*listbox)[listbox_index]);
		}
	}


	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if ((sub_fun_state == 1) || (sub_fun_state == 2))	// init screen / line
	{
		if (sub_fun_state == 1)	// init screen
		{
			if (fun_result1 < 5)
				fun_para_no = 0;
			else
				fun_para_no = fun_result1 - 4;

			GLCD_clear_screen(false);						// clear the lcd display

			// Display Screen - Title
			usnprintf(disp_tempstr, 21, "MANAUL OUTPUT");
			GLCD_write_titlebar(disp_tempstr);

//			fun_line_addr = SCRN_MAP_CUR_Z;

			for (z_addr=0; z_addr<5; z_addr++)
			{
				goto_screen_map_yz(5, SCRN_MAP_CUR_Z);
//				usnprintf(disp_tempstr, 21, "PROGRAM %01u   %s", (fun_para_no+1), state_list[manprog[fun_para_no].state]);
				usnprintf(disp_tempstr, 21, "%02u. %10s  %s", (fun_para_no+1), output[fun_para_no].opname, state_list[manoutput[fun_para_no].state]);
				GLCD_WriteString(disp_tempstr, false);

				fun_para_no++;
			}

			// display down arrow key
			goto_screen_map_yz(0, (MIDDLE_PAGE_TOP + 4*GLCD_LINE_HEIGHT + 1));
			GLCD_writesymbol((uint8_t)(96+32), false);
		}

		sub_fun_state = 3;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(edit_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void OutputTest(uint8_t key_code)
{

}

/********************************************************/

/********************************************************/

void alert_status_update(uint8_t index, uint8_t state)
{
	char i=0;
	if (state == 0)
	{
		// clear alert data
		alert[index].logdate[0] = 0;	alert[index].logdate[1] = 0;	alert[index].logdate[2] = 0;
		alert[index].logtime[0] = 0;	alert[index].logtime[1] = 0;	alert[index].logtime[2] = 0;
		alert[index].status = 0;
		for(i=0;i<ALERT_MAX;i++)
		{
			if(alert[i].status == 1)
			{
				break;
			}
			if(i==(ALERT_MAX-1))
			{
				led_control(LED_ALERTS, OFF);		// Turn off the Alerts LED
			}
		}
	}
	else
	{
		led_control(LED_ALERTS, ON);		// Turn on the Alerts LED
		// set alert data
		alert[index].logdate[0] = CurTime.tm_mday;	alert[index].logdate[1] = CurTime.tm_mon;	alert[index].logdate[2] = CurTime.tm_year;
		alert[index].logtime[0] = CurTime.tm_24hr;	alert[index].logtime[1] = CurTime.tm_min;	alert[index].logtime[2] = CurTime.tm_sec;
		alert[index].status = 1;
	}

//	eeprom_write_alert(index);
//	eeprom_read_alert(index);
}

/********************************************************/

/********************************************************/
/*
void ViewLog(uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen - title
		strcpy(disp_tempstr, "VIEW LOG - ");

		if (fun_no == 0)
			strcat(disp_tempstr, "IRRIGATION");
		else if (fun_no == 1)
			strcat(disp_tempstr, "EVENT");
		else if (fun_no == 2)
			strcat(disp_tempstr, "SENSOR");
		else if (fun_no == 3)
			strcat(disp_tempstr, "SYSTEM");

		GLCD_write_titlebar(disp_tempstr);

		z_addr = SCRN_MAP_CUR_Z + 10;

		// Get Current date
		log_from_date.tm_mday = log_to_date.tm_mday = CurTime.tm_mday;
		log_from_date.tm_mon  = log_to_date.tm_mon	= CurTime.tm_mon;
		log_from_date.tm_year = log_to_date.tm_year	= CurTime.tm_year;

	    // Display 'From' date
	    goto_screen_map_yz(3, z_addr);
		usnprintf(disp_tempstr, 13, "FROM        ");
	    GLCD_WriteString(disp_tempstr, false);

	    // Display 'To' date
	    goto_screen_map_yz(3, z_addr + 10);
	    if (dateformat == 0)
			usnprintf(disp_tempstr, 21, "TO          %02u/%02u/%02u", log_to_date.tm_mday, log_to_date.tm_mon, log_to_date.tm_year);
	    else
	    	usnprintf(disp_tempstr, 21, "TO          %02u/%02u/%02u", log_to_date.tm_mon, log_to_date.tm_mday, log_to_date.tm_year);
	    GLCD_WriteString(disp_tempstr, false);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;		tool_type = 2;	    sub_fun_state = 1;

		//init variables for textbox_num / textbox_alphanum
		textbox_char_encoded_trancated = 0;	textbox_size = 8;

		if (dateformat == 0)
			usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", log_from_date.tm_mday, log_from_date.tm_mon, log_from_date.tm_year);
		else
			usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", log_from_date.tm_mon, log_from_date.tm_mday, log_from_date.tm_year);

		fun_line_addr = z_addr;		fun_col_addr = 76;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// date -> from
	{
		if (key_code == kbEnter)
		{
			if (date_strtodec(edit_tempstr, log_to_date, 1) == false)
			{
				sub_fun_state = 1;

			    if (dateformat == 0)
					usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", log_from_date.tm_mday, log_from_date.tm_mon, log_from_date.tm_year);
			    else
			    	usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", log_from_date.tm_mon, log_from_date.tm_mday, log_from_date.tm_year);

			    y_start = 76;	z_start = fun_line_addr;		y_end = 127;	z_end = fun_line_addr+9;
			}
			else
			{
				log_from_date = rd_date;

				// set fun_stage = value for next stage
				// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
				fun_stage = 2;		tool_type = 2;	    sub_fun_state = 1;

				//init variables for textbox_num / textbox_alphanum
				textbox_char_encoded_trancated = 0;	textbox_size = 8;

				if (dateformat == 0)
					usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", log_to_date.tm_mday, log_to_date.tm_mon, log_to_date.tm_year);
				else
					usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", log_to_date.tm_mon, log_to_date.tm_mday, log_to_date.tm_year);

				fun_line_addr += 10;		fun_col_addr = 76;
				y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
			}
		}
	}
	else if (fun_stage == 2)	// date -> to
	{
		if (key_code == kbEnter)
		{
			if (date_strtodec(edit_tempstr, log_from_date, 2) == false)
			{
				sub_fun_state = 1;

			    if (dateformat == 0)
					usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", log_to_date.tm_mday, log_to_date.tm_mon, log_to_date.tm_year);
			    else
			    	usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", log_to_date.tm_mon, log_to_date.tm_mday, log_to_date.tm_year);

			    y_start = 76;	z_start = fun_line_addr;		y_end = 127;	z_end = fun_line_addr+9;
			}
			else
			{
				log_to_date = rd_date;

//				// set fun_stage = value for next stage
//				// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
//				fun_stage = 3;		tool_type = 2;	    sub_fun_state = 1;
//
//				//init variables for textbox_num / textbox_alphanum
//				textbox_char_encoded_trancated = 0;	textbox_size = 8;
//
//				if (dateformat == 0)
//					usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", log_to_date.tm_mday, log_to_date.tm_mon, log_to_date.tm_year);
//				else
//					usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", log_to_date.tm_mon, log_to_date.tm_mday, log_to_date.tm_year);
//
//				fun_line_addr += 10;		fun_col_addr = 76;
//				y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;

				GetFunFlag = 0;
			}
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if (sub_fun_state == 1)	// init
	{
		sub_fun_state = 2;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(disp_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}
*/
//$$NR$$//dated:23Aug15

/********************************************************/

/********************************************************/
uint16_t logfile_rd_start_index, logfile_lastlog_found_ptr = 0xFFFF;
uint16_t logbuff_rd_start_index, logstrsize=0;

void ViewIrrigationLog(uint8_t key_code)
{
uint8_t	temp_opid;		//$$NR$$//dated:23Aug15
uint8_t z_addr, fun_para_no, search_dir, log_para_no;
uint8_t logfile_rd_flag = 0, logfile_return = 0, logsearch_init = 0;
uint8_t log_rd_flag = 0, log_found_flag = 0;	//0-no log found
uint16_t logbuff_curLog_start_ptr;

char *strtemp, *strtempdate, *strtemptime;
static char *strlogdate, *strlogtime;
static char *strlogzonepd, *strlogzonead;
//static char *strlogfv1ad, *strlogfv2ad, *strlogfv3ad, *strlogfv4ad;		//$$NR$$//dated:23Aug15
static char *strlogfvad[4];													//$$NR$$//dated:23Aug15
static uint8_t log_mode, log_progtype, log_zoneprogno, log_zoneno, log_fertiprogno;

	if (fun_stage == 0)
	{
		logfile_rd_start_index = 0;	logfile_rd_flag = 1;
		logsearch_init = 1; 	log_found_flag = 0;		logfile_lastlog_found_ptr = 0xFFFF; 	//log_found_state = 0;

		logbuff_rd_start_index = 0;	search_dir = 1;

		fun_stage = 1;	sub_fun_state = 1;
	}
	else if ((fun_stage == 1) || (fun_stage == 2))
	{
		if (key_code == kbLeft)
		{
			//			search_dir = 0;							//$$NR$$//dated:24Aug15

			//			fun_stage = 1;	sub_fun_state = 1;		//$$NR$$//dated:24Aug15
		}
		else if (key_code == kbRight)
		{
			search_dir = 1;

			fun_stage = 1;	sub_fun_state = 1;
		}
		else if (key_code == kbUp)			// Check if Up Key pressed, scroll up the screen
		{
			fun_stage--;
			if(fun_stage == 0)	fun_stage = 2;

			sub_fun_state = 2;
		}
		else if (key_code == kbDown)	// Check if Down Key pressed, scroll down the screen
		{
			fun_stage++;
			if(fun_stage >= 3)	fun_stage = 1;

			sub_fun_state = 2;
		}
	}
	else if (fun_stage == 3)
	{
	}

	if (key_code == kbBack)				//$$NR$$//dated:24Aug15
//	if (key_code == kbDot)				//$$NR$$//dated:24Aug15
	{
		GetFunFlag = 0;
		return;
	}

	if ((sub_fun_state == 1) ||	(sub_fun_state == 2) ||	(sub_fun_state == 3))	// init screen
	{
		if (sub_fun_state == 1)		// get next/prev log data
		{
			while (1)
			{
				// read log file from usb pendrive
				if (logfile_rd_flag == 1)
				{
					logfile_rd_flag = 0;
					logfile_return = Read_log(logfile_rd_start_index);

					if (logfile_return == 0)
					{
						fun_stage = 3;	sub_fun_state = 3;
						break;
					}
				}

				if (search_dir == 1)	//forward
				{
					log_rd_flag = 2;
				}
				else					//back
				{
					log_rd_flag = 0;

					logbuff_curLog_start_ptr = logfile_lastlog_found_ptr - logfile_rd_start_index;

					while (logbuff_curLog_start_ptr)
					{
						logbuff_curLog_start_ptr--;
						if (logfile_buff[logbuff_curLog_start_ptr] == '\n')
						{
							logbuff_rd_start_index = logbuff_curLog_start_ptr+1;

							log_rd_flag++;
							if (log_rd_flag >= 2)	break;
						}
					}
				}

				if (log_rd_flag >= 2)
				{
					log_para_no = 0;	logstrsize = 0;
					strtemp = strtok((logfile_buff + logbuff_rd_start_index), ",\r\n"); 	log_para_no++;

					while (strtemp != NULL)
					{
						if (log_para_no == 1)
						{
							logbuff_curLog_start_ptr = logbuff_rd_start_index;
							strtempdate = strtemp;
						}
						else if (log_para_no == 2)
							strtemptime = strtemp;
						else if (log_para_no == 3)
						{
							if(*strtemp == '1')
							{
								strlogdate = strtempdate;	strlogtime = strtemptime;
							}
							else
							{
								log_para_no = 0;

								if (search_dir == 1)	//forward
								{
									strtemp = strtok(NULL, "\r\n");

									logbuff_rd_start_index = (uint16_t)(strtemp - logfile_buff);
								}
								else					//back
								{
									log_rd_flag = 0;

//									logbuff_curLog_start_ptr = logfile_lastlog_found_ptr - logfile_rd_start_index;

									while (logbuff_curLog_start_ptr)
									{
										logbuff_curLog_start_ptr--;
										if (logfile_buff[logbuff_curLog_start_ptr] == '\n')
										{
											logbuff_rd_start_index = logbuff_curLog_start_ptr+1;

											log_rd_flag++;
											if (log_rd_flag >= 2)	break;
										}
									}

									if (log_rd_flag < 2)	break;
								}
							}
						}
						else if (log_para_no == 4)
							log_mode = (*strtemp - '0');
						else if (log_para_no == 5)
							log_progtype = *strtemp - 0x30;
						else if (log_para_no == 6)
							log_zoneprogno = *strtemp - 0x30;
						else if (log_para_no == 7)
							log_zoneno = *strtemp - 0x30;
						else if (log_para_no == 8)
							strlogzonepd = strtemp;
						else if (log_para_no == 9)
							strlogzonead = strtemp;
						else if (log_para_no == 10)
							log_fertiprogno = *strtemp - 0x30;
						else if (log_para_no == 11)
							strlogfvad[0] = strtemp;
						else if (log_para_no == 12)
							strlogfvad[1] = strtemp;
						else if (log_para_no == 13)
							strlogfvad[2] = strtemp;
						else if (log_para_no == 14)
						{
							strlogfvad[3] = strtemp;
							strlogdate = strtempdate;	strlogtime = strtemptime;

							log_found_flag = 1;

							logfile_lastlog_found_ptr = logfile_rd_start_index + logbuff_curLog_start_ptr;

//							strtemp = strtok(NULL, ",\r\n");
							logbuff_rd_start_index = (uint16_t)(strtemp - logfile_buff) + strlen(strtemp) + 2;

							sub_fun_state = 2;
							break;
						}

	//					strtemp = strtok(NULL, ",,\r\n"); 	log_para_no++;
						strtemp = strtok(NULL, ",\r\n"); 	log_para_no++;
//						logstrsize = (uint16_t)(strtemp - logfile_buff) + strlen(strtemp);
//						strtemp = strtok((logfile_buff + logstrsize), ",\r\n"); 	log_para_no++;
					}
				}

				if (sub_fun_state > 1)	break;

				// log file size <= Max limit of Log buffer Size
				if (logfile_size <= LOGFILE_BUFF_SIZE)
				{
					if (search_dir == 1)	//forward
					{
						// if no irrig log found
						if ((logsearch_init == 1) && (log_found_flag == 0))
						{
							fun_stage = 3;	sub_fun_state = 3;
							break;
						}
						else
						{
							logfile_rd_flag = 1;	//$$NR$$//dated:08Nov15
						}

						logbuff_rd_start_index = 0;
					}
					else					//back
					{
						logbuff_rd_start_index = logfile_rddata_size;
					}
				}
				else
				{
					if (search_dir == 1)	//forward
					{
						logfile_rd_start_index += logbuff_rd_start_index;

						if (logfile_rd_start_index >= logfile_size)
						{
							// if no irrig log found
							if ((logsearch_init == 1) && (log_found_flag == 0))
							{
								fun_stage = 3;	sub_fun_state = 3;
								break;
							}

							logfile_rd_start_index = 0;
						}

						logbuff_rd_start_index = 0;
					}
					else					//back
					{
						if ((logfile_rd_start_index + logbuff_rd_start_index) == 0)
							logfile_rd_start_index = logfile_size - LOGFILE_BUFF_SIZE;
						else if ((logfile_rd_start_index + logbuff_rd_start_index) <= LOGFILE_BUFF_SIZE)
							logfile_rd_start_index = 0;
						else
							logfile_rd_start_index = (logfile_rd_start_index + logbuff_rd_start_index) - LOGFILE_BUFF_SIZE;

						logbuff_rd_start_index = LOGFILE_BUFF_SIZE - 1;
					}

					logfile_rd_flag = 1;
				}

//				log_rd_flag = 0;
			}
		}

		if (sub_fun_state == 2)		// view log data
		{
			if (fun_stage <= 1)
				fun_para_no = 0;
			else
				fun_para_no = 5;

			GLCD_clear_screen(false);						// clear the lcd display

			// Display Screen - Title
			usnprintf(disp_tempstr, 21, "< IRRIGATION LOGS  >");
			GLCD_write_titlebar(disp_tempstr);

	//		fun_line_addr = SCRN_MAP_CUR_Z;

			for (z_addr=0; z_addr<5; z_addr++)
			{
				goto_screen_map_yz(5, SCRN_MAP_CUR_Z);

				if (fun_para_no == 0)
					usnprintf(disp_tempstr, 21, "%08s    %08s", strlogdate, strlogtime);
				else if (fun_para_no == 1)
					usnprintf(disp_tempstr, 21, "%04s            %04s", log_mode_list[log_mode], log_progtype_list[log_progtype]);
				else if (fun_para_no == 2)
					usnprintf(disp_tempstr, 21, "Zone Prog No %01u      ", log_zoneprogno);
				else if (fun_para_no == 3)
					usnprintf(disp_tempstr, 21, "%010s PD %06s", output[log_zoneno].opname, strlogzonepd);
				else if (fun_para_no == 4)
					usnprintf(disp_tempstr, 21, "           AD %06s", strlogzonead);
				else if (fun_para_no == 5)
					usnprintf(disp_tempstr, 21, "Ferti Prog No %01u     ", log_fertiprogno);
				else if ((fun_para_no >= 6) && (fun_para_no <= 9))	//$$NR$$//dated:23Aug15
				{
					// get opid of fvalve 1 using optype & typeid
					temp_opid = get_output_id(5, (fun_para_no - 5));

					if (temp_opid < OUTPUT_MAX)
						usnprintf(disp_tempstr, 21, "%010s AD %06s", output[temp_opid].opname, strlogfvad[fun_para_no-6]);
					else
						usnprintf(disp_tempstr, 21, "FERT VALVE AD %06s", strlogfvad[fun_para_no-6]);
				}
				//$$NR$$//dated:23Aug15

				GLCD_WriteString(disp_tempstr, false);

				fun_para_no++;
			}

			// display down arrow key
			goto_screen_map_yz(0, (MIDDLE_PAGE_TOP + 4*GLCD_LINE_HEIGHT + 1));
			GLCD_writesymbol((uint8_t)(96+32), false);
		}
		else if (sub_fun_state == 3)		// view no log status
		{
			GLCD_clear_screen(false);						// clear the lcd display

			// Display Screen - Title
			usnprintf(disp_tempstr, 21, "< IRRIGATION LOGS  >");
			GLCD_write_titlebar(disp_tempstr);

		    goto_screen_map_yz(3, SCRN_MAP_CUR_Z+20);
			usnprintf(disp_tempstr, 21, "  No Log Available  ");
		    GLCD_WriteString(disp_tempstr, false);
		}

		sub_fun_state = 4;

		GLCD_MsgLine_Update(0, 0, 126, 63);
	}
}

/********************************************************/

/********************************************************/

void ViewEventLog(uint8_t key_code)
{
//uint8_t y_start, z_start, y_end, z_end;
uint8_t z_addr, fun_para_no, search_dir, log_para_no;
uint8_t logfile_rd_flag = 0, logfile_return = 0, logsearch_init = 0;
uint8_t log_rd_flag = 0, log_found_flag = 0;	//0-no log found
uint16_t logbuff_curLog_start_ptr;

char *strtemp, *strtempdate, *strtemptime;
static char *strlogdate, *strlogtime;
static uint8_t log_outputno, log_outputstatus;

	if (fun_stage == 0)
	{
		logfile_rd_start_index = 0;	logfile_rd_flag = 1;
		logsearch_init = 1; 	log_found_flag = 0;		logfile_lastlog_found_ptr = 0xFFFF; 	//log_found_state = 0;

		logbuff_rd_start_index = 0;	search_dir = 1;

		fun_stage = 1;	sub_fun_state = 1;
	}
	else if ((fun_stage == 1) || (fun_stage == 2))
	{
		if (key_code == kbLeft)
		{
			//			search_dir = 0;							//$$NR$$//dated:24Aug15

			//			fun_stage = 1;	sub_fun_state = 1;		//$$NR$$//dated:24Aug15
		}
		else if (key_code == kbRight)
		{
			search_dir = 1;

			fun_stage = 1;	sub_fun_state = 1;
		}
	}
	else if (fun_stage == 3)
	{
	}

	if (key_code == kbBack)				//$$NR$$//dated:24Aug15
//	if (key_code == kbDot)				//$$NR$$//dated:24Aug15
	{
		GetFunFlag = 0;
		return;
	}

	if ((sub_fun_state == 1) ||	(sub_fun_state == 2) ||	(sub_fun_state == 3))	// init screen
	{
		if (sub_fun_state == 1)		// get next/prev log data
		{
			while (1)
			{
				// read log file from usb pendrive
				if (logfile_rd_flag == 1)
				{
					logfile_rd_flag = 0;
					logfile_return = Read_log(logfile_rd_start_index);

					if (logfile_return == 0)
					{
						fun_stage = 3;	sub_fun_state = 3;
						break;
					}
				}

				if (search_dir == 1)	//forward
				{
					log_rd_flag = 2;
				}
				else					//back
				{
					log_rd_flag = 0;

					logbuff_curLog_start_ptr = logfile_lastlog_found_ptr - logfile_rd_start_index;

					while (logbuff_curLog_start_ptr)
					{
						logbuff_curLog_start_ptr--;
						if (logfile_buff[logbuff_curLog_start_ptr] == '\n')
						{
							logbuff_rd_start_index = logbuff_curLog_start_ptr+1;

							log_rd_flag++;
							if (log_rd_flag >= 2)	break;
						}
					}
				}

				if (log_rd_flag >= 2)
				{
					log_para_no = 0;
					strtemp = strtok((logfile_buff + logbuff_rd_start_index), ",\r\n"); 	log_para_no++;

					while (strtemp != NULL)
					{
						if (log_para_no == 1)
						{
							logbuff_curLog_start_ptr = logbuff_rd_start_index;
							strtempdate = strtemp;
						}
						else if (log_para_no == 2)
							strtemptime = strtemp;
						else if (log_para_no == 3)
						{
							if(*strtemp == '2')
							{
								strlogdate = strtempdate;	strlogtime = strtemptime;
							}
							else
							{
								log_para_no = 0;

								if (search_dir == 1)	//forward
								{
									strtemp = strtok(NULL, "\r\n");

									logbuff_rd_start_index = (uint16_t)(strtemp - logfile_buff);
								}
								else					//back
								{
									log_rd_flag = 0;

//									logbuff_curLog_start_ptr = logfile_lastlog_found_ptr - logfile_rd_start_index;

									while (logbuff_curLog_start_ptr)
									{
										logbuff_curLog_start_ptr--;
										if (logfile_buff[logbuff_curLog_start_ptr] == '\n')
										{
											logbuff_rd_start_index = logbuff_curLog_start_ptr+1;

											log_rd_flag++;
											if (log_rd_flag >= 2)	break;
										}
									}

									if (log_rd_flag < 2)	break;
								}
							}
						}
						else if (log_para_no == 4)
							log_outputno = *strtemp - 0x30;
						else if (log_para_no == 5)
						{
							log_outputstatus = *strtemp - 0x30;
							strlogdate = strtempdate;	strlogtime = strtemptime;

							log_found_flag = 1;

							logfile_lastlog_found_ptr = logfile_rd_start_index + logbuff_curLog_start_ptr;

//							strtemp = strtok(NULL, ",\r\n");
							logbuff_rd_start_index = (uint16_t)(strtemp - logfile_buff) + strlen(strtemp) + 2;

							sub_fun_state = 2;
							break;
						}

	//					strtemp = strtok(NULL, ",,\r\n"); 	log_para_no++;
						strtemp = strtok(NULL, ",\r\n"); 	log_para_no++;
					}
				}

				if (sub_fun_state > 1)	break;

				// log file size <= Max limit of Log buffer Size
				if (logfile_size <= LOGFILE_BUFF_SIZE)
				{
					if (search_dir == 1)	//forward
					{
						// if no irrig log found
						if ((logsearch_init == 1) && (log_found_flag == 0))
						{
							fun_stage = 3;	sub_fun_state = 3;
							break;
						}
						else
						{
							logfile_rd_flag = 1;	//$$NR$$//dated:08Nov15
						}

						logbuff_rd_start_index = 0;
					}
					else					//back
					{
						logbuff_rd_start_index = logfile_rddata_size;
					}
				}
				else
				{
					if (search_dir == 1)	//forward
					{
						logfile_rd_start_index += logbuff_rd_start_index;

						if (logfile_rd_start_index >= logfile_size)
						{
							// if no irrig log found
							if ((logsearch_init == 1) && (log_found_flag == 0))
							{
								fun_stage = 3;	sub_fun_state = 3;
								break;
							}

							logfile_rd_start_index = 0;
						}

						logbuff_rd_start_index = 0;
					}
					else					//back
					{
						if ((logfile_rd_start_index + logbuff_rd_start_index) == 0)
							logfile_rd_start_index = logfile_size - LOGFILE_BUFF_SIZE;
						else if ((logfile_rd_start_index + logbuff_rd_start_index) <= LOGFILE_BUFF_SIZE)
							logfile_rd_start_index = 0;
						else
							logfile_rd_start_index = (logfile_rd_start_index + logbuff_rd_start_index) - LOGFILE_BUFF_SIZE;

						logbuff_rd_start_index = LOGFILE_BUFF_SIZE - 1;
					}

					logfile_rd_flag = 1;
				}
			}
		}

		if (sub_fun_state == 2)		// view log data
		{
			fun_para_no = 0;

			GLCD_clear_screen(false);						// clear the lcd display

			// Display Screen - Title
			usnprintf(disp_tempstr, 21, "<    EVENT LOGS    >");
			GLCD_write_titlebar(disp_tempstr);

	//		fun_line_addr = SCRN_MAP_CUR_Z;

			for (z_addr=0; z_addr<5; z_addr++)
			{
				goto_screen_map_yz(5, SCRN_MAP_CUR_Z);

				if (fun_para_no == 0)
					usnprintf(disp_tempstr, 21, "%08s    %08s", strlogdate, strlogtime);
				else if (fun_para_no == 1)
					usnprintf(disp_tempstr, 21, "OP NO %02u- %010s", log_outputno, output[log_outputno].opname);
				else if (fun_para_no == 2)
					usnprintf(disp_tempstr, 21, "Output Status %03s", state_list[log_outputstatus]);

				GLCD_WriteString(disp_tempstr, false);

				fun_para_no++;
			}
		}
		else if (sub_fun_state == 3)		// view no log status
		{
			GLCD_clear_screen(false);						// clear the lcd display

			// Display Screen - Title
			usnprintf(disp_tempstr, 21, "<    EVENT LOGS    >");
			GLCD_write_titlebar(disp_tempstr);

			goto_screen_map_yz(3, SCRN_MAP_CUR_Z+20);
			usnprintf(disp_tempstr, 21, "  No Log Available  ");
			GLCD_WriteString(disp_tempstr, false);
		}

		sub_fun_state = 4;

		GLCD_MsgLine_Update(0, 0, 126, 63);
	}
}

/********************************************************/

/********************************************************/

void ViewSensorLog(uint8_t key_code)
{
//uint8_t y_start, z_start, y_end, z_end;
uint8_t z_addr, fun_para_no, search_dir, log_para_no;
uint8_t logfile_rd_flag = 0, logfile_return = 0, logsearch_init = 0;
uint8_t log_rd_flag = 0, log_found_flag = 0;	//0-no log found
uint16_t logbuff_curLog_start_ptr;

char *strtemp, *strtempdate, *strtemptime;
static char *strlogdate, *strlogtime;
static uint8_t log_sensorno, log_sensorval;

	if (fun_stage == 0)
	{
		logfile_rd_start_index = 0;	logfile_rd_flag = 1;
		logsearch_init = 1; 	log_found_flag = 0;		logfile_lastlog_found_ptr = 0xFFFF; 	//log_found_state = 0;

		logbuff_rd_start_index = 0;	search_dir = 1;

		fun_stage = 1;	sub_fun_state = 1;
	}
	else if ((fun_stage == 1) || (fun_stage == 2))
	{
		if (key_code == kbLeft)
		{
			//			search_dir = 0;							//$$NR$$//dated:24Aug15

			//			fun_stage = 1;	sub_fun_state = 1;		//$$NR$$//dated:24Aug15
		}
		else if (key_code == kbRight)
		{
			search_dir = 1;

			fun_stage = 1;	sub_fun_state = 1;
		}
	}
	else if (fun_stage == 3)
	{
	}

	if (key_code == kbBack)				//$$NR$$//dated:24Aug15
//	if (key_code == kbDot)				//$$NR$$//dated:24Aug15
	{
		GetFunFlag = 0;
		return;
	}

	if ((sub_fun_state == 1) ||	(sub_fun_state == 2) ||	(sub_fun_state == 3))	// init screen
	{
		if (sub_fun_state == 1)		// get next/prev log data
		{
			while (1)
			{
				// read log file from usb pendrive
				if (logfile_rd_flag == 1)
				{
					logfile_rd_flag = 0;
					logfile_return = Read_log(logfile_rd_start_index);

					if (logfile_return == 0)
					{
						fun_stage = 3;	sub_fun_state = 3;
						break;
					}
				}

				if (search_dir == 1)	//forward
				{
					log_rd_flag = 2;
				}
				else					//back
				{
					log_rd_flag = 0;

					logbuff_curLog_start_ptr = logfile_lastlog_found_ptr - logfile_rd_start_index;

					while (logbuff_curLog_start_ptr)
					{
						logbuff_curLog_start_ptr--;
						if (logfile_buff[logbuff_curLog_start_ptr] == '\n')
						{
							logbuff_rd_start_index = logbuff_curLog_start_ptr+1;

							log_rd_flag++;
							if (log_rd_flag >= 2)	break;
						}
					}
				}

				if (log_rd_flag >= 2)
				{
					log_para_no = 0;
					strtemp = strtok((logfile_buff + logbuff_rd_start_index), ",\r\n"); 	log_para_no++;

					while (strtemp != NULL)
					{
						if (log_para_no == 1)
						{
							logbuff_curLog_start_ptr = logbuff_rd_start_index;
							strtempdate = strtemp;
						}
						else if (log_para_no == 2)
							strtemptime = strtemp;
						else if (log_para_no == 3)
						{
							if(*strtemp == '3')
							{
								strlogdate = strtempdate;	strlogtime = strtemptime;
							}
							else
							{
								log_para_no = 0;

								if (search_dir == 1)	//forward
								{
									strtemp = strtok(NULL, "\r\n");

									logbuff_rd_start_index = (uint16_t)(strtemp - logfile_buff);
								}
								else					//back
								{
									log_rd_flag = 0;

//									logbuff_curLog_start_ptr = logfile_lastlog_found_ptr - logfile_rd_start_index;

									while (logbuff_curLog_start_ptr)
									{
										logbuff_curLog_start_ptr--;
										if (logfile_buff[logbuff_curLog_start_ptr] == '\n')
										{
											logbuff_rd_start_index = logbuff_curLog_start_ptr+1;

											log_rd_flag++;
											if (log_rd_flag >= 2)	break;
										}
									}

									if (log_rd_flag < 2)	break;
								}
							}
						}
						else if (log_para_no == 4)
							log_sensorno = *strtemp - 0x30;
						else if (log_para_no == 5)
						{
							log_sensorval = *strtemp - 0x30;
							strlogdate = strtempdate;	strlogtime = strtemptime;

							log_found_flag = 1;

							logfile_lastlog_found_ptr = logfile_rd_start_index + logbuff_curLog_start_ptr;

//							strtemp = strtok(NULL, ",\r\n");
							logbuff_rd_start_index = (uint16_t)(strtemp - logfile_buff) + strlen(strtemp) + 2;

							sub_fun_state = 2;
							break;
						}

	//					strtemp = strtok(NULL, ",,\r\n"); 	log_para_no++;
						strtemp = strtok(NULL, ",\r\n"); 	log_para_no++;
					}
				}

				if (sub_fun_state > 1)	break;

				// log file size <= Max limit of Log buffer Size
				if (logfile_size <= LOGFILE_BUFF_SIZE)
				{
					if (search_dir == 1)	//forward
					{
						// if no irrig log found
						if ((logsearch_init == 1) && (log_found_flag == 0))
						{
							fun_stage = 3;	sub_fun_state = 3;
							break;
						}

						logbuff_rd_start_index = 0;
					}
					else					//back
					{
						logbuff_rd_start_index = logfile_rddata_size;
					}
				}
				else
				{
					if (search_dir == 1)	//forward
					{
						logfile_rd_start_index += logbuff_rd_start_index;

						if (logfile_rd_start_index >= logfile_size)
						{
							// if no irrig log found
							if ((logsearch_init == 1) && (log_found_flag == 0))
							{
								fun_stage = 3;	sub_fun_state = 3;
								break;
							}

							logfile_rd_start_index = 0;
						}

						logbuff_rd_start_index = 0;
					}
					else					//back
					{
						if ((logfile_rd_start_index + logbuff_rd_start_index) == 0)
							logfile_rd_start_index = logfile_size - LOGFILE_BUFF_SIZE;
						else if ((logfile_rd_start_index + logbuff_rd_start_index) <= LOGFILE_BUFF_SIZE)
							logfile_rd_start_index = 0;
						else
							logfile_rd_start_index = (logfile_rd_start_index + logbuff_rd_start_index) - LOGFILE_BUFF_SIZE;

						logbuff_rd_start_index = LOGFILE_BUFF_SIZE - 1;
					}

					logfile_rd_flag = 1;
				}
			}
		}

		if (sub_fun_state == 2)		// view log data
		{
			fun_para_no = 0;

			GLCD_clear_screen(false);						// clear the lcd display

			// Display Screen - Title
			usnprintf(disp_tempstr, 21, "<   SENSOR LOGS    >");
			GLCD_write_titlebar(disp_tempstr);

	//		fun_line_addr = SCRN_MAP_CUR_Z;

			for (z_addr=0; z_addr<5; z_addr++)
			{
				goto_screen_map_yz(5, SCRN_MAP_CUR_Z);

				if (fun_para_no == 0)
					usnprintf(disp_tempstr, 21, "%08s    %08s", strlogdate, strlogtime);
				else if (fun_para_no == 1)
					usnprintf(disp_tempstr, 21, "Sensor ID     %02u", log_sensorno);
				else if (fun_para_no == 2)
					usnprintf(disp_tempstr, 21, "Sensor Value  %06u", log_sensorval);

				GLCD_WriteString(disp_tempstr, false);

				fun_para_no++;
			}
		}
		else if (sub_fun_state == 3)		// view no log status
		{
			GLCD_clear_screen(false);						// clear the lcd display

			// Display Screen - Title
			usnprintf(disp_tempstr, 21, "<   SENSOR LOGS    >");
			GLCD_write_titlebar(disp_tempstr);

			goto_screen_map_yz(3, SCRN_MAP_CUR_Z+20);
			usnprintf(disp_tempstr, 21, "  No Log Available  ");
			GLCD_WriteString(disp_tempstr, false);
		}

		sub_fun_state = 4;

		GLCD_MsgLine_Update(0, 0, 126, 63);
	}
}

/********************************************************/

/********************************************************/

void ViewSystemLog(uint8_t key_code)
{
//uint8_t y_start, z_start, y_end, z_end;
uint8_t z_addr, fun_para_no, search_dir, log_para_no;
uint8_t logfile_rd_flag = 0, logfile_return = 0, logsearch_init = 0;
uint8_t log_rd_flag = 0, log_found_flag = 0;	//0-no log found
uint16_t logbuff_curLog_start_ptr;

char *strtemp, *strtempdate, *strtemptime;
static char *strlogdate, *strlogtime;
static char *strlogmenuindex, *strlogparaname;
static uint8_t log_mode, log_user;

	if (fun_stage == 0)
	{
		logfile_rd_start_index = 0;	logfile_rd_flag = 1;
		logsearch_init = 1; 	log_found_flag = 0;		logfile_lastlog_found_ptr = 0xFFFF; 	//log_found_state = 0;

		logbuff_rd_start_index = 0;	search_dir = 1;

		fun_stage = 1;	sub_fun_state = 1;
	}
	else if ((fun_stage == 1) || (fun_stage == 2))
	{
		if (key_code == kbLeft)
		{
			//			search_dir = 0;							//$$NR$$//dated:24Aug15

			//			fun_stage = 1;	sub_fun_state = 1;		//$$NR$$//dated:24Aug15
		}
		else if (key_code == kbRight)
		{
			search_dir = 1;

			fun_stage = 1;	sub_fun_state = 1;
		}
		else if (key_code == kbUp)			// Check if Up Key pressed, scroll up the screen
		{
			fun_stage--;
			if(fun_stage == 0)	fun_stage = 2;

			sub_fun_state = 2;
		}
		else if (key_code == kbDown)	// Check if Down Key pressed, scroll down the screen
		{
			fun_stage++;
			if(fun_stage >= 3)	fun_stage = 1;

			sub_fun_state = 2;
		}
	}
	else if (fun_stage == 3)
	{
	}

	if (key_code == kbBack)				//$$NR$$//dated:24Aug15
//	if (key_code == kbDot)				//$$NR$$//dated:24Aug15
	{
		GetFunFlag = 0;
		return;
	}

	if ((sub_fun_state == 1) ||	(sub_fun_state == 2) ||	(sub_fun_state == 3))	// init screen
	{
		if (sub_fun_state == 1)		// get next/prev log data
		{
			while (1)
			{
				// read log file from usb pendrive
				if (logfile_rd_flag == 1)
				{
					logfile_rd_flag = 0;
					logfile_return = Read_log(logfile_rd_start_index);

					if (logfile_return == 0)
					{
						fun_stage = 3;	sub_fun_state = 3;
						break;
					}
				}

				if (search_dir == 1)	//forward
				{
					log_rd_flag = 2;
				}
				else					//back
				{
					log_rd_flag = 0;

					logbuff_curLog_start_ptr = logfile_lastlog_found_ptr - logfile_rd_start_index;

					while (logbuff_curLog_start_ptr)
					{
						logbuff_curLog_start_ptr--;
						if (logfile_buff[logbuff_curLog_start_ptr] == '\n')
						{
							logbuff_rd_start_index = logbuff_curLog_start_ptr+1;

							log_rd_flag++;
							if (log_rd_flag >= 2)	break;
						}
					}
				}

				if (log_rd_flag >= 2)
				{
					log_para_no = 0;
					strtemp = strtok((logfile_buff + logbuff_rd_start_index), ",\r\n"); 	log_para_no++;

					while (strtemp != NULL)
					{
						if (log_para_no == 1)
						{
							logbuff_curLog_start_ptr = logbuff_rd_start_index;
							strtempdate = strtemp;
						}
						else if (log_para_no == 2)
							strtemptime = strtemp;
						else if (log_para_no == 3)
						{
							if(*strtemp == '4')
							{
								strlogdate = strtempdate;	strlogtime = strtemptime;
							}
							else
							{
								log_para_no = 0;

								if (search_dir == 1)	//forward
								{
									strtemp = strtok(NULL, "\r\n");

									logbuff_rd_start_index = (uint16_t)(strtemp - logfile_buff);
								}
								else					//back
								{
									log_rd_flag = 0;

//									logbuff_curLog_start_ptr = logfile_lastlog_found_ptr - logfile_rd_start_index;

									while (logbuff_curLog_start_ptr)
									{
										logbuff_curLog_start_ptr--;
										if (logfile_buff[logbuff_curLog_start_ptr] == '\n')
										{
											logbuff_rd_start_index = logbuff_curLog_start_ptr+1;

											log_rd_flag++;
											if (log_rd_flag >= 2)	break;
										}
									}

									if (log_rd_flag < 2)	break;
								}
							}
						}
						else if (log_para_no == 4)
							log_user = *strtemp - 0x30;
						else if (log_para_no == 5)
							log_mode = *strtemp - 0x30;
						else if (log_para_no == 6)
							strlogmenuindex = strtemp;
						else if (log_para_no == 7)
						{
							strlogparaname = strtemp;
							strlogdate = strtempdate;	strlogtime = strtemptime;

							log_found_flag = 1;

							logfile_lastlog_found_ptr = logfile_rd_start_index + logbuff_curLog_start_ptr;

//							strtemp = strtok(NULL, ",\r\n");
							logbuff_rd_start_index = (uint16_t)(strtemp - logfile_buff) + strlen(strtemp) + 2;

							sub_fun_state = 2;
							break;
						}

	//					strtemp = strtok(NULL, ",,\r\n"); 	log_para_no++;
						strtemp = strtok(NULL, ",\r\n"); 	log_para_no++;
					}
				}

				if (sub_fun_state > 1)	break;

				// log file size <= Max limit of Log buffer Size
				if (logfile_size <= LOGFILE_BUFF_SIZE)
				{
					if (search_dir == 1)	//forward
					{
						// if no irrig log found
						if ((logsearch_init == 1) && (log_found_flag == 0))
						{
							fun_stage = 3;	sub_fun_state = 3;
							break;
						}

						logbuff_rd_start_index = 0;
					}
					else					//back
					{
						logbuff_rd_start_index = logfile_rddata_size;
					}
				}
				else
				{
					if (search_dir == 1)	//forward
					{
//						logfile_rd_start_index += logfile_rddata_size;
						logfile_rd_start_index += logbuff_rd_start_index;

						if (logfile_rd_start_index >= logfile_size)
						{
							// if no irrig log found
							if ((logsearch_init == 1) && (log_found_flag == 0))
							{
								fun_stage = 3;	sub_fun_state = 3;
								break;
							}

							logfile_rd_start_index = 0;
						}

						logbuff_rd_start_index = 0;
					}
					else					//back
					{
						if ((logfile_rd_start_index + logbuff_rd_start_index) == 0)
							logfile_rd_start_index = logfile_size - LOGFILE_BUFF_SIZE;
						else if ((logfile_rd_start_index + logbuff_rd_start_index) <= LOGFILE_BUFF_SIZE)
							logfile_rd_start_index = 0;
						else
							logfile_rd_start_index = (logfile_rd_start_index + logbuff_rd_start_index) - LOGFILE_BUFF_SIZE;

						logbuff_rd_start_index = LOGFILE_BUFF_SIZE - 1;
					}

					logfile_rd_flag = 1;
				}

//				log_rd_flag = 0;
			}
		}

		if (sub_fun_state == 2)		// view log data
		{
			fun_para_no = 0;

			GLCD_clear_screen(false);						// clear the lcd display

			// Display Screen - Title
			usnprintf(disp_tempstr, 21, "<    SYSTEM LOGS   >");
			GLCD_write_titlebar(disp_tempstr);

	//		fun_line_addr = SCRN_MAP_CUR_Z;

			for (z_addr=0; z_addr<5; z_addr++)
			{
				goto_screen_map_yz(5, SCRN_MAP_CUR_Z);

				if (fun_para_no == 0)
					usnprintf(disp_tempstr, 21, "%08s    %08s", strlogdate, strlogtime);
				else if (fun_para_no == 1)
					usnprintf(disp_tempstr, 21, "%08s  %010s", loginuser_list[log_user], log_wireless_mode_list[log_mode]);
				else if (fun_para_no == 2)
					usnprintf(disp_tempstr, 21, "%s ", strlogmenuindex);
				else if (fun_para_no == 3)
					usnprintf(disp_tempstr, 21, "%s ", strlogparaname);

				GLCD_WriteString(disp_tempstr, false);

				fun_para_no++;
			}

			// display down arrow key
			goto_screen_map_yz(0, (MIDDLE_PAGE_TOP + 4*GLCD_LINE_HEIGHT + 1));
			GLCD_writesymbol((uint8_t)(96+32), false);
		}
		else if (sub_fun_state == 3)		// view no log status
		{
			GLCD_clear_screen(false);						// clear the lcd display

			// Display Screen - Title
			usnprintf(disp_tempstr, 21, "<   SYSTEM LOGS    >");
			GLCD_write_titlebar(disp_tempstr);

			goto_screen_map_yz(3, SCRN_MAP_CUR_Z+20);
			usnprintf(disp_tempstr, 21, "  No Log Available  ");
			GLCD_WriteString(disp_tempstr, false);
		}

		sub_fun_state = 4;

		GLCD_MsgLine_Update(0, 0, 126, 63);
	}
}
//$$NR$$//dated:23Aug15

/********************************************************/

/********************************************************/

void DiagnosticAlerts(uint8_t fun_no, uint8_t key_code)
{
uint8_t z_addr, i=0;

//	if (tool_type == 1)
//		key_code = select_list_option(key_code);
//	else if (tool_type == 2)
//		key_code = edit_text_numeric(key_code);
//	else if (tool_type == 3)
//		key_code = edit_text_alphanumeric(key_code);
//
//	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		if (fun_no == 0)
		{
			fun_stage = 1;
			usnprintf(edit_tempstr, 21, "All Alerts");
		}
		else
		{
			// get index of menu option
			i = (uint8_t)(Menu_Index % 0x10);
			i = menutree_menu_base_index + (i - 1);
			usnprintf(edit_tempstr, 21, menu_tree[i].MenuLabel);

			for (i=20; i>0; i--)
			{
				if (edit_tempstr[i] == ' ')
					edit_tempstr[i] = 0;
				else
					break;
			}
		}
	}

	if (fun_stage == 0)
	{
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen - Title
		GLCD_write_titlebar("ALERTS");
		z_addr = SCRN_MAP_CUR_Z;

	    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
		usnprintf(disp_tempstr, 21, "Alert %s", edit_tempstr);
	    GLCD_WriteString(disp_tempstr, false);

		i = fun_no - 1;

		goto_screen_map_yz(3, z_addr + 10);
	    if (dateformat == 0)
			usnprintf(disp_tempstr, 21, "DATE  %02u/%02u/%02u", alert[i].logdate[0], alert[i].logdate[1], alert[i].logdate[2]);
	    else
	    	usnprintf(disp_tempstr, 21, "DATE  %02u/%02u/%02u", alert[i].logdate[1], alert[i].logdate[0], alert[i].logdate[2]);
	    GLCD_WriteString(disp_tempstr, false);

	    // set time value of display
		set_disp_time(alert[i].logtime, true);

	    // Display date and time of selected source
	    goto_screen_map_yz(3, z_addr + 20);
    	usnprintf(disp_tempstr, 18, "TIME  %02u:%02u:%02u", disp_time.tm_hour, alert[i].logtime[1], alert[i].logtime[2]);
	    if (disp_clockformat == 0)	strcat(disp_tempstr, (disp_time.tm_slot ? " PM":" AM"));
	    GLCD_WriteString(disp_tempstr, false);

//	    fun_line_addr = z_addr;
		fun_stage = 1;

		GLCD_Screen_Update();
	}
	else if (fun_stage == 1)
	{
		if (((fun_no == 0) && (key_code == kbEnter)) || ((fun_no > 0) && (key_code == kbClear)))
		{
			GLCD_clear_screen(false);						// clear the lcd display

			// Display Screen - Title
			GLCD_write_titlebar("ALERTS");
			z_addr = SCRN_MAP_CUR_Z + 10;

		    goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
		    GLCD_WriteString("Do You Want to Clear", false);
		    goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
			usnprintf(disp_tempstr, 21, "   %s?", edit_tempstr);
		    GLCD_WriteString(disp_tempstr, false);

		    fun_line_addr = z_addr;
			fun_stage = 2;

			GLCD_Screen_Update();
		}
	}
	else if (fun_stage == 2)
	{
		if (key_code == kbEnter)
		{
		    goto_screen_map_yz(3, fun_line_addr);
		    GLCD_WriteString("   System Clearing  ", false);
		    goto_screen_map_yz(3, fun_line_addr+10);
			usnprintf(disp_tempstr, 21, "   %s...", edit_tempstr);
		    GLCD_WriteString(disp_tempstr, false);

		    GLCD_MsgLine_Update(0, fun_line_addr, 126, fun_line_addr + 20);

			if (fun_no == 0)
			{
				for(i=0; i<ALERT_MAX; i++)
				{
					// clear alert data
					alert_status_update(i, 0);
				}

				menuoptions_selected &= 0x03;
				menuoptions_selected_count = 1;
			}
			else
			{
				i = fun_no - 1;

				// clear alert data
				alert_status_update(i, 0);

				menuoptions_selected &= ~(0x01 << (i+2));
				menuoptions_selected_count--;

				Menu_Index = (Menu_Index & 0xFFFFFFF0) + 1;
				menu_cursor_pos = 0;	menutree_menu_base_index = 0xFF;
			}

		    //delay_sec(1);
			Task_sleep(2000);

			GetFunFlag = 0;
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}
}



//$$NR$$//dated:01Nov15

/********************************************************/

/********************************************************/
/*
void Test(uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		usnprintf(disp_tempstr, 21, "ZONE PROG%d- DETAILS", (prog_index+1));
		GLCD_write_titlebar(disp_tempstr);

		z_addr = SCRN_MAP_CUR_Z+10;

		goto_screen_map_yz(3, SCRN_MAP_CUR_Z+10);
		strcpy(disp_tempstr, "TYPE    ");
		GLCD_WriteString(disp_tempstr, false);

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 2;

		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
		listbox_type = 0;	listbox_index = zoneprog[prog_index].type;		listbox_size = 2;    listbox_selected = 0;
		listbox = &progtype_list;
		strcpy(edit_tempstr, (*listbox)[listbox_index]);

		fun_col_addr = 52;		fun_line_addr = z_addr;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	//
	{
//		 = listbox_index;

		// set fun_stage = value for next stage
		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		fun_stage = 2;	    tool_type = 2;		sub_fun_state = 1;

		textbox_char_encoded_trancated = 0;	textbox_size = 5;
		usnprintf(edit_tempstr, 6, "%02u:%02u", rd_time.tm_hour, rd_time.tm_min);

		textbox_char_encoded_trancated = 4;			textbox_size = 5;
		usnprintf(edit_tempstr, 6, "%05d", zoneprog[prog_index].zone[fun_result1].startflow);
		strcpy(edit_data_str, edit_tempstr);

		SCRN_MAP_CUR_Z = fun_line_addr+10;

		fun_line_addr += 20;		fun_col_addr = 70;
//		y_start = fun_col_addr;		z_start = fun_line_addr-20;		y_end = 126;	z_end = fun_line_addr+9;
	}


	if (key_code == kbBack)
	{
		GetFunFlag = 0;
	}

	if ((sub_fun_state == 1) || (sub_fun_state == 2))	// init screen / line
	{
		if (sub_fun_state == 1)	// init screen
		{
			SCRN_MAP_CUR_Z = MIDDLE_PAGE_TOP+10;

			goto_screen_map_yz(70, SCRN_MAP_CUR_Z);
			usnprintf(disp_tempstr, 6, "%05d", );
			GLCD_WriteString(disp_tempstr, false);

			goto_screen_map_yz(70, SCRN_MAP_CUR_Z);
			usnprintf(disp_tempstr, 6, "%05d", );
			GLCD_WriteString(disp_tempstr, false);
		}

		sub_fun_state = 3;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(edit_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}
*/

/********************************************************/

/********************************************************/

void Keypad_Test(uint8_t key_code)
{
uint8_t z_addr;
static uint8_t esckey_press_cnt = 0;
uint8_t y_start, z_start, y_end, z_end;

//	if (tool_type == 1)
//		key_code = select_list_option(key_code);
//	else if (tool_type == 2)
//		key_code = edit_text_numeric(key_code);
//	else if (tool_type == 3)
//		key_code = edit_text_alphanumeric(key_code);
//
//	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		usnprintf(disp_tempstr, 21, "KEYPAD TEST");
		GLCD_write_titlebar(disp_tempstr);

		esckey_press_cnt = 0;
		strcpy(disp_tempstr, "                    ");

		z_addr = SCRN_MAP_CUR_Z+10;
		fun_stage = 1;		sub_fun_state = 2;

		fun_col_addr = 2;		fun_line_addr = z_addr;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// disp
	{
		// check Key Codes
		switch(key_code)
		{
			case kbVirtualTest:
				strcpy(disp_tempstr, "                    ");
				break;

			case kbWaterOnOff:		// System Operating Keys
				strcpy(disp_tempstr, "    WaterOnOff      ");
				break;

			case kbManual:
				strcpy(disp_tempstr, "      Manual        ");
				break;

			case kbProgram:
				strcpy(disp_tempstr, "      Program       ");
				break;

			case kbLogs:
				strcpy(disp_tempstr, "       Logs         ");
				break;

			case kbDiagnostic:
				strcpy(disp_tempstr, "     Diagnostic     ");
				break;

			case kbUp:				// Arrow Keys
				strcpy(disp_tempstr, "         Up         ");
				break;

			case kbDown:
				strcpy(disp_tempstr, "        Down        ");
				break;

			case kbLeft:
//			case kbBack:
//			case kbEsc:
				strcpy(disp_tempstr, "    Left / Back     ");
				break;

			case kbRight:
//			case kbNext:
				strcpy(disp_tempstr, "    Right / Next    ");
				break;

			case kbEnter:
//			case kbSelect:
				strcpy(disp_tempstr, "   Enter / Select   ");
				break;

			case kbSetup:			// System Operating Keys
				strcpy(disp_tempstr, "       Setup        ");
				break;

			case kbHome:
				strcpy(disp_tempstr, "        Home        ");
				break;

			case 0x30:		//kbNum0:			// Num keys
			case 0x31:		//kbNum1:
			case 0x32:		//kbNum2:
			case 0x33:		//kbNum3:
			case 0x34:		//kbNum4:
			case 0x35:		//kbNum5:
			case 0x36:		//kbNum6:
			case 0x37:		//kbNum7:
			case 0x38:		//kbNum8:
			case 0x39:		//kbNum9:
				strcpy(disp_tempstr, "                    ");
				disp_tempstr[9] = key_code;
				break;

			case kbDot:
				strcpy(disp_tempstr, "         .          ");
				break;

			case kbClear:
				strcpy(disp_tempstr, "       Clear        ");
				break;
		}

		if (key_code != 0)	sub_fun_state = 2;

//		fun_col_addr = 2;		fun_line_addr = z_addr;
		y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+19;
	}


	if (key_code == kbBack)
	{
		esckey_press_cnt++;
		if (esckey_press_cnt >= 2)	GetFunFlag = 0;
	}
	else
		esckey_press_cnt = 0;

	if ((sub_fun_state == 1) || (sub_fun_state == 2))	// init screen / line
	{
		goto_screen_map_yz(fun_col_addr, fun_line_addr);
		GLCD_WriteString(disp_tempstr, false);

		if (key_code != kbVirtualTest)
			strcpy(disp_tempstr, "    Key Pressed     ");

		goto_screen_map_yz(fun_col_addr, fun_line_addr+10);
		GLCD_WriteString(disp_tempstr, false);

		sub_fun_state = 3;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);
		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void Display_Test(uint8_t key_code)
{
uint8_t y_addr, z_addr;
uint8_t y_start, z_start, y_end, z_end;

//	if (tool_type == 1)
//		key_code = select_list_option(key_code);
//	else if (tool_type == 2)
//		key_code = edit_text_numeric(key_code);
//	else if (tool_type == 3)
//		key_code = edit_text_alphanumeric(key_code);
//
//	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		GLCD_clear_screen(true);						// clear the lcd display

		// Display Screen

		z_addr = 0;
		fun_stage = 1;		sub_fun_state = 1;

		fun_col_addr = 0;		fun_line_addr = z_addr;
		y_start = 0;	z_start = 0;		y_end = 127;	z_end = 63;
	}

//	if (key_code == kbBack)
//	{
//		GetFunFlag = 0;
//	}

	if ((sub_fun_state == 1) || (sub_fun_state == 2))	// init screen / line
	{
		sub_fun_state = 3;

		for(z_addr = 0; z_addr < 3; z_addr++)
		{
			goto_screen_map_yz(fun_col_addr, fun_line_addr);
			GLCD_fill_screen(0xFF);							// turn on all pixel
			GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);

		    Task_sleep(1000);			// delay of 10 seconds

			goto_screen_map_yz(fun_col_addr, fun_line_addr);
			GLCD_clear_screen(true);						// clear the lcd display
			GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);

		    Task_sleep(500);			// delay of 10 seconds
		}

		// turn on pixels rowwise and column wise with 100 msec delay
		for(z_addr = 0; z_addr < 64; z_addr++)
		{
			for(y_addr = 0; y_addr < 128; y_addr++)
			{
				Draw_Point(y_addr, z_addr, 0x01);
	    	    Task_sleep(5);			// delay of 10 msec
			}
		}

		GetFunFlag = 0;
	}
}

/********************************************************/

/********************************************************/

void Buzzer_Test(uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;

//	if (tool_type == 1)
//		key_code = select_list_option(key_code);
//	else if (tool_type == 2)
//		key_code = edit_text_numeric(key_code);
//	else if (tool_type == 3)
//		key_code = edit_text_alphanumeric(key_code);
//
//	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen
		usnprintf(disp_tempstr, 21, "BUZZER TEST");
		GLCD_write_titlebar(disp_tempstr);

		strcpy(disp_tempstr, "  Buzzer bipping..  ");

		z_addr = SCRN_MAP_CUR_Z+20;
		fun_stage = 1;		sub_fun_state = 2;

		fun_col_addr = 2;		fun_line_addr = z_addr;
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}

//	if (key_code == kbBack)
//	{
//		GetFunFlag = 0;
//	}

	if ((sub_fun_state == 1) || (sub_fun_state == 2))	// init screen / line
	{
		goto_screen_map_yz(fun_col_addr, fun_line_addr);
		GLCD_WriteString(disp_tempstr, false);

		sub_fun_state = 3;

//		goto_screen_map_yz(fun_col_addr, fun_line_addr);
		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);

		// beep buzzer for 5 second
		Beep_Buzzer(5);

		GetFunFlag = 0;
	}
}

/********************************************************/

/********************************************************/

void Output_Test(uint8_t key_code)
{
uint8_t z_addr, fun_para_no;
uint8_t y_start, z_start, y_end, z_end;

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen - Title
		usnprintf(disp_tempstr, 21, "OUTPUT TEST");
		GLCD_write_titlebar(disp_tempstr);

		z_addr = SCRN_MAP_CUR_Z + 20;

//		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
//		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
		GLCD_MsgLine_Update(0, 0, 126, 63);

		// set fun_stage = value for next stage
		fun_stage = 1;		sub_fun_state = 2;

		fun_col_addr = 2;		fun_line_addr = z_addr;
		y_start = 0;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+10;
	}

//    if (key_code == kbBack)
//    {
//        GetFunFlag = 0;
//    }

	if ((sub_fun_state == 1) || (sub_fun_state == 2))		// init screen
	{
		sub_fun_state = 3;			key_code = 0;

		for (fun_para_no = 0; fun_para_no < OUTPUT_MAX; fun_para_no++)
		{
//			OP_Update((fun_para_no+1), 0, 0);		//on
			OP_Update((fun_para_no+1), 0,0);			//on

			goto_screen_map_yz(5, fun_line_addr);
			usnprintf(disp_tempstr, 21, " %10s  %s", output[fun_para_no].opname, state_list[output_state[fun_para_no]]);
			GLCD_WriteString(disp_tempstr, false);

			GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);

    	    Task_sleep(5000);			// delay of 5 sec

//    	    OP_Update((fun_para_no+1), (~0), 0);	//off
    	    OP_Update((fun_para_no+1), (~0),0);		//off
		}

		GetFunFlag = 0;
	}
}

/********************************************************/

/********************************************************/

void Input_Test(uint8_t key_code)
{
uint8_t z_addr;//, fun_para_no;
uint8_t y_start, z_start, y_end, z_end;

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen - Title
		usnprintf(disp_tempstr, 21, "INPUT TEST");
		GLCD_write_titlebar(disp_tempstr);

		// set fun_stage = value for next stage
		fun_stage = 1;	    sub_fun_state = 1;

		fun_col_addr = 108; 		fun_line_addr = SCRN_MAP_CUR_Z;

		for (z_addr=0; z_addr<4; z_addr++)
		{
			goto_screen_map_yz(5, SCRN_MAP_CUR_Z);

			if (z_addr <= 2)
				usnprintf(disp_tempstr, 21, "%01u. %13s    ", (z_addr+1), ipname_pump_list[input[z_addr].iptype], state_list[input_state[z_addr]]);
			else
				usnprintf(disp_tempstr, 21, "%01u. %13s    ", (z_addr+1), ipname_fmeter_list[input[z_addr].iptype], state_list[input_state[z_addr]]);

			GLCD_WriteString(disp_tempstr, false);
		}

		goto_screen_map_yz(0, 0);

		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
	else if (fun_stage == 1)	// status
	{
		if (key_code == kbVirtualTest)
		{
			// set fun_stage = value for next stage
			fun_stage = 1;	    sub_fun_state = 1;

			y_start = fun_col_addr;		z_start = fun_line_addr;	y_end = 126;	z_end = 63;
		}
	}

    if (key_code == kbBack)
    {
        GetFunFlag = 0;
    }

	if ((sub_fun_state == 1) || (sub_fun_state == 2))		// init screen
	{
/*
		if (sub_fun_state == 1)	// init screen
		{
			fun_para_no = 0;

			GLCD_clear_screen(false);						// clear the lcd display

			// Display Screen - Title
			usnprintf(disp_tempstr, 21, "INPUT TEST");
			GLCD_write_titlebar(disp_tempstr);

			for (z_addr=0; z_addr<4; z_addr++)
			{
				goto_screen_map_yz(5, SCRN_MAP_CUR_Z);

				if (fun_para_no <= 2)
					usnprintf(disp_tempstr, 21, "%01u. %13s %03s", (fun_para_no+1), ipname_pump_list[input[fun_para_no].iptype], state_list[input_state[fun_para_no]]);
				else
					usnprintf(disp_tempstr, 21, "%01u. %13s %03s", (fun_para_no+1), ipname_fmeter_list[input[fun_para_no].iptype], state_list[input_state[fun_para_no]]);

				GLCD_WriteString(disp_tempstr, false);

				fun_para_no++;
			}
		}
*/
		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		for (z_addr=0; z_addr<4; z_addr++)
		{
			goto_screen_map_yz(fun_col_addr, SCRN_MAP_CUR_Z);

			usnprintf(disp_tempstr, 4, "%03s", state_list[input_state[z_addr]]);

			GLCD_WriteString(disp_tempstr, false);
		}

		sub_fun_state = 3;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		y_start = fun_col_addr;		z_start = fun_line_addr;	y_end = 126;	z_end = 63;
		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void Sensor_Test(uint8_t key_code)
{
uint8_t z_addr, fun_para_no;
uint8_t y_start, z_start, y_end, z_end;

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// set fun_stage = value for next stage
		fun_stage = 1;	    sub_fun_state = 1;

//		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// status
	{
		if (key_code == kbVirtualTest)
		{
			// set fun_stage = value for next stage
			fun_stage = 1;	    sub_fun_state = 1;

//			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
		else if ((key_code == kbUp) || (key_code == kbDown))
		{
			if (key_code == kbUp)			// Check if Up Key pressed, scroll up the screen
			{
				if(fun_result1 == 0)
					fun_result1 = 5;
				else
					fun_result1--;
			}
			else if (key_code == kbDown)	// Check if Down Key pressed, scroll down the screen
			{
				fun_result1++;
				if(fun_result1 >= 6)	fun_result1 = 0;
			}

			// set fun_stage = value for next stage
			fun_stage = 1;	    sub_fun_state = 1;

//			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
	}

    if (key_code == kbBack)
    {
        GetFunFlag = 0;
    }

	if (sub_fun_state == 1)		// init screen
	{
		sub_fun_state = 2;			key_code = 0;

		fun_para_no = fun_result1;

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen - Title
		usnprintf(disp_tempstr, 21, "SENSOR TEST");
		GLCD_write_titlebar(disp_tempstr);

		for (z_addr=0; z_addr<4; z_addr++)
		{
			goto_screen_map_yz(5, SCRN_MAP_CUR_Z);
//$$NR$$//dated:7Feb16
			if (fun_para_no == 0)
				usnprintf(disp_tempstr, 21, "OP PRES   %5u  bar", oppressure_curvalue);
			else if (fun_para_no == 1)
				usnprintf(disp_tempstr, 21, "IP PRES   %5u  bar", ippressure_curvalue);
			else if (fun_para_no == 2)
				usnprintf(disp_tempstr, 21, "TEMP      %5u  'C ", temperature_curvalue);
			else if (fun_para_no == 3)
            {
//              usnprintf(disp_tempstr, 21, "HUMI      %5u  %RH", humidity_curvalue);
                usnprintf(disp_tempstr, 18, "HUMI      %5u  ", humidity_curvalue);
                strcat(disp_tempstr, "%RH");
            }
			else if (fun_para_no == 4)
				usnprintf(disp_tempstr, 21, "RAIN        %3s     ", state_list[rain_curvalue]);
			else if (fun_para_no == 5)
				usnprintf(disp_tempstr, 21, "FLOW    %4u Lit/Min", flow_curvalue);
//$$NR$$//dated:7Feb16

			GLCD_WriteString(disp_tempstr, false);

			fun_para_no++;
			if (fun_para_no >= 6)	fun_para_no = 0;
		}

		// display down arrow key
		goto_screen_map_yz(0, (MIDDLE_PAGE_TOP + 3*GLCD_LINE_HEIGHT + 1));
		GLCD_writesymbol((uint8_t)(96+32), false);

//		GLCD_write_messagebar("                  ");

		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/
//$$NR$$//dated:08Nov15
void USB_Test(uint8_t key_code)
{
//uint8_t z_addr
uint8_t  usbresult;
uint8_t y_start, z_start, y_end, z_end;

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// set fun_stage = value for next stage
		fun_stage = 1;	    sub_fun_state = 1;

		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}

	if ((sub_fun_state == 1) || (sub_fun_state == 2))		// init screen
	{
		sub_fun_state = 3;

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen - Title
		usnprintf(disp_tempstr, 21, "USB TEST");
		GLCD_write_titlebar(disp_tempstr);

		fun_col_addr = 2;		fun_line_addr = SCRN_MAP_CUR_Z+10;

		// usb writing
		goto_screen_map_yz(fun_col_addr, fun_line_addr);
		strcpy(disp_tempstr, "    USB Writing     ");
		GLCD_WriteString(disp_tempstr, false);

		goto_screen_map_yz(fun_col_addr, fun_line_addr+10);
		strcpy(disp_tempstr, "   in Progress...   ");
		GLCD_WriteString(disp_tempstr, false);

		goto_screen_map_yz(fun_col_addr, fun_line_addr);
		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);

		usbresult = usb_test_write("usb test");

		y_start = 0;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+20;

		// check if usb write successful
		if (usbresult == 0)
		{
			// usb writing
			goto_screen_map_yz(fun_col_addr, fun_line_addr);
			strcpy(disp_tempstr, "    USB Reading     ");
			GLCD_WriteString(disp_tempstr, false);

			goto_screen_map_yz(fun_col_addr, fun_line_addr+10);
			strcpy(disp_tempstr, "   in Progress...   ");
			GLCD_WriteString(disp_tempstr, false);

			goto_screen_map_yz(fun_col_addr, fun_line_addr);
			GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);

			usb_test_read("usb test");
		}

		// check if usb write / read failed
		if (usbresult)
		{
			goto_screen_map_yz(fun_col_addr, fun_line_addr+10);
			strcpy(disp_tempstr, "     failed.        ");
			GLCD_WriteString(disp_tempstr, false);
		}
		else
		{
			// usb test successful
			goto_screen_map_yz(fun_col_addr, fun_line_addr);
			strcpy(disp_tempstr, "    USB Testing     ");
			GLCD_WriteString(disp_tempstr, false);

			goto_screen_map_yz(fun_col_addr, fun_line_addr+10);
			strcpy(disp_tempstr, "    successed.      ");
			GLCD_WriteString(disp_tempstr, false);
		}

		goto_screen_map_yz(fun_col_addr, fun_line_addr);
		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);

		Task_sleep(5000);	// delay of 5 second

		GetFunFlag = 0;
	}
}
//$$NR$$//dated:08Nov15
//$$NR$$//dated:07Feb16
/********************************************************/

/********************************************************/

void WiFi_Local_Test(uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;

//  if (tool_type == 1)
//      key_code = select_list_option(key_code);
//  else if (tool_type == 2)
//      key_code = edit_text_numeric(key_code);
//  else if (tool_type == 3)
//      key_code = edit_text_alphanumeric(key_code);
//
//  if (key_code == 0)  return;

    if (fun_stage == 0)
    {
        GLCD_clear_screen(false);                       // clear the lcd display

        // Display Screen
        usnprintf(disp_tempstr, 21, "WIFI LOCAL TEST");
        GLCD_write_titlebar(disp_tempstr);

        strcpy(disp_tempstr, "  To be implement.. ");

        z_addr = SCRN_MAP_CUR_Z+20;
        fun_stage = 1;      sub_fun_state = 2;

        fun_col_addr = 2;       fun_line_addr = z_addr;
        y_start = 0;    z_start = 0;        y_end = 126;    z_end = 63;
    }

//  if (key_code == kbBack)
//  {
//      GetFunFlag = 0;
//  }

    if ((sub_fun_state == 1) || (sub_fun_state == 2))   // init screen / line
    {
        goto_screen_map_yz(fun_col_addr, fun_line_addr);
        GLCD_WriteString(disp_tempstr, false);

        sub_fun_state = 3;

//      goto_screen_map_yz(fun_col_addr, fun_line_addr);
        GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);

        Task_sleep(5000);           // delay of 5 sec

        GetFunFlag = 0;
    }
}

/********************************************************/

/********************************************************/

void WiFi_Internet_Test(uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;

//  if (tool_type == 1)
//      key_code = select_list_option(key_code);
//  else if (tool_type == 2)
//      key_code = edit_text_numeric(key_code);
//  else if (tool_type == 3)
//      key_code = edit_text_alphanumeric(key_code);
//
//  if (key_code == 0)  return;

    if (fun_stage == 0)
    {
        GLCD_clear_screen(false);                       // clear the lcd display

        // Display Screen
        usnprintf(disp_tempstr, 21, "WIFI INTERNET TEST");
        GLCD_write_titlebar(disp_tempstr);

        strcpy(disp_tempstr, "  To be implement.. ");

        z_addr = SCRN_MAP_CUR_Z+20;
        fun_stage = 1;      sub_fun_state = 2;

        fun_col_addr = 2;       fun_line_addr = z_addr;
        y_start = 0;    z_start = 0;        y_end = 126;    z_end = 63;
    }

//  if (key_code == kbBack)
//  {
//      GetFunFlag = 0;
//  }

    if ((sub_fun_state == 1) || (sub_fun_state == 2))   // init screen / line
    {
        goto_screen_map_yz(fun_col_addr, fun_line_addr);
        GLCD_WriteString(disp_tempstr, false);

        sub_fun_state = 3;

//      goto_screen_map_yz(fun_col_addr, fun_line_addr);
        GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);

        Task_sleep(5000);           // delay of 5 sec

        GetFunFlag = 0;
    }
}

/********************************************************/

/********************************************************/

void GSM_Test(uint8_t key_code)
{
uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;

//  if (tool_type == 1)
//      key_code = select_list_option(key_code);
//  else if (tool_type == 2)
//      key_code = edit_text_numeric(key_code);
//  else if (tool_type == 3)
//      key_code = edit_text_alphanumeric(key_code);
//
//  if (key_code == 0)  return;

    if (fun_stage == 0)
    {
        GLCD_clear_screen(false);                       // clear the lcd display

        // Display Screen
        usnprintf(disp_tempstr, 21, "GSM TEST");
        GLCD_write_titlebar(disp_tempstr);

        strcpy(disp_tempstr, "  To be implement.. ");

        z_addr = SCRN_MAP_CUR_Z+20;
        fun_stage = 1;      sub_fun_state = 2;

        fun_col_addr = 2;       fun_line_addr = z_addr;
        y_start = 0;    z_start = 0;        y_end = 126;    z_end = 63;
    }

//  if (key_code == kbBack)
//  {
//      GetFunFlag = 0;
//  }

    if ((sub_fun_state == 1) || (sub_fun_state == 2))   // init screen / line
    {
        goto_screen_map_yz(fun_col_addr, fun_line_addr);
        GLCD_WriteString(disp_tempstr, false);

        sub_fun_state = 3;

//      goto_screen_map_yz(fun_col_addr, fun_line_addr);
        GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);

        Task_sleep(5000);           // delay of 5 sec

        GetFunFlag = 0;
    }
}
//$$NR$$//dated:07Feb16

//$$NR$$//dated:01Nov15

/********************************************************************************/
/* 					Common Routines For Function Handling						*/
/********************************************************************************/
/* 				Routines For data edit Handling						*/
/********************************************************************/

/********************************************************/

uint8_t select_list_option(uint8_t key_code)
{
uint8_t temp_dx, temp_dy;
int8_t temp_val;

	if ((key_code == kbUp) || (key_code == kbDown))
	{
		do
		{
			if (key_code == kbUp)			// Check if Up Key pressed, scroll up the Menubar
			{
				if(listbox_index == 0)
					listbox_index = listbox_size - 1;
				else
					listbox_index--;
			}
			else if (key_code == kbDown)					// Check if Down Key pressed, scroll down the Menubar
			{
				listbox_index++;
				if(listbox_index >= listbox_size)
					listbox_index = 0;
			}
		}while (listbox_selected & (0x01 << listbox_index));

		if (listbox_type == 0)
			strcpy(disp_tempstr, (*listbox)[listbox_index]);
		else if (listbox_type == 1)
		{
			temp_val = listbox_baseval + listbox_index;
	    	usnprintf(disp_tempstr, 4, ((temp_val<0)?"%03d":"+%02d"), temp_val);
		}
		else if (listbox_type == 2)
		{
	    	usnprintf(disp_tempstr, 3, "%02u", (listbox_baseval + listbox_index));
		}
		else if (listbox_type == 3)
		{
			if (key_code == kbUp)			// Check if Up Key pressed, scroll up the Menubar
			{
				if(fun_result1 == 0)
					fun_result1 = fun_result2 - 1;
				else
					fun_result1--;
			}
			else if (key_code == kbDown)	// Check if Down Key pressed, scroll down the Menubar
			{
				fun_result1++;
				if(fun_result1 >= fun_result2)
					fun_result1 = 0;
			}
			usnprintf(disp_tempstr, 14, "%01u. %s", output[listbox_index].typeid, output[listbox_index].opname);
		}

		if(sub_fun_state != 4)
		{
		    temp_dx = fun_col_addr + (strlen(disp_tempstr))*6;
		    temp_dy = fun_line_addr + 7;

			goto_screen_map_yz(fun_col_addr, fun_line_addr);
		    GLCD_WriteString(disp_tempstr, true);

	    	GLCD_MsgLine_Update(fun_col_addr, (fun_line_addr - 1), temp_dx, temp_dy);
		}
		else if(sub_fun_state == 4)
	    	return(key_code);
	}
	else if (key_code == kbEnter)
	{
		if (listbox_type == 0)
			strcpy(disp_tempstr, (*listbox)[listbox_index]);
		else if (listbox_type == 1)
		{
			temp_val = listbox_baseval + listbox_index;
	    	usnprintf(disp_tempstr, 4, ((temp_val<0)?"%03d":"+%02d"), temp_val);
		}
		else if (listbox_type == 2)
		{
	    	usnprintf(disp_tempstr, 3, "%02u", (listbox_baseval + listbox_index));
		}
		else if (listbox_type == 3)
		{
			usnprintf(disp_tempstr, 14, "%01u. %s", output[listbox_index].typeid, output[listbox_index].opname);
		}

//	    temp_dx = fun_col_addr + (strlen(disp_tempstr))*6;
//	    temp_dy = fun_line_addr + 7;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);
	    GLCD_WriteString(disp_tempstr, false);

//	    GLCD_MsgLine_Update(fun_col_addr, (fun_line_addr - 1), temp_dx, temp_dy);

		return(key_code);
	}
	else if ((key_code == kbBack) || (key_code == kbVirtualEdit))
	{
		return(key_code);
	}

    return(0);
}

/********************************************************/

/********************************************************/

/*

uint8_t edit_text_numeric(uint8_t key_code)
{
uint8_t temp_dx, temp_dy, tempkey_code = 0;

	if ((0x30 <= key_code) && (key_code <= 0x39))
	{
		if (textbox_char_count >= textbox_size) return 0;

		tempkey_code = edit_tempstr[textbox_char_count];
		if ((textbox_char_encoded_trancated == 0) && ((tempkey_code < 0x30) || (tempkey_code > 0x39)))
			textbox_char_count++;

		edit_tempstr[textbox_char_count] = key_code;		//ascii code

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		strcpy(disp_tempstr, edit_tempstr);
		if (textbox_char_encoded_trancated & 0x02)		memset(disp_tempstr, '*', textbox_char_count+1);

		GLCD_WriteString(disp_tempstr, true);

		textbox_char_count++;
	    temp_dx = fun_col_addr + strlen(edit_tempstr)*6;
	    temp_dy = fun_line_addr + 7;

	    GLCD_MsgLine_Update(fun_col_addr, fun_line_addr - 1, temp_dx, temp_dy);
	}
	else if (key_code == kbEnter)
	{
		strcpy(disp_tempstr, edit_tempstr);

		if (textbox_char_encoded_trancated & 0x02)		memset(disp_tempstr, '*', textbox_char_count);

		goto_screen_map_yz(fun_col_addr, fun_line_addr);
		GLCD_WriteString(disp_tempstr, false);

		if (textbox_char_encoded_trancated & 0x01)		edit_tempstr[textbox_char_count] = 0;

//	    GLCD_MsgLine_Update(fun_col_addr, fun_line_addr - 1, temp_dx, temp_dy);

	    return(key_code);
	}
	else if (key_code == kbClear)
	{
		if (textbox_char_count == 0) return 0;

		textbox_char_count--;

		tempkey_code = edit_tempstr[textbox_char_count];
		if ((textbox_char_encoded_trancated == 0) && ((tempkey_code < 0x30) || (tempkey_code > 0x39)))
			textbox_char_count--;

//		edit_tempstr[textbox_char_count] = ' ';
		edit_tempstr[textbox_char_count] = '0';		//$$NR$$//dated:23Aug15

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		strcpy(disp_tempstr, edit_tempstr);

		if (textbox_char_encoded_trancated & 0x02)		memset(disp_tempstr, '*', textbox_char_count);

		GLCD_WriteString(disp_tempstr, true);

	    temp_dx = fun_col_addr + strlen(edit_tempstr)*6;
	    temp_dy = fun_line_addr + 7;

	    GLCD_MsgLine_Update(fun_col_addr, fun_line_addr - 1, temp_dx, temp_dy);
	}
	else if ((key_code == kbBack) || (key_code == kbVirtualEdit))
	{
		return(key_code);
	}

	return (0);
}

 */
/********************************************************/

/********************************************************/
//$$NR$$//dated:19Sep15

uint8_t check_data_validity (void)
{
float		float_edited_value;
//uint32_t	uint32_edited_value;

//	uint32_edited_value = (uint32_t)strtoint(edit_tempstr, textbox_size);//strtofloat(edit_tempstr);
//	float_edited_value = (float)strtoint(edit_tempstr, textbox_size);//strtofloat(edit_tempstr);

	if (textbox_char_encoded_trancated & 0x04)
	{
		float_edited_value = strtofloat(edit_tempstr);

		// check if editting value is out of limit
		if ((editvalue_min_limit > float_edited_value) || (editvalue_max_limit < float_edited_value))
		{
			strcpy(edit_tempstr, edit_data_str);

			return (0);
		}
	}

	return (1);
}

//$$NR$$//dated:19Sep15

/********************************************************/

/********************************************************/

//$$NR$$//dated:23Aug15
uint8_t edit_text_numeric(uint8_t key_code)
{
uint8_t temp_dx, temp_dy, tempkey_code = 0;

	if ((0x30 <= key_code) && (key_code <= 0x39))
	{
		if (textbox_char_count >= textbox_size) return 0;

//		tempkey_code = edit_tempstr[textbox_char_count];
		//$$NR$$//dated:19Sep15
//		if (((textbox_char_encoded_trancated & 0x03) == 0) && ((tempkey_code < 0x30) || (tempkey_code > 0x39)))
//			textbox_char_count++;

		edit_tempstr[textbox_char_count] = key_code;		//ascii code

		prev_key_code = key_code;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		strcpy(disp_tempstr, edit_tempstr);
		if (textbox_char_encoded_trancated & 0x02)		memset(disp_tempstr, '*', textbox_char_count+1);

		GLCD_WriteString(disp_tempstr, true);

		if ((textbox_char_count+1) < textbox_size)
		{
			textbox_char_count++;
			tempkey_code = edit_tempstr[textbox_char_count];
			//$$NR$$//dated:19Sep15
			if (((textbox_char_encoded_trancated & 0x03) == 0) && ((tempkey_code < 0x30) || (tempkey_code > 0x39)))
				textbox_char_count++;
		}

		temp_dx = fun_col_addr + strlen(edit_tempstr)*6;
	    temp_dy = fun_line_addr + 7;

	    GLCD_MsgLine_Update(fun_col_addr, fun_line_addr - 1, temp_dx, temp_dy);
	}
	else if (key_code == kbEnter)
	{
//$$NR$$//dated:19Sep15
		if (check_data_validity() == 0)		key_code = 0;

		strcpy(disp_tempstr, edit_tempstr);

		if (textbox_char_encoded_trancated & 0x02)		memset(disp_tempstr, '*', textbox_char_count);

//		if (textbox_char_encoded_trancated & 0x01)		edit_tempstr[textbox_char_count] = 0;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (key_code)
		{
			GLCD_WriteString(disp_tempstr, false);

			return(key_code);
		}
		else
		{
			GLCD_WriteString(disp_tempstr, true);

		    temp_dx = fun_col_addr + strlen(edit_tempstr)*6;
		    temp_dy = fun_line_addr + 7;

		    GLCD_MsgLine_Update(fun_col_addr, fun_line_addr - 1, temp_dx, temp_dy);
		}
		//$$NR$$//dated:19Sep15
	}
	else if (key_code == kbClear)
	{
//		if (textbox_char_count == 0) return 0;
		if ((prev_key_code == key_code) && ((textbox_char_count + 1) > 1))		textbox_char_count--;
		prev_key_code = key_code;


		tempkey_code = edit_tempstr[textbox_char_count];
		//$$NR$$//dated:19Sep15
		if (((textbox_char_encoded_trancated & 0x03) == 0) && ((tempkey_code < 0x30) || (tempkey_code > 0x39)))
			textbox_char_count--;

//		edit_tempstr[textbox_char_count] = ' ';
		edit_tempstr[textbox_char_count] = '0';

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		strcpy(disp_tempstr, edit_tempstr);

		if (textbox_char_encoded_trancated & 0x02)		memset(disp_tempstr, '*', textbox_char_count);

		GLCD_WriteString(disp_tempstr, true);

	    temp_dx = fun_col_addr + strlen(edit_tempstr)*6;
	    temp_dy = fun_line_addr + 7;

	    GLCD_MsgLine_Update(fun_col_addr, fun_line_addr - 1, temp_dx, temp_dy);
	}
	else if ((key_code == kbBack) || (key_code == kbVirtualEdit))
	{
		return(key_code);
	}

	return (0);
}
//$$NR$$//dated:23Aug15

/********************************************************/

/********************************************************/

uint8_t edit_text_alphanumeric(uint8_t key_code)
{
uint8_t temp_dx, temp_dy;
uint8_t key_index, char_poschanged = 0, tempkey_code = 0;

	if ((0x30 <= key_code) && (key_code <= 0x39))
	{
		key_index = key_code - 0x30;

		if ((prev_key_code == key_code) && (typingdelay_elapsed_flag == false))
		{
			numkey_press_cnt++;
			if (numkey_press_cnt >= alphalimits[key_index])
				numkey_press_cnt = 0;
		}
		else
		{
			numkey_press_cnt = 0;		prev_key_code = key_code;
			if (numkey_pressed_flag)	char_poschanged = 1;
		}

		numkey_pressed_flag = true;

//		textbox_char_count += char_poschanged;
		if ((textbox_char_count + char_poschanged) >= textbox_size) return 0;
		textbox_char_count += char_poschanged;

		// get character for the selected key
		tempkey_code = alphanums[key_index][numkey_press_cnt];

		//change char to uppercase
//		if (((tempkey_code >= 'a') && (tempkey_code =< 'z')) && (key_press_detected & 0x22))	tempkey_code -= 32;
		if (((tempkey_code >= 97) && (tempkey_code <= 122)) && (key_press_detected & 0x44))		tempkey_code -= 32;



		edit_tempstr[textbox_char_count] = tempkey_code;		//ascii code

		strcpy(disp_tempstr, edit_tempstr);

		if (textbox_char_encoded_trancated & 0x02)		memset(disp_tempstr, '*', textbox_char_count+1);

		goto_screen_map_yz(fun_col_addr, fun_line_addr);
//	    GLCD_WriteString(edit_tempstr, true);
		GLCD_WriteString(disp_tempstr, true);

	    temp_dx = fun_col_addr + strlen(edit_tempstr)*6;
	    temp_dy = fun_line_addr + 7;

	    GLCD_MsgLine_Update(fun_col_addr, fun_line_addr - 1, temp_dx, temp_dy);

		start_typing_delay();						// Initialise Typing Delay for 1 sec
	}
	else if (key_code == kbEnter)
	{
		strcpy(disp_tempstr, edit_tempstr);

		if (textbox_char_encoded_trancated & 0x02)	memset(disp_tempstr, '*', textbox_char_count);

		goto_screen_map_yz(fun_col_addr, fun_line_addr);
//	    GLCD_WriteString(edit_tempstr, false);
		GLCD_WriteString(disp_tempstr, false);

		//$$NR$$//dated:30Aug15
		//check for last space char and trancate the string
		if (textbox_char_encoded_trancated & 0x01)
		{
			if (edit_tempstr[textbox_char_count] == ' ')
				edit_tempstr[textbox_char_count] = 0;
			else
				edit_tempstr[textbox_char_count + 1] = 0;
		}
		//$$NR$$//dated:30Aug15

//	    GLCD_MsgLine_Update(fun_col_addr, fun_line_addr - 1, temp_dx, temp_dy);

		return(key_code);
	}
	else if (key_code == kbClear)
	{
//		if (textbox_char_count == 0) return 0;
		if ((prev_key_code == key_code) && ((textbox_char_count + 1) > 1))		textbox_char_count--;
		prev_key_code = key_code;

//		if (textbox_char_encoded_trancated & 0x01)
//			edit_tempstr[textbox_char_count] = 0;
//		else
			edit_tempstr[textbox_char_count] = ' ';

		strcpy(disp_tempstr, edit_tempstr);

		if (textbox_char_encoded_trancated & 0x02)	memset(disp_tempstr, '*', textbox_char_count);

		goto_screen_map_yz(fun_col_addr, fun_line_addr);
//	    GLCD_WriteString(edit_tempstr, true);
		GLCD_WriteString(disp_tempstr, true);

	    temp_dx = fun_col_addr + strlen(edit_tempstr)*6;
	    temp_dy = fun_line_addr + 7;

	    GLCD_MsgLine_Update(fun_col_addr, fun_line_addr - 1, temp_dx, temp_dy);

		numkey_pressed_flag = false;	numkey_press_cnt = 0;
//		prev_key_code = 0;
		typingdelay_elapsed_flag = false;	typingdelay_count = 0;
	}
	else if ((key_code == kbBack) || (key_code == kbVirtualEdit))
	{
		return(key_code);
	}

	return (0);
}

/********************************************************/
// Initialise Typing Delay for 1 sec
/********************************************************/

void start_typing_delay(void)
{
	typingdelay_count = 100;
	//typingdelay_count = 10;
	typingdelay_elapsed_flag = false;
}

/********************************************************/
// set initial cursor position of textbox
/********************************************************/

void textbox_set_initial_cursrpos(void)
{
	//$$NR$$//dated:21Aug15
//	textbox_char_count = 0;

	textbox_char_count = strlen(edit_tempstr);
	while(textbox_char_count)
	{
		if (edit_tempstr[textbox_char_count-1] != ' ')	break;
		textbox_char_count--;
	}

	if (textbox_char_count >= textbox_size)		textbox_char_count--;
	//$$NR$$//dated:21Aug15
}

/********************************************************************************/
/* 						Routines For Initial Setup								*/
/********************************************************************************/

/********************************************************************/

void ShowInitialSetupScreen()
{
uint8_t z_addr;

	// get default values and store data into eeprom
	eeprom_factory_reset();

	GLCD_clear_screen(false);						// clear the lcd display

	// Display Initial Setup Screen
	goto_screen_map_yz(0, 1);
	GLCD_WriteString("        SETUP        ", true);

	z_addr = SCRN_MAP_CUR_Z;

	goto_screen_map_yz(3, SCRN_MAP_CUR_Z);
    GLCD_WriteString("LANGUAGE     ENGLISH", false);
    goto_screen_map_yz(3, SCRN_MAP_CUR_Z - 1);
    GLCD_WriteString("DATE FORMAT DD/MM/YY", false);
    goto_screen_map_yz(3, SCRN_MAP_CUR_Z - 1);
    GLCD_WriteString("CLOCK        12 HOUR", false);

	SetTime = CurTime;
    // Display date and time of selected source
    goto_screen_map_yz(3, SCRN_MAP_CUR_Z - 1);
//  GLCD_WriteString("TIME        00:00 AM", false);
    usnprintf(disp_tempstr, 18, "TIME        %02u:%02u", SetTime.tm_hour, SetTime.tm_min);
    if (disp_clockformat == 0)	strcat(disp_tempstr, (SetTime.tm_slot ? " PM":" AM"));
    GLCD_WriteString(disp_tempstr, false);

    goto_screen_map_yz(3, SCRN_MAP_CUR_Z - 1);
//  GLCD_WriteString("DATE        00/00/00", false);
    if (dateformat == 0)
		usnprintf(disp_tempstr, 21, "DATE        %02u/%02u/%02u", SetTime.tm_mday, SetTime.tm_mon, SetTime.tm_year);
    else
    	usnprintf(disp_tempstr, 21, "DATE        %02u/%02u/%02u", SetTime.tm_mon, SetTime.tm_mday, SetTime.tm_year);
    GLCD_WriteString(disp_tempstr, false);

    goto_screen_map_yz(3, SCRN_MAP_CUR_Z - 1);
    GLCD_WriteString("BEGIN WEEK    SUNDAY", false);

	// set fun_stage = value for next stage
	// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
	fun_stage = 1;	    tool_type = 1;		//sub_fun_state = 1;

    listbox_type = 0;	listbox_index = language;		listbox_size = 1;    listbox_selected = 0;
	fun_col_addr = 82;		fun_line_addr = z_addr;
	listbox = &language_list;

	strcpy(disp_tempstr, (*listbox)[listbox_index]);

	goto_screen_map_yz(fun_col_addr, fun_line_addr);
//	GLCD_WriteString((*listbox)[listbox_index], true);
	GLCD_WriteString(disp_tempstr, true);

	GLCD_Screen_Update();
}

/********************************************************/

/********************************************************/

void SetInitialSetup(uint8_t key_code)
{
//uint8_t z_addr;
uint8_t y_start, z_start, y_end, z_end;

	if (tool_type == 1)
		key_code = select_list_option(key_code);
	else if (tool_type == 2)
		key_code = edit_text_numeric(key_code);
	else if (tool_type == 3)
		key_code = edit_text_alphanumeric(key_code);

	if (key_code == 0)	return;

	if (fun_stage == 1)	//languages
	{
		if (key_code == kbEnter)
		{
			language = listbox_index;

			// store data into eeprom
			eeprom_write_systemsetup(0x01);
			eeprom_read_systemsetup(0x01);

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 2;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 0;	listbox_index = dateformat;		listbox_size = 2;    listbox_selected = 0;
			listbox = &dateformat_list;
			strcpy(disp_tempstr, (*listbox)[listbox_index]);

			fun_line_addr += 9;		fun_col_addr = 76;
			y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 2)	//date format
	{
		if (key_code == kbEnter)
		{
			dateformat = listbox_index;

			// store data into eeprom
			eeprom_write_systemsetup(0x08);
			eeprom_read_systemsetup(0x08);

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 3;	    tool_type = 1;		sub_fun_state = 1;

			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
			listbox_type = 0;	listbox_index = disp_clockformat;		listbox_size = 2;    listbox_selected = 0;
			listbox = &disp_clockformat_list;
			strcpy(disp_tempstr, (*listbox)[listbox_index]);

			fun_line_addr += 9;		fun_col_addr = 82;
			y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 3)	// clock format
	{
		if (key_code == kbEnter)
		{
			disp_clockformat = listbox_index;

			// store data into eeprom
			eeprom_write_systemsetup(0x04);
			eeprom_read_systemsetup(0x04);

			SetTime = CurTime;

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
			fun_stage = 4;	    tool_type = 2;		sub_fun_state = 1;

			//init variables for textbox_num / textbox_alphanum
		    textbox_char_encoded_trancated = 0;	textbox_size = 5;
		    usnprintf((char *)edit_tempstr, 6, "%02u:%02u", SetTime.tm_hour, SetTime.tm_min);

			fun_line_addr += 9;		fun_col_addr = 76;
			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
		}
	}
	else if (fun_stage == 4)	// time
	{
		if (key_code == kbEnter)
		{
			if (DateTimeStrToDec(edit_tempstr, 1) == false)
			{
				sub_fun_state = 1;

				usnprintf((char *)edit_tempstr, 6, "%02u:%02u", SetTime.tm_hour, SetTime.tm_min);
				y_start = 76;	z_start = fun_line_addr;		y_end = 127;	z_end = fun_line_addr+9;
			}
			else
			{
				if (disp_clockformat == 0)
				{
					// set fun_stage = value for next stage
					// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
					fun_stage = 5;	    tool_type = 1;		sub_fun_state = 1;

					listbox_type = 0;	listbox_index = CurTime.tm_slot;	listbox_size = 2;    listbox_selected = 0;
					listbox = &timeslot_list;
					strcpy(disp_tempstr, (*listbox)[listbox_index]);

					fun_line_addr = fun_line_addr;		fun_col_addr = 112;
					y_start = 76;	z_start = fun_line_addr;	y_end = 126;	z_end = fun_line_addr+9;
				}
				else
				{
					// set fun_stage = value for next stage
					// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
				    fun_stage = 6;		tool_type = 2;	    sub_fun_state = 1;

					//init variables for textbox_num / textbox_alphanum
				    textbox_char_encoded_trancated = 0;	textbox_size = 8;
	//				strcpy((char *)edit_tempstr, disp_rtcdate);

				    if (dateformat == 0)
						usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", SetTime.tm_mday, SetTime.tm_mon, SetTime.tm_year);
				    else
				    	usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", SetTime.tm_mon, SetTime.tm_mday, SetTime.tm_year);

					fun_line_addr += 9;		fun_col_addr = 76;
					y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;
				}

			    g_bSetDate = 0x01;//true;
			}
		}
	}
	else if (fun_stage == 5)	// time slot
	{
		if (key_code == kbEnter)
		{
//			rtctimeslot = listbox_index;
			SetTime.tm_slot = listbox_index;

			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		    fun_stage = 6;		tool_type = 2;	    sub_fun_state = 1;

			//init variables for textbox_num / textbox_alphanum
		    textbox_char_encoded_trancated = 0;	textbox_size = 8;
//			strcpy((char *)edit_tempstr, disp_rtcdate);

		    if (dateformat == 0)
				usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", SetTime.tm_mday, SetTime.tm_mon, SetTime.tm_year);
		    else
		    	usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", SetTime.tm_mon, SetTime.tm_mday, SetTime.tm_year);

			fun_line_addr += 9;		fun_col_addr = 76;
			y_start = fun_col_addr;	z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+9;

		    g_bSetDate = 0x02;//true;
		}
	}
	else if (fun_stage == 6)	// date
	{
		if (key_code == kbEnter)
		{
//			strcpy(disp_rtcdate, edit_tempstr);
			if (DateTimeStrToDec(edit_tempstr, 2) == false)
			{
				sub_fun_state = 1;

			    if (dateformat == 0)
					usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", SetTime.tm_mday, SetTime.tm_mon, SetTime.tm_year);
			    else
			    	usnprintf(edit_tempstr, 9, "%02u/%02u/%02u", SetTime.tm_mon, SetTime.tm_mday, SetTime.tm_year);

			    y_start = 76;	z_start = fun_line_addr;		y_end = 127;	z_end = fun_line_addr+9;
			}
			else
			{
				g_bSetDate = 0x04;//true;

				// set fun_stage = value for next stage
				// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
				fun_stage = 7;	    tool_type = 1;		sub_fun_state = 1;

				// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
				listbox_type = 0;	listbox_index = beginweek;		listbox_size = 7;    listbox_selected = 0;
				listbox = &beginweek_list;
				strcpy(disp_tempstr, (*listbox)[listbox_index]);

				fun_line_addr += 9;		fun_col_addr = 70;
				y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = 63;//fun_line_addr+9;
			}
		}
	}
	else if (fun_stage == 7)	// begin week
	{
		if (key_code == kbEnter)
		{
			beginweek = listbox_index;

			// store data into eeprom
			eeprom_write_systemsetup(0x10);
			eeprom_read_systemsetup(0x10);

//			GetFunFlag = 0;
			ShowHomeMode();		selected_ps_keycode = 0;
		}
	}

	if (key_code == kbBack)
	{
//		GetFunFlag = 0;
		ShowHomeMode();		selected_ps_keycode = 0;
	}

	if (sub_fun_state == 1)	// init
	{
		sub_fun_state = 2;

		goto_screen_map_yz(fun_col_addr, fun_line_addr);

		if (tool_type == 1)
			GLCD_WriteString(disp_tempstr, true);
		else if (tool_type >= 2)
		{
			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			GLCD_WriteString(edit_tempstr, true);
		}

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/*
void SetInitialSetup(uint8_t key_code)
{
	if (fun_stage == 1)	//languages
	{
		key_code = select_list_option(key_code);

		if (key_code == 0)	return;

		if (key_code == kbEnter)
		{
			language = listbox_index;

			// store data into eeprom
			eeprom_write_systemsetup(0x01);

			listbox_type = 0;	listbox_index = dateformat;		listbox_size = 2;    listbox_selected = 0;
			fun_line_addr += 9;		fun_col_addr = 76;
			listbox = &dateformat_list;
			fun_stage = 2;

			goto_screen_map_yz(fun_col_addr, fun_line_addr);
		    GLCD_WriteString((*listbox)[listbox_index], true);

		    GLCD_MsgLine_Update(fun_col_addr, fun_line_addr-10, 126, fun_line_addr + 9);
		}
	}
	else if (fun_stage == 2)	//date format
	{
		key_code = select_list_option(key_code);

		if (key_code == 0)	return;

		if (key_code == kbEnter)
		{
			dateformat = listbox_index;

			// store data into eeprom
			eeprom_write_systemsetup(0x08);

			listbox_type = 0;	listbox_index = disp_clockformat;		listbox_size = 2;    listbox_selected = 0;
			fun_line_addr += 9;		fun_col_addr = 82;
			listbox = &disp_clockformat_list;
			fun_stage = 3;

			goto_screen_map_yz(fun_col_addr, fun_line_addr);
		    GLCD_WriteString((*listbox)[listbox_index], true);

		    GLCD_MsgLine_Update(fun_col_addr, fun_line_addr-10, 126, fun_line_addr + 9);
		}
	}
	else if (fun_stage == 3)	// clock format
	{
		key_code = select_list_option(key_code);

		if (key_code == 0)	return;

		if (key_code == kbEnter)
		{
			disp_clockformat = listbox_index;

			// store data into eeprom
			eeprom_write_systemsetup(0x04);

			textbox_char_encoded_trancated = 0;	textbox_size = 6;

			strcpy((char *)edit_tempstr, "00:00");//disp_rtctime);

			fun_line_addr += 9;		fun_col_addr = 76;
			fun_stage = 4;

			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			goto_screen_map_yz(fun_col_addr, fun_line_addr);
			GLCD_WriteString(edit_tempstr, true);

			GLCD_MsgLine_Update(fun_col_addr, fun_line_addr-10, 126, fun_line_addr + 9);
		}
	}
	else if (fun_stage == 4)	// time
	{
		key_code = edit_text_numeric(key_code);

		if (key_code == 0)	return;

		if (key_code == kbEnter)
		{
			listbox_type = 0;	listbox_index = CurTime.tm_slot;		listbox_size = 2;    listbox_selected = 0;
			fun_line_addr = fun_line_addr;		fun_col_addr = 112;
			listbox = &timeslot_list;
			fun_stage = 5;

			goto_screen_map_yz(fun_col_addr, fun_line_addr);
		    GLCD_WriteString((*listbox)[listbox_index], true);

		    GLCD_MsgLine_Update(76, fun_line_addr, 126, fun_line_addr + 9);
		}
	}
	else if (fun_stage == 5)	// time slot
	{
		key_code = select_list_option(key_code);

		if (key_code == 0)	return;

		if (key_code == kbEnter)
		{
			CurTime.tm_slot = listbox_index;

			textbox_char_encoded_trancated = 0;	textbox_size = 8;

			strcpy((char *)edit_tempstr, "00/00/00");//disp_rtcdate);

			fun_line_addr += 9;		fun_col_addr = 76;
			fun_stage = 6;

			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			goto_screen_map_yz(fun_col_addr, fun_line_addr);
			GLCD_WriteString(edit_tempstr, true);

			GLCD_MsgLine_Update(fun_col_addr, fun_line_addr-10, 126, fun_line_addr + 9);
		}
	}
	else if (fun_stage == 6)	// date
	{
		key_code = edit_text_numeric(key_code);

		if (key_code == 0)	return;

		if (key_code == kbEnter)
		{
			listbox_type = 0;	listbox_index = beginweek;		listbox_size = 7;    listbox_selected = 0;
			fun_line_addr += 9;		fun_col_addr = 70;
			listbox = &beginweek_list;
			fun_stage = 7;

			goto_screen_map_yz(fun_col_addr, fun_line_addr);
		    GLCD_WriteString((*listbox)[listbox_index], true);

		    GLCD_MsgLine_Update(fun_col_addr, fun_line_addr-10, 126, 63);
		}
	}
	else if (fun_stage == 7)	// begin week
	{
		key_code = select_list_option(key_code);

		if (key_code == 0)	return;

		if (key_code == kbEnter)
		{
			beginweek = listbox_index;

			// store data into eeprom
			eeprom_write_systemsetup(0x10);

			ShowHomeMode();		selected_ps_keycode = 0;
		}
	}

	if (key_code == kbBack)
	{
		ShowHomeMode();		selected_ps_keycode = 0;
	}
}
*/
/********************************************************************************/
/* 						Routines For Home Screen Handling						*/
/********************************************************************************/
//
/********************************************************************/

void ShowHomeMode(void)
{
	led_control(LED_PROGRAM, OFF);			// Turn off the PROGRAM LED
	cntrl_mode=cntrl_mode_prev;
	cntrl_mode_prev=cntrl_mode;
//	led_control(LED_MANUAL, OFF);			// Turn off the MANUAL LED

	access_level = alNorm;
	PanelState =  psHome;					// Set Panel State = Home
	cntrl_homescrn_type = 0;				// 0-home screen

//	ViewHomeScreen();

	InitFunctionScreen ();					// Initialise Function screen
	HomeStatus(kbVirtualHome);
}

/********************************************************/
// Display Home screen
/********************************************************/

void ViewHomeScreen(void)
{
//uint8_t z_addr;

//	PanelState = psHome;							// Set Panel State = Home

	GLCD_clear_screen(false);						// clear the lcd display

	GLCD_write_titlebar("        HOME       >");
//	z_addr = SCRN_MAP_CUR_Z;

	// Display current date and time
	ViewDateTimeMode (false);

//    goto_screen_map_yz(2, MIDDLE_PAGE_TOP + 30);
////  GLCD_WriteString("NEXT START  06:00 PM", false);
//    if (nextprog_time[0] != 24)			// check if valid data
//    {
//    	set_disp_time(nextprog_time, true);
//    	usnprintf(disp_tempstr, 18, "NEXT START  %02u:%02u", disp_time.tm_hour, disp_time.tm_min);
//        if (disp_clockformat == 0)	strcat(disp_tempstr, (disp_time.tm_slot ? " PM":" AM"));
//    	GLCD_WriteString(disp_tempstr, false);
//    }

    GLCD_write_messagebar("                ");

	GLCD_Screen_Update();
}

/********************************************************/
// Display Current Date, Time and Mode
/********************************************************/

void ViewDateTimeMode (bool screen_refresh)
{
uint8_t z_addr;

	z_addr = TITLEBAR_TOP_SPACE + TITLEBAR_HEIGHT;

	//display day of week
	goto_screen_map_yz(106, z_addr);
    strcpy(disp_tempstr, g_ppcDay[CurTime.tm_wday]);
    GLCD_WriteString(disp_tempstr, false);

	//display date (dd/mm  or mm/dd)
	goto_screen_map_yz(96, z_addr+10);
    if (dateformat == 0)
		usnprintf(disp_tempstr, 6, "%02u/%02u", CurTime.tm_mday, CurTime.tm_mon);
    else
    	usnprintf(disp_tempstr, 6, "%02u/%02u", CurTime.tm_mon, CurTime.tm_mday);
    GLCD_WriteString(disp_tempstr, false);

    //display time (hh:mm)
    goto_screen_map_yz(2, 16);
    usnprintf(disp_tempstr, 6, "%02u:%02u", CurTime.tm_hour, CurTime.tm_min);
    if (disp_clockformat == 0)	strcat(disp_tempstr, (CurTime.tm_slot ? " PM":" AM"));
	GLCD_WriteStringLargeFont(disp_tempstr, false);


    goto_screen_map_yz(2, MIDDLE_PAGE_TOP + 30);
//  GLCD_WriteString("NEXT START  06:00 PM", false);
    nextprogstart();
    if (nextprog_time[0] != 24)			// check if valid data
    {
    	set_disp_time(nextprog_time, true);
    	usnprintf(disp_tempstr, 18, "NEXT START  %02u:%02u", disp_time.tm_hour, disp_time.tm_min);
        if (disp_clockformat == 0)	strcat(disp_tempstr, (disp_time.tm_slot ? " PM":" AM"));
    	GLCD_WriteString(disp_tempstr, false);
    }
    else
    {
    	usnprintf(disp_tempstr, 21, "NO FURTHER SCHEDULE  ");
    	GLCD_WriteString(disp_tempstr, false);
    }

    if (screen_refresh)		GLCD_MsgLine_Update(0, z_addr, 126, z_addr + 20);

//	Prev_AL = access_level;
//	DispDateTimeMode = 0x01;						// Enable flag for display Date/Time/Mode
}

/********************************************************************
* Function Name  : RefreshHomeScreen
* Description    : Refresh Date and Time on LCD display.
* Input          : - TimeVar: RTC counter value.
* Output         : None
* Return         : None
********************************************************************/
// Address of Date :- 0x01
// Address of Time :- 0x0A
// if Norm mode, 		Keyswitch LED (100 % - OFF)
// if User mode, 		Keyswitch LED (100 % - ON)
// if Supr mode, 		Keyswitch LED (25 % / 75 % - ON/OFF)
// if Engr/Fact mode, 	Keyswitch LED (75 % / 25 % - ON/OFF)

void RefreshHomeScreen(void)
{
/*	if (DispDateTimeMode == 0x03)
	{
		CheckRTC = 1;
		Prev_AL = alNoMode;
	}
*/
	if (PanelState != psHome)	return;

	// check if flag for RTC Refresh is set.
	if (CheckRTC)
	{
		CheckRTC = 0;

//		if (prev_access_level != access_level)
//		{
//			if (access_level == alNorm)
//				ViewHomeScreen ();
//
//			prev_access_level = access_level;
////			DispDateTimeMode = 0x01;									// Enable flag for display Date/Time/Mode
//		}
//
//		// Display current date and time
//		ViewDateTimeMode (true);

		HomeStatus(kbVirtualHome);
	}
}

/********************************************************/

/********************************************************/

void HomeStatus(uint8_t key_code)
{
uint8_t temp_status;

	do
	{
		temp_status = cntrl_homescrn_type;

		if (cntrl_homescrn_type == 0)
			HomeDateTimeStatus(key_code);
		else if (cntrl_homescrn_type == 1)
			HomeProgramStatus(key_code);
		else if (cntrl_homescrn_type == 2)
			HomeSensorStatus(key_code);
		else if (cntrl_homescrn_type == 3)
			HomeOutputStatus(key_code);
	}while (temp_status != cntrl_homescrn_type);

}

/********************************************************/

/********************************************************/
//InitFunctionScreen ();					// Initialise Function screen

void HomeDateTimeStatus(uint8_t key_code)
{
uint8_t z_addr;//, fun_para_no;
uint8_t y_start, z_start, y_end, z_end;

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// set fun_stage = value for next stage
		fun_stage = 1;	    sub_fun_state = 1;

//		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// status
	{
		if (key_code == kbVirtualHome)
		{
			// set fun_stage = value for next stage
			fun_stage = 1;	    sub_fun_state = 1;

//			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
		else if ((key_code == kbLeft) || (key_code == kbRight) || (key_code == kbHome))
		{
			InitFunctionScreen ();					// Initialise Function screen

			if (key_code == kbLeft)
			{
				cntrl_homescrn_type = 3;			// 3-output status screen
				key_code = kbVirtualHome;
			}
			else if (key_code == kbRight)
			{
				cntrl_homescrn_type = 1;			// 1-program status screen
				key_code = kbVirtualHome;
			}
			else if (key_code == kbHome)
			{
				cntrl_homescrn_type = 0;			// 0-Home screen
				key_code = kbVirtualHome;
			}
		}
	}

	if (sub_fun_state == 1)		// init screen
	{
		sub_fun_state = 2;			key_code = 0;

//		fun_para_no = fun_result1;

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen - Title
		usnprintf(disp_tempstr, 21, "<       HOME       >");
		GLCD_write_titlebar(disp_tempstr);

		z_addr = SCRN_MAP_CUR_Z;
//		z_addr = TITLEBAR_TOP_SPACE + TITLEBAR_HEIGHT;

		//display day of week
		goto_screen_map_yz(106, z_addr);
	    strcpy(disp_tempstr, g_ppcDay[CurTime.tm_wday]);
	    GLCD_WriteString(disp_tempstr, false);

		//display date (dd/mm  or mm/dd)
		goto_screen_map_yz(96, z_addr+10);
	    if (dateformat == 0)
			usnprintf(disp_tempstr, 6, "%02u/%02u", CurTime.tm_mday, CurTime.tm_mon);
	    else
	    	usnprintf(disp_tempstr, 6, "%02u/%02u", CurTime.tm_mon, CurTime.tm_mday);
	    GLCD_WriteString(disp_tempstr, false);

	    //display time (hh:mm)
	    goto_screen_map_yz(2, 16);
	    usnprintf(disp_tempstr, 6, "%02u:%02u", CurTime.tm_hour, CurTime.tm_min);
	    if (disp_clockformat == 0)	strcat(disp_tempstr, (CurTime.tm_slot ? " PM":" AM"));
		GLCD_WriteStringLargeFont(disp_tempstr, false);


	    goto_screen_map_yz(2, MIDDLE_PAGE_TOP + 30);
	//  GLCD_WriteString("NEXT START  06:00 PM", false);
	    nextprogstart();

	    if (nextprog_time[0] != 24)			// check if valid data
	    {
	    	set_disp_time(nextprog_time, true);
	    	usnprintf(disp_tempstr, 18, "NEXT START  %02u:%02u", disp_time.tm_hour, disp_time.tm_min);
	        if (disp_clockformat == 0)	strcat(disp_tempstr, (disp_time.tm_slot ? " PM":" AM"));
	    	GLCD_WriteString(disp_tempstr, false);
	    }
		else
		{
			usnprintf(disp_tempstr, 21, "NO FURTHER SCHEDULE  ");
			GLCD_WriteString(disp_tempstr, false);
		}



	    if(shift != 0)
	    {
	    	usnprintf(disp_tempstr, 18, "DELAYED BY  %02u:%02u", (shift/60),(shift%60));
	    	GLCD_write_messagebar(disp_tempstr);
	    }
	    else
	    {
	    	GLCD_write_messagebar("                  ");
	    }

	    //GLCD_write_messagebar("ALERT: PRESS HELP");

		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
//		GLCD_MsgLine_Update(0, z_addr, 126, z_addr + 20);
		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/
//InitFunctionScreen ();					// Initialise Function screen

void HomeProgramStatus(uint8_t key_code)
{
uint8_t z_addr;//, fun_para_no;
uint8_t y_start, z_start, y_end, z_end;

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// set fun_stage = value for next stage
		fun_stage = 1;	    sub_fun_state = 1;

//		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// status
	{
		if (key_code == kbVirtualHome)
		{
			// set fun_stage = value for next stage
			fun_stage = 1;	    sub_fun_state = 1;

//			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
		else if ((key_code == kbLeft) || (key_code == kbRight) || (key_code == kbHome))
		{
			InitFunctionScreen ();					// Initialise Function screen

			if (key_code == kbLeft)
			{
				cntrl_homescrn_type = 0;			// 0-home screen
				key_code = kbVirtualHome;
			}
			else if (key_code == kbRight)
			{
				cntrl_homescrn_type = 2;			// 2-sensor status screen
				key_code = kbVirtualHome;
			}
			else if (key_code == kbHome)
			{
				cntrl_homescrn_type = 0;			// 0-Home screen
				key_code = kbVirtualHome;
			}
		}
	}

	if (sub_fun_state == 1)		// init screen
	{
		sub_fun_state = 2;			key_code = 0;

//		fun_para_no = fun_result1;

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen - Title
		usnprintf(disp_tempstr, 21, "<  PROGRAM STATUS  >");
		GLCD_write_titlebar(disp_tempstr);

		z_addr = SCRN_MAP_CUR_Z;

		goto_screen_map_yz(5, z_addr);
		strcpy(disp_tempstr, ((cntrl_mode>0) ? "MANUAL MODE":"AUTO MODE  "));
		strcat(disp_tempstr, (cntrl_state ? "  STANDBY":"  RUNNING"));
		GLCD_WriteString(disp_tempstr, false);

		if (cur_prog)
		{
			goto_screen_map_yz(5, z_addr + 10);
		    usnprintf(disp_tempstr, 21, "PROGRAM %01u  ", cur_prog);
			strcat(disp_tempstr, (cur_prog_state ? "  STANDBY":"  RUNNING"));
			GLCD_WriteString(disp_tempstr, false);

			goto_screen_map_yz(5, z_addr + 20);
		   // usnprintf(disp_tempstr, 21, "ELAPSED TIME   %02u:%02u", cur_prog_elapsedtime[0], cur_prog_elapsedtime[1]);
		    usnprintf(disp_tempstr, 21, "REMAINING TIME %02u:%02u", cur_prog_elapsedtime[0], cur_prog_elapsedtime[1]);
			GLCD_WriteString(disp_tempstr, false);

			goto_screen_map_yz(5, z_addr + 30);
		  //usnprintf(disp_tempstr, 21, "ELAPSED TIME   %02u:%02u", cur_prog_elapsedtime[0], cur_prog_elapsedtime[1]);
			usnprintf(disp_tempstr, 21, "CYCLE %d                 ",(cycle+1));
			GLCD_WriteString(disp_tempstr, false);
		}

	   // GLCD_write_messagebar("ALERT: PRESS HELP");
		if(shift != 0)
		{
			usnprintf(disp_tempstr, 18, "DELAYED BY  %02u:%02u", (shift/60),(shift%60));
			GLCD_write_messagebar(disp_tempstr);
		}
		else
		{
			GLCD_write_messagebar("                  ");
		}

	    y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void HomeSensorStatus(uint8_t key_code)
{
uint8_t z_addr, fun_para_no;
uint8_t y_start, z_start, y_end, z_end;

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// set fun_stage = value for next stage
		fun_stage = 1;	    sub_fun_state = 1;

//		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// status
	{
		if (key_code == kbVirtualHome)
		{
			// set fun_stage = value for next stage
			fun_stage = 1;	    sub_fun_state = 1;

//			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
		else if ((key_code == kbUp) || (key_code == kbDown))
		{
			if (key_code == kbUp)			// Check if Up Key pressed, scroll up the screen
			{
				if(fun_result1 == 0)
					fun_result1 = 5;
				else
					fun_result1--;
			}
			else if (key_code == kbDown)	// Check if Down Key pressed, scroll down the screen
			{
				fun_result1++;
				if(fun_result1 >= 6)	fun_result1 = 0;
			}

			// set fun_stage = value for next stage
			fun_stage = 1;	    sub_fun_state = 1;

//			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
		else if ((key_code == kbLeft) || (key_code == kbRight) || (key_code == kbHome))
		{
			InitFunctionScreen ();					// Initialise Function screen

			if (key_code == kbLeft)
			{
				cntrl_homescrn_type = 1;			// 1-program status screen
				key_code = kbVirtualHome;
			}
			else if (key_code == kbRight)
			{
				cntrl_homescrn_type = 3;			// 3-output status screen
				key_code = kbVirtualHome;
			}
			else if (key_code == kbHome)
			{
				cntrl_homescrn_type = 0;			// 0-Home screen
				key_code = kbVirtualHome;
			}
		}
	}

	if (sub_fun_state == 1)		// init screen
	{
		sub_fun_state = 2;			key_code = 0;

		fun_para_no = fun_result1;

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen - Title
		usnprintf(disp_tempstr, 21, "<  SENSOR STATUS   >");
		GLCD_write_titlebar(disp_tempstr);

		for (z_addr=0; z_addr<4; z_addr++)
		{
			goto_screen_map_yz(5, SCRN_MAP_CUR_Z);

//$$NR$$//dated:7Feb16
            if (fun_para_no == 0)
                usnprintf(disp_tempstr, 21, "OP PRES   %5u  bar", oppressure_curvalue);
            else if (fun_para_no == 1)
                usnprintf(disp_tempstr, 21, "IP PRES   %5u  bar", ippressure_curvalue);
            else if (fun_para_no == 2)
                usnprintf(disp_tempstr, 21, "TEMP      %5u  'C ", temperature_curvalue);
            else if (fun_para_no == 3)
            {
//              usnprintf(disp_tempstr, 21, "HUMI      %5u  %RH", humidity_curvalue);
                usnprintf(disp_tempstr, 18, "HUMI      %5u  ", humidity_curvalue);
                strcat(disp_tempstr, "%RH");
            }
            else if (fun_para_no == 4)
                usnprintf(disp_tempstr, 21, "RAIN        %3s     ", state_list[rain_curvalue]);
            else if (fun_para_no == 5)
                usnprintf(disp_tempstr, 21, "FLOW    %4u Lit/Min", flow_curvalue);
//$$NR$$//dated:7Feb16

			GLCD_WriteString(disp_tempstr, false);

			fun_para_no++;
			if (fun_para_no >= 6)	fun_para_no = 0;
		}

		// display down arrow key
//		goto_screen_map_yz(0, (MIDDLE_PAGE_TOP + 4*GLCD_LINE_HEIGHT + 1));
		goto_screen_map_yz(0, (MIDDLE_PAGE_TOP + 3*GLCD_LINE_HEIGHT + 1));
		GLCD_writesymbol((uint8_t)(96+32), false);

		//GLCD_write_messagebar("ALERT: PRESS HELP");
		if(shift != 0)
		{
			usnprintf(disp_tempstr, 18, "DELAYED BY  %02u:%02u", (shift/60),(shift%60));
			GLCD_write_messagebar(disp_tempstr);
		}
		else
		{
			GLCD_write_messagebar("                  ");
		}

		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/********************************************************/

/********************************************************/

void HomeOutputStatus(uint8_t key_code)
{
uint8_t z_addr, fun_para_no;
uint8_t y_start, z_start, y_end, z_end;

	if (key_code == 0)	return;

	if (fun_stage == 0)
	{
		// set fun_stage = value for next stage
		fun_stage = 1;	    sub_fun_state = 1;

//		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// status
	{
		if (key_code == kbVirtualHome)
		{
			// set fun_stage = value for next stage
			fun_stage = 1;	    sub_fun_state = 1;

//			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
		else if ((key_code == kbUp) || (key_code == kbDown))
		{
			if (key_code == kbUp)			// Check if Up Key pressed, scroll up the screen
			{
				if(fun_result1 == 0)
					fun_result1 = OUTPUT_MAX - 1;
				else
					fun_result1--;
			}
			else if (key_code == kbDown)	// Check if Down Key pressed, scroll down the screen
			{
				fun_result1++;
				if(fun_result1 >= OUTPUT_MAX)	fun_result1 = 0;
			}

			// set fun_stage = value for next stage
			fun_stage = 1;	    sub_fun_state = 1;

//			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
		else if ((key_code == kbLeft) || (key_code == kbRight) || (key_code == kbHome))
		{
			InitFunctionScreen ();					// Initialise Function screen

			if (key_code == kbLeft)
			{
				cntrl_homescrn_type = 2;			// 2-sensor status screen
				key_code = kbVirtualHome;
			}
			else if (key_code == kbRight)
			{
				cntrl_homescrn_type = 0;			// 0-home screen
				key_code = kbVirtualHome;
			}
			else if (key_code == kbHome)
			{
				cntrl_homescrn_type = 0;			// 0-Home screen
				key_code = kbVirtualHome;
			}
		}
	}

	if (sub_fun_state == 1)		// init screen
	{
		sub_fun_state = 2;			key_code = 0;

		fun_para_no = fun_result1;

		GLCD_clear_screen(false);						// clear the lcd display

		// Display Screen - Title
		usnprintf(disp_tempstr, 21, "<  OUTPUT STATUS   >");
		GLCD_write_titlebar(disp_tempstr);

		for (z_addr=0; z_addr<4; z_addr++)
		{
			goto_screen_map_yz(5, SCRN_MAP_CUR_Z);
			usnprintf(disp_tempstr, 21, "%02u. %10s  %s", (fun_para_no+1), output[fun_para_no].opname, state_list[output_state[fun_para_no]]);
			GLCD_WriteString(disp_tempstr, false);

			fun_para_no++;
			if (fun_para_no >= OUTPUT_MAX)	fun_para_no = 0;
		}

		// display down arrow key
//		goto_screen_map_yz(0, (MIDDLE_PAGE_TOP + 4*GLCD_LINE_HEIGHT + 1));
		goto_screen_map_yz(0, (MIDDLE_PAGE_TOP + 3*GLCD_LINE_HEIGHT + 1));
		GLCD_writesymbol((uint8_t)(96+32), false);

		//GLCD_write_messagebar("ALERT: PRESS HELP");
		if(shift != 0)
		{
			usnprintf(disp_tempstr, 18, "DELAYED BY  %02u:%02u", (shift/60),(shift%60));
			GLCD_write_messagebar(disp_tempstr);
		}
		else
		{
			GLCD_write_messagebar("                  ");
		}

		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}

/*
void HomeOutputStatus(uint8_t key_code)
{
uint8_t z_addr, fun_para_no;
uint8_t y_start, z_start, y_end, z_end;

	if (fun_stage == 0)
	{
		// set fun_stage = value for next stage
		fun_stage = 1;	    sub_fun_state = 1;

//		// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
//		fun_stage = 1;	    tool_type = 1;		sub_fun_state = 1;
//
//		// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
//		listbox_type = 0;	listbox_index = manoutput[fun_result1].state;		listbox_size = 2;    listbox_selected = 0;
//		listbox = &state_list;
//		strcpy(edit_tempstr, (*listbox)[listbox_index]);
//
//		fun_col_addr = 102;		fun_line_addr = MIDDLE_PAGE_TOP;
//		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
	}
	else if (fun_stage == 1)	// status
	{
//		if (key_code == kbEnter)
//		{
////			manoutput[fun_result1].state = listbox_index;
////
////			// store data into eeprom
////			eeprom_write_man_output(fun_result1);
//
//			// set fun_stage = value for next stage
//			fun_stage = 1;	    sub_fun_state = 2;
//
////			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
////			fun_stage = 1;	    tool_type = 1;		sub_fun_state = 2;
////
////			fun_col_addr = 102;
//
//			fun_result1++;
//			if (fun_result1 < 5)
//			{
////				fun_line_addr += 10;
//				y_start = fun_col_addr;		z_start = fun_line_addr-10;		y_end = 126;	z_end = fun_line_addr+10;
//			}
//			else if (fun_result1 >= 5)
//			{
//				sub_fun_state = 1;
////				fun_line_addr += 10;
//
//				if (fun_result1 >= OUTPUT_MAX)
//				{
//					fun_result1 = 0;
////					fun_line_addr = MIDDLE_PAGE_TOP;
//				}
//
//				y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
//			}
//
////			// set listbox_type = 0(const str) / 1(int8_t) / 2(uint8_t)
////			listbox_type = 0;	listbox_index = manoutput[fun_result1].state;		listbox_size = 2;    listbox_selected = 0;
////			listbox = &state_list;
////			strcpy(edit_tempstr, (*listbox)[listbox_index]);
//		}

		if ((key_code == kbUp) || (key_code == kbDown))
		{
			if (key_code == kbUp)			// Check if Up Key pressed, scroll up the screen
			{
				if(fun_result1 == 0)
					fun_result1 = OUTPUT_MAX - 1;
				else
					fun_result1--;
			}
			else if (key_code == kbDown)	// Check if Down Key pressed, scroll down the screen
			{
				fun_result1++;
				if(fun_result1 >= OUTPUT_MAX)	fun_result1 = 0;
			}

			// set fun_stage = value for next stage
			fun_stage = 1;	    sub_fun_state = 1;

			y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;
		}
		else if (key_code == kbLeft)
		{
			InitFunctionScreen ();					// Initialise Function screen

		}
		else if (key_code == kbRight)
		{

		}
	}


//	if (key_code == kbBack)
//	{
//		GetFunFlag = 0;
//	}

//	if ((sub_fun_state == 1) || (sub_fun_state == 2))	// init screen / line
	if (sub_fun_state == 1)		// init screen
	{
//		if (sub_fun_state == 1)	// init screen
//		{
//			if (fun_result1 < 5)
//				fun_para_no = 0;
//			else
//				fun_para_no = fun_result1 - 4;

			fun_para_no = fun_result1;

			GLCD_clear_screen(false);						// clear the lcd display

			// Display Screen - Title
			usnprintf(disp_tempstr, 21, "<  OUTPUT STATUS  >");
			GLCD_write_titlebar(disp_tempstr);

//			fun_line_addr = SCRN_MAP_CUR_Z;

			for (z_addr=0; z_addr<5; z_addr++)
			{
				goto_screen_map_yz(5, SCRN_MAP_CUR_Z);
//				usnprintf(disp_tempstr, 21, "PROGRAM %01u   %s", (fun_para_no+1), state_list[manprog[fun_para_no].state]);
				usnprintf(disp_tempstr, 21, "%02u. %10s  %s", (fun_para_no+1), output[fun_para_no].opname, state_list[output_state[fun_para_no]]);
				GLCD_WriteString(disp_tempstr, false);

				fun_para_no++;
				if (fun_para_no >= OUTPUT_MAX)	fun_para_no = 0;
			}

			// display down arrow key
			goto_screen_map_yz(0, (MIDDLE_PAGE_TOP + 4*GLCD_LINE_HEIGHT + 1));
			GLCD_writesymbol((uint8_t)(96+32), false);
//		}

		sub_fun_state = 2;
//
//		goto_screen_map_yz(fun_col_addr, fun_line_addr);
//
//		if (tool_type == 1)
//			GLCD_WriteString(edit_tempstr, true);
//		else if (tool_type >= 2)
//		{
//			// Check if first char of the editing mode
//			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
//			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
//			typingdelay_elapsed_flag = false;	typingdelay_count = 0;
//
//			GLCD_WriteString(edit_tempstr, true);
//		}
		y_start = 0;	z_start = 0;		y_end = 126;	z_end = 63;

		GLCD_MsgLine_Update(y_start, z_start, y_end, z_end);
	}
}
 */
/********************************************************************************/
/* 						Routines For Login Handling								*/
/********************************************************************************/
//
/********************************************************************/

void ShowPasswordScreen()
{
uint8_t z_addr;

	GLCD_clear_screen(false);						// clear the lcd display

	PanelState =  psPassword;					// Set PanelState = Password

	InitFunctionScreen ();					// Initialise Function screen	//$$NR$$//dated:30Aug15

	GLCD_write_titlebar("User Login");
	z_addr = SCRN_MAP_CUR_Z + 10;

	goto_screen_map_yz(3, z_addr);
    GLCD_WriteString("User ID :           ", false);
    goto_screen_map_yz(3, z_addr + 10);
    GLCD_WriteString("Password:           ", false);
	GLCD_write_messagebar("Back           Enter");

	listbox_type = 0;	listbox_index = 0;		listbox_size = 4;    listbox_selected = 0;
	fun_col_addr = 66;		fun_line_addr = z_addr;
	listbox = &loginuser_list;

	goto_screen_map_yz(fun_col_addr, fun_line_addr);
    GLCD_WriteString((*listbox)[listbox_index], true);

	GLCD_Screen_Update();

	fun_stage = 1;

//	strcpy((char *)edit_tempstr, "      ");
//	textbox_char_count = 0;
}

/********************************************************/

/********************************************************/

void GetPassword(uint8_t key_code)
{
//uint8_t tmp_access = 0;

	if (fun_stage == 1)
	{
		key_code = select_list_option(key_code);

		if (key_code == 0)	return;

		if (key_code == kbEnter)
		{
			fun_result1 = listbox_index;

			// read data into eeprom
			eeprom_read_authentication(fun_result1);

			//$$NR$$//dated:30Aug15
			// set fun_stage = value for next stage
			// set tool_type = 1(listbox) /  2(textbox_num) /  3(textbox_alphanum)
		    fun_stage = 2;		tool_type = 3;	    //sub_fun_state = 1;

			//init variables for textbox_num / textbox_alphanum
		    textbox_char_encoded_trancated = 3;	textbox_size = 10;//strlen(loginpasswords[fun_result1]);
//			usnprintf(edit_tempstr, 11, "          ");
			strcpy((char *)edit_tempstr, "          ");

			// Check if first char of the editing mode
			textbox_set_initial_cursrpos();		//$$NR$$//dated:23Aug15
			numkey_pressed_flag = false;	prev_key_code = 0;	numkey_press_cnt = 0;			//$$NR$$//dated:23Aug15
			typingdelay_elapsed_flag = false;	typingdelay_count = 0;

			fun_line_addr += 10;	//fun_col_addr = 66;

			goto_screen_map_yz(fun_col_addr, fun_line_addr);
			GLCD_WriteString(edit_tempstr, true);

		    GLCD_MsgLine_Update(fun_col_addr, fun_line_addr - 11, 126, fun_line_addr + 9);
		    //$$NR$$//dated:30Aug15
		}
	}
	else if (fun_stage == 2)
	{
//		key_code = edit_text_numeric(key_code);
		key_code = edit_text_alphanumeric(key_code);

		if (key_code == 0)	return;

		if (key_code == kbEnter)
		{
			GetFunFlag = 0;		//$$NR$$//dated:30Aug15

			if (strcmp((char *)edit_tempstr, (const char *)loginpasswords[fun_result1]) == 0)
			{
				access_level = listbox_index + 1;
				Init_Menu_Screen ();
				return;
			}
			else
			{
				goto_screen_map_yz(3, fun_line_addr - 10);
			    GLCD_WriteString("  Wrong Password   ", false);
			    goto_screen_map_yz(3, fun_line_addr);
			    GLCD_WriteString("Please, Re - Enter ", false);

			    GLCD_MsgLine_Update(0, fun_line_addr - 11, 126, fun_line_addr + 9);

			    //delay_sec(2);
			    Task_sleep(2000);
				ShowPasswordScreen();
				return;
			}
		}
	}

	if (key_code == kbBack)
	{
		GetFunFlag = 0;		//$$NR$$//dated:30Aug15
		ShowHomeMode();		selected_ps_keycode = 0;
	}
}



