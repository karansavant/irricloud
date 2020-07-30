/********************************************************************************/
/*  ======== database.h ========												*/
/*  Defines all the datastructures required for the program						*/
/********************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __DATABASE_REMOTE_H
#define __DATABASE_REMOTE_H

/********************************************************************/

#include "database_struct.h"
#include "GlobalConst.h"

/********************************************************************/
// Variables used for Global purpose
/********************************************/

/********************************************************************/
// Variables related to setup (System Settings / Advanced)

char
	contact_phone_remote[2][21] = {"   +91 1234567890   ", "   +91 1234567890   "},
	contact_email_remote[21]	 = " info@infosavant.com";

uint8_t
	datetimesource_remote = 0,				// 0= Manual / 1=Computer / 2=NTP Server
	serverdate_remote[3] = {01, 01, 14},
	servertime_remote[3] = {12, 01, 0};

userdef_tm	server_time_remote;

uint8_t
	noopmodules_remote = 1,			// 0= 8 ports, 1= 16 ports
	fertigation_remote = 0;

uint32_t
	flowperpulse_remote = 111;			// Flow volume per pulse

float
	minop_remote = 22.2,				// min operating pressure
	maxdp_remote = 00.0;				// max delta pressure

uint8_t
	language_remote = 0,				// 0=English
	contrast_remote = 0,				//-15 to +15
	disp_clockformat_remote = 0,
	dateformat_remote = 0,
	beginweek_remote = 0;

/********************************************/
// Variables related to backflush

uint8_t		backflushstatus_remote=0;
uint8_t		backflushinterval_remote[2]={0};
uint16_t	backflushduration_remote=0;
uint16_t	backflushdelay_remote=0;
uint8_t		deltapres_remote=0;
uint16_t	deltadelay_remote=0;
uint8_t		deltaitera_remote=0;
uint16_t	dwelltime_remote=0;

//GSM Settings
//Network Data
char wireless_mode_remote = 2;// 0=Local Wifi, 1=Internet Wifi, 2=Internet GSM

//GSM Settings
char gsm_module_remote = 0; // 0=Disconnected, 1=Connected
char gsm_apn_remote[20] = {0};// Store the operator defined access point name
char gsm_operator_remote[20] = {0};// Operator Name
char sig_strength_remote=0; // Store the signal strength
//WIFI Settings
char wifi_module_remote = 0; // 0=Disconnected, 1=Connected
//Local Mode Parameters
char wifi_name_remote[20] = {0};// Store the device SSID for local mode
char wifi_pass_remote[20] = {0};// Store the device password for local mode
//Internet Mode Parameters
char ext_apn_remote[20] = {0};// Store the SSID of the external device to be connected to
char ext_pass_remote[20] = {0};// Store the password of the external device to be connected to

char portal_ip_remote[4] = {0};
char portal_name_remote[40] = {0};
/********************************************/

char
	loginpasswords_remote[4][11] = {pwAL1, pwAL2, pwAL3, pwAL4};



/********************************************/
// Variables related to setup menu

// This defines object for o/p Ports
op_data	output_remote[OUTPUT_MAX] = {0};

// This defines object for i/p Ports
ip_data	input_remote[INPUT_MAX] = {0};

// This defines parameters for sensor Ports
sensor_data	sensor_remote[SENSOR_MAX] = {0};

/*********************************************************/
// Variables related to program menu

// This defines object for fertigation prog
ferti_prog_data	fertiprog_remote[FERTI_PROG_MAX] = {0};

// This defines object for zone program
zone_prog_data	zoneprog_remote[ZONE_PROG_MAX] = {0};



#endif

