/********************************************************************************/
/*  ======== database.h ========												*/
/*  Defines all the datastructures required for the program						*/
/********************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __DATABASE_H
#define __DATABASE_H

/********************************************************************/

#include "database_struct.h"

/********************************************************************/
// Define Local Constants
static char
//	alphanums[10][6]	= {" ()0", ".-/:1", "ABC2", "DEF3", "GHI4", "JKL5", "MNO6", "PQRS7", "TUV8", "WXYZ9"},
	alphanums[10][6]	= {" ()0", ".-/:1", "abc2", "def3", "ghi4", "jkl5", "mno6", "pqrs7", "tuv8", "wxyz9"},
	alphalimits[10]		= {4, 5, 4, 4, 4, 4, 4, 5, 4, 5};

static char
	*loginuser_list[4] = {/*"Normal  ",*/ "   Admin", "Engineer", "   Owner", "Operator"};
	
static char
	*virtual_list[1]	= {" "},
//	*noopmodules_list[2] = {" 8 PORTS", "16 PORTS"},
	*language_list[1]	= {"ENGLISH"},
	*datetimesource_list[3]	= {"    Manual", "  Computer", "NTP Server"},
	*dateformat_list[2] = {"DD/MM/YY", "MM/DD/YY"},
	*disp_clockformat_list[2]= {"12 HOUR", "24 HOUR"},
	*timeslot_list[2] = {"AM", "PM"},
	*beginweek_list[7]	= {"   SUNDAY", "   MONDAY", "  TUESDAY", "WEDNESDAY", " THURSDAY", "   FRIDAY", " SATURDAY"},
	*g_ppcDay[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};	// Lookup table to convert numerical value of a month into text.

static char
	*optype_list[6]	= {"       NONE", "MASTER PUMP", "FLUSH VALVE", "       ZONE", " FERTI PUMP", "FERTI VALVE"},
	*iptype_list[4]	= {"   NONE", "DIGITAL", " ANALOG", " SERIAL"},
	*ipname_pump_list[3]	= {"         NONE", " MAIN PUMP OK", "FERTI PUMP OK"},		//$$NR$$//dated:16Aug15
	*ipname_fmeter_list[3]	= {"         NONE", "   FLOW METER", "NO FLOW METER"},		//$$NR$$//dated:16Aug15

//	*sensortype_list[7]	= {" NONE  ", " RAIN  ", " TEMP  ", " HUMID ", " FLOW  ", "I PRESR", "O PRESR"},
	*sensortype_list[7]	= {"       RAIN", "TEMPERATURE", "   HUMIDITY", "       FLOW", "IP PRESSURE", "OP PRESSURE"},
	*unitname_rain_list[2]	= {" NONE", "   mm"},			//$$NR$$//dated:16Aug15
	*unitname_temp_list[3]	= {" NONE", "deg C","deg F"},			//$$NR$$//dated:16Aug15
	*unitname_humid_list[2]	= {" NONE", " % RH"},			//$$NR$$//dated:16Aug15
	*unitname_flow_list[5]	= {" NONE", " Litr","   m3", "  ft3"," Galn",},			//$$NR$$//dated:16Aug15
	*unitname_presr_list[3]	= {" NONE", "  Bar", "Kg/m2"},	//$$NR$$//dated:16Aug15
	*status_list[2] = {"DISABLE", "ENABLE "},
	*state_list[2] = {"OFF", "ON "};

static char
	*progtype_list[2] = {"TIME BASED", "FLOW BASED"},
	*method_list[2] = {"        BULK", "PROPORTIONAL"};

static char
	*precond_list[2] = {"INDEPENDANT", "  DEPENDANT"},
	*rundays_list[3] = {" DAYWISE", "ODD/EVEN", "  CUSTOM"},
	*rundays_daywise_list[2] = {" ", "!"},
	*rundays_oddeven_list[2] = {" ODD", "EVEN"};

static char
	*wireless_mode_list[3] = {"LOCAL WIFI   ", "INTERNET WIFI", "INTERNET GSM "},
	*log_wireless_mode_list[3] = {"LOCAL WIFI", "I'NET WIFI", "I'NET GSM "},		//$$NR$$//dated:23Aug15
	*wireless_module_state_list[2] = {"CONNECTED   ", "DISCONNECTED"},
	*wifi_mode_list[2] = {"LOCAL   ", "INTERNET"};

static char
	*automanmode_list[2] = {"AUTO  ", "MANUAL"};

static char
	*log_mode_list[2] = {"AUTO", "MAN "},
	*log_progtype_list[2] = {"TIME", "FLOW"};
//	*log_state_list[2] = {"OFF", "ON "};

/********************************************************************/
// Variables used for Global purpose
/********************************************/

/********************************************************************/
// Variables related to setup (System Settings / Advanced)

char
	contact_phone[2][21] = {"   +91 1234567890   ", "   +91 1234567890   "},
	contact_email[21]	 = " info@infosavant.com";

uint8_t
	datetimesource = 0,				// 0= Manual / 1=Computer / 2=NTP Server
	serverdate[3] = {01, 01, 14},
	servertime[3] = {12, 01, 0};

userdef_tm	server_time;

uint8_t
	noopmodules = 1,			// 0= 8 ports, 1= 16 ports
	fertigation = 0;

uint32_t
	flowperpulse = 111;			// Flow volume per pulse

float
	minop = 22.2,				// min operating pressure
	maxdp = 00.0;				// max delta pressure

uint8_t
	language = 0,				// 0=English
	contrast = 0,				//-15 to +15
	disp_clockformat = 0,
	dateformat = 0,
	beginweek = 0;

/********************************************/
// Variables related to backflush

uint8_t		backflushstatus;
uint8_t		backflushinterval[2];
uint16_t	backflushduration;
uint16_t	backflushdelay;
uint8_t		deltapres;
uint16_t	deltadelay;
uint8_t		deltaitera;
uint16_t	dwelltime;

/********************************************/

char
	loginpasswords[4][11] = {pwAL1, pwAL2, pwAL3, pwAL4};

char
	model[13]={0},
	deviceid[11]={0};				// device unique id

float
	firmwareversion;			// xx.y

/********************************************/
// Variables related to network setting
char
	enetmacid[11] = "",
	wifimacid[11] = "",
	imeino[11]	  = "";

//Network Data
char wireless_mode = 2;// 0=Local Wifi, 1=Internet Wifi, 2=Internet GSM
char wifi_mode = 0;	   // 0=Local, 1=Internet
//GSM Settings
char gsm_module = 0; // 0=Disconnected, 1=Connected
char gsm_apn[20] = {0};// Store the operator defined access point name
char gsm_operator[20] = {0};// Operator Name
char sig_strength=0; // Store the signal strength
//WIFI Settings
char wifi_module = 0; // 0=Disconnected, 1=Connected
//Local Mode Parameters
char wifi_name[20] = {0};// Store the device SSID for local mode
char wifi_pass[20] = {0};// Store the device password for local mode
//Internet Mode Parameters
char ext_apn[20] = {0};// Store the SSID of the external device to be connected to
char ext_pass[20] = {0};// Store the password of the external device to be connected to
char portal_ip[4] = {0};
char portal_name[40] = {0};

/********************************************/
// Variables related to program status
// Used to save program status to eeprom in case of power failure

char interrupted_prog=0;//0 if no prog interrupted else the prog number value
uint16_t remtim[6]={0};
char currentday=0;
/*********************************************************/
// Variables related to change in data
char table_changed[15]={0};


/********************************************/
// Variables related to setup menu

// This defines object for o/p Ports
op_data	output[OUTPUT_MAX] = {0};

// This defines object for i/p Ports
ip_data	input[INPUT_MAX] = {0};

// This defines parameters for sensor Ports
sensor_data	sensor[SENSOR_MAX] = {0};

/*********************************************************/
// Variables related to program menu

// This defines object for fertigation prog
ferti_prog_data	fertiprog[FERTI_PROG_MAX] = {0};

// This defines object for zone program
zone_prog_data	zoneprog[ZONE_PROG_MAX] = {0};

/*********************************************************/
// Variables related to manual menu

uint8_t	manual_mode_enabled = 0;		//0-auto / 1-manual

// This defines object for manual program
man_prog_data	manprog[MAN_PROG_MAX] = {0};

// This defines object for manual o/p Ports
man_output_data	manoutput[OUTPUT_MAX] = {0};

/*********************************************************/
// Variables related to diagnostic menu

// This defines object for alerts
//alert_data	alert[ALERT_MAX] = {0, 0, 0, 1, 1, 1, 0, 1, 1, 1};
alert_data	alert[ALERT_MAX] = {0};


/*********************************************************/
// Variables related to log menu

// This defines object for logs
//log_data			log[LOGS_MAX];
char log_buffer[1000]={0};
char log_temp[100]={0};
//char log_buffer[1000]={"18/05/15,23:22:33,1,0,3,11,10,1,2,2,2,2\r\n"};

#endif



/********************************************************************/
// Variables used for Home Screen or Status Variables

uint8_t
	cntrl_homescrn_type = 0;			// 0-home / 1-program status / 2-sensor status / 3-output status

uint8_t
	nextprog_time[2] = {0};				// hh:mm

uint8_t
	cntrl_mode=0,						// 0-Auto / 1-Manual /2-Program
	access_mode=0,						// 0-Keypad /1-Wifi / 2-GSM / 3-Ethernet
	cntrl_state=1,						// 0-running / 1-standby
	cur_prog = 0,						// current program number
	cur_prog_state = 1,					// 0-running / 1-standby
	cur_prog_elapsedtime[2] = {0};

//$$NR$$//dated:6Feb16
uint16_t
    ippressure_curvalue = 0,
    oppressure_curvalue = 0,
    diffpressure_curvalue = 0,
    temperature_curvalue = 0,
    humidity_curvalue = 0,
    sensor_errors = 0,
    flow_curvalue = 0;

uint8_t
    rain_curvalue = 0,
    sensor_error_bit = 0,
    limit_status_on_dP = 0;
//$$NR$$//dated:6Feb16

uint8_t
	output_state[OUTPUT_MAX] = {0};		// 0-OFF / 1-ON

uint8_t
	input_state[INPUT_MAX] = {0};		// 0-OFF / 1-ON

uint8_t
	localaccessflag=2;


/******************************************************************************/
