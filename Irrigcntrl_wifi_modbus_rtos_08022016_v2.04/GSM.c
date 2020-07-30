
/*
 *  ======== GSM.c ========
 */

/* Example/Board Header files */
#include "Board.h"
#include "database_remote.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <inc/hw_memmap.h>
#include <inc/hw_nvic.h>
#include <inc/hw_types.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include "driverlib/UART.h"

#include "keypad.h"

#define LOGFILE_BUFF_SIZE       20000
#define	GSM_UART_FOR_DEBUG	0		//$$NR$$//dated:23Aug15

/********************************************************************/
// Declare Global Functions
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

//void eeprom_read_network(uint8_t parameter_index);
//void eeprom_write_network(uint8_t parameter_index);
void eeprom_read_network_gsm(uint8_t parameter_index);
void eeprom_write_network_gsm(uint8_t parameter_index);
//void eeprom_read_network1(uint8_t parameter_index);
//void eeprom_write_network1(uint8_t parameter_index);
void eeprom_read_network_wifi(uint8_t parameter_index);
void eeprom_write_network_wifi(uint8_t parameter_index);
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

extern void eeprom_read_man_program(uint8_t index);
extern void eeprom_write_man_program(uint8_t index);
extern void eeprom_read_man_output(uint8_t index);
extern void eeprom_write_man_output(uint8_t index);

extern void eeprom_read_alert(uint8_t index);
extern void eeprom_write_alert(uint8_t index);

extern void eeprom_read_logs(uint8_t index);
extern void eeprom_write_logs(uint8_t index);

extern uint8_t Read_log(uint16_t read_startloc);
extern  void Read_log_portal();
extern void Remaintime_update1(uint8_t x);//x is the prog no

extern void get_default_value_fertiprog(uint8_t index);
extern void get_default_value_zoneprog(uint8_t index);
extern void eeprom_factory_reset(void);

extern void eeprom_write_progstatus(uint8_t parameter_index);
extern void eeprom_read_progstatus(uint8_t parameter_index);

extern void OP_Update(uint8_t opid,uint32_t on_off,uint8_t log);
/********************************************************************/
// Declare Local Functions

void  message_store();
void  synctoportal(char table);
void  synctowifi(char table);
int GSM_Activate();
int GSM_Configure();
void GSM_Main();
void init_gsm();
void GSM_EEPROM_UPDATE();

/********************************************************************/
// Local variables
char message[8][30]={0};	//for temporary storing the message
char temp_ip[30]={0};
char input_buffer[1000]={0};
char ouput_buffer[20000]={0};
UART_Handle uart;

extern char logfile_buff[20000];
extern volatile uint8_t PanelState;
extern uint32_t tempmsgid;
extern uint32_t noofmsg;
extern int gsm_activ;
extern int gsm_config;
extern uint8_t remote_data_recevied_flag;
extern void *netIF;
extern UART_Params uartParams;

extern int taskactive;
extern int logflag;
extern int logsend;


extern char str[20],len[10],temp[100];

extern int PrevPanelState;
extern char synccount;
extern char syncsubcount;
extern char commu_on;//1 if GSM Module in connect mode
extern char sendack;
extern char system_rst;

extern char table_changed[15];

char dial1[200] = {0};//socket dial


//const char msgfromportal1[] = "POST http://portal.avkon.in/IOT_Local/sendmessage.php HTTP/1.0\r\nContent-Type: application/x-www-form-urlencoded;charset=utf-8\r\nContent-Length: ";//21\r\n\r\ndeviceiddata=";
//const char msgtoportal1[] = "POST http://portal.avkon.in/IOT_Local/recivedata.php HTTP/1.0\r\nContent-Type: application/x-www-form-urlencoded;charset=utf-8\r\nContent-Length: ";//41\r\n\r\nackdata=*|LocalAccessOn||1||Deviceid||1|*";
//const char logtoportal1[] = "POST http://portal.avkon.in/IOT_Local/recivelogsdata.php HTTP/1.0\r\nContent-Type: application/x-www-form-urlencoded;charset=utf-8\r\nContent-Length: ";//41\r\n\r\nackdata=*|LocalAccessOn||1||Deviceid||1|*";


extern char msgtoportal[200];
extern char logtoportal[200];
extern char msgfromportal1[200];

/********************************************************************/
// Global Variables
extern userdef_tm CurTime, SetTime;
extern	zone_prog_data	zoneprog[ZONE_PROG_MAX];
extern	op_data	output[OUTPUT_MAX];
extern	ip_data	input[INPUT_MAX];
extern	sensor_data	sensor[SENSOR_MAX];
extern	ferti_prog_data	fertiprog[FERTI_PROG_MAX];
extern	man_prog_data	manprog[MAN_PROG_MAX];
extern	man_output_data	manoutput[OUTPUT_MAX];
extern int flag_30s;
extern	uint8_t	cntrl_mode;

// Variables related to setup (System Settings / Advanced)


extern uint8_t
		datetimesource;				// 0= Manual / 1=Computer / 2=NTP Server


extern uint8_t
		noopmodules,			// 0= 8 ports, 1= 16 ports
		fertigation;

extern uint32_t
		flowperpulse;			// Flow volume per pulse

extern	float
			minop,				// min operating pressure
			maxdp;				// max delta pressure

extern uint8_t
			language,				// 0=English
			contrast,				//-15 to +15
			disp_clockformat,
			dateformat,
			beginweek ;

// Variables related to backflush

extern	uint8_t		backflushstatus;
extern	uint8_t		backflushinterval[2];
extern	uint16_t	backflushduration;
extern	uint16_t	backflushdelay;
extern	uint8_t		deltapres;
extern	uint16_t	deltadelay;
extern	uint8_t		deltaitera;
extern	uint16_t	dwelltime;

extern char deviceid[11];
extern uint8_t	localaccessflag;
extern uint32_t tempmsgid;
extern uint32_t noofmsg;
extern uint8_t remote_data_recevied_flag;



//Variables related to Network Data
extern char wireless_mode;// 0=Local Wifi, 1=Internet Wifi, 2=Internet GSM

//GSM Settings
extern char gsm_module; // 0=Disconnected, 1=Connected
extern char gsm_apn[20];// Store the operator defined access point name
extern char gsm_operator[20];// Operator Name
extern char sig_strength; // Store the signal strength
//WIFI Settings
extern char wifi_module; // 0=Disconnected, 1=Connected
//Local Mode Parameters
extern char wifi_name[20];// Store the device SSID for local mode
extern char wifi_pass[20];// Store the device password for local mode
//Internet Mode Parameters
extern char ext_apn[20];// Store the SSID of the external device to be connected to
extern char ext_pass[20];// Store the password of the external device to be connected to

extern char portal_ip[4];
extern char portal_name[40];
// Variables used for Home Screen or Status Variables

extern uint8_t
	cntrl_mode,						// 0-Auto / 1-Manual /2-Program
	cntrl_state,						// 0-running / 1-standby
	cur_prog,						// current program number
	cur_prog_state,					// 0-running / 1-standby
	cur_prog_elapsedtime[2];


extern uint16_t
	oppressure_curvalue,
	ippressure_curvalue,
	temperature_curvalue;
extern uint16_t humidity_curvalue;      //$$NR$$//dated:6Feb16
extern uint8_t rain_curvalue;
extern uint16_t flow_curvalue;

extern uint8_t	output_state[OUTPUT_MAX];		// 0-OFF / 1-ON

extern uint8_t	input_state[INPUT_MAX];		// 0-OFF / 1-ON


char strmsglog[50];		//$$NR$$//dated:23Aug15

/********************************************************************/
//
/********************************************************************/

uint32_t strtoint1(char *tempstring)
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

float strtofloat1(char *tempstring)
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

char progid_clear=0,zone_clear=0,ferti_clear=0,factory_clear=0;
char cntrl_mode_change=0;
void  message_store()
{
	uint8_t progid=0,ioid=0,i=0;

	if (strcmp(message[0], "table_data")==0)
	{
		if (strcmp(message[1], "table1")==0)
		{
			synctowifi(1);
		}
		if (strcmp(message[1], "table2")==0)
		{
			synctowifi(2);
		}
		if (strcmp(message[1], "table3")==0)
		{
			synctowifi(3);
		}
		if (strcmp(message[1], "table4")==0)
		{
			synctowifi(4);
		}
		if (strcmp(message[1], "table5")==0)
		{
			synctowifi(5);
		}
		if (strcmp(message[1], "table6")==0)
		{
			synctowifi(6);
		}
		if (strcmp(message[1], "table7")==0)
		{
			synctowifi(7);
		}
		if (strcmp(message[1], "table8")==0)
		{
			synctowifi(8);
		}
		if (strcmp(message[1], "table9")==0)
		{
			synctowifi(9);
		}

	}

	if (strcmp(message[0], "1")==0)
	{
		ioid=(uint8_t)strtoint1(message[6]);	//find IO ID
		if(ioid>16)
		{
			ioid=16;
		}
		ioid = ioid-1;
		if (strcmp(message[1], "1")==0)
		{
			output_remote[ioid].optype=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "2")==0)
		{
			output_remote[ioid].typeid=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "3")==0)
		{
			strncpy(output_remote[ioid].opname,message[2],10);
		}

		if (wireless_mode == 0) {//if in local wifi mode
			table_changed[1] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);
		}
	}
	if (strcmp(message[0], "2")==0)
	{
		ioid=(uint8_t)strtoint1(message[6]);	//find IO ID

		if(ioid>4)
		{
			ioid=4;
		}
		ioid = ioid-1;

		if (strcmp(message[1], "1")==0)
		{
			//$$NR$$//dated:16Aug15
//			strncpy(input_remote[ioid].ipname,message[2],10);
			input_remote[ioid].iptype = (uint8_t)strtoint1(message[2]);
			//$$NR$$//dated:16Aug15
		}
		if (wireless_mode == 0) {//if in local wifi mode
			table_changed[1] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);
		}
	}
	if (strcmp(message[0], "3")==0)
	{
		ioid=(uint8_t)strtoint1(message[6]);	//find IO ID
		if(ioid>6)
		{
			ioid=6;
		}
		ioid = ioid-1;

		if (strcmp(message[1], "1")==0)
		{
			sensor_remote[ioid].sensortype=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "2")==0)
		{
			sensor_remote[ioid].serialid=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "3")==0)
		{
			sensor_remote[ioid].typeid=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "4")==0)
		{
			sensor_remote[ioid].min=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "5")==0)
		{
			sensor_remote[ioid].max=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "6")==0)
		{
			sensor_remote[ioid].unittype=strtoint1(message[2]);	//$$NR$$//dated:16Aug15
		}
		if (wireless_mode == 0) {//if in local wifi mode
			table_changed[2] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);
		}
	}
	if (strcmp(message[0], "4")==0)
	{
		progid=(uint8_t)strtoint1(message[5]);	//find Program ID
		if(progid>0)
		{
			progid=progid-1;
		}
		ioid=(uint8_t)strtoint1(message[6]);	//find IO ID//todo
		if(ioid>10)
		{
			ioid=10;
		}
		ioid = ioid-1;
		if (strcmp(message[1], "1")==0)
		{
			zoneprog_remote[progid].zone[ioid].oppartno=(uint8_t)strtoint1(message[2]);
			zoneprog_remote[progid].zone[ioid].oppartno=zoneprog_remote[progid].zone[ioid].oppartno-1;
		}
		if (strcmp(message[1], "3[0]")==0)
		{
			zoneprog_remote[progid].zone[ioid].starttime[0]=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "3[1]")==0)
		{
			zoneprog_remote[progid].zone[ioid].starttime[1]=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "4[0]")==0)
		{
			zoneprog_remote[progid].zone[ioid].stoptime[0]=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "4[1]")==0)
		{
			zoneprog_remote[progid].zone[ioid].stoptime[1]=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "5")==0)
		{
			zoneprog_remote[progid].zone[ioid].startflow=strtoint1(message[2]);
		}
		if (strcmp(message[1], "6")==0)
		{
			zoneprog_remote[progid].zone[ioid].stopflow=strtoint1(message[2]);
		}
		if (strcmp(message[1], "2")==0)
		{
			zoneprog_remote[progid].zone[ioid].fertprogid=(uint8_t)strtoint1(message[2]);
			//if(zoneprog_remote[progid].zone[ioid].fertprogid != 0){
			//zoneprog_remote[progid].zone[ioid].fertprogid=zoneprog_remote[progid].zone[ioid].fertprogid-1;
			//}
		}

		if (wireless_mode == 0) {//if in local wifi mode
			table_changed[4] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);
		}
	}

	if (strcmp(message[0], "5")==0)
	{
		progid=(uint8_t)strtoint1(message[5]);	//find Program ID
		if(progid>0)
		{
			progid=progid-1;
		}
		if (strcmp(message[1], "id")==0)
		{
			zoneprog_remote[progid].id=(uint8_t)strtoint1(message[2]);
			zoneprog_remote[progid].id=zoneprog_remote[progid].id-1;
		}
		if (strcmp(message[1], "1")==0)
		{
			zoneprog_remote[progid].type=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "2")==0)
		{
			zoneprog_remote[progid].status=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "3")==0)
		{
			zoneprog_remote[progid].repeatcycles=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "4")==0)
		{
			zoneprog_remote[progid].cycledelay=(uint16_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "5")==0)
		{
			zoneprog_remote[progid].rain.condition=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "6")==0)
		{
			zoneprog_remote[progid].rain.currentval=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "7")==0)
		{
			zoneprog_remote[progid].flow.condition=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "8")==0)
		{
			zoneprog_remote[progid].flow.currentval=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "9")==0)
		{
			zoneprog_remote[progid].temperature.condition=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "10")==0)
		{
			zoneprog_remote[progid].temperature.currentval=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "11")==0)
		{
			zoneprog_remote[progid].temperature.min=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "12")==0)
		{
			zoneprog_remote[progid].temperature.max=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "13")==0)
		{
			zoneprog_remote[progid].humidity.condition=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "14")==0)
		{
			zoneprog_remote[progid].humidity.currentval=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "15")==0)
		{
			zoneprog_remote[progid].humidity.min=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "16")==0)
		{
			zoneprog_remote[progid].humidity.max=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "17")==0)
		{
			zoneprog_remote[progid].ippressure.condition=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "18")==0)
		{
			zoneprog_remote[progid].ippressure.currentval=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "19")==0)
		{
			zoneprog_remote[progid].ippressure.min=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "20")==0)
		{
			zoneprog_remote[progid].ippressure.max=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "21")==0)
		{
			zoneprog_remote[progid].oppressure.condition=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "22")==0)
		{
			zoneprog_remote[progid].oppressure.currentval=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "23")==0)
		{
			zoneprog_remote[progid].oppressure.min=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "24")==0)
		{
			zoneprog_remote[progid].oppressure.max=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "25")==0)
		{
			zoneprog_remote[progid].rundaysmode=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "26[0]")==0)
		{
			zoneprog_remote[progid].rundaysval[0]=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "26[1]")==0)
		{
			zoneprog_remote[progid].rundaysval[1]=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "26[2]")==0)
		{
			zoneprog_remote[progid].rundaysval[2]=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "27[0]")==0)
		{
			zoneprog_remote[progid].starttime[0]=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "27[1]")==0)
		{
			zoneprog_remote[progid].starttime[1]=(uint8_t)strtoint1(message[2]);

		}
		if (strcmp(message[1], "29")==0)
		{
			zoneprog_remote[progid].zonecount=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "28[0]")==0)
		{
			zoneprog_remote[progid].totaltime[0]=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "28[1]")==0)
		{
			zoneprog_remote[progid].totaltime[1]=(uint8_t)strtoint1(message[2]);

		}
		if (strcmp(message[1], "30")==0)
		{
			zoneprog_remote[progid].totalflow=(uint16_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "31")==0)
		{
			zoneprog_remote[progid].pumpdelay=(uint16_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "32")==0)
		{
			zoneprog_remote[progid].zonedelay=(uint16_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "33")==0)
		{
			//clear program data
			progid_clear=progid;
			zone_clear=1;

		}

		if (wireless_mode == 0) {//if in local wifi mode
			table_changed[3] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);
		}
	//	eeprom_write_zoneprog(progid);
	}

	if (strcmp(message[0], "7")==0)
	{
		progid=(uint8_t)strtoint1(message[5]);	//find Program ID
		if(progid>0)
		{
			progid=progid-1;
		}
		ioid=(uint8_t)strtoint1(message[6]);	//find IO ID
		if(ioid>4)
		{
			ioid=4;
		}
		ioid = ioid-1;
		if (strcmp(message[1], "1")==0)
		{
			fertiprog_remote[progid].valve[ioid].oppartno=(uint8_t)strtoint1(message[2]);
			fertiprog_remote[progid].valve[ioid].oppartno=fertiprog_remote[progid].valve[ioid].oppartno-1;
		}
		if (strcmp(message[1], "2")==0)
		{
			fertiprog_remote[progid].valve[ioid].starttime=(uint16_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "3")==0)
		{
			fertiprog_remote[progid].valve[ioid].stoptime=(uint16_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "4")==0)
		{
			fertiprog_remote[progid].valve[ioid].startflow=strtoint1(message[2]);
		}
		if (strcmp(message[1], "5")==0)
		{
			fertiprog_remote[progid].valve[ioid].stopflow=strtoint1(message[2]);
		}
		//eeprom_write_fertiprog(progid);
	}
	if (strcmp(message[0], "6")==0)
	{
		progid=(uint8_t)strtoint1(message[5]);	//find Program ID
		if(progid>0)
		{
			progid=progid-1;
		}
//		if (strcmp(message[1], "id")==0)
//		{
//			fertiprog_remote[progid].id=(uint8_t)strtoint1(message[2]);
//			fertiprog_remote[progid].id= fertiprog_remote[progid].id-1;
//		}
		if (strcmp(message[1], "2")==0)
		{
			fertiprog_remote[progid].method=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "3")==0)
		{
			fertiprog_remote[progid].propcycles=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "1")==0)
		{
			fertiprog_remote[progid].type=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "6")==0)
		{
			fertiprog_remote[progid].valvecount=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "4")==0)
		{
			fertiprog_remote[progid].startdelay=(uint16_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "5")==0)
		{
			fertiprog_remote[progid].stopdelay=(uint16_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "7")==0)
		{
			fertiprog_remote[progid].totaltime=(uint16_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "8")==0)
		{
			fertiprog_remote[progid].totalflow=strtoint1(message[2]);
		}
		if (strcmp(message[1], "9")==0)
		{
			//clear ferti prog
			progid_clear=progid;
			ferti_clear=1;
		}

		if (wireless_mode == 0) {//if in local wifi mode
			table_changed[5] = 1;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);
		}
	//	eeprom_write_fertiprog(progid);
	}
	if (strcmp(message[0], "8")==0)
	{
		cntrl_mode=1;
		progid=(uint8_t)strtoint1(message[5]);	//find Program ID
		if(progid>0)
		{
			progid=progid-1;
		}

		if (strcmp(message[1], "1")==0)
		{
			manprog[progid].state=(uint8_t)strtoint1(message[2]);
		}
		//eeprom_write_man_program(progid);
	}
	if (strcmp(message[0], "9")==0)
	{
		cntrl_mode=1;
		ioid=(uint8_t)strtoint1(message[6]);	//find IO ID
		if(ioid>16)
		{
			ioid=16;
		}
		ioid = ioid-1;
		if (strcmp(message[1], "1")==0)
		{
			manoutput[ioid].state=(uint8_t)strtoint1(message[2]);
		}
	//	eeprom_write_man_output(ioid);
	}
	if (strcmp(message[0], "10")==0)
	{
		if (strcmp(message[1], "1")==0)
		{
			datetimesource_remote=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "2")==0)
		{
			noopmodules_remote=(uint8_t)strtoint1(message[2]);
			noopmodules_remote=noopmodules_remote-1;
		}
		if (strcmp(message[1], "3")==0)
		{
			fertigation_remote=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "4")==0)
		{
			flowperpulse_remote=strtoint1(message[2]);
		}
		if (strcmp(message[1], "5")==0)
		{
			minop_remote=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "6")==0)
		{
			maxdp_remote=strtofloat1(message[2]);
		}
		if (strcmp(message[1], "7")==0)
		{
			language_remote=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "8")==0)
		{
			contrast_remote=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "9")==0)
		{
			disp_clockformat_remote=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "10")==0)
		{
			dateformat_remote=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "11")==0)
		{
			beginweek_remote=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "14")==0)
		{
			//factory reset
			// get default values and store data into eeprom
			factory_clear=1;
		}
		if (wireless_mode == 0) {//if in local wifi mode
					table_changed[6] = 1;
					eeprom_write_progstatus(0x40);
					eeprom_read_progstatus(0x40);
				}

	}
	if (strcmp(message[0], "11")==0)
	{
		if (strcmp(message[1], "1")==0)
		{
			backflushstatus_remote=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "2[0]")==0)
		{
			backflushinterval_remote[0]=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "2[1]")==0)
		{
			backflushinterval_remote[1]=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "3")==0)
		{
			backflushduration_remote=(uint16_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "4")==0)
		{
			backflushdelay_remote=(uint16_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "5")==0)
		{
			deltapres_remote=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "6")==0)
		{
			deltadelay_remote=(uint16_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "7")==0)
		{
			deltaitera_remote=(uint8_t)strtoint1(message[2]);
		}
		if (strcmp(message[1], "8")==0)
		{
			dwelltime_remote=(uint16_t)strtoint1(message[2]);
		}

		if (wireless_mode == 0) {//if in local wifi mode
					table_changed[6] = 1;
					eeprom_write_progstatus(0x40);
					eeprom_read_progstatus(0x40);
				}
	}

	if (strcmp(message[0], "12")==0)
	{
		if (strcmp(message[1], "1")==0)
		{
			wireless_mode_remote=(char)strtoint1(message[2]);
		}
		if (strcmp(message[1], "2")==0)
		{
			gsm_module_remote=(char)strtoint1(message[2]);
		}
		if (strcmp(message[1], "3")==0)
		{
			strncpy(gsm_apn_remote,message[2],20);
		}
		if (strcmp(message[1], "4")==0)
		{
			strncpy(gsm_operator_remote,message[2],20);
		}
		if (strcmp(message[1], "6")==0)
		{
			wifi_module_remote=(char)strtoint1(message[2]);
		}
		if (strcmp(message[1], "7")==0)
		{
			strncpy(wifi_name_remote,message[2],20);
		}
		if (strcmp(message[1], "8")==0)
		{
			strncpy(wifi_pass_remote,message[2],20);
		}
		if (strcmp(message[1], "9")==0)
		{
			strncpy(ext_apn_remote,message[2],20);
		}
		if (strcmp(message[1], "10")==0)
		{
			strncpy(ext_pass_remote,message[2],20);
		}
		if (strcmp(message[1], "11[0]")==0)
		{
			portal_ip_remote[0]=(char)strtoint1(message[2]);
		}
		if (strcmp(message[1], "11[1]")==0)
		{
			portal_ip_remote[1]=(char)strtoint1(message[2]);
		}
		if (strcmp(message[1], "11[2]")==0)
		{
			portal_ip_remote[2]=(char)strtoint1(message[2]);
		}
		if (strcmp(message[1], "11[3]")==0)
		{
			portal_ip_remote[3]=(char)strtoint1(message[2]);
		}
		if (strcmp(message[1], "12")==0)
		{
			strncpy(portal_name_remote,message[2],40);
		}
		if (wireless_mode == 0) {//if in local wifi mode
					table_changed[7] = 1;
					eeprom_write_progstatus(0x40);
					eeprom_read_progstatus(0x40);
				}
	}

	extern char on_zones;//No of on zones for auto/manual mode
	extern char on_op;//No of on op for manual mode
	extern int zone_onoff[10];
	extern uint8_t	cntrl_mode_prev;
	extern int op_onoff[16];
	if (strcmp(message[0], "13")==0)
	{
		if (strcmp(message[1], "1")==0)
		{
			cntrl_mode=(char)strtoint1(message[2]);
			if(cntrl_mode == 0)
			{
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
			}

			if(cntrl_mode == 1)
			{
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
			}
		}
	}
}


char temp1[20000]={0};

void  synctowifi(char table)
{
	char temp[200]={0};
	char ioid=0;
	char progid=0;
	int z=0;

	//Clear the Output Buffer
	memset(ouput_buffer,0, sizeof(ouput_buffer));
	//memset(temp1,0, sizeof(temp1));

	if(table==1)
	{
		for(ioid=0;ioid<16;ioid++)
		{
			sprintf (temp,"*|1||1||%d||1||%s||0||%d|* ",output[ioid].optype,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|1||2||%d||1||%s||0||%d|* ",output[ioid].typeid,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|1||3||%s||1||%s||0||%d|* ",output[ioid].opname,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));

		}
		for(ioid=0;ioid<4;ioid++)
		{
			//$$NR$$//dated:16Aug15
			//			sprintf (temp,"*|2||1||%s||1||%s||0||%d|* ",input[ioid].ipname,deviceid,(ioid+1));
			sprintf (temp,"*|2||1||%d||1||%s||0||%d|* ",input[ioid].iptype, deviceid, (ioid+1));

			//			if (ioid <= 2)
			//				sprintf (temp,"*|2||1||%s||1||%s||0||%d|* ", ipname_pump_list[input[ioid].iptype], deviceid, (ioid+1));
			//			else
			//				sprintf (temp,"*|2||1||%s||1||%s||0||%d|* ", ipname_fmeter_list[input[ioid].iptype], deviceid, (ioid+1));
			//$$NR$$//dated:16Aug15
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
		}
	}

	if(table==2)
	{
		for(ioid=0;ioid<6;ioid++)
		{
			sprintf (temp,"*|3||1||%d||1||%s||0||%d|* ",sensor[ioid].sensortype,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|3||2||%d||1||%s||0||%d|* ",sensor[ioid].serialid,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|3||3||%d||1||%s||0||%d|* ",sensor[ioid].typeid,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			//sprintf (temp,"*|3||6||%s||1||%s||0||%d|* ",sensor[ioid].unit,deviceid,(ioid+1));		//$$NR$$//dated:16Aug15
			sprintf (temp,"*|3||6||%d||1||%s||0||%d|* ",sensor[ioid].unittype,deviceid,(ioid+1));	//$$NR$$//dated:16Aug15
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|3||5||%.2lf||1||%s||0||%d|* ",sensor[ioid].max,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|3||4||%.2lf||1||%s||0||%d|* ",sensor[ioid].min,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
		}
	}

	if(table==4)
	{
		for(progid=0;progid<6;progid++)
		{
			for(ioid=0;ioid<10;ioid++)
			{
				sprintf (temp,"*|4||1||%d||1||%s||%d||%d|* ",(zoneprog[progid].zone[ioid].oppartno+1),deviceid,(progid+1),(ioid+1));
				strcat (temp1,temp);
				memset(temp,0, sizeof(temp));
				sprintf (temp,"*|4||2||%d||1||%s||%d||%d|* ",(zoneprog[progid].zone[ioid].fertprogid),deviceid,(progid+1),(ioid+1));
				strcat (temp1,temp);
				memset(temp,0, sizeof(temp));
				//time_minutes=(zoneprog[progid].zone[ioid].starttime[0]*60)+zoneprog[progid].zone[ioid].starttime[1];
				sprintf (temp,"*|4||3[0]||%d||1||%s||%d||%d|* ",zoneprog[progid].zone[ioid].starttime[0],deviceid,(progid+1),(ioid+1));
				strcat (temp1,temp);
				memset(temp,0, sizeof(temp));
				sprintf (temp,"*|4||3[1]||%d||1||%s||%d||%d|* ",zoneprog[progid].zone[ioid].starttime[1],deviceid,(progid+1),(ioid+1));
				strcat (temp1,temp);
				memset(temp,0, sizeof(temp));
				//time_minutes=(zoneprog[progid].zone[ioid].stoptime[0]*60)+zoneprog[progid].zone[ioid].stoptime[1];
				sprintf (temp,"*|4||4[0]||%d||1||%s||%d||%d|* ",zoneprog[progid].zone[ioid].stoptime[0],deviceid,(progid+1),(ioid+1));
				strcat (temp1,temp);
				memset(temp,0, sizeof(temp));
				sprintf (temp,"*|4||4[1]||%d||1||%s||%d||%d|* ",zoneprog[progid].zone[ioid].stoptime[1],deviceid,(progid+1),(ioid+1));
				strcat (temp1,temp);
				memset(temp,0, sizeof(temp));
				sprintf (temp,"*|4||5||%d||1||%s||%d||%d|* ",zoneprog[progid].zone[ioid].startflow,deviceid,(progid+1),(ioid+1));
				strcat (temp1,temp);
				memset(temp,0, sizeof(temp));
				sprintf (temp,"*|4||6||%d||1||%s||%d||%d|* ",zoneprog[progid].zone[ioid].stopflow,deviceid,(progid+1),(ioid+1));
				strcat (temp1,temp);
				memset(temp,0, sizeof(temp));
			}
		}
	}

	if(table==3)
	{
		for(progid=0;progid<6;progid++)
		{
//			sprintf (temp,"*|5||id||%d||1||%s||%d||0|* ",(zoneprog[progid].id+1),deviceid,(progid+1));
//			strcat (temp1,temp);
//			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||1||%d||1||%s||%d||0|* ",zoneprog[progid].type,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||2||%d||1||%s||%d||0|* ",zoneprog[progid].status,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||3||%d||1||%s||%d||0|* ",zoneprog[progid].repeatcycles,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||4||%d||1||%s||%d||0|* ",zoneprog[progid].cycledelay,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||5||%d||1||%s||%d||0|* ",zoneprog[progid].rain.condition,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||6||%.2lf||1||%s||%d||0|* ",zoneprog[progid].rain.currentval,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||7||%d||1||%s||%d||0|* ",zoneprog[progid].flow.condition,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||8||%.2lf||1||%s||%d||0|* ",zoneprog[progid].flow.currentval,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||9||%d||1||%s||%d||0|* ",zoneprog[progid].temperature.condition,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||10||%.2lf||1||%s||%d||0|* ",zoneprog[progid].temperature.currentval,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||11||%.2lf||1||%s||%d||0|* ",zoneprog[progid].temperature.min,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||12||%.2lf||1||%s||%d||0|* ",zoneprog[progid].temperature.max,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||13||%d||1||%s||%d||0|* ",zoneprog[progid].humidity.condition,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||14||%.2lf||1||%s||%d||0|* ",zoneprog[progid].humidity.currentval,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||15||%.2lf||1||%s||%d||0|* ",zoneprog[progid].humidity.min,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||16||%.2lf||1||%s||%d||0|* ",zoneprog[progid].humidity.max,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||17||%d||1||%s||%d||0|* ",zoneprog[progid].ippressure.condition,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||18||%.2lf||1||%s||%d||0|* ",zoneprog[progid].ippressure.currentval,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||19||%.2lf||1||%s||%d||0|* ",zoneprog[progid].ippressure.min,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||20||%.2lf||1||%s||%d||0|* ",zoneprog[progid].ippressure.max,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||21||%d||1||%s||%d||0|* ",zoneprog[progid].oppressure.condition,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||22||%.2lf||1||%s||%d||0|* ",zoneprog[progid].oppressure.currentval,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||23||%.2lf||1||%s||%d||0|* ",zoneprog[progid].oppressure.min,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||24||%.2lf||1||%s||%d||0|* ",zoneprog[progid].oppressure.max,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||25||%d||1||%s||%d||0|* ",zoneprog[progid].rundaysmode,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||26[0]||%d||1||%s||%d||0|* ",zoneprog[progid].rundaysval[0],deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||26[1]||%d||1||%s||%d||0|* ",zoneprog[progid].rundaysval[1],deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||26[2]||%d||1||%s||%d||0|* ",zoneprog[progid].rundaysval[2],deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||27[0]||%d||1||%s||%d||0|* ",zoneprog[progid].starttime[0],deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||27[1]||%d||1||%s||%d||0|* ",zoneprog[progid].starttime[1],deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||28[0]||%d||1||%s||%d||0|* ",zoneprog[progid].totaltime[0],deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||28[1]||%d||1||%s||%d||0|* ",zoneprog[progid].totaltime[1],deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||29||%d||1||%s||%d||0|* ",zoneprog[progid].zonecount,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||30||%d||1||%s||%d||0|* ",zoneprog[progid].totalflow,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||31||%d||1||%s||%d||0|* ",zoneprog[progid].pumpdelay,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||32||%d||1||%s||%d||0|* ",zoneprog[progid].zonedelay,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
		}
	}

	if(table==5)
	{
		for(progid=0;progid<6;progid++)
		{
//			sprintf (temp,"*|6||id||%d||1||%s||%d||0|* ",(fertiprog[progid].id+1),deviceid,(progid+1));
//			strcat (temp1,temp);
//			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|6||1||%d||1||%s||%d||0|* ",fertiprog[progid].type,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|6||2||%d||1||%s||%d||0|* ",fertiprog[progid].method,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|6||3||%d||1||%s||%d||0|* ",fertiprog[progid].propcycles,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|6||4||%d||1||%s||%d||0|* ",fertiprog[progid].startdelay,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|6||5||%d||1||%s||%d||0|* ",fertiprog[progid].stopdelay,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));

			sprintf (temp,"*|6||6||%d||1||%s||%d||0|* ",fertiprog[progid].valvecount,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|6||7||%d||1||%s||%d||0|* ",fertiprog[progid].totaltime,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|6||8||%d||1||%s||%d||0|* ",fertiprog[progid].totalflow,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
		}
	}

	if(table==5)
	{
		for(progid=0;progid<6;progid++)
		{
			for(ioid=0;ioid<4;ioid++)
			{
				sprintf (temp,"*|7||1||%d||1||%s||%d||%d|* ",(fertiprog[progid].valve[ioid].oppartno+1),deviceid,(progid+1),(ioid+1));
				strcat (temp1,temp);
				memset(temp,0, sizeof(temp));
				sprintf (temp,"*|7||2||%d||1||%s||%d||%d|* ",fertiprog[progid].valve[ioid].starttime,deviceid,(progid+1),(ioid+1));
				strcat (temp1,temp);
				memset(temp,0, sizeof(temp));
				sprintf (temp,"*|7||3||%d||1||%s||%d||%d|* ",fertiprog[progid].valve[ioid].stoptime,deviceid,(progid+1),(ioid+1));
				strcat (temp1,temp);
				memset(temp,0, sizeof(temp));
				sprintf (temp,"*|7||4||%d||1||%s||%d||%d|* ",fertiprog[progid].valve[ioid].startflow,deviceid,(progid+1),(ioid+1));
				strcat (temp1,temp);
				memset(temp,0, sizeof(temp));
				sprintf (temp,"*|7||5||%d||1||%s||%d||%d|* ",fertiprog[progid].valve[ioid].stopflow,deviceid,(progid+1),(ioid+1));
				strcat (temp1,temp);
				memset(temp,0, sizeof(temp));
			}
		}
	}



	if(table==6)
	{
		for(progid=0;progid<6;progid++)
		{
			sprintf (temp,"*|8||1||%d||1||%s||%d||0|* ",manprog[progid].state,deviceid,(progid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
		}
		for(ioid=0;ioid<16;ioid++)
		{
			sprintf (temp,"*|9||1||%d||1||%s||0||%d|* ",manoutput[ioid].state,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
		}

		sprintf (temp,"*|10||1||%d||1||%s||0||0|* ",datetimesource,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||2||%d||1||%s||0||0|* ",(noopmodules+1),deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||3||%d||1||%s||0||0|* ",fertigation,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||4||%d||1||%s||0||0|* ",flowperpulse,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||5||%.2lf||1||%s||0||0|* ",minop,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||6||%.2lf||1||%s||0||0|* ",maxdp,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||7||%d||1||%s||0||0|* ",language,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||8||%d||1||%s||0||0|* ",contrast,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||9||%d||1||%s||0||0|* ",disp_clockformat,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||10||%d||1||%s||0||0|* ",dateformat,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||11||%d||1||%s||0||0|* ",beginweek,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|11||1||%d||1||%s||0||0|* ",backflushstatus,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|11||2[0]||%d||1||%s||0||0|* ",backflushinterval[0],deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|11||2[1]||%d||1||%s||0||0|* ",backflushinterval[1],deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|11||3||%d||1||%s||0||0|* ",backflushduration,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|11||4||%d||1||%s||0||0|* ",backflushdelay,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|11||5||%d||1||%s||0||0|* ",deltapres,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|11||6||%d||1||%s||0||0|* ",deltadelay,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|11||7||%d||1||%s||0||0|* ",deltaitera,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|11||8||%d||1||%s||0||0|* ",dwelltime,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
	}

	if(table==7)
	{
		sprintf (temp,"*|12||1||%d||1||%s||0||0|* ",wireless_mode,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||2||%d||1||%s||0||0|* ",gsm_module,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||3||%s||1||%s||0||0|* ",gsm_apn,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||4||%s||1||%s||0||0|* ",gsm_operator,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||5||%d||1||%s||0||0|* ",sig_strength,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||6||%d||1||%s||0||0|* ",wifi_module,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||7||%s||1||%s||0||0|* ",wifi_name,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||8||%s||1||%s||0||0|* ",wifi_pass,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||9||%s||1||%s||0||0|* ",ext_apn,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||10||%s||1||%s||0||0|* ",ext_pass,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||11[0]||%d||1||%s||0||0|* ",portal_ip[0],deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||11[1]||%d||1||%s||0||0|* ",portal_ip[1],deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||11[2]||%d||1||%s||0||0|* ",portal_ip[2],deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||11[3]||%d||1||%s||0||0|* ",portal_ip[3],deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||12||%s||1||%s||0||0|* ",portal_name,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
	}
	if(table==8)
	{
		sprintf (temp,"*|13||1||%d||1||%s||0||0|* ",cntrl_mode,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||2||%d||1||%s||0||0|* ",cntrl_state,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||3||%d||1||%s||0||0|* ",cur_prog,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||4||%d||1||%s||0||0|* ",cur_prog_state,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||5[0]||%d||1||%s||0||0|* ",cur_prog_elapsedtime[0],deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||5[1]||%d||1||%s||0||0|* ",cur_prog_elapsedtime[1],deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||6||%d||1||%s||0||0|* ",oppressure_curvalue,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||7||%d||1||%s||0||0|* ",ippressure_curvalue,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||8||%d||1||%s||0||0|* ",temperature_curvalue,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||9||%d||1||%s||0||0|* ",humidity_curvalue,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||10||%d||1||%s||0||0|* ",rain_curvalue,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||11||%d||1||%s||0||0|* ",flow_curvalue,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		for(ioid=0;ioid<16;ioid++)
		{
			sprintf (temp,"*|13||12||%d||1||%s||0||%d|* ",output_state[ioid],deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
		}
		for(ioid=0;ioid<4;ioid++)
		{
			sprintf (temp,"*|13||13||%d||1||%s||0||%d|* ",input_state[ioid],deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
		}

	}


	if(table == 9)
	{

		//Read_log(0);
		Read_log_portal();

		for(z=0;z<LOGFILE_BUFF_SIZE;z++)
		{
			if((logfile_buff[z]==10)||(logfile_buff[z]==13))
			{
				logfile_buff[z] = 32;
			}
		}

		//strcat (ouput_buffer,"logdata=");
		strcat (temp1,"*|log_data||");
		strcat (temp1,deviceid);
		strcat (temp1,"||");
		strcat (temp1,logfile_buff);
		strcat (temp1,"|*\r\n");
		memset(logfile_buff,0, sizeof(logfile_buff));
	}


	//for wifi , post message not required for wifi
	strcat (ouput_buffer,temp1);

	memset(temp1,0, sizeof(temp1));
}


char progid1=0;
void  synctoportal(char table)
{
	char temp[200]={0};
	char len[10]={0};
	//const char msgtoportal[] = "POST http://portal.lotuscomputers.com/IOT_Local/recivedata.php HTTP/1.0\r\nContent-Type: application/x-www-form-urlencoded;charset=utf-8\r\nContent-Length: ";//56\r\n\r\nackdata=*|4||stopflow||52||5||1||1||3|*";
//	const char msgtoportal[] = "POST http://portal.avkon.in/IOT_Local/recivedata.php HTTP/1.0\r\nContent-Type: application/x-www-form-urlencoded;charset=utf-8\r\nContent-Length: ";//56\r\n\r\nackdata=*|4||stopflow||52||5||1||1||3|*";

	//const char msgtoportal[] = "POST http://202.189.230.50:999//IOT_Local/recivedata.php HTTP/1.0\r\nContent-Type: application/x-www-form-urlencoded;charset=utf-8\r\nContent-Length: ";//56\r\n\r\nackdata=*|4||stopflow||52||5||1||1||3|*";
	char ioid=0;

//	char deviceid=1;

	//Clear the Output Buffer
	memset(ouput_buffer,0, sizeof(ouput_buffer));
	//memset(temp1,0, sizeof(temp1));



	if(table==1)
	{
		for(ioid=0;ioid<16;ioid++)
		{
			sprintf (temp,"*|1||1||%d||1||%s||0||%d|*\r\n",output[ioid].optype,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|1||2||%d||1||%s||0||%d|*\r\n",output[ioid].typeid,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|1||3||%s||1||%s||0||%d|*\r\n",output[ioid].opname,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
		}
		for(ioid=0;ioid<4;ioid++)
		{
			//$$NR$$//dated:16Aug15
			//			sprintf (temp,"*|2||1||%s||1||%s||0||%d|*\r\n",input[ioid].ipname,deviceid,(ioid+1));
						sprintf (temp,"*|2||1||%d||1||%s||0||%d|*\r\n",input[ioid].iptype, deviceid, (ioid+1));

			//			if (ioid <= 2)
			//				sprintf (temp,"*|2||1||%s||1||%s||0||%d|*\r\n", ipname_pump_list[input[ioid].iptype], deviceid, (ioid+1));
			//			else
			//				sprintf (temp,"*|2||1||%s||1||%s||0||%d|*\r\n", ipname_fmeter_list[input[ioid].iptype], deviceid, (ioid+1));
			//$$NR$$//dated:16Aug15
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
		}

		table_changed[synccount] = 0;
		eeprom_write_progstatus(0x40);
		eeprom_read_progstatus(0x40);

		synccount = 2;
	}

	if(table==2)
	{
		for(ioid=0;ioid<6;ioid++)
		{
			sprintf (temp,"*|3||1||%d||1||%s||0||%d|*\r\n",sensor[ioid].sensortype,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|3||2||%d||1||%s||0||%d|*\r\n",sensor[ioid].serialid,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|3||3||%d||1||%s||0||%d|*\r\n",sensor[ioid].typeid,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
//			sprintf (temp,"*|3||6||%s||1||%s||0||%d|*\r\n",sensor[ioid].unit,deviceid,(ioid+1));		//$$NR$$//dated:16Aug15
			sprintf (temp,"*|3||6||%d||1||%s||0||%d|*\r\n",sensor[ioid].unittype,deviceid,(ioid+1));	//$$NR$$//dated:16Aug15
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|3||5||%.2lf||1||%s||0||%d|*\r\n",sensor[ioid].max,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|3||4||%.2lf||1||%s||0||%d|*\r\n",sensor[ioid].min,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
		}

		table_changed[synccount] = 0;
		eeprom_write_progstatus(0x40);
		eeprom_read_progstatus(0x40);
		synccount = 3;
	}

	if(table==3)
	{
//			sprintf (temp,"*|5||id||%d||1||%s||%d||0|*\r\n",(zoneprog[progid].id+1),deviceid,(progid+1));
//			strcat (temp1,temp);
//			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||1||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].type,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||2||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].status,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||3||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].repeatcycles,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||4||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].cycledelay,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||5||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].rain.condition,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||6||%.2lf||1||%s||%d||0|*\r\n",zoneprog[progid1].rain.currentval,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||7||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].flow.condition,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||8||%.2lf||1||%s||%d||0|*\r\n",zoneprog[progid1].flow.currentval,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||9||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].temperature.condition,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||10||%.2lf||1||%s||%d||0|*\r\n",zoneprog[progid1].temperature.currentval,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||11||%.2lf||1||%s||%d||0|*\r\n",zoneprog[progid1].temperature.min,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||12||%.2lf||1||%s||%d||0|*\r\n",zoneprog[progid1].temperature.max,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||13||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].humidity.condition,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||14||%.2lf||1||%s||%d||0|*\r\n",zoneprog[progid1].humidity.currentval,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||15||%.2lf||1||%s||%d||0|*\r\n",zoneprog[progid1].humidity.min,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||16||%.2lf||1||%s||%d||0|*\r\n",zoneprog[progid1].humidity.max,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||17||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].ippressure.condition,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||18||%.2lf||1||%s||%d||0|*\r\n",zoneprog[progid1].ippressure.currentval,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||19||%.2lf||1||%s||%d||0|*\r\n",zoneprog[progid1].ippressure.min,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||20||%.2lf||1||%s||%d||0|*\r\n",zoneprog[progid1].ippressure.max,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||21||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].oppressure.condition,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||22||%.2lf||1||%s||%d||0|*\r\n",zoneprog[progid1].oppressure.currentval,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||23||%.2lf||1||%s||%d||0|*\r\n",zoneprog[progid1].oppressure.min,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||24||%.2lf||1||%s||%d||0|*\r\n",zoneprog[progid1].oppressure.max,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||25||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].rundaysmode,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||26[0]||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].rundaysval[0],deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||26[1]||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].rundaysval[1],deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||26[2]||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].rundaysval[2],deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||27||%d:%d||1||%s||%d||0|*\r\n",zoneprog[progid1].starttime[0],zoneprog[progid1].starttime[1],deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||28||%d:%d||1||%s||%d||0|*\r\n",zoneprog[progid1].totaltime[0],zoneprog[progid1].totaltime[1],deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||29||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].zonecount,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||30||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].totalflow,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||31||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].pumpdelay,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|5||32||%d||1||%s||%d||0|*\r\n",zoneprog[progid1].zonedelay,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));

			progid1++;
			if(progid1>=6)
			{
				progid1=0;

				table_changed[synccount] = 0;
				eeprom_write_progstatus(0x40);
				eeprom_read_progstatus(0x40);
				synccount = 4;
			}
	}

	if(table==4)
	{
		for(ioid=0;ioid<10;ioid++)
		{
			sprintf (temp,"*|4||1||%d||1||%s||%d||%d|*\r\n",(zoneprog[progid1].zone[ioid].oppartno+1),deviceid,(progid1+1),(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|4||2||%d||1||%s||%d||%d|*\r\n",(zoneprog[progid1].zone[ioid].fertprogid),deviceid,(progid1+1),(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			//time_minutes=(zoneprog[progid1].zone[ioid].starttime[0]*60)+zoneprog[progid1].zone[ioid].starttime[1];
			sprintf (temp,"*|4||3||%d:%d||1||%s||%d||%d|*\r\n",zoneprog[progid1].zone[ioid].starttime[0],zoneprog[progid1].zone[ioid].starttime[1],deviceid,(progid1+1),(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			//time_minutes=(zoneprog[progid1].zone[ioid].stoptime[0]*60)+zoneprog[progid1].zone[ioid].stoptime[1];
			sprintf (temp,"*|4||4||%d:%d||1||%s||%d||%d|*\r\n",zoneprog[progid1].zone[ioid].stoptime[0],zoneprog[progid1].zone[ioid].stoptime[1],deviceid,(progid1+1),(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|4||5||%d||1||%s||%d||%d|*\r\n",zoneprog[progid1].zone[ioid].startflow,deviceid,(progid1+1),(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|4||6||%d||1||%s||%d||%d|*\r\n",zoneprog[progid1].zone[ioid].stopflow,deviceid,(progid1+1),(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
		}
		progid1++;
		if(progid1>=6)
		{

			table_changed[synccount] = 0;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);
			progid1=0;
			synccount = 5;
		}
	}

	if(table==5)
	{
		sprintf (temp,"*|6||1||%d||1||%s||%d||0|*\r\n",fertiprog[progid1].type,deviceid,(progid1+1));
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|6||2||%d||1||%s||%d||0|*\r\n",fertiprog[progid1].method,deviceid,(progid1+1));
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|6||3||%d||1||%s||%d||0|*\r\n",fertiprog[progid1].propcycles,deviceid,(progid1+1));
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|6||4||%d||1||%s||%d||0|*\r\n",fertiprog[progid1].startdelay,deviceid,(progid1+1));
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|6||5||%d||1||%s||%d||0|*\r\n",fertiprog[progid1].stopdelay,deviceid,(progid1+1));
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|6||6||%d||1||%s||%d||0|*\r\n",fertiprog[progid1].valvecount,deviceid,(progid1+1));
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|6||7||%d||1||%s||%d||0|*\r\n",fertiprog[progid1].totaltime,deviceid,(progid1+1));
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|6||8||%d||1||%s||%d||0|*\r\n",fertiprog[progid1].totalflow,deviceid,(progid1+1));
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		for(ioid=0;ioid<4;ioid++)
		{
			sprintf (temp,"*|7||1||%d||1||%s||%d||%d|*\r\n",(fertiprog[progid1].valve[ioid].oppartno+1),deviceid,(progid1+1),(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|7||2||%d||1||%s||%d||%d|*\r\n",fertiprog[progid1].valve[ioid].starttime,deviceid,(progid1+1),(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|7||3||%d||1||%s||%d||%d|*\r\n",fertiprog[progid1].valve[ioid].stoptime,deviceid,(progid1+1),(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|7||4||%d||1||%s||%d||%d|*\r\n",fertiprog[progid1].valve[ioid].startflow,deviceid,(progid1+1),(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
			sprintf (temp,"*|7||5||%d||1||%s||%d||%d|*\r\n",fertiprog[progid1].valve[ioid].stopflow,deviceid,(progid1+1),(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
		}
		progid1++;
		if(progid1>=6)
		{

			table_changed[synccount] = 0;
			eeprom_write_progstatus(0x40);
			eeprom_read_progstatus(0x40);

			progid1=0;
			synccount = 6;
		}

	}

	if(table==6)
	{
		for(progid1=0;progid1<6;progid1++)
		{
			sprintf (temp,"*|8||1||%d||1||%s||%d||0|*\r\n",manprog[progid1].state,deviceid,(progid1+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
		}
		for(ioid=0;ioid<16;ioid++)
		{
			sprintf (temp,"*|9||1||%d||1||%s||0||%d|*\r\n",manoutput[ioid].state,deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
		}

		sprintf (temp,"*|10||1||%d||1||%s||0||0|*\r\n",datetimesource,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||2||%d||1||%s||0||0|*\r\n",(noopmodules+1),deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||3||%d||1||%s||0||0|*\r\n",fertigation,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||4||%d||1||%s||0||0|*\r\n",flowperpulse,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||5||%.2lf||1||%s||0||0|*\r\n",minop,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||6||%.2lf||1||%s||0||0|*\r\n",maxdp,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||7||%d||1||%s||0||0|*\r\n",language,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||8||%d||1||%s||0||0|*\r\n",contrast,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||9||%d||1||%s||0||0|*\r\n",disp_clockformat,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||10||%d||1||%s||0||0|*\r\n",dateformat,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|10||11||%d||1||%s||0||0|*\r\n",beginweek,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|11||1||%d||1||%s||0||0|*\r\n",backflushstatus,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|11||2||%d:%d||1||%s||0||0|*\r\n",backflushinterval[0],backflushinterval[1],deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|11||3||%d||1||%s||0||0|*\r\n",backflushduration,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|11||4||%d||1||%s||0||0|*\r\n",backflushdelay,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|11||5||%d||1||%s||0||0|*\r\n",deltapres,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|11||6||%d||1||%s||0||0|*\r\n",deltadelay,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|11||7||%d||1||%s||0||0|*\r\n",deltaitera,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|11||8||%d||1||%s||0||0|*\r\n",dwelltime,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));

		table_changed[synccount] = 0;
		eeprom_write_progstatus(0x40);
		eeprom_read_progstatus(0x40);
		synccount = 7;
	}

	if(table==7)
	{
		sprintf (temp,"*|12||1||%d||1||%s||0||0|*\r\n",wireless_mode,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||2||%d||1||%s||0||0|*\r\n",gsm_module,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||3||%s||1||%s||0||0|*\r\n",gsm_apn,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||4||%s||1||%s||0||0|*\r\n",gsm_operator,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||5||%d||1||%s||0||0|*\r\n",sig_strength,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||6||%d||1||%s||0||0|*\r\n",wifi_module,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||7||%s||1||%s||0||0|*\r\n",wifi_name,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||8||%s||1||%s||0||0|*\r\n",wifi_pass,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||9||%s||1||%s||0||0|*\r\n",ext_apn,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||10||%s||1||%s||0||0|*\r\n",ext_pass,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||11[0]||%d||1||%s||0||0|*\r\n",portal_ip[0],deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||11[1]||%d||1||%s||0||0|*\r\n",portal_ip[1],deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||11[2]||%d||1||%s||0||0|*\r\n",portal_ip[2],deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||11[3]||%d||1||%s||0||0|*\r\n",portal_ip[3],deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|12||12||%s||1||%s||0||0|*\r\n",portal_name,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));

		table_changed[synccount] = 0;
		eeprom_write_progstatus(0x40);
		eeprom_read_progstatus(0x40);

		synccount = 8;
	}
	if(table==8)
	{
		sprintf (temp,"*|13||1||%d||1||%s||0||0|*\r\n",cntrl_mode,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||2||%d||1||%s||0||0|*\r\n",cntrl_state,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||3||%d||1||%s||0||0|*\r\n",cur_prog,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||4||%d||1||%s||0||0|*\r\n",cur_prog_state,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||5[0]||%d||1||%s||0||0|*\r\n",cur_prog_elapsedtime[0],deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||5[1]||%d||1||%s||0||0|*\r\n",cur_prog_elapsedtime[1],deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||6||%d||1||%s||0||0|*\r\n",oppressure_curvalue,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||7||%d||1||%s||0||0|*\r\n",ippressure_curvalue,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||8||%d||1||%s||0||0|*\r\n",temperature_curvalue,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||9||%d||1||%s||0||0|*\r\n",humidity_curvalue,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||10||%d||1||%s||0||0|*\r\n",rain_curvalue,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		sprintf (temp,"*|13||11||%d||1||%s||0||0|*\r\n",flow_curvalue,deviceid);
		strcat (temp1,temp);
		memset(temp,0, sizeof(temp));
		for(ioid=0;ioid<16;ioid++)
		{
			sprintf (temp,"*|13||12||%d||1||%s||0||%d|*\r\n",output_state[ioid],deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
		}
		for(ioid=0;ioid<4;ioid++)
		{
			sprintf (temp,"*|13||13||%d||1||%s||0||%d|*\r\n",input_state[ioid],deviceid,(ioid+1));
			strcat (temp1,temp);
			memset(temp,0, sizeof(temp));
		}

		table_changed[synccount] = 0;
		eeprom_write_progstatus(0x40);
		eeprom_read_progstatus(0x40);

		synccount = 9;
	}

	if(table==9)
	{
		sprintf (temp,"*|Access_data||LocalAccessOn||%d||0||%s||0||0|*\r\n",localaccessflag,deviceid);
		strcat (temp1,temp);
		synccount = 10;
	}

	if(table==10)
	{
		sprintf (temp,"*|Ack||%d||%d||0||%s||0||0|*\r\n",tempmsgid,noofmsg,deviceid);
		strcat (temp1,temp);
	}

	strcat (ouput_buffer,msgtoportal);
	sprintf(len, "%d", (strlen(temp1)+8));
	strcat (ouput_buffer,len);
	strcat (ouput_buffer,"\r\n\r\n");
	strcat (ouput_buffer,"ackdata=");
	strcat (ouput_buffer,temp1);


	memset(temp1,0, sizeof(temp1));
}



int GSM_Activate()
{
	char str[20]={0};
	const char msg1[] = "AT#SGACT=1,1\r";
	const char msg2[] = "AT#SGACT=1,0\r";

	flag_30s=1;
	memset(input_buffer,0, sizeof(input_buffer));
	UART_writePolling(uart, msg2, sizeof(msg2));
	while(1)
	{
		if(UARTCharsAvail(UART3_BASE))
		{
			 UART_read(uart, &input_buffer, sizeof(input_buffer));
			//System_printf("%s",input_buffer);
		}
		strcpy(str,"OK");
		if(strncmp(input_buffer,str,2)==0)
		{
			Task_sleep(20);
			UART_writePolling(uart, msg1, sizeof(msg1));
		}
		strcpy(str,"#SGACT");
		if(strncmp(input_buffer,str,6)==0)
		{
			memset(temp_ip,0, sizeof(temp_ip));
			strcpy(temp_ip,input_buffer);
		//	ip = strtok (input_buffer, ":");
		//	ip = strtok (NULL, "/r");
			return 1;
		}
		strcpy(str,"ERROR");
		if(strncmp(input_buffer,str,5)==0)
		{
			//return 0;
			//Task_sleep(20);
			return 0;
			//UART_writePolling(uart, msg2, sizeof(msg2));
		}
		memset(input_buffer,0, sizeof(input_buffer));


		if(flag_30s>500) break;//5 secs timeout
		//Task_sleep(20);
	}
	return 0;
}


int GSM_Configure()
{
	char str[20]={0};
	char* temp3;
	char* temp4;
	//char x=0;
	uint32_t an_val=4;
	char sig_str[5]={0};
	char msg1[50] = {0};
	//const char msg1[] = "AT+CGDCONT= 1,\"IP\",\"internet\"\r";//for Idea
	//const char msg1[] = "AT+CGDCONT= 1,\"IP\",\"bsnlnet\"\r";//for bsnl
	const char msg2[] = "AT#SCFG=1,1,515,0,600,10\r";//"AT#SCFG=1,1,515,0,120,10\r";//"AT#SCFG=1,1,300,90,600,50\r";
	const char msg3[] = "AT#SGACT=1,0\r";//"+++\r";
	const char msg4[] = "AT#SCFGEXT2=1,1,0,0,0,0\r";
	const char msg5[] = "AT#SCFGEXT=1,0,0,0,0,0\r";
//	const char msg6[] = "AT#SH=1\r";
	const char msg6[] = "AT+CSQ\r";
	const char msg7[] = "AT&K0\r";
	const char msg8[] = "AT+COPS?\r";


	strcat (msg1,"AT+CGDCONT= 1,\"IP\",\"");
	strcat (msg1,gsm_apn);
	strcat (msg1,"\"\r");


	while(an_val!=0)
	{
		//TODO GSM ALERT
		an_val=GPIO_read(Board_GSMAN);
		Task_sleep(20);
	}

//	x=0;
	flag_30s=1;
	UART_writePolling(uart, msg8, sizeof(msg8));
	while(1)
	{
		if(UARTCharsAvail(UART3_BASE))
		{
			UART_read(uart, &input_buffer, sizeof(input_buffer));
//			x++;
//			if(x==2)
			strcpy(str,"+COPS");
			if(strncmp(input_buffer,str,5)==0)
			{
				temp3 = strtok (input_buffer, "\"");
				temp3 = strtok (NULL, "\"");
				strcpy(gsm_operator,temp3);
				  while( temp3 != NULL )
				  {
					  temp3 = strtok(NULL, "\"");
				  }
			}
			//System_printf("%s",input_buffer);
		}
		strcpy(str,"OK");
		if(strncmp(input_buffer,str,2)==0)
		{
			memset(input_buffer,0, sizeof(input_buffer));
			break;
		}
		strcpy(str,"ERROR");
		if(strncmp(input_buffer,str,5)==0)
		{
			//return 0;
			//Task_sleep(20);
			return 0;
			//UART_writePolling(uart, msg8, sizeof(msg8));
		}
		memset(input_buffer,0, sizeof(input_buffer));
		if(flag_30s>500) break;//5 secs timeout
	}
	Task_sleep(20);

//	x=0;
	flag_30s=1;
	UART_writePolling(uart, msg6, sizeof(msg6));
	while(1)
	{
		if(UARTCharsAvail(UART3_BASE))
		{
			 UART_read(uart, &input_buffer, sizeof(input_buffer));
//			x++;
//			if(x==2)
			strcpy(str,"+CSQ:");
			if(strncmp(input_buffer,str,5)==0)
			{
				temp4 = strtok (input_buffer, " ");
				temp4 = strtok (NULL, ",");
				strcpy(sig_str,temp4);
				sig_strength=(uint8_t)strtoint1(sig_str);
				while( temp4 != NULL )
				{
				  temp4 = strtok(NULL, ",");
				}
			}
			//System_printf("%s",input_buffer);
		}
		strcpy(str,"OK");
		if(strncmp(input_buffer,str,2)==0)
		{
			memset(input_buffer,0, sizeof(input_buffer));
			break;
		}
		strcpy(str,"ERROR");
		if(strncmp(input_buffer,str,5)==0)
		{
			//return 0;
			//Task_sleep(20);
			return 0;
			//UART_writePolling(uart, msg6, sizeof(msg6));
		}
		memset(input_buffer,0, sizeof(input_buffer));
		if(flag_30s>500) break;//5 secs timeout
		//Task_sleep(20);
	}

	flag_30s=1;
	UART_writePolling(uart, msg7, sizeof(msg7));
	while(1)
	{
		if(UARTCharsAvail(UART3_BASE))
		{
			 UART_read(uart, &input_buffer, sizeof(input_buffer));
			//System_printf("%s",input_buffer);
		}
		strcpy(str,"OK");
		if(strncmp(input_buffer,str,2)==0)
		{
			memset(input_buffer,0, sizeof(input_buffer));
			break;
		}
		strcpy(str,"ERROR");
		if(strncmp(input_buffer,str,5)==0)
		{
			//return 0;
			//Task_sleep(20);
			return 0;
			//UART_writePolling(uart, msg7, sizeof(msg7));
		}
		memset(input_buffer,0, sizeof(input_buffer));
		if(flag_30s>500) break;//5 secs timeout
	}
	Task_sleep(20);

	flag_30s=1;
	UART_writePolling(uart, msg3, sizeof(msg3));
	while(1)
	{
		if(UARTCharsAvail(UART3_BASE))
		{
			 UART_read(uart, &input_buffer, sizeof(input_buffer));
			//System_printf("%s",input_buffer);
		}
		strcpy(str,"OK");
		if(strncmp(input_buffer,str,2)==0)
		{
			memset(input_buffer,0, sizeof(input_buffer));
			break;
		}
		strcpy(str,"ERROR");
		if(strncmp(input_buffer,str,5)==0)
		{
			//return 0;
			//Task_sleep(20);
			return 0;
			//UART_writePolling(uart, msg3, sizeof(msg3));
		}
		memset(input_buffer,0, sizeof(input_buffer));
		if(flag_30s>500) break;//5 secs timeout
	}
	Task_sleep(20);

	flag_30s=1;
	UART_writePolling(uart, msg4, sizeof(msg4));
	while(1)
	{
		if(UARTCharsAvail(UART3_BASE))
		{
			 UART_read(uart, &input_buffer, sizeof(input_buffer));
			//System_printf("%s",input_buffer);
		}
		strcpy(str,"OK");
		if(strncmp(input_buffer,str,2)==0)
		{
			memset(input_buffer,0, sizeof(input_buffer));
			break;
		}
		strcpy(str,"ERROR");
		if(strncmp(input_buffer,str,5)==0)
		{
			//return 0;
			//Task_sleep(20);
			return 0;
			//UART_writePolling(uart, msg4, sizeof(msg4));
		}
		memset(input_buffer,0, sizeof(input_buffer));
		if(flag_30s>500) break;//5 secs timeout
		//Task_sleep(20);
	}
	Task_sleep(20);

	flag_30s=1;
	UART_writePolling(uart, msg5, sizeof(msg5));
	while(1)
	{
		if(UARTCharsAvail(UART3_BASE))
		{
			 UART_read(uart, &input_buffer, sizeof(input_buffer));
			//System_printf("%s",input_buffer);
		}
		strcpy(str,"OK");
		if(strncmp(input_buffer,str,2)==0)
		{
			memset(input_buffer,0, sizeof(input_buffer));
			break;
		}
		strcpy(str,"ERROR");
		if(strncmp(input_buffer,str,5)==0)
		{
			//return 0;
			//Task_sleep(20);
			return 0;
			//UART_writePolling(uart, msg5, sizeof(msg5));
		}
		memset(input_buffer,0, sizeof(input_buffer));
		if(flag_30s>500) break;//5 secs timeout
		//Task_sleep(20);
	}
	Task_sleep(20);

	flag_30s=1;
	UART_writePolling(uart, msg1, strlen(msg1));//todo  sizeof(msg1)
	while(1)
	{
		if(UARTCharsAvail(UART3_BASE))
		{
			 UART_read(uart, &input_buffer, sizeof(input_buffer));
			//System_printf("%s",input_buffer);
		}
		strcpy(str,"OK");
		if(strncmp(input_buffer,str,2)==0)
		{
			memset(input_buffer,0, sizeof(input_buffer));
			break;
		}
		strcpy(str,"ERROR");
		if(strncmp(input_buffer,str,5)==0)
		{
			//return 0;
			//Task_sleep(20);
			return 0;
			//UART_writePolling(uart, msg1, sizeof(msg1));
		}
		memset(input_buffer,0, sizeof(input_buffer));
		if(flag_30s>500) break;//5 secs timeout
		//Task_sleep(20);
	}
	Task_sleep(20);

	flag_30s=1;
	UART_writePolling(uart, msg2, sizeof(msg2));
	while(1)
	{
		if(UARTCharsAvail(UART3_BASE))
		{
			 UART_read(uart, &input_buffer, sizeof(input_buffer));
			//System_printf("%s",input_buffer);
		}
		strcpy(str,"OK");
		if(strncmp(input_buffer,str,2)==0)
		{
			memset(input_buffer,0, sizeof(input_buffer));
			break;
		}
		strcpy(str,"ERROR");
		if(strncmp(input_buffer,str,5)==0)
		{
			//return 0;
			//Task_sleep(20);
			return 0;
			//UART_writePolling(uart, msg2, sizeof(msg2));
		}
		memset(input_buffer,0, sizeof(input_buffer));
		if(flag_30s>500) break;//5 secs timeout
		//Task_sleep(20);
	}
	return 1;
}


void GSM_EEPROM_UPDATE()
{
	int i,ioid,progid;
	char updated=0,m=0;

	if(remote_data_recevied_flag == 0)	return;

	if(factory_clear == 1)
	{
		eeprom_factory_reset();
		factory_clear = 0;
	}
	if(ferti_clear == 1)
	{
		// get default value
		get_default_value_fertiprog(progid_clear);

		// write data into eeprom
		//eeprom_write_fertiprog(prog_index);
		eeprom_write_fertiprog1(progid_clear,0XFF);
		eeprom_write_fertiprog2(progid_clear,0XFF);
		eeprom_read_fertiprog1(progid_clear,0XFF);
		eeprom_read_fertiprog2(progid_clear,0XFF);
		for(m=0;m<4;m++)
		{
			eeprom_write_fertiprog3(progid_clear,m,0XFF);
			eeprom_read_fertiprog3(progid_clear,m,0XFF);
		}
		ferti_clear = 0;
		progid_clear=0;
	}

	if(zone_clear == 1)
	{

		// get default value
		get_default_value_zoneprog(progid_clear);

		// write data into eeprom
		//eeprom_write_zoneprog(prog_index);
		eeprom_write_zoneprog1(progid_clear,0XFF);
		eeprom_write_zoneprog2(progid_clear,0XFF);
		eeprom_write_zoneprog3(progid_clear,0XFF);
		eeprom_read_zoneprog1(progid_clear,0XFF);
		eeprom_read_zoneprog2(progid_clear,0XFF);
		eeprom_read_zoneprog3(progid_clear,0XFF);
		for(m=0;m<10;m++)
		{
			eeprom_write_zoneprog4(progid_clear,m,0XFF);
			eeprom_read_zoneprog4(progid_clear,m,0XFF);
		}
		zone_clear = 0;
		progid_clear=0;
	}



	for(i=0;i<16;i++)
	{
		if(memcmp(&output[i],&output_remote[i],sizeof(output[i]))!=0)
		{
			output[i]=output_remote[i];
			eeprom_write_output(i);
		}
	}

	for(i=0;i<4;i++)
	{
		if(memcmp(&input[i],&input_remote[i],sizeof(input[i]))!=0)
		{
			input[i]=input_remote[i];
			eeprom_write_input(i);
		}
	}

	for(i=0;i<6;i++)
	{
		if(memcmp(&sensor[i],&sensor_remote[i],sizeof(sensor[i]))!=0)
		{
			sensor[i]=sensor_remote[i];
			eeprom_write_sensor(i);
		}
	}

	//Zone Data
	for(progid=0;progid<6;progid++)
	{
		for(ioid=0;ioid<10;ioid++)
		{
			if (zoneprog[progid].zone[ioid].oppartno!=zoneprog_remote[progid].zone[ioid].oppartno)
			{
				zoneprog[progid].zone[ioid].oppartno=zoneprog_remote[progid].zone[ioid].oppartno;
				eeprom_write_zoneprog4(progid,ioid,0x01);
			}
			if (zoneprog[progid].zone[ioid].starttime[0]!=zoneprog_remote[progid].zone[ioid].starttime[0])
			{
				zoneprog[progid].zone[ioid].starttime[0]=zoneprog_remote[progid].zone[ioid].starttime[0];
				eeprom_write_zoneprog4(progid,ioid,0x02);
			}
			if (zoneprog[progid].zone[ioid].starttime[1]!=zoneprog_remote[progid].zone[ioid].starttime[1])
			{
				zoneprog[progid].zone[ioid].starttime[1]=zoneprog_remote[progid].zone[ioid].starttime[1];
				eeprom_write_zoneprog4(progid,ioid,0x02);
			}
			if (zoneprog[progid].zone[ioid].stoptime[0]!=zoneprog_remote[progid].zone[ioid].stoptime[0])
			{
				zoneprog[progid].zone[ioid].stoptime[0]=zoneprog_remote[progid].zone[ioid].stoptime[0];
				eeprom_write_zoneprog4(progid,ioid,0x04);
			}
			if (zoneprog[progid].zone[ioid].stoptime[1]!=zoneprog_remote[progid].zone[ioid].stoptime[1])
			{
				zoneprog[progid].zone[ioid].stoptime[1]=zoneprog_remote[progid].zone[ioid].stoptime[1];
				eeprom_write_zoneprog4(progid,ioid,0x04);
			}
			if (zoneprog[progid].zone[ioid].startflow!=zoneprog_remote[progid].zone[ioid].startflow)
			{
				zoneprog[progid].zone[ioid].startflow=zoneprog_remote[progid].zone[ioid].startflow;
				eeprom_write_zoneprog4(progid,ioid,0x08);
			}
			if (zoneprog[progid].zone[ioid].stopflow!=zoneprog_remote[progid].zone[ioid].stopflow)
			{
				zoneprog[progid].zone[ioid].stopflow=zoneprog_remote[progid].zone[ioid].stopflow;
				eeprom_write_zoneprog4(progid,ioid,0x10);
			}
			if (zoneprog[progid].zone[ioid].fertprogid!=zoneprog_remote[progid].zone[ioid].fertprogid)
			{
				zoneprog[progid].zone[ioid].fertprogid=zoneprog_remote[progid].zone[ioid].fertprogid;
				eeprom_write_zoneprog4(progid,ioid,0x20);
			}
		}
	}

	//Program Data
	for(progid=0;progid<6;progid++)
	{
		if (zoneprog[progid].id!=zoneprog_remote[progid].id)
		{
			zoneprog[progid].id=zoneprog_remote[progid].id;
			eeprom_write_zoneprog1(progid,0x01);
		}
		if (zoneprog[progid].type!=zoneprog_remote[progid].type)
		{
			zoneprog[progid].type=zoneprog_remote[progid].type;
			eeprom_write_zoneprog1(progid,0x02);
		}
		if (zoneprog[progid].status!=zoneprog_remote[progid].status)
		{
			zoneprog[progid].status=zoneprog_remote[progid].status;
			eeprom_write_zoneprog1(progid,0x04);
			updated = 1;
		}
		if (zoneprog[progid].repeatcycles!=zoneprog_remote[progid].repeatcycles)
		{
			zoneprog[progid].repeatcycles=zoneprog_remote[progid].repeatcycles;
			eeprom_write_zoneprog1(progid,0x08);
			updated = 1;
		}
		if (zoneprog[progid].cycledelay!=zoneprog_remote[progid].cycledelay)
		{
			zoneprog[progid].cycledelay=zoneprog_remote[progid].cycledelay;
			eeprom_write_zoneprog1(progid,0x10);
			updated = 1;
		}
		if (zoneprog[progid].rain.condition!=zoneprog_remote[progid].rain.condition)
		{
			zoneprog[progid].rain.condition=zoneprog_remote[progid].rain.condition;
			eeprom_write_zoneprog3(progid,0x01);
		}
		if (zoneprog[progid].rain.currentval!=zoneprog_remote[progid].rain.currentval)
		{
			zoneprog[progid].rain.currentval=zoneprog_remote[progid].rain.currentval;
			eeprom_write_zoneprog3(progid,0x01);
		}
		if (zoneprog[progid].flow.condition!=zoneprog_remote[progid].flow.condition)
		{
			zoneprog[progid].flow.condition=zoneprog_remote[progid].flow.condition;
			eeprom_write_zoneprog3(progid,0x08);
		}
		if (zoneprog[progid].flow.currentval!=zoneprog_remote[progid].flow.currentval)
		{
			zoneprog[progid].flow.currentval=zoneprog_remote[progid].flow.currentval;
			eeprom_write_zoneprog3(progid,0x08);
		}
		if (zoneprog[progid].temperature.condition!=zoneprog_remote[progid].temperature.condition)
		{
			zoneprog[progid].temperature.condition=zoneprog_remote[progid].temperature.condition;
			eeprom_write_zoneprog3(progid,0x02);
		}
		if (zoneprog[progid].temperature.currentval!=zoneprog_remote[progid].temperature.currentval)
		{
			zoneprog[progid].temperature.currentval=zoneprog_remote[progid].temperature.currentval;
			eeprom_write_zoneprog3(progid,0x02);
		}
		if (zoneprog[progid].temperature.min!=zoneprog_remote[progid].temperature.min)
		{
			zoneprog[progid].temperature.min=zoneprog_remote[progid].temperature.min;
			eeprom_write_zoneprog3(progid,0x02);
		}
		if (zoneprog[progid].temperature.max!=zoneprog_remote[progid].temperature.max)
		{
			zoneprog[progid].temperature.max=zoneprog_remote[progid].temperature.max;
			eeprom_write_zoneprog3(progid,0x02);
		}
		if (zoneprog[progid].humidity.condition!=zoneprog_remote[progid].humidity.condition)
		{
			zoneprog[progid].humidity.condition=zoneprog_remote[progid].humidity.condition;
			eeprom_write_zoneprog3(progid,0x04);
		}
		if (zoneprog[progid].humidity.currentval!=zoneprog_remote[progid].humidity.currentval)
		{
			zoneprog[progid].humidity.currentval=zoneprog_remote[progid].humidity.currentval;
			eeprom_write_zoneprog3(progid,0x04);
		}
		if (zoneprog[progid].humidity.min!=zoneprog_remote[progid].humidity.min)
		{
			zoneprog[progid].humidity.min=zoneprog_remote[progid].humidity.min;
			eeprom_write_zoneprog3(progid,0x04);
		}
		if (zoneprog[progid].humidity.max!=zoneprog_remote[progid].humidity.max)
		{
			zoneprog[progid].humidity.max=zoneprog_remote[progid].humidity.max;
			eeprom_write_zoneprog3(progid,0x04);
		}
		if (zoneprog[progid].ippressure.condition!=zoneprog_remote[progid].ippressure.condition)
		{
			zoneprog[progid].ippressure.condition=zoneprog_remote[progid].ippressure.condition;
			eeprom_write_zoneprog3(progid,0x20);
		}
		if (zoneprog[progid].ippressure.currentval!=zoneprog_remote[progid].ippressure.currentval)
		{
			zoneprog[progid].ippressure.currentval=zoneprog_remote[progid].ippressure.currentval;
			eeprom_write_zoneprog3(progid,0x20);
		}
		if (zoneprog[progid].ippressure.min!=zoneprog_remote[progid].ippressure.min)
		{
			zoneprog[progid].ippressure.min=zoneprog_remote[progid].ippressure.min;
			eeprom_write_zoneprog3(progid,0x20);
		}
		if (zoneprog[progid].ippressure.max!=zoneprog_remote[progid].ippressure.max)
		{
			zoneprog[progid].ippressure.max=zoneprog_remote[progid].ippressure.max;
			eeprom_write_zoneprog3(progid,0x20);
		}
		if (zoneprog[progid].oppressure.condition!=zoneprog_remote[progid].oppressure.condition)
		{
			zoneprog[progid].oppressure.condition=zoneprog_remote[progid].oppressure.condition;
			eeprom_write_zoneprog3(progid,0x10);
		}
		if (zoneprog[progid].oppressure.currentval!=zoneprog_remote[progid].oppressure.currentval)
		{
			zoneprog[progid].oppressure.currentval=zoneprog_remote[progid].oppressure.currentval;
			eeprom_write_zoneprog3(progid,0x10);
		}
		if (zoneprog[progid].oppressure.min!=zoneprog_remote[progid].oppressure.min)
		{
			zoneprog[progid].oppressure.min=zoneprog_remote[progid].oppressure.min;
			eeprom_write_zoneprog3(progid,0x10);
		}
		if (zoneprog[progid].oppressure.max!=zoneprog_remote[progid].oppressure.max)
		{
			zoneprog[progid].oppressure.max=zoneprog_remote[progid].oppressure.max;
			eeprom_write_zoneprog3(progid,0x10);
		}
		if (zoneprog[progid].rundaysmode!=zoneprog_remote[progid].rundaysmode)
		{
			zoneprog[progid].rundaysmode=zoneprog_remote[progid].rundaysmode;
			eeprom_write_zoneprog1(progid,0x20);
			updated = 1;
		}
		if (zoneprog[progid].rundaysval[0]!=zoneprog_remote[progid].rundaysval[0])
		{
			zoneprog[progid].rundaysval[0]=zoneprog_remote[progid].rundaysval[0];
			eeprom_write_zoneprog1(progid,0x40);
			updated = 1;
		}
		if (zoneprog[progid].rundaysval[1]!=zoneprog_remote[progid].rundaysval[1])
		{
			zoneprog[progid].rundaysval[1]=zoneprog_remote[progid].rundaysval[1];
			eeprom_write_zoneprog1(progid,0x40);
			updated = 1;
		}
		if (zoneprog[progid].rundaysval[2]!=zoneprog_remote[progid].rundaysval[2])
		{
			zoneprog[progid].rundaysval[2]=zoneprog_remote[progid].rundaysval[2];
			eeprom_write_zoneprog1(progid,0x40);
			updated = 1;
		}
		if (zoneprog[progid].starttime[0]!=zoneprog_remote[progid].starttime[0])
		{
			zoneprog[progid].starttime[0]=zoneprog_remote[progid].starttime[0];
			eeprom_write_zoneprog2(progid,0x01);
			updated = 1;
		}
		if (zoneprog[progid].starttime[1]!=zoneprog_remote[progid].starttime[1])
		{
			zoneprog[progid].starttime[1]=zoneprog_remote[progid].starttime[1];
			eeprom_write_zoneprog2(progid,0x01);

			//on start time change update all zone start and stop times to program start time
			if(wireless_mode != 0)
			{//dont do for wifi local mode
			for(m=0;m<10;m++)
			{
				zoneprog[progid].zone[m].starttime[0]=zoneprog_remote[progid].starttime[0];
				zoneprog[progid].zone[m].starttime[1]=zoneprog_remote[progid].starttime[1];
				eeprom_write_zoneprog4(progid,m,0x02);
				eeprom_read_zoneprog4(progid,m,0x02);

				zoneprog[progid].zone[m].stoptime[0]=zoneprog_remote[progid].starttime[0];
				zoneprog[progid].zone[m].stoptime[1]=zoneprog_remote[progid].starttime[1];
				eeprom_write_zoneprog4(progid,m,0x04);
				eeprom_read_zoneprog4(progid,m,0x04);
			}
			}


			updated = 1;
		}
		if (zoneprog[progid].zonecount!=zoneprog_remote[progid].zonecount)
		{
			zoneprog[progid].zonecount=zoneprog_remote[progid].zonecount;
			eeprom_write_zoneprog2(progid,0x02);
			updated = 1;
		}
		if (zoneprog[progid].totaltime[0]!=zoneprog_remote[progid].totaltime[0])
		{
			zoneprog[progid].totaltime[0]=zoneprog_remote[progid].totaltime[0];
			eeprom_write_zoneprog2(progid,0x04);
			updated = 1;
		}
		if (zoneprog[progid].totaltime[1]!=zoneprog_remote[progid].totaltime[1])
		{
			zoneprog[progid].totaltime[1]=zoneprog_remote[progid].totaltime[1];
			eeprom_write_zoneprog2(progid,0x04);
			//on total time change update all zone start and stop times to program start time
			if(wireless_mode != 0)
			{
			for(m=0;m<10;m++)
			{
				zoneprog[progid].zone[m].starttime[0]=zoneprog_remote[progid].starttime[0];
				zoneprog[progid].zone[m].starttime[1]=zoneprog_remote[progid].starttime[1];
				eeprom_write_zoneprog4(progid,m,0x02);
				eeprom_read_zoneprog4(progid,m,0x02);

				zoneprog[progid].zone[m].stoptime[0]=zoneprog_remote[progid].starttime[0];
				zoneprog[progid].zone[m].stoptime[1]=zoneprog_remote[progid].starttime[1];
				eeprom_write_zoneprog4(progid,m,0x04);
				eeprom_read_zoneprog4(progid,m,0x04);
			}
			}
			updated = 1;
		}
		if (zoneprog[progid].totalflow!=zoneprog_remote[progid].totalflow)
		{
			zoneprog[progid].totalflow=zoneprog_remote[progid].totalflow;
			eeprom_write_zoneprog2(progid,0x08);
		}
		if (zoneprog[progid].pumpdelay!=zoneprog_remote[progid].pumpdelay)
		{
			zoneprog[progid].pumpdelay=zoneprog_remote[progid].pumpdelay;
			eeprom_write_zoneprog2(progid,0x10);
		}
		if (zoneprog[progid].zonedelay!=zoneprog_remote[progid].zonedelay)
		{
			zoneprog[progid].zonedelay=zoneprog_remote[progid].zonedelay;
			eeprom_write_zoneprog2(progid,0x20);
		}

		if(updated == 1)
		{
			updated = 0;
			if(CurTime.tm_24hr <= zoneprog[progid].starttime[0])//update remain time on program paramaters change
			{
				if(CurTime.tm_min <= zoneprog[progid].starttime[1])
				{
					Remaintime_update1(progid);
				}
			}
			else
			{
				//give alert program will start from tomm
			}
		}
	}

	//Ferti Valve Data
	for(progid=0;progid<6;progid++)
	{
		for(ioid=0;ioid<4;ioid++)
		{
			if (fertiprog[progid].valve[ioid].oppartno!=fertiprog_remote[progid].valve[ioid].oppartno)
			{
				fertiprog[progid].valve[ioid].oppartno=fertiprog_remote[progid].valve[ioid].oppartno;
				eeprom_write_fertiprog3(progid,ioid,0x01);
			}
			if (fertiprog[progid].valve[ioid].starttime!=fertiprog_remote[progid].valve[ioid].starttime)
			{
				fertiprog[progid].valve[ioid].starttime=fertiprog_remote[progid].valve[ioid].starttime;
				eeprom_write_fertiprog3(progid,ioid,0x02);
			}
			if (fertiprog[progid].valve[ioid].stoptime!=fertiprog_remote[progid].valve[ioid].stoptime)
			{
				fertiprog[progid].valve[ioid].stoptime=fertiprog_remote[progid].valve[ioid].stoptime;
				eeprom_write_fertiprog3(progid,ioid,0x04);
			}
			if (fertiprog[progid].valve[ioid].startflow!=fertiprog_remote[progid].valve[ioid].startflow)
			{
				fertiprog[progid].valve[ioid].startflow=fertiprog_remote[progid].valve[ioid].startflow;
				eeprom_write_fertiprog3(progid,ioid,0x08);
			}
			if (fertiprog[progid].valve[ioid].stopflow!=fertiprog_remote[progid].valve[ioid].stopflow)
			{
				fertiprog[progid].valve[ioid].stopflow=fertiprog_remote[progid].valve[ioid].stopflow;
				eeprom_write_fertiprog3(progid,ioid,0x10);
			}
		}
	}


	//Ferti Program Data
	for(progid=0;progid<6;progid++)
	{
		if (fertiprog[progid].id!=fertiprog_remote[progid].id)
		{
			fertiprog[progid].id=fertiprog_remote[progid].id;
			eeprom_write_fertiprog1(progid,0x01);
		}
		if (fertiprog[progid].method!=fertiprog_remote[progid].method)
		{
			fertiprog[progid].method=fertiprog_remote[progid].method;
			eeprom_write_fertiprog1(progid,0x04);
		}
		if (fertiprog[progid].propcycles!=fertiprog_remote[progid].propcycles)
		{
			fertiprog[progid].propcycles=fertiprog_remote[progid].propcycles;
			eeprom_write_fertiprog1(progid,0x08);
		}
		if (fertiprog[progid].type!=fertiprog_remote[progid].type)
		{
			fertiprog[progid].type=fertiprog_remote[progid].type;
			eeprom_write_fertiprog1(progid,0x02);
		}
		if (fertiprog[progid].valvecount!=fertiprog_remote[progid].valvecount)
		{
			fertiprog[progid].valvecount=fertiprog_remote[progid].valvecount;
			eeprom_write_fertiprog2(progid,0x01);
		}
		if (fertiprog[progid].startdelay!=fertiprog_remote[progid].startdelay)
		{
			fertiprog[progid].startdelay=fertiprog_remote[progid].startdelay;
			eeprom_write_fertiprog1(progid,0x10);
		}
		if (fertiprog[progid].stopdelay!=fertiprog_remote[progid].stopdelay)
		{
			fertiprog[progid].stopdelay=fertiprog_remote[progid].stopdelay;
			eeprom_write_fertiprog1(progid,0x20);
		}
		if (fertiprog[progid].totaltime!=fertiprog_remote[progid].totaltime)
		{
			fertiprog[progid].totaltime=fertiprog_remote[progid].totaltime;
			eeprom_write_fertiprog2(progid,0x02);
		}
		if (fertiprog[progid].totalflow!=fertiprog_remote[progid].totalflow)
		{
			fertiprog[progid].totalflow=fertiprog_remote[progid].totalflow;
			eeprom_write_fertiprog2(progid,0x04);
		}

	}

	//Setup Data
	if (datetimesource != datetimesource_remote)
	{
		datetimesource = datetimesource_remote;
		eeprom_write_timedate(0x01);
	}
	if (noopmodules != noopmodules_remote)
	{
		noopmodules = noopmodules_remote;
		eeprom_write_hardwareconfig(0x01);
	}
	if (fertigation!=fertigation_remote)
	{
		fertigation=fertigation_remote;
		eeprom_write_hardwareconfig(0x02);
	}
	if (flowperpulse!=flowperpulse_remote)
	{
		flowperpulse=flowperpulse_remote;
		eeprom_write_hardwareconfig(0x04);
	}
	if (minop != minop_remote)
	{
		minop=minop_remote;
		eeprom_write_hardwareconfig(0x08);
	}
	if (maxdp != maxdp_remote)
	{
		maxdp=maxdp_remote;
		eeprom_write_hardwareconfig(0x10);
	}
	if (language!= language_remote)
	{
		language=language_remote;
		eeprom_write_systemsetup(0x01);
	}
	if (contrast!=contrast_remote)
	{
		contrast=contrast_remote;
		eeprom_write_systemsetup(0x02);
	}
	if (disp_clockformat != disp_clockformat_remote)
	{
		disp_clockformat=disp_clockformat_remote;
		eeprom_write_systemsetup(0x04);
	}
	if (dateformat!=dateformat_remote)
	{
		dateformat=dateformat_remote;
		eeprom_write_systemsetup(0x08);
	}
	if (beginweek!=beginweek_remote)
	{
		beginweek=beginweek_remote;
		eeprom_write_systemsetup(0x10);
	}

	//Backflush
	if (backflushstatus!=backflushstatus_remote)
	{
		backflushstatus=backflushstatus_remote;
		eeprom_write_backflush(0x01);
	}
	if (backflushinterval[0]!=backflushinterval_remote[0])
	{
		backflushinterval[0]=backflushinterval_remote[0];
		eeprom_write_backflush(0x02);
	}
	if (backflushinterval[1]!=backflushinterval_remote[1])
	{
		backflushinterval[1]=backflushinterval_remote[1];
		eeprom_write_backflush(0x02);
	}
	if (backflushduration!=backflushduration_remote)
	{
		backflushduration=backflushduration_remote;
		eeprom_write_backflush(0x04);
	}
	if (backflushdelay!=backflushdelay_remote)
	{
		backflushdelay=backflushdelay_remote;
		eeprom_write_backflush(0x08);
	}
	if (deltapres!=deltapres_remote)
	{
		deltapres=deltapres_remote;
		eeprom_write_backflush(0x10);
	}
	if (deltadelay!=deltadelay_remote)
	{
		deltadelay=deltadelay_remote;
		eeprom_write_backflush(0x20);
	}
	if (deltaitera!=deltaitera_remote)
	{
		deltaitera=deltaitera_remote;
		eeprom_write_backflush(0x40);
	}
	if (dwelltime!=dwelltime_remote)
	{
		dwelltime=dwelltime_remote;
		eeprom_write_backflush(0x80);
	}

	//Network Settings
	if (wireless_mode!=wireless_mode_remote)
	{
		wireless_mode=wireless_mode_remote;
		eeprom_write_network_gsm(0x01);
		//used to reset the controller using software
		HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
	}
	if (gsm_module!=gsm_module_remote)
	{
		gsm_module=gsm_module_remote;
		eeprom_write_network_gsm(0x02);
	}
	if(memcmp(&gsm_apn,&gsm_apn_remote,sizeof(gsm_apn))!=0)
	{
		strncpy(gsm_apn,gsm_apn_remote,20);
		eeprom_write_network_gsm(0x04);
	}
	if(memcmp(&gsm_operator,&gsm_operator_remote,sizeof(gsm_operator))!=0)
	{
		strncpy(gsm_operator,gsm_operator_remote,20);
		eeprom_write_network_gsm(0x08);
	}
	if (sig_strength!=sig_strength_remote)
	{
		sig_strength=sig_strength_remote;
		eeprom_write_network_gsm(0x10);
	}
	if (wifi_module!=wifi_module_remote)
	{
		wifi_module=wifi_module_remote;
		eeprom_write_network_wifi(0x01);
	}
	if(memcmp(&wifi_name,&wifi_name_remote,sizeof(wifi_name))!=0)
	{
		strncpy(wifi_name,wifi_name_remote,20);
		eeprom_write_network_wifi(0x02);
	}
	if(memcmp(&wifi_pass,&wifi_pass_remote,sizeof(wifi_pass))!=0)
	{
		strncpy(wifi_pass,wifi_pass_remote,20);
		eeprom_write_network_wifi(0x04);
	}
	if(memcmp(&ext_apn,&ext_apn_remote,sizeof(ext_apn))!=0)
	{
		strncpy(ext_apn,ext_apn_remote,20);
		eeprom_write_network_wifi(0x08);
	}
	if(memcmp(&ext_pass,&ext_pass_remote,sizeof(ext_pass))!=0)
	{
		strncpy(ext_pass,ext_pass_remote,20);
		eeprom_write_network_wifi(0x10);
	}
	if(memcmp(&portal_ip,&portal_ip_remote,sizeof(portal_ip))!=0)
	{
		strncpy(portal_ip,portal_ip_remote,4);
		eeprom_write_network_wifi(0x20);
	}

	if(memcmp(&portal_name,&portal_name_remote,sizeof(portal_name))!=0)
	{
		strncpy(portal_name,portal_name_remote,40);
		eeprom_write_network_wifi(0x40);
	}
	remote_data_recevied_flag=0;

}


//void GSMread_callback(UART_Handle p1, void *buf, size_t count)
//{
//
//}
//
//void GSMwrite_callback(UART_Handle p1, void *buf, size_t count)
//{
//
//}
//$$NR$$//dated:23Aug15
void init_gsm_uart()
{
	/* Create a UART with data processing off. */
	UART_Params_init(&uartParams);
	uartParams.writeMode = UART_MODE_BLOCKING;
	uartParams.readMode = UART_MODE_BLOCKING;
	uartParams.writeDataMode = UART_DATA_BINARY;
	uartParams.readDataMode = UART_DATA_BINARY;
	uartParams.readReturnMode = UART_RETURN_NEWLINE;
	uartParams.readEcho = UART_ECHO_OFF;
	uartParams.baudRate = 115200;
	//uartParams.readTimeout = 100;
	//uartParams.writeTimeout = 100;

	uart = UART_open(Board_UART3, &uartParams);

	if (uart == NULL) {
	//	System_abort("Error opening the UART");
	}
}
//$$NR$$//dated:23Aug15

/******************************************************************
* Function Name  : init_gsm
* Description    : Take the Actions as per selected mode
* input_buffer   : None
* Output         : None
* Return         : None
*******************************************************************/
void init_gsm()
{
	/* Create a UART with data processing off. */
	UART_Params_init(&uartParams);
	uartParams.writeMode = UART_MODE_BLOCKING;
	uartParams.readMode = UART_MODE_BLOCKING;
	uartParams.writeDataMode = UART_DATA_BINARY;
	uartParams.readDataMode = UART_DATA_TEXT;
	uartParams.readReturnMode = UART_RETURN_NEWLINE;
	uartParams.readEcho = UART_ECHO_OFF;
	uartParams.baudRate = 19200;//38400;//57600;//115200;
	uartParams.readTimeout = 500;
	uartParams.writeTimeout = 500;

	uart = UART_open(Board_UART3, &uartParams);

	if (uart == NULL) {
	//	System_abort("Error opening the UART");
	}


    //Init GSM
	/*Reset The Module*/
	GPIO_write(Board_GSMRST,(~0));//LOGIC 0---RST ON
	flag_30s=1;
	while(flag_30s<=500){//Wait for 5 Seconds
	Task_sleep(20);
	}
	GPIO_write(Board_GSMRST,(0));//LOGIC 1---RST OFF


	/*Wait for 25sec for the module to stablilize*/
	flag_30s=1;
	while(flag_30s<=2500){
		Task_sleep(20);
	}

	gsm_config=GSM_Configure();

	eeprom_write_network_gsm(0x18);
	eeprom_read_network_gsm(0x18);

	gsm_activ=GSM_Activate();

}



void GSM_Main()
{
	int i=0,j=0,k=0,l=0,m=0;
	int start=0,end=0;

	if((gsm_activ==1)&&(gsm_config==1))
	{
		if((flag_30s>=1000)&&(commu_on==0))
		{
			flag_30s=1;
			commu_on=1;
			UART_writePolling(uart, dial1, sizeof(dial1));
		}
		memset(input_buffer,0, sizeof(input_buffer));

		for(i=0;i<8;i++)
		{
			memset(message[i],0, sizeof(message[i]));
		}

		k=0;

		if(UARTCharsAvail(UART3_BASE))
		{
			UART_readPolling(uart, &input_buffer, sizeof(input_buffer));

		}

		for(i=0;i<1;i++)
		{
			if((input_buffer[i]==0)&&(input_buffer[i+1]==0))		Task_sleep(1);

			if((input_buffer[i]=='*')&&(input_buffer[i+1]=='|'))
			{
				//System_printf("%s",input_buffer);
				noofmsg++;
				j=i+2;	start=j;	k=j;
				do
				{
					if(((input_buffer[k]=='|')&&(input_buffer[k+1]=='|'))||((input_buffer[k]=='|')&&(input_buffer[k+1]=='*')))
					{
						end=k;	k=k+1;
						if(k>999)	k=0;

						while(start<end)
						{
							message[m][l]=input_buffer[start];
							l++;	start++;
						}
						m++;
						if(m==7)	m=0;
						l=0;
						start=start+2;
					}
					k++;
					if(k>199)	k=0;

					j++;
					if(j>199)	j=0;

				}while((input_buffer[j]!='*'));

				strcpy(str, "nodata");
				if(strncmp(message[0],str,6)!=0)
				{
					remote_data_recevied_flag = 1;
				}
				tempmsgid=(uint32_t)strtoint1(message[3]);
			}
		}

		if(remote_data_recevied_flag == 1)	message_store();

		start=0;	end=0;	i=0;	j=0;
		k=0;	l=0;	m=0;

		strcpy(str, "OK");
		if(strncmp(input_buffer,str,2)==0)
		{
			//UART_writePolling(uart, dial, sizeof(dial));
		}
		strcpy(str, "CONNECT");
		if(strncmp(input_buffer,str,7)==0)
		{
			commu_on=1;
			if((PanelState==psPassword)||(PanelState==psSetup)||(PanelState==psManual)||(PanelState==psProgram)||(PanelState==psInitSetup))
			{
				PrevPanelState=PanelState;
				localaccessflag=2;

				memset(temp,0, sizeof(temp));
				sprintf (temp,"*|Access_data||LocalAccessOn||%d||0||%s||0||0|*\r\n",localaccessflag,deviceid);

				memset(ouput_buffer,0, sizeof(ouput_buffer));
				memset(len,0, sizeof(len));

				sprintf(len, "%d", (strlen(temp)+8));
				strcat (ouput_buffer,msgtoportal);
				strcat (ouput_buffer,len);
				strcat (ouput_buffer,"\r\n\r\n");
				strcat (ouput_buffer,"ackdata=");
				strcat (ouput_buffer,temp);

				UART_writePolling(uart, ouput_buffer, strlen(ouput_buffer));
			}
			if(((PanelState==psHome)||(PanelState==psLogs))&&(PrevPanelState==psHome))
			{
				if(logsend == 0)
				{
					//Read_log(0);
					Read_log_portal();
					memset(ouput_buffer,0, sizeof(ouput_buffer));
					memset(len,0, sizeof(len));

					sprintf(len, "%d", (strlen(logfile_buff)+34));
					strcat (ouput_buffer,logtoportal);
					strcat (ouput_buffer,len);
					strcat (ouput_buffer,"\r\n\r\n");
					strcat (ouput_buffer,"logdata=");
					strcat (ouput_buffer,"*|log_data||");
					strcat (ouput_buffer,deviceid);
					strcat (ouput_buffer,"||");
					strcat (ouput_buffer,logfile_buff);
					strcat (ouput_buffer,"|*\r\n");
					memset(logfile_buff,0, sizeof(logfile_buff));
					logsend = 1;
					UART_writePolling(uart, ouput_buffer, strlen(ouput_buffer));
				}
				else
				{
					if(sendack==0)
					{
						noofmsg=0;
						memset(ouput_buffer,0, sizeof(ouput_buffer));
						memset(len,0, sizeof(len));

						strcat (ouput_buffer,msgfromportal1);
						sprintf(len, "%d", (strlen(deviceid)+13));
						strcat (ouput_buffer,len);
						strcat (ouput_buffer,"\r\n\r\n");
						strcat (ouput_buffer,"deviceiddata=");
						strcat (ouput_buffer,deviceid);

						UART_writePolling(uart, ouput_buffer, strlen(ouput_buffer));
						sendack=1;
						//remote_data_recevied_flag = 1;
					}
					else
					{
						sendack=0;
						synctoportal(10);
						UART_writePolling(uart, ouput_buffer, strlen(ouput_buffer));
					}
				}
			}
			if((PanelState==psHome)&&(PrevPanelState!=psHome))
			{
				if(localaccessflag==2)
				{
					memset(temp,0, sizeof(temp));
					sprintf (temp,"*|Access_data||LocalAccessOn||%d||0||%s||0||0|*\r\n",localaccessflag,deviceid);

					memset(ouput_buffer,0, sizeof(ouput_buffer));
					memset(len,0, sizeof(len));

					sprintf(len, "%d", (strlen(temp)+8));
					strcat (ouput_buffer,msgtoportal);
					strcat (ouput_buffer,len);
					strcat (ouput_buffer,"\r\n\r\n");
					strcat (ouput_buffer,"ackdata=");
					strcat (ouput_buffer,temp);
					UART_writePolling(uart, ouput_buffer, strlen(ouput_buffer));
					localaccessflag=0;
				}
				else
				{
					if(system_rst==1)
					{
						memset(temp,0, sizeof(temp));
						memset(ouput_buffer,0, sizeof(ouput_buffer));
						memset(len,0, sizeof(len));

						sprintf (temp,"*|Access_data||LocalAccessOn||%d||0||%s||0||0|*\r\n",localaccessflag,deviceid);
						strcat (temp1,temp);

						sprintf(len, "%d", (strlen(temp1)+8));
						strcat (ouput_buffer,msgtoportal);
						strcat (ouput_buffer,len);
						strcat (ouput_buffer,"\r\n\r\n");
						strcat (ouput_buffer,"ackdata=");
						strcat (ouput_buffer,temp1);
						memset(temp1,0, sizeof(temp1));
						PrevPanelState=psHome;
					}
					if(system_rst==0)
					{
						if(PrevPanelState!=psHome)
						{
						//	synccount=9;//todo
							//synccount++;

							for(synccount=1;synccount<=9;synccount++)
							{
								if(table_changed[synccount] == 1) break;
							}

							synctoportal(synccount);


							if(synccount>9)
							{
								PrevPanelState=psHome;
								synccount=0;
								system_rst=1;
							}
						}
					}
					UART_writePolling(uart, ouput_buffer, strlen(ouput_buffer));
				}
			}
		}
		strcpy(str, "NO CARRIER");
		if(strncmp(input_buffer,str,10)==0)
		{
			commu_on=0;
			flag_30s=0;

			if(PrevPanelState!=psHome)	flag_30s=950;

			if(sendack==1)	flag_30s=950;

		//	GSM_EEPROM_UPDATE();
		}

		strcpy(str, "ERROR");
		if(strncmp(input_buffer,str,5)==0)
		{
			gsm_config=0;
			gsm_activ=0;

			commu_on=0;
			flag_30s=0;
//		  /*Reset The Module*/
//			GPIO_write(Board_GSMRST,(~0));//LOGIC 0---RST ON
//			flag_30s=1;
//			while(flag_30s<=3000){//Wait for 5 Seconds
//			Task_sleep(20);
//			}
//			GPIO_write(Board_GSMRST,(0));//LOGIC 1---RST OFF
//
//			/*Wait for 1/2 Minute for the module to stablilize*/
//			flag_30s=1;
//			while(flag_30s<=3000){
//				Task_sleep(20);
//			}
//			flag_30s=1;
//			while(flag_30s<500) Task_sleep(20);
//			gsm_config=GSM_Configure();
//			gsm_activ=GSM_Activate();
		}

		//timeout 5 min
		if(flag_30s>30000)//todo pls checck
		{
			gsm_config=0;
			gsm_activ=0;

			commu_on=0;
			flag_30s=0;
		}
	}
	else
	{
		//Task_sleep(20);
		Task_sleep(2000);
		/*Reset The Module*/
		GPIO_write(Board_GSMRST,(~0));//LOGIC 0---RST ON
		flag_30s=1;
		while(flag_30s<=500){//Wait for 5 Seconds
		Task_sleep(20);
		}
		GPIO_write(Board_GSMRST,(0));//LOGIC 1---RST OFF

		/*Wait for 1/2 Minute for the module to stablilize*/
		flag_30s=1;
		while(flag_30s<=3000){
			Task_sleep(20);
		}
		flag_30s=1;
		while(flag_30s<500) Task_sleep(20);
		gsm_config=GSM_Configure();
		gsm_activ=GSM_Activate();
	}
}


//$$NR$$//dated:23Aug15
// debug - log
void debug_logdata()
{
#if	GSM_UART_FOR_DEBUG
static uint8_t uart_init_flag = false;

	if (uart_init_flag == false)
	{
		uart_init_flag = true;
		init_gsm_uart();
	}

	UART_writePolling(uart, strmsglog, strlen(strmsglog));
#endif
}

//$$NR$$//dated:23Aug15
