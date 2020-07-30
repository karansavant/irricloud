
/****************************************************************************/
/*						Menu Constants And Declarations						*/
/****************************************************************************/

#ifndef MENU_DEFS
#define MENU_DEFS

#include "GlobalConst.h"

//$$NR$$//dated:07Feb16

/********************************************************/
// menu structure
typedef struct Menu_Struct
{
	uint8_t
		MenuState;
	char
		*MenuTitle;
	uint8_t
		MenuCount;			// No of counts for menus
	uint8_t
		FunCount;			// The largest function number in the menu trees array.
    uint8_t
        AccessRight;        // userwise access rights (bit 4 - alOperator / bit 3 - alOwner / bit 2 - alEngr / bit 1 - alAdmin / bit 0 - alNorm)
}menu_data;


// menu tree structure
typedef struct Menu_Tree_Struct
{
	uint8_t
		FunNo;
	char
		*MenuLabel;
	uint32_t
		MenuIndex;
	uint8_t
		Max_Index;
    uint8_t
        AccessRight;        // userwise access rights (bit 4 - alOperator / bit 3 - alOwner / bit 2 - alEngr / bit 1 - alAdmin / bit 0 - alNorm)
}menu_tree_data;

/********************************************************/
// pointer for selected menu group & menu tree
menu_data	*menu;
menu_tree_data *menu_tree;

/********************************************************/
// menu tree details for Home
#define	HOME_MENU_COUNT_MAX		8

static const menu_data	menu_Home = {psHome, "HOME", HOME_MENU_COUNT_MAX, 1, 0x1F};	// menu state / menu title / menu count / function count

static const menu_tree_data menu_tree_Home[HOME_MENU_COUNT_MAX] = {
		{ 1, "RTC Time", 		0x00000001, 8, 0x1F},
		{ 1, "ON/OFF Control",	0x00000002, 8, 0x1F},
		{ 1, "Run Status", 		0x00000003, 8, 0x1F},
		{ 1, "Zone Status", 	0x00000004, 8, 0x1F},
		{ 1, "Rain Status", 	0x00000005, 8, 0x1F},
		{ 1, "Flow Status", 	0x00000006, 8, 0x1F},
		{ 1, "Temperature", 	0x00000007, 8, 0x1F},
		{ 1, "Humidity", 		0x00000008, 8, 0x1F}
};//todo check if useful  //$$NR$$//dated:19Feb16

/********************************************************/
// menu tree details for Setup
#define	SETUP_MENU_COUNT_MAX		23

static const menu_data	menu_Setup = {psSetup, "SETUP", SETUP_MENU_COUNT_MAX, 19, 0x0F};	// menu state / menu title / menu count / function count

static const menu_tree_data menu_tree_Setup[SETUP_MENU_COUNT_MAX] = {
		{  0, "System Settings    >", 0x00000001, 2, 0x1F},
		{  0, "Advanced           >", 0x00000002, 2, 0x1F},

		{  1, "Contact Info        ", 0x00000011, 6, 0x1F},
		{  2, "Set Time/Date       ", 0x00000012, 6, 0x1F},
		{  3, "Load From USB       ", 0x00000013, 6, 0x1F},
		{  4, "Save to USB         ", 0x00000014, 6, 0x1F},
		{  5, "Hardware Config     ", 0x00000015, 6, 0x1F},
		{  6, "System Setup        ", 0x00000016, 6, 0x1F},

		{  7, "O/P Assignment      ", 0x00000021, 8, 0x1F},
		{  8, "I/P Assignment      ", 0x00000022, 8, 0x1F},
		{  9, "Sensor Assignment   ", 0x00000023, 8, 0x1F},
		{  0, "Network Settings   >", 0x00000024, 8, 0x1F},
		{ 10, "Backflush           ", 0x00000025, 8, 0x1F},
		{  0, "Authentication     >", 0x00000026, 8, 0x1F},
		{ 11, "Firmware            ", 0x00000027, 8, 0x1F},
		{ 12, "Factory Reset       ", 0x00000028, 8, 0x1F},

		{ 13, "Wireless            ", 0x00000241, 3, 0x1F},
		{ 14, "GSM                 ", 0x00000242, 3, 0x1F},
		{ 15, "WiFi                ", 0x00000243, 3, 0x1F},
//		{ 99, "Zigbee              ", 0x00000243, 4, 0x1F},//16
//		{ 99, "CAN                 ", 0x00000244, 4, 0x1F},//17

		{ 20, "Administrator       ", 0x00000261, 4, 0x1F},
		{ 21, "Engineer            ", 0x00000262, 4, 0x1F},
		{ 22, "Farm Owner          ", 0x00000263, 4, 0x1F},
		{ 23, "Operator            ", 0x00000264, 4, 0x1F}
};

/********************************************************/
// menu tree details for Program
#define	PROGRAM_MENU_COUNT_MAX		32

static const menu_data	menu_Program = {psProgram, "PROGRAM", PROGRAM_MENU_COUNT_MAX, 17, 0x1F};	// menu state / menu title / menu count / function count

static const menu_tree_data menu_tree_Program[PROGRAM_MENU_COUNT_MAX] = {
		{ 0,  "Fertigation        >", 0x00000001, 2, 0x1F},
		{ 0,  "Zone               >", 0x00000002, 2, 0x1F},

		{ 0,  "Ferti Program 1    >", 0x00000011, 6, 0x1F},
		{ 0,  "Ferti Program 2    >", 0x00000012, 6, 0x1F},
		{ 0,  "Ferti Program 3    >", 0x00000013, 6, 0x1F},
		{ 0,  "Ferti Program 4    >", 0x00000014, 6, 0x1F},
		{ 0,  "Ferti Program 5    >", 0x00000015, 6, 0x1F},
		{ 0,  "Ferti Program 6    >", 0x00000016, 6, 0x1F},

		{ 0,  "Zone Program 1     >", 0x00000021, 6, 0x1F},
		{ 0,  "Zone Program 2     >", 0x00000022, 6, 0x1F},
		{ 0,  "Zone Program 3     >", 0x00000023, 6, 0x1F},
		{ 0,  "Zone Program 4     >", 0x00000024, 6, 0x1F},
		{ 0,  "Zone Program 5     >", 0x00000025, 6, 0x1F},
		{ 0,  "Zone Program 6     >", 0x00000026, 6, 0x1F},

		{ 31, "Method              ", 0x00000111, 4, 0x1F},
		{ 32, "Delays              ", 0x00000112, 4, 0x1F},
		{ 33, "Valve Details       ", 0x00000113, 4, 0x1F},
		{ 34, "Clear Program       ", 0x00000114, 4, 0x1F},

		{ 35, "Status              ", 0x00000211, 8, 0x1F},
		{ 36, "Run Time            ", 0x00000212, 8, 0x1F},
		{  0, "Precondition       >", 0x00000213, 8, 0x1F},
		{ 37, "Run Days            ", 0x00000214, 8, 0x1F},
		{ 38, "Start Time          ", 0x00000215, 8, 0x1F},
		{ 39, "Zone Details        ", 0x00000216, 8, 0x1F},
//		{ 36, "Total Runtime       ", 0x00000216, 8, 0x1F},
		{ 40, "Delays              ", 0x00000217, 8, 0x1F},
		{ 41, "Clear Program       ", 0x00000218, 8, 0x1F},

		{ 42, "Rain                ", 0x00002131, 6, 0x1F},
		{ 43, "Temperature         ", 0x00002132, 6, 0x1F},
		{ 44, "Humidity            ", 0x00002133, 6, 0x1F},
		{ 45, "Flow                ", 0x00002134, 6, 0x1F},
		{ 46, "Output Pressure     ", 0x00002135, 6, 0x1F},
		{ 47, "Input Pressure      ", 0x00002136, 6, 0x1F}
};

/********************************************************/
// menu tree details for Manual
#define	MANUAL_MENU_COUNT_MAX		4

static const menu_data	menu_Manual = {psManual, "MANUAL", MANUAL_MENU_COUNT_MAX, 3, 0x1F};	// menu state / menu title / menu count / function count

static const menu_tree_data menu_tree_Manual[MANUAL_MENU_COUNT_MAX] = {
		{  0, "Auto                ", 0x00000001, 2, 0x1F},	//50
		{  0, "Manual              ", 0x00000002, 2, 0x1F},

		{ 51, "Program             ", 0x00000021, 2, 0x1F},
		{ 52, "Output              ", 0x00000022, 2, 0x1F}
};

/********************************************************/
// menu tree details for Logs
#define	LOGS_MENU_COUNT_MAX		4

static const menu_data	menu_Logs = {psLogs, "LOGS", LOGS_MENU_COUNT_MAX, 4, 0x1F};	// menu state / menu title / menu count / function count

static const menu_tree_data menu_tree_Logs[LOGS_MENU_COUNT_MAX] = {
		{ 60, "Irrigation Logs     ", 0x00000001, 4, 0x1F},	//60
		{ 61, "Event Logs          ", 0x00000002, 4, 0x1F},
		{ 62, "Sensor Logs         ", 0x00000003, 4, 0x1F},
		{ 63, "System Logs         ", 0x00000004, 4, 0x1F}
};

/********************************************************/
// menu tree details for Diagnostic
//$$NR$$//dated:01Nov15
// menu tree details for Diagnostic
#define	DIAGNOSTIC_MENU_COUNT_MAX		23	//13

//static const menu_data	menu_Diagnostic = {psDiagnostic, "DIAGNOSTIC", DIAGNOSTIC_MENU_COUNT_MAX, 12};	// menu state / menu title / menu count / function count
static const menu_data	menu_Diagnostic = {psDiagnostic, "DIAGNOSTIC", DIAGNOSTIC_MENU_COUNT_MAX, 21, 0x1F};	// menu state / menu title / menu count / function count

static const menu_tree_data menu_tree_Diagnostic[DIAGNOSTIC_MENU_COUNT_MAX] = {
		{  0, "Alerts             >", 0x00000001, 2, 0x1F},	//70
		{  0, "Tests              >", 0x00000002, 2, 0x1F},

		{ 71, "Clear All           ", 0x00000011,11, 0x1F},
		{ 72, "Short Circuit       ", 0x00000012,11, 0x1F},
		{ 73, "Over-Current        ", 0x00000013,11, 0x1F},
		{ 74, "Low Battery         ", 0x00000014,11, 0x1F},
		{ 75, "Low AC Power        ", 0x00000015,11, 0x1F},
		{ 76, "Ethernet Error      ", 0x00000016,11, 0x1F},
		{ 77, "USB Error           ", 0x00000017,11, 0x1F},
		{ 78, "GSM Error           ", 0x00000018,11, 0x1F},
		{ 79, "CAN Error           ", 0x00000019,11, 0x1F},
		{ 80, "Zigbee Error        ", 0x0000001A,11, 0x1F},
		{ 81, "Wi-Fi Error         ", 0x0000001B,11, 0x1F},

		{ 82, "Keypad Test         ", 0x00000021, 10, 0x1F},
		{ 83, "Display Test        ", 0x00000022, 10, 0x1F},
		{ 84, "Buzzer Test         ", 0x00000023, 10, 0x1F},
		{ 85, "Output Test         ", 0x00000024, 10, 0x0F},
		{ 86, "Input Test          ", 0x00000025, 10, 0x1F},
		{ 87, "Sensor Test         ", 0x00000026, 10, 0x1F},
		{ 88, "USB Test            ", 0x00000027, 10, 0x1F},
		{ 89, "WiFi Local Test     ", 0x00000028, 10, 0x0F},
		{ 90, "WiFi Internet Test  ", 0x00000029, 10, 0x0F},
		{ 91, "GSM Test            ", 0x0000002A, 10, 0x1F}
};
//$$NR$$//dated:07Feb16

/********************************************************/

#define Short_Circuit	0
#define	Over_Current    1
#define	Low_Battery     2
#define	Low_AC_Power    3
#define	Ethernet_Error  4
#define	USB_Error       5
#define	GSM_Error       6
#define	CAN_Error       7
#define	Zigbee_Error    8
#define	WiFi_Error 		9



#endif

