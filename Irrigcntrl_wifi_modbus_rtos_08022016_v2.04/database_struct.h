/********************************************************************************/
/*					======== database_struct.h ========							*/
/*  Defines all the data structures required for the controller					*/
/********************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __DATABASE_STRUCT_H
#define __DATABASE_STRUCT_H

/********************************************************************/

#define	OUTPUT_MAX		16
#define	INPUT_MAX		4
#define	SENSOR_MAX		6

#define	FERTI_PROG_MAX	6
#define	ZONE_PROG_MAX	6

#define	MAN_PROG_MAX	6
#define	MAN_ZONE_MAX	6

#define	ALERT_MAX		10

#define	LOGS_MAX		50

//LOG TYPES
#define	 IRRIGATION_LOG	1
#define	 EVENT_LOG		2
#define	 SENSOR_LOG		3
#define  SYSTEM_LOG		4
#define	 ERROR_LOG		5

//SYSTEM LOG SUBTYPES
#define	 SETTINGS		1
#define  ADVANCED		2
#define	 FERTIGATION	3
#define	 ZONEPROG		4

//ERROR TYPE
#define	 POWER_FAILURE	 1
#define	 BATTERY_FAILURE 2
#define	 EEPROM_FAILURE	 3
#define	 USB_FAILURE	 4
#define	 MPUMP_FAILURE	 5
#define	 FPUMP_FAILURE	 6
#define	 TEMP_FAILURE	 7
#define	 HUMI_FAILURE	 8
#define	 FLOW_FAILURE	 9
#define	 PRES1_FAILURE	 10
#define	 PRES2_FAILURE	 11
#define	 RAIN_FAILURE	 12

/*
 STRUCTURE FOR LOGS

 IRRIGATION LOG:
 DATE,TIME,LOG TYPE,MODE(AUTO/MANUAL),PROG TYPE,PROG NUMBER,OPNUMBER,DURATION/FLOW(MIN/LTRS),FERTI PROG,
 FT-VALVE 1- DURATION/FLOW,FT-VALVE 2- DURATION/FLOW,FT-VALVE 3- DURATION/FLOW,
 FT-VALVE 4- DURATION/FLOW<CR><LF>

 EX:
 18/05/15,23:22:33,1,0,3,11,10,1,2,2,2,2/r/n

 RECORD LOG AT THE END OF IRRIGATION CYCLE EX. MANUAL OFF ,MANUAL PROGRAM OFF, ZONE PROG OFF

 EVENT LOG:
 DATE,TIME,LOG TYPE,OPNUMBER,OPSTATUS<CR><LF>

 EX:
 18/05/15,23:22:33,2,13,1/r/n

 SENSOR LOG:
 DATE,TIME,LOG TYPE,SENSOR ID,SENSOR VALUE<CR><LF>

 EX:
 18/05/15,23:22:33,3,1,222/r/n

 SYSTEM LOG:
 DATE,TIME,LOG TYPE,USER,MODE,SUBTYPE,SUBMENU<CR><LF>

 EX:
 18/05/15,23:22:33,4,1,2,1,2/r/n

 ERROR LOG:
 DATE,TIME,LOG TYPE,ERROR TYPE<CR><LF>

 EX:
 18/05/15,23:22:33,5,4/r/n
 */


/********************************************************************/
// Variables used for Global purpose
/********************************************/

typedef struct ud_time
{
	uint8_t tm_24hr;	// hours after the midnight   - [0,23]
	uint8_t tm_hour;	// hours after the midnight   - [0,23]
	uint8_t tm_min;		// minutes after the hour     - [0,59]
	uint8_t tm_sec;		// seconds after the minute   - [0,59]
	uint8_t tm_mday;	// day of the month           - [1,31]
	uint8_t tm_mon;		// months since January       - [0,11]
	uint8_t tm_year;	// years since 1900
	uint8_t tm_wday;	// days since Sunday          - [0,6]
	uint8_t tm_slot;	// time slot          		  - [0,1]

//	uint16_t tm_yday;	// days since Jan 1st         - [0,365]
//	uint8_t tm_isdst;	// Daylight Savings Time flag

}userdef_tm;

/********************************************************************/
// Variables related to setup menu
/*************************************************/
// This structure defines parameters for o/p Ports

typedef	struct op_struct
{
//	uint8_t opid;				// NA // A unique o/p id number
	uint8_t optype;				// 0= NONE, 1= MASTER PUMP, 2= FLUSH VALVE, 3= ZONE, 4= FERTI PUMP, 5= FERTI VALVE
	uint8_t typeid;				// MASTER PUMP (1) / FLUSH VALVE / ZONE / FERTI PUMP (1) / FERTI VALVE (1-4)
	char opname[11];			// o/p name
}op_data;

/*************************************************/
// This structure defines parameters for i/p Ports
typedef	struct ip_struct
{
//	uint8_t ipid;				// NA // A unique i/p id number
	uint8_t iptype;				// 0=None, 1=MainPumpOK/Flowmeter, 2=FertiPumpOK/NoFlowmeter	//$$NR$$//dated:16Aug15
//	uint8_t typeid;				// Type wise ID
//	char ipname[11];			// i/p name														//$$NR$$//dated:16Aug15
//	float currentval;
}ip_data;

/*************************************************/
// Variables related to sensor

typedef	struct sensor_struct
{
	uint8_t serialid;			// A unique i/p id number
//	uint8_t iptype;				// 0=None, 1=Digital, 2=Analog, 3=Serial
	uint8_t sensortype;			// 0=Rain / 1=Temp / 2=Humid / 3=Flow / 4=I Presr / 5=O Presr		// 0=None / 1=Rain / 2=Temp / 3=Humid / 4=Flow / 5=I Presr / 6=O Presr
	uint8_t typeid;				// Type wise ID
	float min;
	float max;
//	char unit[11];																				//$$NR$$//dated:16Aug15
	uint8_t unittype;			// Unit Type													//$$NR$$//dated:16Aug15
//	char sensorname[11];		// sensor name
//	float currentval;
}sensor_data;
/*********************************************************/
// Variables related to program menu

/*************************************************/
// This structure defines parameters for valve

typedef struct valve_struct
{
	uint8_t	 oppartno;
	uint16_t starttime;			// variable for program based//0 - 100%
	uint16_t stoptime;			// variable for program based//0 - 100%
	uint32_t startflow;			// variable for flow based
	uint32_t stopflow;			// variable for flow based
}valve_data;

/*************************************************/
// This structure defines parameters for fertigation prog

typedef struct ferti_prog_struct
{
	uint8_t		id;
	uint8_t		type;			// 0=time based / 1=flow based
	uint8_t		method;			// 0=bulk / 1=Proportional
	uint8_t		propcycles;
	uint16_t	startdelay;
	uint16_t	stopdelay;
	uint8_t 	valvecount;
	valve_data	valve[4];
	uint16_t	totaltime;
	uint32_t	totalflow;
}ferti_prog_data;

/*************************************************/
// This structure defines parameters for sensors

typedef	struct analog_sensor_struct
{
	uint8_t	condition;			// Sensor preconditions 0=independant / 1=dependant
	float	min;				// Minimun Sensor Value
	float	max;				// Maximum Sensor Value
	float	currentval;			// current value of sensor
}analog_sensor_data;

/*************************************************/
// This structure defines parameters for sensors

typedef	struct digital_sensor_struct
{
	uint8_t	condition;			// Sensor preconditions 0=independant / 1=dependant
	float	currentval;			// current value of sensor
}digital_sensor_data;

/*************************************************/
// This structure defines parameters for zone

typedef struct zone_struct
{
	uint8_t oppartno;
	uint8_t starttime[2];		// variable for program based
	uint8_t stoptime[2];		// variable for program based
	uint32_t startflow;			// variable for flow based
	uint32_t stopflow;			// variable for flow based
//	uint8_t delay;
	uint8_t fertprogid;
}zone_data;

/*************************************************/
// This structure defines parameters for zone program

typedef struct zone_prog_struct
{
	uint8_t	id;						// Fixed Program ID's (Values from 1-6)
	uint8_t	type;					// 0=time based / 1=flow based
	uint8_t	status;					// program 0=disable / 1=enable

	uint8_t 	repeatcycles;
	uint16_t	cycledelay;

	digital_sensor_data	rain;
	analog_sensor_data	temperature;
	analog_sensor_data	humidity;
	digital_sensor_data	flow;
	analog_sensor_data	oppressure;
	analog_sensor_data	ippressure;

	uint8_t		rundaysmode;		// 0=daywise / 1= oddeven / 2=custom
	uint8_t		rundaysval[3];		// for daywise (rundaysval[0]),	bit	7  6  5  4  3  2  1  0
									//									x  S  M  T  W  T  F  S		(bit value - 0=not selected / 1=selected)
									// for oddeven (rundaysval[1]), 0=odd / 1=even
									// for custom  (rundaysval[2]), every xth day from week begin

	uint8_t		starttime[2];
	uint8_t 	zonecount;
	zone_data	zone[10];
	uint8_t		totaltime[2];
	uint16_t	totalflow;
	uint16_t	pumpdelay;
	uint16_t	zonedelay;
}zone_prog_data;

/*********************************************************/
// Variables related to manual menu

typedef struct man_prog_struct
{
//	uint8_t	id;						// Fixed Program ID's (Values from 1-6)
//	uint8_t	type;					// 0=time based / 1=flow based
//	uint8_t	status;					// program 0=disable / 1=enable
	uint8_t	state;					// program 0=OFF / 1=ON
//	uint8_t		starttime[2];
//	uint8_t		totaltime[2];
}man_prog_data;


typedef struct man_output_struct
{
//	uint8_t	id;						// Fixed Program ID's (Values from 1-6)
//	uint8_t	type;					// 0=time based / 1=flow based
//	uint8_t	status;					// program 0=disable / 1=enable
	uint8_t	state;					// output 0=OFF / 1=ON
//	uint8_t		starttime[2];
//	uint8_t		totaltime[2];
}man_output_data;

/*********************************************************/
// Variables related to diagnostic menu

/*************************************************/
// This structure defines parameters for alerts
typedef struct alert_struct
{
//	uint8_t alertno;
	uint8_t logdate[3];			// variable for alert log date
	uint8_t logtime[3];			// variable for alert log time
	uint8_t	status;				// variable for alert status
}alert_data;

/*********************************************************/
// Variables related to logs menu

/*************************************************/
// This structure defines parameters for logs
typedef struct log_struct
{
//	uint8_t logno;
	uint8_t logdate[3];			// variable for log date
	uint8_t logtime[3];			// variable for log time
	uint8_t	iotype;				// variable for i/o type
	uint8_t	iopartno;			// variable for i/o part no
	uint8_t	status;				// variable for i/o status
}log_data;

#endif


/******************************************************************************/
