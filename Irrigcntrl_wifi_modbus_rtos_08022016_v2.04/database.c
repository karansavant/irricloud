//*****************************************************************************
//
// database.c - EEPROM.
//
//*****************************************************************************


#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Tivaware Libraries*/
#include <driverlib/sysctl.h>

//#include "driverlib/eeprom.h"
#include "eeprom_ud.h"

#include "driverlib/sysctl.h"
#include "utils/ustdlib.h"
#include "utils/cmdline.h"
#include <xdc/runtime/System.h>

#include "database_struct.h"
//*****************************************************************************
#define Get_EPPROM_Address		0
//*****************************************************************************

#define SoftwareVersionID "0.52"				//  Software Version.//$$NR$$//dated:16Aug15

#define EEPROM_BASE_ADDR					0x0000

#define EEPROM_INITIAL_BASE_ADDR			EEPROM_BASE_ADDR
#define EEPROM_INITIAL_SIZE					(EEPROM_BASE_ADDR + (sizeof(uint8_t) * 50))

#define EEPROM_CONFIG_BASE_ADDR				(EEPROM_BASE_ADDR + 0x0032)

#define EEPROM_CONTACT_INFO_BASE_ADDR		EEPROM_CONFIG_BASE_ADDR
#define EEPROM_CONTACT_INFO_PARA_1_ADDR		EEPROM_CONTACT_INFO_BASE_ADDR
#define EEPROM_CONTACT_INFO_PARA_2_ADDR		(EEPROM_CONTACT_INFO_BASE_ADDR + (sizeof(char) * 21))
#define EEPROM_CONTACT_INFO_PARA_3_ADDR		(EEPROM_CONTACT_INFO_BASE_ADDR + (sizeof(char) * 21) + (sizeof(char) * 21))
#define EEPROM_CONTACT_INFO_PARA_SIZE		((sizeof(char) * 21) + (sizeof(char) * 21) + (sizeof(char) * 21))	// phone 1 / phone 2 / e-mail

#define EEPROM_TIMEDATE_BASE_ADDR			(EEPROM_CONTACT_INFO_BASE_ADDR + EEPROM_CONTACT_INFO_PARA_SIZE)
#define EEPROM_TIMEDATE_PARA_1_ADDR			EEPROM_TIMEDATE_BASE_ADDR
#define EEPROM_TIMEDATE_SIZE				sizeof(uint8_t)					// date time source

#define EEPROM_HW_CONFIG_BASE_ADDR			(EEPROM_TIMEDATE_BASE_ADDR + EEPROM_TIMEDATE_SIZE)
#define EEPROM_HW_CONFIG_PARA_1_ADDR		EEPROM_HW_CONFIG_BASE_ADDR
#define EEPROM_HW_CONFIG_PARA_2_ADDR		(EEPROM_HW_CONFIG_BASE_ADDR + sizeof(uint8_t))
#define EEPROM_HW_CONFIG_PARA_3_ADDR		(EEPROM_HW_CONFIG_BASE_ADDR + sizeof(uint8_t) + sizeof(uint8_t))
#define EEPROM_HW_CONFIG_PARA_4_ADDR		(EEPROM_HW_CONFIG_BASE_ADDR + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint32_t))
#define EEPROM_HW_CONFIG_PARA_5_ADDR		(EEPROM_HW_CONFIG_BASE_ADDR + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(float))
#define EEPROM_HW_CONFIG_SIZE				(sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(float) + sizeof(float))	// noopmodules / fertigation / flowperpulse / min op / max dp

#define EEPROM_SYS_SETUP_BASE_ADDR			(EEPROM_HW_CONFIG_BASE_ADDR + EEPROM_HW_CONFIG_SIZE)
#define EEPROM_SYS_SETUP_PARA_1_ADDR		EEPROM_SYS_SETUP_BASE_ADDR
#define EEPROM_SYS_SETUP_PARA_2_ADDR		(EEPROM_SYS_SETUP_BASE_ADDR + sizeof(uint8_t))
#define EEPROM_SYS_SETUP_PARA_3_ADDR		(EEPROM_SYS_SETUP_BASE_ADDR + sizeof(uint8_t) + sizeof(uint8_t))
#define EEPROM_SYS_SETUP_PARA_4_ADDR		(EEPROM_SYS_SETUP_BASE_ADDR + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t))
#define EEPROM_SYS_SETUP_PARA_5_ADDR		(EEPROM_SYS_SETUP_BASE_ADDR + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t))
#define EEPROM_SYS_SETUP_SIZE				(sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t))	// language / contrast / disp_clockformat / dateformat / beginweek

#define EEPROM_OUTPUT_BASE_ADDR				(EEPROM_SYS_SETUP_BASE_ADDR + EEPROM_SYS_SETUP_SIZE)
#define EEPROM_OUTPUT_SIZE					(sizeof(op_data) * OUTPUT_MAX)	// outputs

#define EEPROM_INPUT_BASE_ADDR				(EEPROM_OUTPUT_BASE_ADDR + EEPROM_OUTPUT_SIZE)
#define EEPROM_INPUT_SIZE					(sizeof(ip_data) * INPUT_MAX)	// inputs

#define EEPROM_SENSOR_BASE_ADDR				(EEPROM_INPUT_BASE_ADDR + EEPROM_INPUT_SIZE)
#define EEPROM_SENSOR_SIZE					(sizeof(sensor_data) * SENSOR_MAX)	// sensors

#define EEPROM_NETWORK_BASE_ADDR			(EEPROM_SENSOR_BASE_ADDR + EEPROM_SENSOR_SIZE)
#define EEPROM_NETWORK_PARA_1_ADDR			EEPROM_NETWORK_BASE_ADDR//wireless_mode
#define EEPROM_NETWORK_PARA_2_ADDR			(EEPROM_NETWORK_BASE_ADDR + sizeof(char))//gsm_module
#define EEPROM_NETWORK_PARA_3_ADDR			(EEPROM_NETWORK_BASE_ADDR + sizeof(char) + sizeof(char))// gsm_apn[20]
#define EEPROM_NETWORK_PARA_4_ADDR			(EEPROM_NETWORK_BASE_ADDR + sizeof(char) + sizeof(char) + (sizeof(char) * 20))// gsm_operator[20]
#define EEPROM_NETWORK_PARA_5_ADDR			(EEPROM_NETWORK_BASE_ADDR + sizeof(char) + sizeof(char) + (sizeof(char) * 20) + (sizeof(char) * 20))//sig_strength
#define EEPROM_NETWORK_SIZE					(sizeof(char) + sizeof(char) + (sizeof(char) * 20) + (sizeof(char) * 20) + sizeof(char))


#define EEPROM_NETWORK1_BASE_ADDR			(EEPROM_NETWORK_BASE_ADDR + EEPROM_NETWORK_SIZE)
#define EEPROM_NETWORK1_PARA_1_ADDR			EEPROM_NETWORK1_BASE_ADDR//wifi_module
#define EEPROM_NETWORK1_PARA_2_ADDR			(EEPROM_NETWORK1_BASE_ADDR + sizeof(char))//wifi_name[20]
#define EEPROM_NETWORK1_PARA_3_ADDR			(EEPROM_NETWORK1_BASE_ADDR + sizeof(char) + (sizeof(char) * 20))// wifi_pass[20]
#define EEPROM_NETWORK1_PARA_4_ADDR			(EEPROM_NETWORK1_BASE_ADDR + sizeof(char) + (sizeof(char) * 20) + (sizeof(char) * 20))// //ext_apn[20]
#define EEPROM_NETWORK1_PARA_5_ADDR			(EEPROM_NETWORK1_BASE_ADDR + sizeof(char) + (sizeof(char) * 20) + (sizeof(char) * 20) + (sizeof(char) * 20))//ext_pass[20]
#define EEPROM_NETWORK1_PARA_6_ADDR			(EEPROM_NETWORK1_BASE_ADDR + sizeof(char) + (sizeof(char) * 20) + (sizeof(char) * 20) + (sizeof(char) * 20)+ (sizeof(char) * 20))//portal_ip[20]
#define EEPROM_NETWORK1_PARA_7_ADDR			(EEPROM_NETWORK1_BASE_ADDR + sizeof(char) + (sizeof(char) * 20) + (sizeof(char) * 20) + (sizeof(char) * 20)+ (sizeof(char) * 20)+ (sizeof(char) * 20))//portal_name[40]
#define EEPROM_NETWORK1_SIZE				(sizeof(char) + (sizeof(char) * 20) + (sizeof(char) * 20) + (sizeof(char) * 20) + (sizeof(char) * 20)+ (sizeof(char) * 20)+ (sizeof(char) * 40))


#define EEPROM_BACKFLUSH_BASE_ADDR			(EEPROM_NETWORK1_BASE_ADDR + EEPROM_NETWORK1_SIZE)
#define EEPROM_BACKFLUSH_PARA_1_ADDR		EEPROM_BACKFLUSH_BASE_ADDR
#define EEPROM_BACKFLUSH_PARA_2_ADDR		(EEPROM_BACKFLUSH_BASE_ADDR + sizeof(uint8_t))
#define EEPROM_BACKFLUSH_PARA_3_ADDR		(EEPROM_BACKFLUSH_BASE_ADDR + sizeof(uint8_t) + sizeof(uint16_t))
#define EEPROM_BACKFLUSH_PARA_4_ADDR		(EEPROM_BACKFLUSH_BASE_ADDR + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t))
#define EEPROM_BACKFLUSH_PARA_5_ADDR		(EEPROM_BACKFLUSH_BASE_ADDR + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t))
#define EEPROM_BACKFLUSH_PARA_6_ADDR		(EEPROM_BACKFLUSH_BASE_ADDR + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint8_t))
#define EEPROM_BACKFLUSH_PARA_7_ADDR		(EEPROM_BACKFLUSH_BASE_ADDR + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint16_t))
#define EEPROM_BACKFLUSH_PARA_8_ADDR		(EEPROM_BACKFLUSH_BASE_ADDR + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint8_t))
#define EEPROM_BACKFLUSH_SIZE				(sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint16_t))
											// backflushstatus / backflushinterval[2] / backflushduration / backflushdelay / deltapres / deltadelay / deltaitera / dwelltime

#define EEPROM_AUTHEN_BASE_ADDR				(EEPROM_BACKFLUSH_BASE_ADDR + EEPROM_BACKFLUSH_SIZE)
#define EEPROM_AUTHEN_SIZE					((sizeof(char) * 11) + (sizeof(char) * 11) + (sizeof(char) * 11) + (sizeof(char) * 11))	// Admin / Engineer / Owner / Operator passwords

#define EEPROM_FIRMWARE_BASE_ADDR			(EEPROM_AUTHEN_BASE_ADDR + EEPROM_AUTHEN_SIZE)
#define EEPROM_FIRMWARE_PARA_1_ADDR			EEPROM_FIRMWARE_BASE_ADDR
#define EEPROM_FIRMWARE_PARA_2_ADDR			(EEPROM_FIRMWARE_BASE_ADDR + (sizeof(char) * 13))
#define EEPROM_FIRMWARE_PARA_3_ADDR			(EEPROM_FIRMWARE_BASE_ADDR + (sizeof(char) * 13) + (sizeof(char) * 11))
#define EEPROM_FIRMWARE_PARA_4_ADDR			(EEPROM_FIRMWARE_BASE_ADDR + (sizeof(char) * 13) + (sizeof(char) * 11) + sizeof(float))
#define EEPROM_FIRMWARE_PARA_5_ADDR			(EEPROM_FIRMWARE_BASE_ADDR + (sizeof(char) * 13) + (sizeof(char) * 11) + sizeof(float) + (sizeof(char) * 11))
#define EEPROM_FIRMWARE_PARA_6_ADDR			(EEPROM_FIRMWARE_BASE_ADDR + (sizeof(char) * 13) + (sizeof(char) * 11) + sizeof(float) + (sizeof(char) * 11) + (sizeof(char) * 11))
#define EEPROM_FIRMWARE_SIZE				((sizeof(char) * 13) + (sizeof(char) * 11) + sizeof(float) + (sizeof(char) * 11) + (sizeof(char) * 11) + (sizeof(char) * 11))
											// model / device unique id / firmware version / enet mac id / wifi mac id / imei no

#define EEPROM_FERTIPROG_BASE_ADDR			(EEPROM_FIRMWARE_BASE_ADDR + EEPROM_FIRMWARE_SIZE)
#define EEPROM_FERTIPROG_SIZE				(sizeof(ferti_prog_data) * FERTI_PROG_MAX)	// ferti progs

#define EEPROM_FERTIPROG1_BASE_ADDR			EEPROM_FERTIPROG_BASE_ADDR
#define EEPROM_FERTIPROG1_PARA_1_ADDR		EEPROM_FERTIPROG1_BASE_ADDR//id
#define EEPROM_FERTIPROG1_PARA_2_ADDR		(EEPROM_FERTIPROG1_BASE_ADDR+sizeof(uint8_t))//type
#define EEPROM_FERTIPROG1_PARA_3_ADDR		(EEPROM_FERTIPROG1_BASE_ADDR+sizeof(uint8_t)+sizeof(uint8_t))//method
#define EEPROM_FERTIPROG1_PARA_4_ADDR		(EEPROM_FERTIPROG1_BASE_ADDR+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t))//propcycles
#define EEPROM_FERTIPROG1_PARA_5_ADDR		(EEPROM_FERTIPROG1_BASE_ADDR+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t))//startdelay
#define EEPROM_FERTIPROG1_PARA_6_ADDR		(EEPROM_FERTIPROG1_BASE_ADDR+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint16_t))//stopdelay
#define EEPROM_FERTIPROG1_SIZE				(sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint16_t)+sizeof(uint16_t))


#define EEPROM_FERTIPROG2_BASE_ADDR			(EEPROM_FERTIPROG1_BASE_ADDR+EEPROM_FERTIPROG1_SIZE)
#define EEPROM_FERTIPROG2_PARA_1_ADDR		EEPROM_FERTIPROG2_BASE_ADDR//valvecount
#define EEPROM_FERTIPROG2_PARA_2_ADDR		(EEPROM_FERTIPROG2_BASE_ADDR+sizeof(uint8_t))//totaltime
#define EEPROM_FERTIPROG2_PARA_3_ADDR		(EEPROM_FERTIPROG2_BASE_ADDR+sizeof(uint8_t)+sizeof(uint16_t))//totalflow
#define EEPROM_FERTIPROG2_SIZE				(sizeof(uint8_t)+sizeof(uint16_t)+sizeof(uint32_t))//totalflow

#define EEPROM_FERTIPROG3_BASE_ADDR			(EEPROM_FERTIPROG2_BASE_ADDR+EEPROM_FERTIPROG2_SIZE)
#define EEPROM_FERTIPROG3_PARA_1_ADDR		EEPROM_FERTIPROG3_BASE_ADDR//oppartno
#define EEPROM_FERTIPROG3_PARA_2_ADDR		(EEPROM_FERTIPROG3_BASE_ADDR+sizeof(uint8_t))//starttime
#define EEPROM_FERTIPROG3_PARA_3_ADDR		(EEPROM_FERTIPROG3_BASE_ADDR+sizeof(uint8_t)+sizeof(uint16_t))//stoptime
#define EEPROM_FERTIPROG3_PARA_4_ADDR		(EEPROM_FERTIPROG3_BASE_ADDR+sizeof(uint8_t)+sizeof(uint16_t)+sizeof(uint16_t))//startflow
#define EEPROM_FERTIPROG3_PARA_5_ADDR		(EEPROM_FERTIPROG3_BASE_ADDR+sizeof(uint8_t)+sizeof(uint16_t)+sizeof(uint16_t)+sizeof(uint32_t))//stopflow
#define EEPROM_FERTIPROG3_SIZE				(sizeof(uint8_t)+sizeof(uint16_t)+sizeof(uint16_t)+sizeof(uint32_t)+sizeof(uint32_t))



#define EEPROM_ZONEPROG_BASE_ADDR			(EEPROM_FERTIPROG_BASE_ADDR + EEPROM_FERTIPROG_SIZE)
#define EEPROM_ZONEPROG_SIZE				(sizeof(zone_prog_data) * ZONE_PROG_MAX)	// zone progs

#define EEPROM_ZONEPROG1_BASE_ADDR			EEPROM_ZONEPROG_BASE_ADDR
#define EEPROM_ZONEPROG1_PARA_1_ADDR		EEPROM_ZONEPROG1_BASE_ADDR//id
#define EEPROM_ZONEPROG1_PARA_2_ADDR		(EEPROM_ZONEPROG1_BASE_ADDR+sizeof(uint8_t))//type
#define EEPROM_ZONEPROG1_PARA_3_ADDR		(EEPROM_ZONEPROG1_BASE_ADDR+sizeof(uint8_t)+sizeof(uint8_t))//status
#define EEPROM_ZONEPROG1_PARA_4_ADDR		(EEPROM_ZONEPROG1_BASE_ADDR+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t))//repeatcycles
#define EEPROM_ZONEPROG1_PARA_5_ADDR		(EEPROM_ZONEPROG1_BASE_ADDR+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t))//cycledelay
#define EEPROM_ZONEPROG1_PARA_6_ADDR		(EEPROM_ZONEPROG1_BASE_ADDR+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint16_t))//rundaysmode
#define EEPROM_ZONEPROG1_PARA_7_ADDR		(EEPROM_ZONEPROG1_BASE_ADDR+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint16_t)+sizeof(uint8_t))//rundaysval
#define EEPROM_ZONEPROG1_SIZE		        (sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint8_t)+sizeof(uint16_t)+sizeof(uint8_t)+(sizeof(uint8_t)*3))

#define EEPROM_ZONEPROG2_BASE_ADDR			(EEPROM_ZONEPROG1_BASE_ADDR+EEPROM_ZONEPROG1_SIZE)
#define EEPROM_ZONEPROG2_PARA_1_ADDR		EEPROM_ZONEPROG2_BASE_ADDR//starttime
#define EEPROM_ZONEPROG2_PARA_2_ADDR		(EEPROM_ZONEPROG2_BASE_ADDR+(sizeof(uint8_t)*2))//zonecount
#define EEPROM_ZONEPROG2_PARA_3_ADDR		(EEPROM_ZONEPROG2_BASE_ADDR+(sizeof(uint8_t)*2)+sizeof(uint8_t))//totaltime
#define EEPROM_ZONEPROG2_PARA_4_ADDR		(EEPROM_ZONEPROG2_BASE_ADDR+(sizeof(uint8_t)*2)+sizeof(uint8_t)+(sizeof(uint8_t)*2))//totalflow
#define EEPROM_ZONEPROG2_PARA_5_ADDR		(EEPROM_ZONEPROG2_BASE_ADDR+(sizeof(uint8_t)*2)+sizeof(uint8_t)+(sizeof(uint8_t)*2)+sizeof(uint16_t))//pumpdelay
#define EEPROM_ZONEPROG2_PARA_6_ADDR		(EEPROM_ZONEPROG2_BASE_ADDR+(sizeof(uint8_t)*2)+sizeof(uint8_t)+(sizeof(uint8_t)*2)+sizeof(uint16_t)+sizeof(uint16_t))//zonedelay
#define EEPROM_ZONEPROG2_SIZE				((sizeof(uint8_t)*2)+sizeof(uint8_t)+(sizeof(uint8_t)*2)+sizeof(uint16_t)+sizeof(uint16_t)+sizeof(uint16_t))

#define EEPROM_ZONEPROG3_BASE_ADDR			(EEPROM_ZONEPROG2_BASE_ADDR+EEPROM_ZONEPROG2_SIZE)
#define EEPROM_ZONEPROG3_PARA_1_ADDR		EEPROM_ZONEPROG3_BASE_ADDR//rain
#define EEPROM_ZONEPROG3_PARA_2_ADDR		(EEPROM_ZONEPROG3_BASE_ADDR+sizeof(digital_sensor_data))//temperature
#define EEPROM_ZONEPROG3_PARA_3_ADDR		(EEPROM_ZONEPROG3_BASE_ADDR+sizeof(digital_sensor_data)+sizeof(analog_sensor_data))//humidity
#define EEPROM_ZONEPROG3_PARA_4_ADDR		(EEPROM_ZONEPROG3_BASE_ADDR+sizeof(digital_sensor_data)+sizeof(analog_sensor_data)+sizeof(analog_sensor_data))//flow
#define EEPROM_ZONEPROG3_PARA_5_ADDR		(EEPROM_ZONEPROG3_BASE_ADDR+sizeof(digital_sensor_data)+sizeof(analog_sensor_data)+sizeof(analog_sensor_data)+sizeof(digital_sensor_data))//oppressure
#define EEPROM_ZONEPROG3_PARA_6_ADDR		(EEPROM_ZONEPROG3_BASE_ADDR+sizeof(digital_sensor_data)+sizeof(analog_sensor_data)+sizeof(analog_sensor_data)+sizeof(digital_sensor_data)+sizeof(analog_sensor_data))//ippressure
#define EEPROM_ZONEPROG3_SIZE				(sizeof(digital_sensor_data)+sizeof(analog_sensor_data)+sizeof(analog_sensor_data)+sizeof(digital_sensor_data)+sizeof(analog_sensor_data)+sizeof(analog_sensor_data))

#define EEPROM_ZONEPROG4_BASE_ADDR			(EEPROM_ZONEPROG3_BASE_ADDR+EEPROM_ZONEPROG3_SIZE)
#define EEPROM_ZONEPROG4_PARA_1_ADDR		EEPROM_ZONEPROG4_BASE_ADDR//oppartno
#define EEPROM_ZONEPROG4_PARA_2_ADDR		(EEPROM_ZONEPROG4_BASE_ADDR+sizeof(uint8_t))//starttime
#define EEPROM_ZONEPROG4_PARA_3_ADDR		(EEPROM_ZONEPROG4_BASE_ADDR+sizeof(uint8_t)+(sizeof(uint8_t)*2))//stoptime
#define EEPROM_ZONEPROG4_PARA_4_ADDR		(EEPROM_ZONEPROG4_BASE_ADDR+sizeof(uint8_t)+(sizeof(uint8_t)*2)+(sizeof(uint8_t)*2))//startflow
#define EEPROM_ZONEPROG4_PARA_5_ADDR		(EEPROM_ZONEPROG4_BASE_ADDR+sizeof(uint8_t)+(sizeof(uint8_t)*2)+(sizeof(uint8_t)*2)+sizeof(uint32_t))//stopflow
#define EEPROM_ZONEPROG4_PARA_6_ADDR		(EEPROM_ZONEPROG4_BASE_ADDR+sizeof(uint8_t)+(sizeof(uint8_t)*2)+(sizeof(uint8_t)*2)+sizeof(uint32_t)+sizeof(uint32_t))//fertprogid
#define EEPROM_ZONEPROG4_SIZE				(sizeof(uint8_t)+(sizeof(uint8_t)*2)+(sizeof(uint8_t)*2)+sizeof(uint32_t)+sizeof(uint32_t)+sizeof(uint8_t))

#define EEPROM_PROGSTATUS_BASE_ADDR			(EEPROM_ZONEPROG4_BASE_ADDR+EEPROM_ZONEPROG4_SIZE)
#define EEPROM_PROGSTATUS_PARA_1_ADDR		EEPROM_PROGSTATUS_BASE_ADDR//remtim[6]
#define EEPROM_PROGSTATUS_PARA_2_ADDR		(EEPROM_PROGSTATUS_BASE_ADDR+(sizeof(uint16_t)*6))//interrupted_prog
#define EEPROM_PROGSTATUS_PARA_3_ADDR		(EEPROM_PROGSTATUS_BASE_ADDR+(sizeof(uint16_t)*6)+sizeof(char))//currentday
#define EEPROM_PROGSTATUS_PARA_4_ADDR		(EEPROM_PROGSTATUS_BASE_ADDR+(sizeof(uint16_t)*6)+sizeof(char)+sizeof(char))//bf_valno
#define EEPROM_PROGSTATUS_PARA_5_ADDR		(EEPROM_PROGSTATUS_BASE_ADDR+(sizeof(uint16_t)*6)+sizeof(char)+sizeof(char)+sizeof(char))//backflushon
#define EEPROM_PROGSTATUS_PARA_6_ADDR		(EEPROM_PROGSTATUS_BASE_ADDR+(sizeof(uint16_t)*6)+sizeof(char)+sizeof(char)+sizeof(char)+sizeof(char))//bf_1min
#define EEPROM_PROGSTATUS_PARA_7_ADDR		(EEPROM_PROGSTATUS_BASE_ADDR+(sizeof(uint16_t)*6)+sizeof(char)+sizeof(char)+sizeof(char)+sizeof(char)+sizeof(int))//table_changed
#define EEPROM_PROGSTATUS_SIZE				((sizeof(uint16_t)*6)+sizeof(char)+sizeof(char)+sizeof(char)+sizeof(char)+sizeof(int)+(sizeof(char)*15))

//*****************************************************************************
//
//*****************************************************************************

void get_default_value_contactinfo(void);
void get_default_value_timedate(void);
void get_default_value_hardwareconfig(void);
void get_default_value_systemsetup(void);

void get_default_value_output(void);
void get_default_value_input(void);
void get_default_value_sensor(void);

void get_default_value_network(void);
void get_default_value_backflush(void);
void get_default_value_authentication(void);
void get_default_value_firmware(void);

//void get_default_value_fertiprog(void);
//void get_default_value_zoneprog(void);
void get_default_value_fertiprog(uint8_t index);
void get_default_value_zoneprog(uint8_t index);

void get_default_value_man_program(void);
void get_default_value_man_output(void);

void get_default_value_diag_alert(void);

void get_default_value_logs(void);

void get_default_value_progstatus(void);

void eeprom_init(void);

void eeprom_factory_reset(void);
void eeprom_read_all_data(void);
void eeprom_read_contactinfo(uint8_t parameter_index);
void eeprom_write_contactinfo(uint8_t parameter_index);
void eeprom_read_timedate(uint8_t parameter_index);
void eeprom_write_timedate(uint8_t parameter_index);
void eeprom_read_hardwareconfig(uint8_t parameter_index);
void eeprom_write_hardwareconfig(uint8_t parameter_index);
void eeprom_read_systemsetup(uint8_t parameter_index);
void eeprom_write_systemsetup(uint8_t parameter_index);

void eeprom_read_output(uint8_t index);
void eeprom_write_output(uint8_t index);
void eeprom_read_input(uint8_t index);
void eeprom_write_input(uint8_t index);
void eeprom_read_sensor(uint8_t index);
void eeprom_write_sensor(uint8_t index);

//void eeprom_read_network(uint8_t parameter_index);
//void eeprom_write_network(uint8_t parameter_index);
void eeprom_read_network_gsm(uint8_t parameter_index);
void eeprom_write_network_gsm(uint8_t parameter_index);
//void eeprom_read_network1(uint8_t parameter_index);
//void eeprom_write_network1(uint8_t parameter_index);
void eeprom_read_network_wifi(uint8_t parameter_index);
void eeprom_write_network_wifi(uint8_t parameter_index);
void eeprom_read_backflush(uint8_t parameter_index);
void eeprom_write_backflush(uint8_t parameter_index);
void eeprom_read_authentication(uint8_t index);
void eeprom_write_authentication(uint8_t index);
void eeprom_read_firmware(uint8_t parameter_index);
void eeprom_write_firmware(uint8_t parameter_index);

//void eeprom_read_fertiprog(uint8_t index);
void eeprom_read_fertiprog1(uint8_t progid,uint8_t parameter_index);
void eeprom_read_fertiprog2(uint8_t progid,uint8_t parameter_index);
void eeprom_read_fertiprog3(uint8_t progid,uint8_t ioid,uint8_t parameter_index);

//void eeprom_write_fertiprog(uint8_t index);
void eeprom_write_fertiprog1(uint8_t progid,uint8_t parameter_index);
void eeprom_write_fertiprog2(uint8_t progid,uint8_t parameter_index);
void eeprom_write_fertiprog3(uint8_t progid,uint8_t ioid,uint8_t parameter_index);

//void eeprom_read_zoneprog(uint8_t index);
void eeprom_read_zoneprog1(uint8_t progid,uint8_t parameter_index);
void eeprom_read_zoneprog2(uint8_t progid,uint8_t parameter_index);
void eeprom_read_zoneprog3(uint8_t progid,uint8_t parameter_index);
void eeprom_read_zoneprog4(uint8_t progid,uint8_t ioid,uint8_t parameter_index);

//void eeprom_write_zoneprog(uint8_t index);
void eeprom_write_zoneprog1(uint8_t progid,uint8_t parameter_index);
void eeprom_write_zoneprog2(uint8_t progid,uint8_t parameter_index);
void eeprom_write_zoneprog3(uint8_t progid,uint8_t parameter_index);
void eeprom_write_zoneprog4(uint8_t progid,uint8_t ioid,uint8_t parameter_index);

void eeprom_write_progstatus(uint8_t parameter_index);
void eeprom_read_progstatus(uint8_t parameter_index);
//*****************************************************************************
// Variables
//*****************************************************************************
extern char
	contact_phone[2][21],
	contact_email[21];

extern uint8_t
	datetimesource;				// 0= Manual / 1=Computer / 2=NTP Server

extern uint8_t
	noopmodules,				// 0= 8 ports, 1= 16 ports
	fertigation;

extern uint32_t
	flowperpulse;				// Flow volume per pulse

extern float
	minop,						// min operating pressure
	maxdp;						// max delta pressure

extern uint8_t
	language,					// 0=English
	contrast,					//-15 to +15
	disp_clockformat,
	dateformat,
	beginweek;

extern op_data			output[OUTPUT_MAX];
extern ip_data			input[INPUT_MAX];
extern sensor_data		sensor[SENSOR_MAX];

extern uint8_t		backflushstatus;
extern uint8_t		backflushinterval[2];
extern uint16_t		backflushduration;
extern uint16_t		backflushdelay;
extern uint8_t		deltapres;
extern uint16_t		deltadelay;
extern uint8_t		deltaitera;
extern uint16_t		dwelltime;

extern char
	loginpasswords[4][11];

extern char
	model[13],
	deviceid[11];				// device unique id

extern float
	firmwareversion;			// xx.y

extern char
	enetmacid[11],
	wifimacid[11],
	imeino[11];

extern ferti_prog_data	fertiprog[FERTI_PROG_MAX];
extern zone_prog_data	zoneprog[ZONE_PROG_MAX];

extern man_prog_data	manprog[MAN_PROG_MAX];
extern man_output_data	manoutput[OUTPUT_MAX];

extern alert_data		alert[ALERT_MAX];

extern log_data			log[LOGS_MAX];

//related to program time shift
extern char interrupted_prog;//0 if no prog interrupted else the prog number value
extern uint16_t remtim[6];
extern char currentday;


extern char
	contact_phone_remote[2][21],
	contact_email_remote[21];

extern uint8_t
	datetimesource_remote,				// 0= Manual / 1=Computer / 2=NTP Server
	serverdate_remote[3],
	servertime_remote[3];

extern userdef_tm	server_time_remote;

extern uint8_t
	noopmodules_remote,			// 0= 8 ports, 1= 16 ports
	fertigation_remote;

extern uint32_t
	flowperpulse_remote;			// Flow volume per pulse

extern float
	minop_remote,				// min operating pressure
	maxdp_remote;				// max delta pressure

extern uint8_t
	language_remote,				// 0=English
	contrast_remote,				//-15 to +15
	disp_clockformat_remote,
	dateformat_remote,
	beginweek_remote ;

/********************************************/
// Variables related to backflush

extern uint8_t		backflushstatus_remote;
extern uint8_t		backflushinterval_remote[2];
extern uint16_t	backflushduration_remote;
extern uint16_t	backflushdelay_remote;
extern uint8_t		deltapres_remote;
extern uint16_t	deltadelay_remote;
extern uint8_t		deltaitera_remote;
extern uint16_t	dwelltime_remote;


//Variables related to Network Data
extern char wireless_mode;		// 0=Local Wifi, 1=Internet Wifi, 2=Internet GSM

//GSM Settings
extern char gsm_module;			// 0=Disconnected, 1=Connected
extern char gsm_apn[20];		// Store the operator defined access point name
extern char gsm_operator[20];	// Operator Name
extern char sig_strength;		// Store the signal strength

//WIFI Settings
extern char wifi_module;		// 0=Disconnected, 1=Connected
//Local Mode Parameters
extern char wifi_name[20];		// Store the device SSID for local mode
extern char wifi_pass[20];		// Store the device password for local mode
//Internet Mode Parameters
extern char ext_apn[20];		// Store the SSID of the external device to be connected to
extern char ext_pass[20];		// Store the password of the external device to be connected to

extern char portal_ip[4];
extern char portal_name[40];
/********************************************/


//Variables related to Network Data
extern char wireless_mode_remote;// 0=Local Wifi, 1=Internet Wifi, 2=Internet GSM

//GSM Settings
extern char gsm_module_remote; // 0=Disconnected, 1=Connected
extern char gsm_apn_remote[20];// Store the operator defined access point name
extern char gsm_operator_remote[20];// Operator Name
extern char sig_strength_remote; // Store the signal strength
//WIFI Settings
extern char wifi_module_remote; // 0=Disconnected, 1=Connected
//Local Mode Parameters
extern char wifi_name_remote[20];// Store the device SSID for local mode
extern char wifi_pass_remote[20];// Store the device password for local mode
//Internet Mode Parameters
extern char ext_apn_remote[20];// Store the SSID of the external device to be connected to
extern char ext_pass_remote[20];// Store the password of the external device to be connected to

extern char portal_ip_remote[4];
extern char portal_name_remote[40];


extern char
	loginpasswords_remote[4][11];



/********************************************/
// Variables related to setup menu

// This defines object for o/p Ports
extern op_data	output_remote[OUTPUT_MAX];

// This defines object for i/p Ports
extern ip_data	input_remote[INPUT_MAX];

// This defines parameters for sensor Ports
extern sensor_data	sensor_remote[SENSOR_MAX];

/*********************************************************/
// Variables related to program menu

// This defines object for fertigation prog
extern ferti_prog_data	fertiprog_remote[FERTI_PROG_MAX];

// This defines object for zone program
extern zone_prog_data	zoneprog_remote[ZONE_PROG_MAX];

extern char bf_valno;
extern char backflushon;
extern int bf_1min;
extern char table_changed[15];

//*****************************************************************************
// Flag
//*****************************************************************************
//volatile bool

//*****************************************************************************
// The error routine that is called if the driver library encounters an error.
//*****************************************************************************

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


/********************************************************************************/
/* 						Routines For Default Handling							*/
/********************************************************************************/
// This function loads default values.
/********************************************************************/

void get_default_value_contactinfo(void)
{
	strcpy(contact_phone[0], "   +91 1234567890   ");
	strcpy(contact_phone[1], "   +91 1234567890   ");
	strcpy(contact_email, " info@infosavant.com");
}

/********************************************************************/
// This function loads default values.
/********************************************************************/

void get_default_value_timedate(void)
{
	datetimesource = 0;		// 0= Manual
}

/********************************************************************/
// This function loads default values.
/********************************************************************/

void get_default_value_progstatus(void)
{
	int i=0;
	for(i=0;i<=5;i++)
	{
		remtim[i]=0;
	}
	interrupted_prog=0;
	currentday=0;
	bf_valno =0;
	backflushon=0;
	bf_1min=0;

	for(i=0;i<=14;i++)
	{
		table_changed[i] = 1;// 1 incidates table data changed and needs to be synced
	}

}


/********************************************************************/
// This function loads default values.
/********************************************************************/

void get_default_value_hardwareconfig(void)
{
	noopmodules = 1;			// 0= 8 ports, 1= 16 ports
	fertigation = 1;			//1=enabled 0=disabled
	flowperpulse = 111;			// Flow volume per pulse
	minop = 22.2;				// min operating pressure
	maxdp = 00.0;				// max delta pressure
}

/********************************************************************/
// This function loads default values.
/********************************************************************/

void get_default_value_systemsetup(void)
{
	language = 0;				// 0=English
	contrast = 0;				//-15 to +15
	disp_clockformat = 0;
	dateformat = 0;
	beginweek = 0;
}

/********************************************************************/
// This function loads default values.
/********************************************************************/

void get_default_value_output(void)
{
uint8_t index=0, temp_id;

	for (index=0; index<OUTPUT_MAX; index++)
	{
		temp_id = index + 1;
		switch (temp_id)
		{
			case 1:
				output[index].optype = 1;
				output[index].typeid = 1;
			    usnprintf(output[index].opname, 11, "MasterPump");
				break;

			case 2:
				output[index].optype = 4;
				output[index].typeid = 1;
			    usnprintf(output[index].opname, 11, "Ferti Pump");
				break;

			case 3:
			case 4:
			case 5:
			case 6:
				output[index].optype = 5;
				output[index].typeid = temp_id - 2;
			    usnprintf(output[index].opname, 11, "FTValve %01u", (temp_id - 2));
				break;

			case 7:
			case 8:
			case 9:
			case 10:
				output[index].optype = 2;
				output[index].typeid = temp_id - 6;
			    usnprintf(output[index].opname, 11, "FLValve %01u", (temp_id - 6));
				break;

			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
				output[index].optype = 3;
				output[index].typeid =  temp_id - 10;
			    usnprintf(output[index].opname, 11, "Zone %01u", (temp_id - 10));
				break;

		default:
				break;
		}
	}
}

/********************************************************************/
// This function loads default values.
/********************************************************************/

void get_default_value_input(void)
{
	//$$NR$$//dated:23Aug15

	//	uint8_t index=0;

	//	for (index=0; index<INPUT_MAX; index++)
	//	{
	//		// set input name
	//		usnprintf(input[index].ipname, 11, "input %02u", (index + 1));
	//	}

		input[0].iptype = 1;	// 1 - MainPumpOK
		input[1].iptype = 2;	// 2 - FertiPumpOK
		input[2].iptype = 0;	//0 - None
		input[3].iptype = 1;	// 1- Flowmeter

	//$$NR$$//dated:23Aug15
}

/********************************************************************/
// This function loads default values.
/********************************************************************/


void get_default_value_sensor(void)
{
uint8_t index=0;

	for (index=0; index<SENSOR_MAX; index++)
	{
		switch (index+1)
		{
			case 1:
				sensor[index].serialid = 1;
				sensor[index].sensortype = 1;
				sensor[index].typeid = 1;
				sensor[index].min = 0.00;
				sensor[index].max = 9.99;
//				strcpy(sensor[index].unit, "unit");			//$$NR$$//dated:16Aug15
				sensor[index].unittype = 0;					//$$NR$$//dated:16Aug15
				break;

			case 2:
				sensor[index].serialid = 2;
				sensor[index].sensortype = 2;
				sensor[index].typeid = 1;
				sensor[index].min = 0.00;
				sensor[index].max = 9.99;
//				strcpy(sensor[index].unit, "unit");			//$$NR$$//dated:16Aug15
				sensor[index].unittype = 0;					//$$NR$$//dated:16Aug15
				break;

			case 3:
				sensor[index].serialid = 3;
				sensor[index].sensortype = 3;
				sensor[index].typeid = 1;
				sensor[index].min = 0.00;
				sensor[index].max = 9.99;
//				strcpy(sensor[index].unit, "unit");			//$$NR$$//dated:16Aug15
				sensor[index].unittype = 0;					//$$NR$$//dated:16Aug15
				break;

			case 4:
				sensor[index].serialid = 4;
				sensor[index].sensortype = 4;
				sensor[index].typeid = 1;
				sensor[index].min = 0.00;
				sensor[index].max = 9.99;
//				strcpy(sensor[index].unit, "unit");			//$$NR$$//dated:16Aug15
				sensor[index].unittype = 0;					//$$NR$$//dated:16Aug15
				break;

			case 5:
				sensor[index].serialid = 5;
				sensor[index].sensortype = 5;
				sensor[index].typeid = 1;
				sensor[index].min = 0.00;
				sensor[index].max = 9.99;
//				strcpy(sensor[index].unit, "unit");			//$$NR$$//dated:16Aug15
				sensor[index].unittype = 0;					//$$NR$$//dated:16Aug15
				break;

			case 6:
				sensor[index].serialid = 6;
				sensor[index].sensortype = 6;
				sensor[index].typeid = 1;
				sensor[index].min = 0.00;
				sensor[index].max = 9.99;
//				strcpy(sensor[index].unit, "unit");			//$$NR$$//dated:16Aug15
				sensor[index].unittype = 0;					//$$NR$$//dated:16Aug15
				break;

		default:
				break;
		}
	}
}

/********************************************************************/
// This function loads default values.
/********************************************************************/

void get_default_value_network(void)
{
	wireless_mode=0;// 0=Local Wifi, 1=Internet Wifi, 2=Internet GSM

	//GSM Settings
	gsm_module=1; // 0=Disconnected, 1=Connected

	usnprintf(gsm_apn, 20, "internet");// Store the operator defined access point name
	usnprintf(gsm_operator, 20, "Idea");// Operator Name

	sig_strength=0; // Store the signal strength
	//WIFI Settings
	wifi_module=1; // 0=Disconnected, 1=Connected
	//Local Mode Parameters
	usnprintf(wifi_name, 11, deviceid);// Store the device SSID for local mode
	usnprintf(wifi_pass, 20, "avkon134");//  Store the device password for local mode
	//Internet Mode Parameters
	usnprintf(ext_apn, 20, "Karan");// Store the device SSID for internet mode
	usnprintf(ext_pass, 20, "lotus*123");//  Store the device password for internet mode

	portal_ip[3]=52;//103;
	portal_ip[2]=35;//53;
	portal_ip[1]=11;//42;
	portal_ip[0]=114;//113;

	usnprintf(portal_name, 40, "portal.avkon.in");//  Store the device password for internet mode

}

/********************************************************************/
// This function loads default values.
/********************************************************************/

void get_default_value_backflush(void)
{
	backflushstatus = 0;
	backflushinterval[0] = 0;	backflushinterval[1] = 0;
	backflushduration = 0;
	backflushdelay = 0;
	deltapres = 0;
	deltadelay = 0;
	deltaitera = 0;
	dwelltime = 0;
}

/********************************************************************/
// This function loads default values.
/********************************************************************/

void get_default_value_authentication(void)
{
	strcpy(loginpasswords[0], "a");//"password");	//pwAL1
	strcpy(loginpasswords[1], "b");//"password");	//pwAL2
	strcpy(loginpasswords[2], "c");//"password");	//pwAL3
	strcpy(loginpasswords[3], "d");//"password");	//pwAL4
}

/********************************************************************/
// This function loads default values.
/********************************************************************/

void get_default_value_firmware(void)
{
	strcpy(model, "Warmblut-141");
	strcpy(deviceid, "IR000009");	// device unique id
	firmwareversion = 01.0;			// xx.y
	strcpy(enetmacid, "          ");
	strcpy(wifimacid, "          ");
	strcpy(imeino, "          ");
}

/********************************************************************/
// This function loads default values.
/********************************************************************/

void get_default_value_fertiprog(uint8_t index)
{
	uint8_t j;

	fertiprog[index].id = index+1;
	fertiprog[index].type = 0;
	fertiprog[index].method = 0;
	fertiprog[index].propcycles = 0;
	fertiprog[index].startdelay = 0;
	fertiprog[index].stopdelay = 0;
	fertiprog[index].valvecount = 4;

	for(j=0; j<4; j++)
	{
		fertiprog[index].valve[j].oppartno = j+2;
		fertiprog[index].valve[j].starttime = 0;
		fertiprog[index].valve[j].startflow = 0;
		fertiprog[index].valve[j].stoptime = 0;
		fertiprog[index].valve[j].stopflow = 0;
	}

	fertiprog[index].totaltime = 0;
	fertiprog[index].totalflow = 0;
}

/********************************************************************/
// This function loads default values.
/********************************************************************/


void get_default_value_zoneprog(uint8_t index)
{
	uint8_t j;
	zoneprog[index].id = index+1;
	zoneprog[index].type = 0;
	zoneprog[index].status = 0;

	zoneprog[index].repeatcycles = 0;
	zoneprog[index].cycledelay = 0;

	zoneprog[index].rain.condition = 0;
	zoneprog[index].rain.currentval = 0;

	zoneprog[index].temperature.condition = 0;			// Sensor preconditions 0=independant / 1=dependant
	zoneprog[index].temperature.min = 00.0;				// Minimun Sensor Value
	zoneprog[index].temperature.max = 99.9;				// Maximum Sensor Value
	zoneprog[index].temperature.currentval = 0;			// current value of sensor

	zoneprog[index].humidity.condition = 0;				// Sensor preconditions 0=independant / 1=dependant
	zoneprog[index].humidity.min = 00.0;				// Minimun Sensor Value
	zoneprog[index].humidity.max = 99.9;				// Maximum Sensor Value
	zoneprog[index].humidity.currentval = 0;			// current value of sensor

	zoneprog[index].flow.condition = 0;
	zoneprog[index].flow.currentval = 0;

	zoneprog[index].oppressure.condition = 0;			// Sensor preconditions 0=independant / 1=dependant
	zoneprog[index].oppressure.min = 00.0;				// Minimun Sensor Value
	zoneprog[index].oppressure.max = 99.9;				// Maximum Sensor Value
	zoneprog[index].oppressure.currentval = 0;			// current value of sensor

	zoneprog[index].ippressure.condition = 0;			// Sensor preconditions 0=independant / 1=dependant
	zoneprog[index].ippressure.min = 00.0;				// Minimun Sensor Value
	zoneprog[index].ippressure.max = 99.9;				// Maximum Sensor Value
	zoneprog[index].ippressure.currentval = 0;			// current value of sensor

	zoneprog[index].rundaysmode = 0;
	zoneprog[index].rundaysval[0] = 0;	zoneprog[index].rundaysval[1] = 0;	zoneprog[index].rundaysval[2] = 0;

	zoneprog[index].starttime[0] = 0;	zoneprog[index].starttime[1] = 0;

	zoneprog[index].zonecount = 6;
	for(j=0; j<10; j++)
	{
		if (j < 6)
			zoneprog[index].zone[j].oppartno = j + 10;
		else
			zoneprog[index].zone[j].oppartno = 0;

		zoneprog[index].zone[j].starttime[0] = 0;	zoneprog[index].zone[j].starttime[1] = 0;
		zoneprog[index].zone[j].stoptime[0] = 0;	zoneprog[index].zone[j].stoptime[1] = 0;
		zoneprog[index].zone[j].startflow = 0;
		zoneprog[index].zone[j].stopflow = 0;

		if (j < 6)
			zoneprog[index].zone[j].fertprogid = j+1;
		else
			zoneprog[index].zone[j].fertprogid = 0;
	}

	zoneprog[index].totaltime[0] = 0;	zoneprog[index].totaltime[1] = 0;
	zoneprog[index].totalflow = 0;
	zoneprog[index].pumpdelay = 0;
	zoneprog[index].zonedelay = 0;
}

/********************************************************************/
// This function loads default values.
/********************************************************************/

void get_default_value_man_program(void)
{
uint8_t i;

	for(i=0; i<MAN_PROG_MAX; i++)
	{
//		manprog[i].id = i+1;
//		manprog[i].type = 0;
//		manprog[i].status = 0;
		manprog[i].state = 0;
//		manprog[i].starttime[0] = 0;
//		manprog[i].starttime[1] = 0;
//		manprog[i].totaltime[0] = 0;
//		manprog[i].totaltime[1] = 0;
	}
}

/********************************************************************/
// This function loads default values.
/********************************************************************/

void get_default_value_man_output(void)
{
uint8_t i;

	for(i=0; i<OUTPUT_MAX; i++)
	{
//		manoutput[i].id = i+1;
//		manoutput[i].type = 0;
//		manoutput[i].status = 0;
		manoutput[i].state = 0;
//		manoutput[i].starttime[0] = 0;
//		manoutput[i].starttime[1] = 0;
//		manoutput[i].totaltime[0] = 0;
//		manoutput[i].totaltime[1] = 0;
	}
}

/********************************************************************/
// This function loads default values.
/********************************************************************/

void get_default_value_diag_alert(void)
{
uint8_t i;

	for(i=0; i<ALERT_MAX; i++)
	{
		alert[i].status = 0;
	}
}

/********************************************************************/
// This function loads default values.
/********************************************************************/

void get_default_value_logs(void)
{
//uint8_t i;
//
//	for(i=0; i<LOGS_MAX; i++)
//	{
//		log[i].status = 0;
//	}
}

/********************************************************************************/
/* 						Routines For EEPROM Handling							*/
/********************************************************************************/
// This function initialise eeprom.
/********************************************************************/

void eeprom_init (void)
{
//uint32_t e2size, e2block;

	// Enable the eeprom module.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0); // EEPROM activate

	// call EEPROM init function
	EEPROMInit(); // EEPROM start

//	EEPROMMassErase();

//	System_printf("EEPROM Test Program \r\n");

//	e2size = EEPROMSizeGet(); // Get EEPROM Size				// 6144
//  System_printf("EEPROM Size %d bytes\n", e2size);
//
//  e2block = EEPROMBlockCountGet(); // Get EEPROM Block Count	// 96
//  System_printf("EEPROM Blok Count: %d\n", e2block);
}

/********************************************************************/
// This function reset data to default value
/********************************************************************/

bool panel_setup_status(void)
{
//uint32_t status = 0x00;
char read_sw_ver[5];

//	EEPROMRead((uint8_t *)&status, EEPROM_BASE_ADDR, sizeof(status)); //Read from struct at EEPROM start from 0x0000
	EEPROMRead((uint8_t *)read_sw_ver, EEPROM_BASE_ADDR, sizeof(char)*5); //Read from struct at EEPROM start from 0x0000
	read_sw_ver[4] = 0;

//	if (status == 0x01)
	if (strcmp((char *)read_sw_ver, (const char *)SoftwareVersionID) == 0)
		return true;
	else
		return false;
}

/********************************************************************/
// This function reset data to default value
/********************************************************************/

void eeprom_factory_reset(void)
{
uint8_t i,j;
//uint32_t status = 0x01;//, status1 = 0xFF;
char sw_version[5];//, read_sw_ver[5];

	strcpy(sw_version, SoftwareVersionID);

//	EEPROMProgram((uint8_t *)&status, EEPROM_BASE_ADDR, sizeof(status)); //Write struct to EEPROM start from 0x0000
	EEPROMProgram((uint8_t *)sw_version, EEPROM_BASE_ADDR, sizeof(char)*5); //Write struct to EEPROM start from 0x0000

//	EEPROMRead((uint8_t *)&status1, EEPROM_BASE_ADDR, sizeof(status1)); //Read from struct at EEPROM start from 0x0000

    // get default values and store data into eeprom
    get_default_value_firmware();
    eeprom_write_firmware(0xFF);

    // get default values and store data into eeprom
    get_default_value_contactinfo();
    eeprom_write_contactinfo(0xFF);

    // get default values and store data into eeprom
    get_default_value_timedate();
    eeprom_write_timedate(0xFF);

    // get default values and store data into eeprom
    get_default_value_hardwareconfig();
    eeprom_write_hardwareconfig(0xFF);

    // get default values and store data into eeprom
    get_default_value_systemsetup();
    eeprom_write_systemsetup(0xFF);

    // get default values and store data into eeprom
    get_default_value_output();
    for (i=0; i<OUTPUT_MAX; i++)	eeprom_write_output(i);

    // get default values and store data into eeprom
    get_default_value_input();
    for (i=0; i<INPUT_MAX; i++)		eeprom_write_input(i);

    // get default values and store data into eeprom
    get_default_value_sensor();
    for (i=0; i<SENSOR_MAX; i++)	eeprom_write_sensor(i);

    // get default values and store data into eeprom
    get_default_value_network();
//	eeprom_write_network(0xFF);
	eeprom_write_network_gsm(0xFF);
//	eeprom_write_network1(0xFF);
	eeprom_write_network_wifi(0xFF);
    // get default values and store data into eeprom
    get_default_value_backflush();
    eeprom_write_backflush(0xFF);

    // get default values and store data into eeprom
    get_default_value_authentication();
    for (i=0; i<4; i++)		eeprom_write_authentication(i);



    // get default values and store data into eeprom

    for (i=0; i<FERTI_PROG_MAX; i++)
	{
    	 get_default_value_fertiprog(i);
    	eeprom_write_fertiprog1(i,0XFF);
		eeprom_write_fertiprog2(i,0XFF);
		for(j=0;j<4;j++)
		{
			eeprom_write_fertiprog3(i,j,0XFF);
		}
	}

    // get default values and store data into eeprom

    for (i=0; i<ZONE_PROG_MAX; i++)
    {
    	get_default_value_zoneprog(i);
    	eeprom_write_zoneprog1(i,0XFF);
		eeprom_write_zoneprog2(i,0XFF);
		eeprom_write_zoneprog3(i,0XFF);
		for(j=0;j<10;j++)
		{
			eeprom_write_zoneprog4(i,j,0XFF);
		}
    }

    get_default_value_progstatus();
    eeprom_write_progstatus(0xFF);


    eeprom_read_all_data();//read all the written data
    // get default values and store data into eeprom
  //  get_default_value_man_program();
//    for (i=0; i<MAN_PROG_MAX; i++)
    //	eeprom_write_man_program(0xFF);//(i);

    // get default values and store data into eeprom
    //get_default_value_man_output();
   // for (i=0; i<OUTPUT_MAX; i++)		eeprom_write_man_output(i);

    // get default values and store data into eeprom
    //get_default_value_diag_alert();
   // for (i=0; i<ALERT_MAX; i++)		eeprom_write_alert(i);

//    // get default values and store data into eeprom
//    get_default_value_logs();
//    for (i=0; i<LOGS_MAX; i++)		eeprom_write_logs(i);
}

/********************************************************************/
// This function reset data to default value
/********************************************************************/

void eeprom_read_all_data(void)
{
uint8_t i,j;

	// get data from eeprom
//    eeprom_read_contactinfo(0xFF);

    // get data from eeprom
//    eeprom_read_timedate(0xFF);

    // get data from eeprom
    eeprom_read_hardwareconfig(0xFF);

    // get data from eeprom
    eeprom_read_systemsetup(0xFF);

    // get data from eeprom
    for (i=0; i<OUTPUT_MAX; i++)	eeprom_read_output(i);

    // get data from eeprom
    for (i=0; i<INPUT_MAX; i++)		eeprom_read_input(i);

    // get data from eeprom
    for (i=0; i<SENSOR_MAX; i++)	eeprom_read_sensor(i);

    // get data from eeprom
//	eeprom_read_network(0xFF);
	eeprom_read_network_gsm(0xFF);
//	eeprom_read_network1(0xFF);
	eeprom_read_network_wifi(0xFF);
    // get data from eeprom
    eeprom_read_backflush(0xFF);

    // get data from eeprom
//    for (i=0; i<4; i++)		eeprom_read_authentication(i);

    // get data from eeprom
    eeprom_read_firmware(0xFF);

    // get data from eeprom
    for (i=0; i<FERTI_PROG_MAX; i++)
	{
    	//eeprom_read_fertiprog(i);
	  for (i=0; i<FERTI_PROG_MAX; i++)
		{
			eeprom_read_fertiprog1(i,0XFF);
			eeprom_read_fertiprog2(i,0XFF);
			for(j=0;j<4;j++)
			{
				eeprom_read_fertiprog3(i,j,0XFF);
			}
		}

	}

    // get data from eeprom
    for (i=0; i<ZONE_PROG_MAX; i++)
	{
    	//eeprom_read_zoneprog(i);
    	eeprom_read_zoneprog1(i,0XFF);
    	eeprom_read_zoneprog2(i,0XFF);
    	eeprom_read_zoneprog3(i,0XFF);
    	for(j=0;j<10;j++)
    	{
    		eeprom_read_zoneprog4(i,j,0XFF);
    	}

	}

    eeprom_read_progstatus(0xFF);
    // get data from eeprom
//    for (i=0; i<MAN_PROG_MAX; i++)	eeprom_read_man_program(0xFF);//(i);

    // get data from eeprom
//    for (i=0; i<OUTPUT_MAX; i++)		eeprom_read_man_output(i);

    // get data from eeprom
//    for (i=0; i<ALERT_MAX; i++)		eeprom_read_alert(i);

    // get data from eeprom
//    for (i=0; i<LOGS_MAX; i++)		eeprom_read_logs(i);
}

/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

void eeprom_read_contactinfo(uint8_t parameter_index)
{
	if (parameter_index & 0x01)
		EEPROMRead((uint8_t *)(&contact_phone[0]), EEPROM_CONTACT_INFO_PARA_1_ADDR, (sizeof(char) * 21));

	if (parameter_index & 0x02)
		EEPROMRead((uint8_t *)(&contact_phone[1]), EEPROM_CONTACT_INFO_PARA_2_ADDR, (sizeof(char) * 21));

	if (parameter_index & 0x04)
		EEPROMRead((uint8_t *)(contact_email), EEPROM_CONTACT_INFO_PARA_3_ADDR, (sizeof(char) * 21));
}

/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

void eeprom_write_contactinfo(uint8_t parameter_index)
{
	if (parameter_index & 0x01)
		EEPROMProgram((uint8_t *)(&contact_phone[0]), EEPROM_CONTACT_INFO_PARA_1_ADDR, (sizeof(char) * 21));

	if (parameter_index & 0x02)
		EEPROMProgram((uint8_t *)(&contact_phone[1]), EEPROM_CONTACT_INFO_PARA_2_ADDR, (sizeof(char) * 21));

	if (parameter_index & 0x04)
		EEPROMProgram((uint8_t *)(contact_email), EEPROM_CONTACT_INFO_PARA_3_ADDR, (sizeof(char) * 21));

#if Get_EPPROM_Address
	System_printf("EEPROM_CONTACT_INFO_BASE_ADDR = 0x%04x		EEPROM_CONTACT_INFO_PARA_SIZE = %d\n", EEPROM_CONTACT_INFO_BASE_ADDR, EEPROM_CONTACT_INFO_PARA_SIZE);
    System_flush();
#endif
}

/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

void eeprom_read_timedate(uint8_t parameter_index)
{
	if (parameter_index & 0x01)
	{
		EEPROMRead((uint8_t *)(&datetimesource), EEPROM_TIMEDATE_PARA_1_ADDR, sizeof(uint8_t));
		datetimesource_remote = datetimesource;
	}
}

/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

void eeprom_write_timedate(uint8_t parameter_index)
{
	if (parameter_index & 0x01)
		EEPROMProgram((uint8_t *)(&datetimesource), EEPROM_TIMEDATE_PARA_1_ADDR, sizeof(uint8_t));

#if Get_EPPROM_Address
	System_printf("EEPROM_TIMEDATE_BASE_ADDR = 0x%04x		EEPROM_TIMEDATE_SIZE = %d\n", EEPROM_TIMEDATE_BASE_ADDR, EEPROM_TIMEDATE_SIZE);
    System_flush();
#endif
}

/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

void eeprom_read_hardwareconfig(uint8_t parameter_index)
{
	//int i = EEPROM_MAN_OUTPUT_BASE_ADDR;
	if (parameter_index & 0x01)
	{
		EEPROMRead((uint8_t *)(&noopmodules), EEPROM_HW_CONFIG_PARA_1_ADDR, sizeof(uint8_t));
		noopmodules_remote = noopmodules;
	}

	if (parameter_index & 0x02)
	{
		EEPROMRead((uint8_t *)(&fertigation), EEPROM_HW_CONFIG_PARA_2_ADDR, sizeof(uint8_t));
		fertigation_remote = fertigation;
	}

	if (parameter_index & 0x04)
	{
		EEPROMRead((uint8_t *)(&flowperpulse), EEPROM_HW_CONFIG_PARA_3_ADDR, sizeof(uint32_t));
		flowperpulse_remote = flowperpulse;
	}

	if (parameter_index & 0x08)
	{
		EEPROMRead((uint8_t *)(&minop), EEPROM_HW_CONFIG_PARA_4_ADDR, sizeof(float));
		minop_remote = minop;
	}

	if (parameter_index & 0x10)
	{
		EEPROMRead((uint8_t *)(&maxdp), EEPROM_HW_CONFIG_PARA_5_ADDR, sizeof(float));
		maxdp_remote = maxdp;
	}
}

/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

void eeprom_write_hardwareconfig(uint8_t parameter_index)
{
	if (parameter_index & 0x01)
		EEPROMProgram((uint8_t *)(&noopmodules), EEPROM_HW_CONFIG_PARA_1_ADDR, sizeof(uint8_t));

	if (parameter_index & 0x02)
		EEPROMProgram((uint8_t *)(&fertigation), EEPROM_HW_CONFIG_PARA_2_ADDR, sizeof(uint8_t));

	if (parameter_index & 0x04)
		EEPROMProgram((uint8_t *)(&flowperpulse), EEPROM_HW_CONFIG_PARA_3_ADDR, sizeof(uint32_t));

	if (parameter_index & 0x08)
		EEPROMProgram((uint8_t *)(&minop), EEPROM_HW_CONFIG_PARA_4_ADDR, sizeof(float));

	if (parameter_index & 0x10)
		EEPROMProgram((uint8_t *)(&maxdp), EEPROM_HW_CONFIG_PARA_5_ADDR, sizeof(float));

#if Get_EPPROM_Address
	System_printf("EEPROM_HW_CONFIG_BASE_ADDR = 0x%04x		EEPROM_HW_CONFIG_SIZE = %d\n", EEPROM_HW_CONFIG_BASE_ADDR, EEPROM_HW_CONFIG_SIZE);
    System_flush();
#endif
}

/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

void eeprom_read_systemsetup(uint8_t parameter_index)
{
	if (parameter_index & 0x01)
	{
		EEPROMRead((uint8_t *)(&language), EEPROM_SYS_SETUP_PARA_1_ADDR, sizeof(uint8_t));
		language_remote = language;
	}

	if (parameter_index & 0x02)
	{
		EEPROMRead((uint8_t *)(&contrast), EEPROM_SYS_SETUP_PARA_2_ADDR, sizeof(uint8_t));
		contrast_remote = contrast;
	}

	if (parameter_index & 0x04)
	{
		EEPROMRead((uint8_t *)(&disp_clockformat), EEPROM_SYS_SETUP_PARA_3_ADDR, sizeof(uint8_t));
		disp_clockformat_remote = disp_clockformat;
	}

	if (parameter_index & 0x08)
	{
		EEPROMRead((uint8_t *)(&dateformat), EEPROM_SYS_SETUP_PARA_4_ADDR, sizeof(uint8_t));
		dateformat_remote = dateformat;
	}

	if (parameter_index & 0x10)
	{
		EEPROMRead((uint8_t *)(&beginweek), EEPROM_SYS_SETUP_PARA_5_ADDR, sizeof(uint8_t));
		beginweek_remote = beginweek;
	}
}

/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

void eeprom_write_systemsetup(uint8_t parameter_index)
{
	if (parameter_index & 0x01)
		EEPROMProgram((uint8_t *)(&language), EEPROM_SYS_SETUP_PARA_1_ADDR, sizeof(uint8_t));

	if (parameter_index & 0x02)
		EEPROMProgram((uint8_t *)(&contrast), EEPROM_SYS_SETUP_PARA_2_ADDR, sizeof(uint8_t));

	if (parameter_index & 0x04)
		EEPROMProgram((uint8_t *)(&disp_clockformat), EEPROM_SYS_SETUP_PARA_3_ADDR, sizeof(uint8_t));

	if (parameter_index & 0x08)
		EEPROMProgram((uint8_t *)(&dateformat), EEPROM_SYS_SETUP_PARA_4_ADDR, sizeof(uint8_t));

	if (parameter_index & 0x10)
		EEPROMProgram((uint8_t *)(&beginweek), EEPROM_SYS_SETUP_PARA_5_ADDR, sizeof(uint8_t));

#if Get_EPPROM_Address
	System_printf("EEPROM_SYS_SETUP_BASE_ADDR = 0x%04x		EEPROM_SYS_SETUP_SIZE = %d\n", EEPROM_SYS_SETUP_BASE_ADDR, EEPROM_SYS_SETUP_SIZE);
    System_flush();
#endif
}

/********************************************************************/
// This function reads output data from eeprom
/********************************************************************/

void eeprom_read_output(uint8_t index)
{
uint32_t ui32Address;

	ui32Address = EEPROM_OUTPUT_BASE_ADDR + (index * sizeof(output[index]));

	// Read from struct at EEPROM start from EEPROM_OUTPUT_BASE_ADDR
	EEPROMRead((uint8_t *)(&output[index]), ui32Address, sizeof(output[index]));
	memcpy (&output_remote[index], &output[index], sizeof (output[index]));
}

/********************************************************************/
// This function writes output data to eeprom
/********************************************************************/

void eeprom_write_output(uint8_t index)
{
uint32_t ui32Address;

	ui32Address = EEPROM_OUTPUT_BASE_ADDR + (index * sizeof(output[index]));

	// Write struct to EEPROM start from EEPROM_OUTPUT_BASE_ADDR
	EEPROMProgram((uint8_t *)(&output[index]), ui32Address, sizeof(output[index]));

#if Get_EPPROM_Address
	System_printf("EEPROM_OUTPUT_BASE_ADDR = 0x%04x		EEPROM_OUTPUT_SIZE = %d\n", EEPROM_OUTPUT_BASE_ADDR, EEPROM_OUTPUT_SIZE);
    System_flush();
#endif
}

/********************************************************************/
// This function reads input data from eeprom
/********************************************************************/

void eeprom_read_input(uint8_t index)
{
uint32_t ui32Address;

	ui32Address = EEPROM_INPUT_BASE_ADDR + (index * sizeof(input[index]));

	// Read from struct at EEPROM start from EEPROM_INPUT_BASE_ADDR
	EEPROMRead((uint8_t *)(&input[index]), ui32Address, sizeof(input[index]));
	memcpy (&input_remote[index], &input[index], sizeof (input[index]));
}

/********************************************************************/
// This function writes input data to eeprom
/********************************************************************/

void eeprom_write_input(uint8_t index)
{
uint32_t ui32Address;

	ui32Address = EEPROM_INPUT_BASE_ADDR + (index * sizeof(input[index]));

	// Write struct to EEPROM start from EEPROM_INPUT_BASE_ADDR
	EEPROMProgram((uint8_t *)(&input[index]), ui32Address, sizeof(input[index]));

#if Get_EPPROM_Address
	System_printf("EEPROM_INPUT_BASE_ADDR = 0x%04x		EEPROM_INPUT_SIZE = %d\n", EEPROM_INPUT_BASE_ADDR, EEPROM_INPUT_SIZE);
    System_flush();
#endif
}

/********************************************************************/
// This function reads sensor data from eeprom
/********************************************************************/

void eeprom_read_sensor(uint8_t index)
{
uint32_t ui32Address;

	ui32Address = EEPROM_SENSOR_BASE_ADDR + (index * sizeof(sensor[index]));

	// Read from struct at EEPROM start from EEPROM_SENSOR_BASE_ADDR
	EEPROMRead((uint8_t *)(&sensor[index]), ui32Address, sizeof(sensor[index]));
	memcpy (&sensor_remote[index], &sensor[index], sizeof (sensor[index]));

}

/********************************************************************/
// This function writes sensor data to eeprom
/********************************************************************/

void eeprom_write_sensor(uint8_t index)
{
uint32_t ui32Address;

	ui32Address = EEPROM_SENSOR_BASE_ADDR + (index * sizeof(sensor[index]));

	// Write struct to EEPROM start from EEPROM_SENSOR_BASE_ADDR
	EEPROMProgram((uint8_t *)(&sensor[index]), ui32Address, sizeof(sensor[index]));

#if Get_EPPROM_Address
	System_printf("EEPROM_SENSOR_BASE_ADDR = 0x%04x		EEPROM_SENSOR_SIZE = %d\n", EEPROM_SENSOR_BASE_ADDR, EEPROM_SENSOR_SIZE);
    System_flush();
#endif
}

/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

//void eeprom_read_network(uint8_t parameter_index)
void eeprom_read_network_gsm(uint8_t parameter_index)
{
	if (parameter_index & 0x01)
	{
		EEPROMRead((uint8_t *)(&wireless_mode), EEPROM_NETWORK_PARA_1_ADDR, sizeof(char));
		wireless_mode_remote = wireless_mode;
	}
	if (parameter_index & 0x02)
	{
		EEPROMRead((uint8_t *)(&gsm_module), EEPROM_NETWORK_PARA_2_ADDR, sizeof(char));
		gsm_module_remote = gsm_module;
	}
	if (parameter_index & 0x04)
	{
		EEPROMRead((uint8_t *)(gsm_apn), EEPROM_NETWORK_PARA_3_ADDR, (sizeof(char) * 20));
		strncpy(gsm_apn_remote,gsm_apn,20);
	}
	if (parameter_index & 0x08)
	{
		EEPROMRead((uint8_t *)(gsm_operator), EEPROM_NETWORK_PARA_4_ADDR, (sizeof(char) * 20));
		strncpy(gsm_operator_remote,gsm_operator,20);
	}
	if (parameter_index & 0x10)
	{
		EEPROMRead((uint8_t *)(&sig_strength), EEPROM_NETWORK_PARA_5_ADDR, sizeof(char));
		sig_strength_remote = sig_strength;
	}
}

/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

//void eeprom_write_network(uint8_t parameter_index)
void eeprom_write_network_gsm(uint8_t parameter_index)
{
	if (parameter_index & 0x01)
	{
		EEPROMProgram((uint8_t *)(&wireless_mode), EEPROM_NETWORK_PARA_1_ADDR, sizeof(char));
	}
	if (parameter_index & 0x02)
	{
		EEPROMProgram((uint8_t *)(&gsm_module), EEPROM_NETWORK_PARA_2_ADDR, sizeof(char));
	}
	if (parameter_index & 0x04)
	{
		EEPROMProgram((uint8_t *)(gsm_apn), EEPROM_NETWORK_PARA_3_ADDR, (sizeof(char) * 20));
	}
	if (parameter_index & 0x08)
	{
		EEPROMProgram((uint8_t *)(gsm_operator), EEPROM_NETWORK_PARA_4_ADDR, (sizeof(char) * 20));
	}
	if (parameter_index & 0x10)
	{
		EEPROMProgram((uint8_t *)(&sig_strength), EEPROM_NETWORK_PARA_5_ADDR, sizeof(char));
	}
}
/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

//void eeprom_write_network1(uint8_t parameter_index)
void eeprom_write_network_wifi(uint8_t parameter_index)
{
	if (parameter_index & 0x01)
	{
		EEPROMProgram((uint8_t *)(&wifi_module), EEPROM_NETWORK1_PARA_1_ADDR, sizeof(char));
	}
	if (parameter_index & 0x02)
	{
		EEPROMProgram((uint8_t *)(wifi_name), EEPROM_NETWORK1_PARA_2_ADDR, (sizeof(char) * 20));
	}
	if (parameter_index & 0x04)
	{
		EEPROMProgram((uint8_t *)(wifi_pass), EEPROM_NETWORK1_PARA_3_ADDR, (sizeof(char) * 20));
	}
	if (parameter_index & 0x08)
	{
		EEPROMProgram((uint8_t *)(ext_apn), EEPROM_NETWORK1_PARA_4_ADDR, (sizeof(char) * 20));
	}
	if (parameter_index & 0x10)
	{
		EEPROMProgram((uint8_t *)(ext_pass), EEPROM_NETWORK1_PARA_5_ADDR, (sizeof(char) * 20));
	}
	if (parameter_index & 0x20)
	{
		EEPROMProgram((uint8_t *)(portal_ip), EEPROM_NETWORK1_PARA_6_ADDR, (sizeof(char) * 4));
	}
	if (parameter_index & 0x40)
	{
		EEPROMProgram((uint8_t *)(portal_name), EEPROM_NETWORK1_PARA_7_ADDR, (sizeof(char) * 40));
	}
}

/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

//void eeprom_read_network1(uint8_t parameter_index)
void eeprom_read_network_wifi(uint8_t parameter_index)
{
	if (parameter_index & 0x01)
	{
		EEPROMRead((uint8_t *)(&wifi_module), EEPROM_NETWORK1_PARA_1_ADDR, sizeof(char));
		wifi_module_remote = wifi_module;
	}
	if (parameter_index & 0x02)
	{
		EEPROMRead((uint8_t *)(wifi_name), EEPROM_NETWORK1_PARA_2_ADDR, (sizeof(char) * 20));
		strncpy(wifi_name_remote,wifi_name,20);
	}
	if (parameter_index & 0x04)
	{
		EEPROMRead((uint8_t *)(wifi_pass), EEPROM_NETWORK1_PARA_3_ADDR, (sizeof(char) * 20));
		strncpy(wifi_pass_remote,wifi_pass,20);
	}
	if (parameter_index & 0x08)
	{
		EEPROMRead((uint8_t *)(ext_apn), EEPROM_NETWORK1_PARA_4_ADDR, (sizeof(char) * 20));
		strncpy(ext_apn_remote,ext_apn,20);
	}
	if (parameter_index & 0x10)
	{
		EEPROMRead((uint8_t *)(ext_pass), EEPROM_NETWORK1_PARA_5_ADDR, (sizeof(char) * 20));
		strncpy(ext_pass_remote,ext_pass,20);
	}
	if (parameter_index & 0x20)
	{
		EEPROMRead((uint8_t *)(portal_ip), EEPROM_NETWORK1_PARA_6_ADDR, (sizeof(char) * 4));
		strncpy(portal_ip_remote,portal_ip,4);
	}
	if (parameter_index & 0x40)
	{
		EEPROMRead((uint8_t *)(portal_name), EEPROM_NETWORK1_PARA_7_ADDR, (sizeof(char) * 40));
		strncpy(portal_name_remote,portal_name,40);
	}

}
/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

void eeprom_read_backflush(uint8_t parameter_index)
{
	if (parameter_index & 0x01)
	{
		EEPROMRead((uint8_t *)(&backflushstatus), EEPROM_BACKFLUSH_PARA_1_ADDR, sizeof(uint8_t));
		backflushstatus_remote = backflushstatus;
	}

	if (parameter_index & 0x02)
	{
		EEPROMRead((uint8_t *)(backflushinterval), EEPROM_BACKFLUSH_PARA_2_ADDR, (sizeof(uint8_t) * 2));
		backflushinterval_remote[0] = backflushinterval[0];
		backflushinterval_remote[1] = backflushinterval[1];
	}

	if (parameter_index & 0x04)
	{
		EEPROMRead((uint8_t *)(&backflushduration), EEPROM_BACKFLUSH_PARA_3_ADDR, sizeof(uint16_t));
		backflushduration_remote = backflushduration;
	}

	if (parameter_index & 0x08)
	{
		EEPROMRead((uint8_t *)(&backflushdelay), EEPROM_BACKFLUSH_PARA_4_ADDR, sizeof(uint16_t));
		backflushdelay_remote = backflushdelay;
	}

	if (parameter_index & 0x10)
	{
		EEPROMRead((uint8_t *)(&deltapres), EEPROM_BACKFLUSH_PARA_5_ADDR, sizeof(uint8_t));
		deltapres_remote = deltapres;
	}

	if (parameter_index & 0x20)
	{
		EEPROMRead((uint8_t *)(&deltadelay), EEPROM_BACKFLUSH_PARA_6_ADDR, sizeof(uint16_t));
		deltadelay_remote = deltadelay;
	}

	if (parameter_index & 0x40)
	{
		EEPROMRead((uint8_t *)(&deltaitera), EEPROM_BACKFLUSH_PARA_7_ADDR, sizeof(uint8_t));
		deltaitera_remote = deltaitera;
	}

	if (parameter_index & 0x80)
	{
		EEPROMRead((uint8_t *)(&dwelltime), EEPROM_BACKFLUSH_PARA_8_ADDR, sizeof(uint16_t));
		dwelltime_remote = dwelltime;
	}
}

/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

void eeprom_write_backflush(uint8_t parameter_index)
{
	if (parameter_index & 0x01)
		EEPROMProgram((uint8_t *)(&backflushstatus), EEPROM_BACKFLUSH_PARA_1_ADDR, sizeof(uint8_t));

	if (parameter_index & 0x02)
		EEPROMProgram((uint8_t *)(backflushinterval), EEPROM_BACKFLUSH_PARA_2_ADDR, (sizeof(uint8_t) * 2));

	if (parameter_index & 0x04)
		EEPROMProgram((uint8_t *)(&backflushduration), EEPROM_BACKFLUSH_PARA_3_ADDR, sizeof(uint16_t));

	if (parameter_index & 0x08)
		EEPROMProgram((uint8_t *)(&backflushdelay), EEPROM_BACKFLUSH_PARA_4_ADDR, sizeof(uint16_t));

	if (parameter_index & 0x10)
		EEPROMProgram((uint8_t *)(&deltapres), EEPROM_BACKFLUSH_PARA_5_ADDR, sizeof(uint8_t));

	if (parameter_index & 0x20)
		EEPROMProgram((uint8_t *)(&deltadelay), EEPROM_BACKFLUSH_PARA_6_ADDR, sizeof(uint16_t));

	if (parameter_index & 0x40)
		EEPROMProgram((uint8_t *)(&deltaitera), EEPROM_BACKFLUSH_PARA_7_ADDR, sizeof(uint8_t));

	if (parameter_index & 0x80)
		EEPROMProgram((uint8_t *)(&dwelltime), EEPROM_BACKFLUSH_PARA_8_ADDR, sizeof(uint16_t));

#if Get_EPPROM_Address
	System_printf("EEPROM_BACKFLUSH_BASE_ADDR = 0x%04x		EEPROM_BACKFLUSH_SIZE = %d\n", EEPROM_BACKFLUSH_BASE_ADDR, EEPROM_BACKFLUSH_SIZE);
    System_flush();
#endif
}

/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

void eeprom_read_authentication(uint8_t index)
{
uint32_t ui32Address;

	ui32Address = EEPROM_AUTHEN_BASE_ADDR + (index * (sizeof(char) * 11));

	EEPROMRead((uint8_t *)(&loginpasswords[index]), ui32Address, (sizeof(char) * 11));
	memcpy (&loginpasswords[index], &loginpasswords[index], (sizeof(char) * 11));
}

/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

void eeprom_write_authentication(uint8_t index)
{
uint32_t ui32Address;

	ui32Address = EEPROM_AUTHEN_BASE_ADDR + (index * (sizeof(char) * 11));

	EEPROMProgram((uint8_t *)(&loginpasswords[index]), ui32Address, (sizeof(char) * 11));

#if Get_EPPROM_Address
	System_printf("EEPROM_AUTHEN_BASE_ADDR = 0x%04x		EEPROM_AUTHEN_SIZE = %d\n", EEPROM_AUTHEN_BASE_ADDR, EEPROM_AUTHEN_SIZE);
    System_flush();
#endif
}

/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

void eeprom_read_firmware(uint8_t parameter_index)
{
	if (parameter_index & 0x01)
	{
		EEPROMRead((uint8_t *)(model), EEPROM_FIRMWARE_PARA_1_ADDR, (sizeof(char) * 13));
	}

	if (parameter_index & 0x02)
	{
		EEPROMRead((uint8_t *)(deviceid), EEPROM_FIRMWARE_PARA_2_ADDR, (sizeof(char) * 11));
	}

	if (parameter_index & 0x04)
	{
		EEPROMRead((uint8_t *)(&firmwareversion), EEPROM_FIRMWARE_PARA_3_ADDR, sizeof(float));
	}

	if (parameter_index & 0x08)
	{
		EEPROMRead((uint8_t *)(enetmacid), EEPROM_FIRMWARE_PARA_4_ADDR, (sizeof(char) * 11));
	}

	if (parameter_index & 0x10)
	{
		EEPROMRead((uint8_t *)(wifimacid), EEPROM_FIRMWARE_PARA_5_ADDR, (sizeof(char) * 11));
	}

	if (parameter_index & 0x20)
	{
		EEPROMRead((uint8_t *)(imeino), EEPROM_FIRMWARE_PARA_6_ADDR, (sizeof(char) * 11));
	}
}

/********************************************************************/
// This function reads ____ data from eeprom
/********************************************************************/

void eeprom_write_firmware(uint8_t parameter_index)
{
	if (parameter_index & 0x01)
		EEPROMProgram((uint8_t *)(model), EEPROM_FIRMWARE_PARA_1_ADDR, (sizeof(char) * 13));

	if (parameter_index & 0x02)
		EEPROMProgram((uint8_t *)(deviceid), EEPROM_FIRMWARE_PARA_2_ADDR, (sizeof(char) * 11));

	if (parameter_index & 0x04)
		EEPROMProgram((uint8_t *)(&firmwareversion), EEPROM_FIRMWARE_PARA_3_ADDR, sizeof(float));

	if (parameter_index & 0x08)
		EEPROMProgram((uint8_t *)(enetmacid), EEPROM_FIRMWARE_PARA_4_ADDR, (sizeof(char) * 11));

	if (parameter_index & 0x10)
		EEPROMProgram((uint8_t *)(wifimacid), EEPROM_FIRMWARE_PARA_5_ADDR, (sizeof(char) * 11));

	if (parameter_index & 0x20)
		EEPROMProgram((uint8_t *)(imeino), EEPROM_FIRMWARE_PARA_6_ADDR, (sizeof(char) * 11));

#if Get_EPPROM_Address
	System_printf("EEPROM_FIRMWARE_BASE_ADDR = 0x%04x		EEPROM_FIRMWARE_SIZE = %d\n", EEPROM_FIRMWARE_BASE_ADDR, EEPROM_FIRMWARE_SIZE);
    System_flush();
#endif
}

/********************************************************************/
// This function reads fertiprog data from eeprom
/********************************************************************/

//void eeprom_read_fertiprog(uint8_t index)
//{
//uint32_t ui32Address;
//
//	ui32Address = EEPROM_FERTIPROG_BASE_ADDR + (index * sizeof(fertiprog[index]));
//
//	// Read from struct at EEPROM start from EEPROM_FERTIPROG_BASE_ADDR
//	EEPROMRead((uint8_t *)(&fertiprog[index]), ui32Address, sizeof(fertiprog[index]));
//}

void eeprom_read_fertiprog1(uint8_t progid,uint8_t parameter_index)
{
	uint32_t ui32Address;

	ui32Address = EEPROM_FERTIPROG_BASE_ADDR + (progid * sizeof(ferti_prog_data));// sizeof(fertiprog[progid]

	if (parameter_index & 0x01)
	{
		EEPROMRead((uint8_t *)(&fertiprog[progid].id), (ui32Address+EEPROM_FERTIPROG1_PARA_1_ADDR), sizeof(uint8_t));
		fertiprog_remote[progid].id = fertiprog[progid].id;
	}

	if (parameter_index & 0x02)
	{
		EEPROMRead((uint8_t *)(&fertiprog[progid].type), (ui32Address+EEPROM_FERTIPROG1_PARA_2_ADDR), sizeof(uint8_t));
		fertiprog_remote[progid].type = fertiprog[progid].type;
	}

	if (parameter_index & 0x04)
	{
		EEPROMRead((uint8_t *)(&fertiprog[progid].method), (ui32Address+EEPROM_FERTIPROG1_PARA_3_ADDR), sizeof(uint8_t));
		fertiprog_remote[progid].method = fertiprog[progid].method;

	}

	if (parameter_index & 0x08)
	{
		EEPROMRead((uint8_t *)(&fertiprog[progid].propcycles), (ui32Address+EEPROM_FERTIPROG1_PARA_4_ADDR), sizeof(uint8_t));
		fertiprog_remote[progid].propcycles = fertiprog[progid].propcycles;
	}

	if (parameter_index & 0x10)
	{
		EEPROMRead((uint8_t *)(&fertiprog[progid].startdelay), (ui32Address+EEPROM_FERTIPROG1_PARA_5_ADDR), sizeof(uint16_t));
		fertiprog_remote[progid].startdelay = fertiprog[progid].startdelay;
	}

	if (parameter_index & 0x20)
	{
		EEPROMRead((uint8_t *)(&fertiprog[progid].stopdelay), (ui32Address+EEPROM_FERTIPROG1_PARA_6_ADDR), sizeof(uint16_t));
		fertiprog_remote[progid].stopdelay = fertiprog[progid].stopdelay;
	}
}



void eeprom_read_fertiprog2(uint8_t progid,uint8_t parameter_index)
{
	uint32_t ui32Address;

	ui32Address = EEPROM_FERTIPROG_BASE_ADDR + (progid * sizeof(ferti_prog_data));

	if (parameter_index & 0x01)
	{
		EEPROMRead((uint8_t *)(&fertiprog[progid].valvecount), (ui32Address+EEPROM_FERTIPROG2_PARA_1_ADDR), sizeof(uint8_t));
		fertiprog_remote[progid].valvecount = fertiprog[progid].valvecount;
	}

	if (parameter_index & 0x02)
	{
		EEPROMRead((uint8_t *)(&fertiprog[progid].totaltime), (ui32Address+EEPROM_FERTIPROG2_PARA_2_ADDR), sizeof(uint16_t));
		fertiprog_remote[progid].totaltime = fertiprog[progid].totaltime;
	}

	if (parameter_index & 0x04)
	{
		EEPROMRead((uint8_t *)(&fertiprog[progid].totalflow), (ui32Address+EEPROM_FERTIPROG2_PARA_3_ADDR), sizeof(uint32_t));
		fertiprog_remote[progid].totalflow = fertiprog[progid].totalflow;
	}
}


void eeprom_read_fertiprog3(uint8_t progid,uint8_t ioid,uint8_t parameter_index)
{
	uint32_t ui32Address,ui32Address1;

	ui32Address = EEPROM_FERTIPROG_BASE_ADDR + (progid * sizeof(ferti_prog_data));
	ui32Address1 = (ioid*sizeof(valve_data));

	if (parameter_index & 0x01)
	{
		EEPROMRead((uint8_t *)(&fertiprog[progid].valve[ioid].oppartno), (ui32Address+ui32Address1+EEPROM_FERTIPROG3_PARA_1_ADDR), sizeof(uint8_t));
		fertiprog_remote[progid].valve[ioid].oppartno = fertiprog[progid].valve[ioid].oppartno;
	}

	if (parameter_index & 0x02)
	{
		EEPROMRead((uint8_t *)(&fertiprog[progid].valve[ioid].starttime), (ui32Address+ui32Address1+EEPROM_FERTIPROG3_PARA_2_ADDR), sizeof(uint16_t));
		fertiprog_remote[progid].valve[ioid].starttime = fertiprog[progid].valve[ioid].starttime;
	}

	if (parameter_index & 0x04)
	{
		EEPROMRead((uint8_t *)(&fertiprog[progid].valve[ioid].stoptime), (ui32Address+ui32Address1+EEPROM_FERTIPROG3_PARA_3_ADDR), sizeof(uint16_t));
		fertiprog_remote[progid].valve[ioid].stoptime = fertiprog[progid].valve[ioid].stoptime;
	}

	if (parameter_index & 0x08)
	{
		EEPROMRead((uint8_t *)(&fertiprog[progid].valve[ioid].startflow), (ui32Address+ui32Address1+EEPROM_FERTIPROG3_PARA_4_ADDR), sizeof(uint32_t));
		fertiprog_remote[progid].valve[ioid].startflow = fertiprog[progid].valve[ioid].startflow;
	}

	if (parameter_index & 0x10)
	{
		EEPROMRead((uint8_t *)(&fertiprog[progid].valve[ioid].stopflow), (ui32Address+ui32Address1+EEPROM_FERTIPROG3_PARA_5_ADDR), sizeof(uint32_t));
		fertiprog_remote[progid].valve[ioid].stopflow = fertiprog[progid].valve[ioid].stopflow;
	}

}

/********************************************************************/
// This function writes fertiprog data to eeprom
/********************************************************************/

//void eeprom_write_fertiprog(uint8_t index)
//{
//uint32_t ui32Address;
//
//	ui32Address = EEPROM_FERTIPROG_BASE_ADDR + (index * sizeof(fertiprog[index]));
//
//	// Write struct to EEPROM start from EEPROM_FERTIPROG_BASE_ADDR
//	EEPROMProgram((uint8_t *)(&fertiprog[index]), ui32Address, sizeof(fertiprog[index]));
//
//#if Get_EPPROM_Address
//	System_printf("EEPROM_FERTIPROG_BASE_ADDR = 0x%04x		EEPROM_FERTIPROG_SIZE = %d\n", EEPROM_FERTIPROG_BASE_ADDR, EEPROM_FERTIPROG_SIZE);
//    System_flush();
//#endif
//}


void eeprom_write_fertiprog1(uint8_t progid,uint8_t parameter_index)
{
	uint32_t ui32Address;

	ui32Address = EEPROM_FERTIPROG_BASE_ADDR + (progid * sizeof(ferti_prog_data));

	if (parameter_index & 0x01)
		EEPROMProgram((uint8_t *)(&fertiprog[progid].id), (ui32Address+EEPROM_FERTIPROG1_PARA_1_ADDR), sizeof(uint8_t));

	if (parameter_index & 0x02)
		EEPROMProgram((uint8_t *)(&fertiprog[progid].type), (ui32Address+EEPROM_FERTIPROG1_PARA_2_ADDR), sizeof(uint8_t));

	if (parameter_index & 0x04)
		EEPROMProgram((uint8_t *)(&fertiprog[progid].method), (ui32Address+EEPROM_FERTIPROG1_PARA_3_ADDR), sizeof(uint8_t));

	if (parameter_index & 0x08)
		EEPROMProgram((uint8_t *)(&fertiprog[progid].propcycles), (ui32Address+EEPROM_FERTIPROG1_PARA_4_ADDR), sizeof(uint8_t));

	if (parameter_index & 0x10)
		EEPROMProgram((uint8_t *)(&fertiprog[progid].startdelay), (ui32Address+EEPROM_FERTIPROG1_PARA_5_ADDR), sizeof(uint16_t));

	if (parameter_index & 0x20)
		EEPROMProgram((uint8_t *)(&fertiprog[progid].stopdelay), (ui32Address+EEPROM_FERTIPROG1_PARA_6_ADDR), sizeof(uint16_t));
}



void eeprom_write_fertiprog2(uint8_t progid,uint8_t parameter_index)
{
	uint32_t ui32Address;

	ui32Address = EEPROM_FERTIPROG_BASE_ADDR + (progid * sizeof(ferti_prog_data));

	if (parameter_index & 0x01)
		EEPROMProgram((uint8_t *)(&fertiprog[progid].valvecount), (ui32Address+EEPROM_FERTIPROG2_PARA_1_ADDR), sizeof(uint8_t));

	if (parameter_index & 0x02)
		EEPROMProgram((uint8_t *)(&fertiprog[progid].totaltime), (ui32Address+EEPROM_FERTIPROG2_PARA_2_ADDR), sizeof(uint16_t));

	if (parameter_index & 0x04)
		EEPROMProgram((uint8_t *)(&fertiprog[progid].totalflow), (ui32Address+EEPROM_FERTIPROG2_PARA_3_ADDR), sizeof(uint32_t));
}


void eeprom_write_fertiprog3(uint8_t progid,uint8_t ioid,uint8_t parameter_index)
{
	uint32_t ui32Address,ui32Address1;

	ui32Address = EEPROM_FERTIPROG_BASE_ADDR + (progid * sizeof(ferti_prog_data));
	ui32Address1 = (ioid*sizeof(valve_data));

	if (parameter_index & 0x01)
		EEPROMProgram((uint8_t *)(&fertiprog[progid].valve[ioid].oppartno), (ui32Address+ui32Address1+EEPROM_FERTIPROG3_PARA_1_ADDR), sizeof(uint8_t));

	if (parameter_index & 0x02)
		EEPROMProgram((uint8_t *)(&fertiprog[progid].valve[ioid].starttime), (ui32Address+ui32Address1+EEPROM_FERTIPROG3_PARA_2_ADDR), sizeof(uint16_t));

	if (parameter_index & 0x04)
		EEPROMProgram((uint8_t *)(&fertiprog[progid].valve[ioid].stoptime), (ui32Address+ui32Address1+EEPROM_FERTIPROG3_PARA_3_ADDR), sizeof(uint16_t));

	if (parameter_index & 0x08)
		EEPROMProgram((uint8_t *)(&fertiprog[progid].valve[ioid].startflow), (ui32Address+ui32Address1+EEPROM_FERTIPROG3_PARA_4_ADDR), sizeof(uint32_t));

	if (parameter_index & 0x10)
		EEPROMProgram((uint8_t *)(&fertiprog[progid].valve[ioid].stopflow), (ui32Address+ui32Address1+EEPROM_FERTIPROG3_PARA_5_ADDR), sizeof(uint32_t));

}

/********************************************************************/
// This function reads zoneprog data from eeprom
/********************************************************************/

//void eeprom_read_zoneprog(uint8_t index)
//{
//uint32_t ui32Address;
//
//	ui32Address = EEPROM_ZONEPROG_BASE_ADDR + (index * sizeof(zoneprog[index]));
//
//	// Read from struct at EEPROM start from EEPROM_ZONEPROG_BASE_ADDR
//	EEPROMRead((uint8_t *)(&zoneprog[index]), ui32Address, sizeof(zoneprog[index]));
//}

/********************************************************************/
// This function writes zoneprog data to eeprom
/********************************************************************/

//void eeprom_write_zoneprog(uint8_t index)
//{
//uint32_t ui32Address;
//
//	ui32Address = EEPROM_ZONEPROG_BASE_ADDR + (index * sizeof(zoneprog[index]));
//
//	// Write struct to EEPROM start from EEPROM_ZONEPROG_BASE_ADDR
//	EEPROMProgram((uint8_t *)(&zoneprog[index]), ui32Address, sizeof(zoneprog[index]));
//
//#if Get_EPPROM_Address
//	System_printf("EEPROM_ZONEPROG_BASE_ADDR = 0x%04x		EEPROM_ZONEPROG_SIZE = %d\n", EEPROM_ZONEPROG_BASE_ADDR, EEPROM_ZONEPROG_SIZE);
//    System_flush();
//#endif
//}


void eeprom_read_zoneprog1(uint8_t progid,uint8_t parameter_index)
{
	uint32_t ui32Address;

	ui32Address = EEPROM_ZONEPROG_BASE_ADDR + (progid * sizeof(zone_prog_data));

	if (parameter_index & 0x01)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].id), (ui32Address+EEPROM_ZONEPROG1_PARA_1_ADDR), sizeof(uint8_t));
		zoneprog_remote[progid].id = zoneprog[progid].id;
	}
	if (parameter_index & 0x02)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].type), (ui32Address+EEPROM_ZONEPROG1_PARA_2_ADDR), sizeof(uint8_t));
		zoneprog_remote[progid].type = zoneprog[progid].type;
	}
	if (parameter_index & 0x04)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].status), (ui32Address+EEPROM_ZONEPROG1_PARA_3_ADDR), sizeof(uint8_t));
		zoneprog_remote[progid].status = zoneprog[progid].status;
	}
	if (parameter_index & 0x08)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].repeatcycles), (ui32Address+ EEPROM_ZONEPROG1_PARA_4_ADDR), sizeof(uint8_t));
		zoneprog_remote[progid].repeatcycles = zoneprog[progid].repeatcycles;
	}
	if (parameter_index & 0x10)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].cycledelay), (ui32Address+EEPROM_ZONEPROG1_PARA_5_ADDR), sizeof(uint16_t));
		zoneprog_remote[progid].cycledelay = zoneprog[progid].cycledelay;
	}
	if (parameter_index & 0x20)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].rundaysmode), (ui32Address+EEPROM_ZONEPROG1_PARA_6_ADDR), sizeof(uint8_t));
		zoneprog_remote[progid].rundaysmode = zoneprog[progid].rundaysmode;
	}
	if (parameter_index & 0x40)
	{
		EEPROMRead((uint8_t *)(zoneprog[progid].rundaysval), (ui32Address+EEPROM_ZONEPROG1_PARA_7_ADDR), (sizeof(uint8_t) * 3));
		zoneprog_remote[progid].rundaysval[0] = zoneprog[progid].rundaysval[0];
		zoneprog_remote[progid].rundaysval[1] = zoneprog[progid].rundaysval[1];
		zoneprog_remote[progid].rundaysval[2] = zoneprog[progid].rundaysval[2];
	}
}

void eeprom_read_zoneprog2(uint8_t progid,uint8_t parameter_index)
{
	uint32_t ui32Address;

	ui32Address = EEPROM_ZONEPROG_BASE_ADDR + (progid * sizeof(zone_prog_data));

	if (parameter_index & 0x01)
	{
		EEPROMRead((uint8_t *)(zoneprog[progid].starttime), (ui32Address+EEPROM_ZONEPROG2_PARA_1_ADDR), (sizeof(uint8_t) * 2));
		zoneprog_remote[progid].starttime[0] = zoneprog[progid].starttime[0];
		zoneprog_remote[progid].starttime[1] = zoneprog[progid].starttime[1];
	}
	if (parameter_index & 0x02)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].zonecount), (ui32Address+EEPROM_ZONEPROG2_PARA_2_ADDR), sizeof(uint8_t));
		zoneprog_remote[progid].zonecount = zoneprog[progid].zonecount;
	}
	if (parameter_index & 0x04)
	{
		EEPROMRead((uint8_t *)(zoneprog[progid].totaltime), (ui32Address+EEPROM_ZONEPROG2_PARA_3_ADDR), (sizeof(uint8_t) * 2));
		zoneprog_remote[progid].totaltime[0] = zoneprog[progid].totaltime[0];
		zoneprog_remote[progid].totaltime[1] = zoneprog[progid].totaltime[1];
	}
	if (parameter_index & 0x08)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].totalflow), (ui32Address+EEPROM_ZONEPROG2_PARA_4_ADDR), sizeof(uint16_t));
		zoneprog_remote[progid].totalflow = zoneprog[progid].totalflow;
	}
	if (parameter_index & 0x10)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].pumpdelay), (ui32Address+EEPROM_ZONEPROG2_PARA_5_ADDR), sizeof(uint16_t));
		zoneprog_remote[progid].pumpdelay = zoneprog[progid].pumpdelay;
	}
	if (parameter_index & 0x20)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].zonedelay), (ui32Address+EEPROM_ZONEPROG2_PARA_6_ADDR), sizeof(uint16_t));
		zoneprog_remote[progid].zonedelay = zoneprog[progid].zonedelay;
	}
}


void eeprom_read_zoneprog3(uint8_t progid,uint8_t parameter_index)
{
	uint32_t ui32Address;

	ui32Address = EEPROM_ZONEPROG_BASE_ADDR + (progid * sizeof(zone_prog_data));

	if (parameter_index & 0x01)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].rain), (ui32Address+EEPROM_ZONEPROG3_PARA_1_ADDR), sizeof(digital_sensor_data));
		zoneprog_remote[progid].rain = zoneprog[progid].rain;
	}
	if (parameter_index & 0x02)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].temperature), (ui32Address+EEPROM_ZONEPROG3_PARA_2_ADDR), sizeof(analog_sensor_data));
		zoneprog_remote[progid].temperature = zoneprog[progid].temperature;
	}
	if (parameter_index & 0x04)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].humidity), (ui32Address+EEPROM_ZONEPROG3_PARA_3_ADDR), sizeof(analog_sensor_data));
		zoneprog_remote[progid].humidity = zoneprog[progid].humidity;
	}
	if (parameter_index & 0x08)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].flow), (ui32Address+EEPROM_ZONEPROG3_PARA_4_ADDR), sizeof(digital_sensor_data));
		zoneprog_remote[progid].flow = zoneprog[progid].flow;
	}
	if (parameter_index & 0x10)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].oppressure), (ui32Address+EEPROM_ZONEPROG3_PARA_5_ADDR), sizeof(analog_sensor_data));
		zoneprog_remote[progid].oppressure = zoneprog[progid].oppressure;
	}
	if (parameter_index & 0x20)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].ippressure), (ui32Address+EEPROM_ZONEPROG3_PARA_6_ADDR), sizeof(analog_sensor_data));
		zoneprog_remote[progid].ippressure = zoneprog[progid].ippressure;
	}
}


void eeprom_read_zoneprog4(uint8_t progid,uint8_t ioid,uint8_t parameter_index)
{
	uint32_t ui32Address,ui32Address1;

	//ioid=ioid-1;

	ui32Address = EEPROM_ZONEPROG_BASE_ADDR + (progid * sizeof(zone_prog_data));
	ui32Address1 = (ioid * sizeof(zone_data));

	if (parameter_index & 0x01)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].zone[ioid].oppartno), (ui32Address+ui32Address1+EEPROM_ZONEPROG4_PARA_1_ADDR), sizeof(uint8_t));
		zoneprog_remote[progid].zone[ioid].oppartno = zoneprog[progid].zone[ioid].oppartno;
	}

	if (parameter_index & 0x02)
	{
		EEPROMRead((uint8_t *)(zoneprog[progid].zone[ioid].starttime), (ui32Address+ui32Address1+EEPROM_ZONEPROG4_PARA_2_ADDR), (sizeof(uint8_t)*2));
		zoneprog_remote[progid].zone[ioid].starttime[0] = zoneprog[progid].zone[ioid].starttime[0];
		zoneprog_remote[progid].zone[ioid].starttime[1] = zoneprog[progid].zone[ioid].starttime[1];
	}

	if (parameter_index & 0x04)
	{
		EEPROMRead((uint8_t *)(zoneprog[progid].zone[ioid].stoptime), (ui32Address+ui32Address1+EEPROM_ZONEPROG4_PARA_3_ADDR), (sizeof(uint8_t)*2));
		zoneprog_remote[progid].zone[ioid].stoptime[0] = zoneprog[progid].zone[ioid].stoptime[0];
		zoneprog_remote[progid].zone[ioid].stoptime[1] = zoneprog[progid].zone[ioid].stoptime[1];
	}

	if (parameter_index & 0x08)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].zone[ioid].startflow), (ui32Address+ui32Address1+EEPROM_ZONEPROG4_PARA_4_ADDR), sizeof(uint32_t));
		zoneprog_remote[progid].zone[ioid].startflow = zoneprog[progid].zone[ioid].startflow;
	}
	if (parameter_index & 0x10)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].zone[ioid].stopflow), (ui32Address+ui32Address1+EEPROM_ZONEPROG4_PARA_5_ADDR), sizeof(uint32_t));
		zoneprog_remote[progid].zone[ioid].stopflow = zoneprog[progid].zone[ioid].stopflow;
	}
	if (parameter_index & 0x20)
	{
		EEPROMRead((uint8_t *)(&zoneprog[progid].zone[ioid].fertprogid), (ui32Address+ui32Address1+EEPROM_ZONEPROG4_PARA_6_ADDR), sizeof(uint8_t));
		zoneprog_remote[progid].zone[ioid].fertprogid = zoneprog[progid].zone[ioid].fertprogid;
	}
}




void eeprom_write_zoneprog1(uint8_t progid,uint8_t parameter_index)
{
	uint32_t ui32Address;

	ui32Address = EEPROM_ZONEPROG_BASE_ADDR + (progid * sizeof(zone_prog_data));

	if (parameter_index & 0x01)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].id), (ui32Address+EEPROM_ZONEPROG1_PARA_1_ADDR), sizeof(uint8_t));

	if (parameter_index & 0x02)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].type), (ui32Address+EEPROM_ZONEPROG1_PARA_2_ADDR), sizeof(uint8_t));

	if (parameter_index & 0x04)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].status), (ui32Address+EEPROM_ZONEPROG1_PARA_3_ADDR), sizeof(uint8_t));

	if (parameter_index & 0x08)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].repeatcycles), (ui32Address+ EEPROM_ZONEPROG1_PARA_4_ADDR), sizeof(uint8_t));

	if (parameter_index & 0x10)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].cycledelay), (ui32Address+EEPROM_ZONEPROG1_PARA_5_ADDR), sizeof(uint16_t));

	if (parameter_index & 0x20)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].rundaysmode), (ui32Address+EEPROM_ZONEPROG1_PARA_6_ADDR), sizeof(uint8_t));

	if (parameter_index & 0x40)
		EEPROMProgram((uint8_t *)(zoneprog[progid].rundaysval), (ui32Address+EEPROM_ZONEPROG1_PARA_7_ADDR), (sizeof(uint8_t) * 3));
}

void eeprom_write_zoneprog2(uint8_t progid,uint8_t parameter_index)
{
	uint32_t ui32Address;

	ui32Address = EEPROM_ZONEPROG_BASE_ADDR + (progid * sizeof(zone_prog_data));

	if (parameter_index & 0x01)
		EEPROMProgram((uint8_t *)(zoneprog[progid].starttime), (ui32Address+EEPROM_ZONEPROG2_PARA_1_ADDR), (sizeof(uint8_t) * 2));

	if (parameter_index & 0x02)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].zonecount), (ui32Address+EEPROM_ZONEPROG2_PARA_2_ADDR), sizeof(uint8_t));

	if (parameter_index & 0x04)
		EEPROMProgram((uint8_t *)(zoneprog[progid].totaltime), (ui32Address+EEPROM_ZONEPROG2_PARA_3_ADDR), (sizeof(uint8_t) * 2));

	if (parameter_index & 0x08)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].totalflow), (ui32Address+EEPROM_ZONEPROG2_PARA_4_ADDR), sizeof(uint16_t));

	if (parameter_index & 0x10)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].pumpdelay), (ui32Address+EEPROM_ZONEPROG2_PARA_5_ADDR), sizeof(uint16_t));

	if (parameter_index & 0x20)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].zonedelay), (ui32Address+EEPROM_ZONEPROG2_PARA_6_ADDR), sizeof(uint16_t));
}


void eeprom_write_zoneprog3(uint8_t progid,uint8_t parameter_index)
{
	uint32_t ui32Address;

	ui32Address = EEPROM_ZONEPROG_BASE_ADDR + (progid * sizeof(zone_prog_data));

	if (parameter_index & 0x01)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].rain), (ui32Address+EEPROM_ZONEPROG3_PARA_1_ADDR), sizeof(digital_sensor_data));

	if (parameter_index & 0x02)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].temperature), (ui32Address+EEPROM_ZONEPROG3_PARA_2_ADDR), sizeof(analog_sensor_data));

	if (parameter_index & 0x04)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].humidity), (ui32Address+EEPROM_ZONEPROG3_PARA_3_ADDR), sizeof(analog_sensor_data));

	if (parameter_index & 0x08)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].flow), (ui32Address+EEPROM_ZONEPROG3_PARA_4_ADDR), sizeof(digital_sensor_data));

	if (parameter_index & 0x10)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].oppressure), (ui32Address+EEPROM_ZONEPROG3_PARA_5_ADDR), sizeof(analog_sensor_data));

	if (parameter_index & 0x20)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].ippressure), (ui32Address+EEPROM_ZONEPROG3_PARA_6_ADDR), sizeof(analog_sensor_data));
}


void eeprom_write_zoneprog4(uint8_t progid,uint8_t ioid,uint8_t parameter_index)
{
	uint32_t ui32Address,ui32Address1;

	ui32Address = EEPROM_ZONEPROG_BASE_ADDR + (progid * sizeof(zone_prog_data));
	ui32Address1 = (ioid * sizeof(zone_data));

	if (parameter_index & 0x01)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].zone[ioid].oppartno), (ui32Address+ui32Address1+EEPROM_ZONEPROG4_PARA_1_ADDR), sizeof(uint8_t));

	if (parameter_index & 0x02)
		EEPROMProgram((uint8_t *)(zoneprog[progid].zone[ioid].starttime), (ui32Address+ui32Address1+EEPROM_ZONEPROG4_PARA_2_ADDR), (sizeof(uint8_t)*2));

	if (parameter_index & 0x04)
		EEPROMProgram((uint8_t *)(zoneprog[progid].zone[ioid].stoptime), (ui32Address+ui32Address1+EEPROM_ZONEPROG4_PARA_3_ADDR), (sizeof(uint8_t)*2));

	if (parameter_index & 0x08)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].zone[ioid].startflow), (ui32Address+ui32Address1+EEPROM_ZONEPROG4_PARA_4_ADDR), sizeof(uint32_t));

	if (parameter_index & 0x10)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].zone[ioid].stopflow), (ui32Address+ui32Address1+EEPROM_ZONEPROG4_PARA_5_ADDR), sizeof(uint32_t));

	if (parameter_index & 0x20)
		EEPROMProgram((uint8_t *)(&zoneprog[progid].zone[ioid].fertprogid), (ui32Address+ui32Address1+EEPROM_ZONEPROG4_PARA_6_ADDR), sizeof(uint8_t));

}


void eeprom_write_progstatus(uint8_t parameter_index)
{
	if (parameter_index & 0x01)
		EEPROMProgram((uint8_t *)(remtim), EEPROM_PROGSTATUS_PARA_1_ADDR, (sizeof(uint16_t) * 6));

	if (parameter_index & 0x02)
		EEPROMProgram((uint8_t *)(&interrupted_prog), EEPROM_PROGSTATUS_PARA_2_ADDR, sizeof(char));

	if (parameter_index & 0x04)
		EEPROMProgram((uint8_t *)(&currentday), EEPROM_PROGSTATUS_PARA_3_ADDR, sizeof(char));

	if (parameter_index & 0x08)
			EEPROMProgram((uint8_t *)(&bf_valno), EEPROM_PROGSTATUS_PARA_4_ADDR, sizeof(char));

	if (parameter_index & 0x10)
			EEPROMProgram((uint8_t *)(&backflushon), EEPROM_PROGSTATUS_PARA_5_ADDR, sizeof(char));

	if (parameter_index & 0x20)
			EEPROMProgram((uint8_t *)(&bf_1min), EEPROM_PROGSTATUS_PARA_6_ADDR, sizeof(int));

	if (parameter_index & 0x40)
			EEPROMProgram((uint8_t *)(table_changed), EEPROM_PROGSTATUS_PARA_7_ADDR, (sizeof(char)*15));

}


void eeprom_read_progstatus(uint8_t parameter_index)
{
	if (parameter_index & 0x01)
		EEPROMRead((uint8_t *)(remtim), EEPROM_PROGSTATUS_PARA_1_ADDR, (sizeof(uint16_t) * 6));

	if (parameter_index & 0x02)
		EEPROMRead((uint8_t *)(&interrupted_prog), EEPROM_PROGSTATUS_PARA_2_ADDR, sizeof(char));

	if (parameter_index & 0x04)
		EEPROMRead((uint8_t *)(&currentday), EEPROM_PROGSTATUS_PARA_3_ADDR, sizeof(char));

	if (parameter_index & 0x08)
			EEPROMRead((uint8_t *)(&bf_valno), EEPROM_PROGSTATUS_PARA_4_ADDR, sizeof(char));

	if (parameter_index & 0x10)
			EEPROMRead((uint8_t *)(&backflushon), EEPROM_PROGSTATUS_PARA_5_ADDR, sizeof(char));

	if (parameter_index & 0x20)
			EEPROMRead((uint8_t *)(&bf_1min), EEPROM_PROGSTATUS_PARA_6_ADDR, sizeof(int));

	if (parameter_index & 0x40)
			EEPROMRead((uint8_t *)(table_changed), EEPROM_PROGSTATUS_PARA_7_ADDR, (sizeof(char)*15));
}

/********************************************************************/
//
/********************************************************************/

#define E2PROM_TEST_ADRES 0x0002

uint32_t	program1_write = 0x55223344;
uint32_t	program1_read = 0x00;

void eeprom_test(void)
{
    // System_printf("Write Try > Address %u: Struct : {%u,%u,%u,%s}\n", E2PROM_TEST_ADRES, e2prom_write_value.value1, e2prom_write_value.value2, e2prom_write_value.value3, e2prom_write_value.value4);
//    EEPROMProgram((uint8_t *)&program1_write, E2PROM_TEST_ADRES, sizeof(program1_write)); //Write struct to EEPROM start from 0x0000

//    EEPROMRead((uint8_t *)&program1_read, E2PROM_TEST_ADRES, sizeof(program1_read)); //Read from struct at EEPROM start from 0x0000
  //  System_printf("Read Try > Address %u: Struct : {%u,%u,%u,%s}\n", E2PROM_TEST_ADRES, e2prom_read_value.value1, e2prom_read_value.value2, e2prom_read_value.value3, e2prom_read_value.value4);

#if Get_EPPROM_Address
	System_printf("EEPROM_OTHERS_BASE_ADDR = 0x%04x		EEPROM_OTHERS_SIZE = %d\n", EEPROM_OTHERS_BASE_ADDR, EEPROM_OTHERS_SIZE);
    System_flush();
#endif
}

//*****************************************************************************
//
//*****************************************************************************

