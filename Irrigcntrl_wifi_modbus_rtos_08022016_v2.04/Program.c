
/********************************************************************/
/*		     Zone and Fertigation Program, Manual Mode				*/
/********************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ti/sysbios/knl/Task.h>
#include "database_struct.h"

/* Board Header files */
#include "Board.h"

/********************************************************************/
// Declare Global Functions
void auto_mode();
void manual_mode();
void Backflush_program();
void Remaintime_update();
void Remaintime_update1(uint8_t x);//x is the prog no
void Remaintime_clear(uint8_t x);//x is the prog no
void Starttime_Shift();
void nextprogstart();
/********************************************************************/
// Declare Local Functions

void zone_update(zone_prog_data prog_index ,char mode );
int sensor_check(void);
//void mainpump_onoff(uint8_t on_off);
void fertipump_onoff(uint8_t on_off);
void OP_Update(uint8_t opid,uint32_t on_off,uint8_t log);
void fertigation_program(zone_prog_data zoneprogram,uint8_t zoneno);

/********************************************************************/
// Global variables
extern volatile bool CheckRTC;

/********************************************************************/
// Local variables
char on_zones=0;//No of on zones for auto/manual mode
char on_op=0;//No of on op for manual mode
int zone_onoff[10]={0};//0=zone is currently off 1=zone is currently on
int fertivalve_onoff[4]={0};//0=zone is currently off 1=zone is currently on
int mainpump_onoff=0;
int fertip_onoff=0;
int op_onoff[16]={0};//0=op is currently off 1=op is currently on
int pumpdelay_count=0;
int mainpump_status=0;
int shift=0;//time shift due to power failure
int cycle_shift=0;//time shift for repeat cycles
int cycle=0;//current cycle of the program
uint8_t ferti_cnt=0;
uint8_t propcycle[4]={0};
//int next_day=0;//is set to 1 if the program end the next day
//for Logs
uint16_t zonestart_log[10]={0};
uint16_t zonestop_log[10]={0};
uint16_t fertivstart_log[4]={0};
uint16_t fertivstop_log[4]={0};
uint16_t fertivduration_log[4]={0};
char bf_valno=0;
char backflushon=0;
char bf_valonoff[4]={0};


/********************************************************************/
// Global Variables
extern int bf_1min;
extern zone_prog_data	zoneprog[ZONE_PROG_MAX];
extern ferti_prog_data	fertiprog[FERTI_PROG_MAX];
extern userdef_tm	CurTime, SetTime;
extern op_data	output[OUTPUT_MAX];
extern uint8_t	disp_clockformat ;
extern man_output_data	manoutput[OUTPUT_MAX];
extern man_prog_data	manprog[MAN_PROG_MAX];
extern uint8_t output_state[OUTPUT_MAX];
extern uint8_t
				nextprog_time[2];				// hh:mm

extern uint8_t
				cntrl_mode,						// 0-Auto / 1-Manual
				cntrl_state,					// 0-running / 1-standby
				cur_prog_state,					// 0-running / 1-standby
				cur_prog,
				cur_prog_elapsedtime[2];

extern char log_buffer[1000];
extern char log_temp[100];
extern int logflag;

//Backflush Parameters
extern uint8_t		backflushstatus;
extern uint8_t		backflushinterval[2];
extern uint16_t	backflushduration;
extern uint16_t	backflushdelay;

//extern char adjstthh[6];	//Adjusted start time of the programs in case of power interruptions hours value
//extern char adjsttmm[6];	//Adjusted start time of the programs in case of power interruptions minutes value
//extern char remtimhh[6];	//Remaining time of the programs to be updated at mignight or power failure hours value
//extern char remtimmm[6];	//Remaining time of the programs to be updated at mignight or power failure minutes value
//extern char powerofftime[2];
//extern char powerontime[2];

extern char interrupted_prog;//0 is no prog interrupted else the prog no value
extern uint16_t remtim[6];
extern uint32_t pwr_good,batt_mon;
extern char currentday;

extern void eeprom_write_progstatus(uint8_t parameter_index);
extern void eeprom_read_progstatus(uint8_t parameter_index);
extern void Write_log();
/********************************************************************************/
/* 			Routines For Zone and Ferigation Program, Manual Mode				*/
/********************************************************************************/
/********************************************************************/
// Function used for Main Pump Updation
/********************************************************************/
void mainpump_update(zone_prog_data zoneprogram )
{
	uint8_t i=0,opid=0;

	for(i=0;i<16;i++)
	{
		if(output[i].optype==1)//if output is a master pump
		{
			opid=i+1;
			break;
		}
	}

	if((mainpump_onoff==0)&&(mainpump_status == 1))
	{
		if(pumpdelay_count >= (zoneprogram.pumpdelay*10))
		{
			OP_Update(opid,0,1);//on
			mainpump_onoff=1;
		}
	}

	if((mainpump_onoff==1)&&(mainpump_status == 0))
	{
		OP_Update(opid,(~0),1);//off
		mainpump_onoff=0;
	}
}

int shift1=0;

/********************************************************************/
// Function used for Zone Updation
// Mode 0=Manual Mode 1= Auto Mode
/********************************************************************/
void zone_update(zone_prog_data zoneprogram, char mode)
{

	int i=0,programmed_time=0;//,j=0,k=0;
	uint16_t curr_val=0,compare_val=0;

	Backflush_program();



	for(i=0;i<16;i++)
	{
		if(output[i].optype==3)//if output is a zone
		{
			zoneprogram.zone[(output[i].typeid-1)].oppartno = i;
		}
	}

	//$$NR$$//dated:03Mar16
	//switch off zones when backflush is on
	if(backflushon == 1)
	{
		mainpump_status=1;
		for(i=0;i<10;i++)
		{
			//switch off zones
			on_zones = 0;

			if(zone_onoff[i]==1)//if zone on then switch it off
			{
				OP_Update((zoneprogram.zone[i].oppartno+1),(~0),1);//~0 is to switch off OP
				zone_onoff[i]=0;
			}
		}
		return;
	}
	//$$NR$$//dated:03Mar16

	if(mode == 0)
	{
		shift1 = cycle_shift;
	}
	else
	{
		shift1 = shift + cycle_shift;
	}

	if((zoneprogram.ippressure.currentval - zoneprogram.oppressure.currentval) > 0.5)//if pressure diff >0.5 bar
	{
		//backflush program
	}
	if(zoneprogram.oppressure.currentval <= zoneprogram.oppressure.min)//if pressure is not greater than min allowed value
	{
		//raise alert
		//return;
	}
	if(zoneprogram.type == 0)//Time Based Operation
	{
		for(i=0;i<10;i++)//for maximun 10 zones
		{
			if((zoneprogram.zone[i].starttime[0]==zoneprogram.zone[i].stoptime[0])&&(zoneprogram.zone[i].starttime[1]==zoneprogram.zone[i].stoptime[1]))
			{
				continue;//if zone start and stop time are same do not execute continue to next i
			}

			curr_val = (CurTime.tm_24hr*60)+CurTime.tm_min;
			compare_val = (zoneprogram.zone[i].starttime[0]*60) + zoneprogram.zone[i].starttime[1] + shift1;

			if(curr_val >= compare_val)
			{
				compare_val = (zoneprogram.zone[i].stoptime[0]*60) + zoneprogram.zone[i].stoptime[1] + shift1;
				if(zoneprogram.zone[i].starttime[0] > zoneprogram.zone[i].stoptime[0])
				{
					compare_val = compare_val + 1439;
					curr_val = curr_val + 1439;
				}
				if(curr_val < compare_val)
				{
					if(zone_onoff[i]==0)//if zone off then switch it on
					{
						if(on_zones>=4)//only 4 simultaneous zones allowed to operate
						{
							return;
						}
						OP_Update((zoneprogram.zone[i].oppartno+1),0,1);//0 is to switch on OP
						mainpump_status=1;
						pumpdelay_count=1;
						//if(pumpdelay_count == (zoneprogram.pumpdelay*10))
						zone_onoff[i]=1;
						on_zones++;
						zonestart_log[i] = curr_val;
					}
					fertigation_program(zoneprogram,i);

				}
			}
			compare_val = (zoneprogram.zone[i].stoptime[0]*60)+zoneprogram.zone[i].stoptime[1] + shift1;

			if(compare_val > 1439)//ie 23:59 hrs//todo max program runtime is 24 hrs
			{
				compare_val = compare_val - 1439;
				curr_val = (CurTime.tm_24hr*60)+CurTime.tm_min;
			}

			programmed_time = (zoneprogram.zone[i].stoptime[0]*60)+zoneprogram.zone[i].stoptime[1];
			programmed_time = programmed_time - ((zoneprogram.zone[i].starttime[0]*60)+zoneprogram.zone[i].starttime[1]);

			if(curr_val == compare_val)
			{
				if(zone_onoff[i]==1)//if zone on then switch it off
				{
					if(on_zones<=0)//all zones already off
					{
						return;
					}
					OP_Update((zoneprogram.zone[i].oppartno+1),(~0),1);//~0 is to switch off OP
					zone_onoff[i]=0;
					on_zones--;

					if(on_zones<=0)
					{
						mainpump_status=0;
					}
					zonestop_log[i] = curr_val;

					//IRRIGATION LOG
					memset(log_temp,0, sizeof(log_temp));

					sprintf (log_temp,"%d/%d/%d,%d:%d:%d,1,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",CurTime.tm_mday,CurTime.tm_mon,CurTime.tm_year,CurTime.tm_24hr,CurTime.tm_min,CurTime.tm_sec,
							cntrl_mode,zoneprogram.type,zoneprogram.id,(zoneprogram.zone[i].oppartno+1),programmed_time,(zonestop_log[i]-zonestart_log[i]),
							zoneprogram.zone[i].fertprogid,fertivduration_log[0],fertivduration_log[1],fertivduration_log[2],fertivduration_log[3]);
					strcat (log_buffer,log_temp);
					logflag=0;
					memset(zonestart_log,0, sizeof(zonestart_log));
					memset(zonestop_log,0, sizeof(zonestop_log));
					memset(fertivstart_log,0, sizeof(fertivstart_log));
					memset(fertivstop_log,0, sizeof(fertivstop_log));
					memset(fertivduration_log,0, sizeof(fertivduration_log));
				}
				fertigation_program(zoneprogram,i);
			}
		}
	}
	else //Flow Based Operation
	{


	}

}

/********************************************************************/
// Function used for OP Updation
/********************************************************************/
void OP_Update(uint8_t opid,uint32_t on_off,uint8_t log)
{
	switch(opid)
	{
		case 1:
			GPIO_write(Board_DO1, on_off);
			if(on_off==0)
			{
				output_state[opid-1]=1;//update the outputstatus for home screen display
			}
			else
			{
				output_state[opid-1]=0;//update the outputstatus for home screen display
			}
			break;
		case 2:
			GPIO_write(Board_DO2, on_off);
			if(on_off==0)
			{
				output_state[opid-1]=1;//update the outputstatus for home screen display
			}
			else
			{
				output_state[opid-1]=0;//update the outputstatus for home screen display
			}
			break;
		case 3:
			GPIO_write(Board_DO3, on_off);
			if(on_off==0)
			{
				output_state[opid-1]=1;//update the outputstatus for home screen display
			}
			else
			{
				output_state[opid-1]=0;//update the outputstatus for home screen display
			}
			break;
		case 4:
			GPIO_write(Board_DO4, on_off);
			if(on_off==0)
			{
				output_state[opid-1]=1;//update the outputstatus for home screen display
			}
			else
			{
				output_state[opid-1]=0;//update the outputstatus for home screen display
			}
			break;
		case 5:
			GPIO_write(Board_DO5, on_off);
			if(on_off==0)
			{
				output_state[opid-1]=1;//update the outputstatus for home screen display
			}
			else
			{
				output_state[opid-1]=0;//update the outputstatus for home screen display
			}
			break;
		case 6:
			GPIO_write(Board_DO6, on_off);
			if(on_off==0)
			{
				output_state[opid-1]=1;//update the outputstatus for home screen display
			}
			else
			{
				output_state[opid-1]=0;//update the outputstatus for home screen display
			}
			break;
		case 7:
			GPIO_write(Board_DO7, on_off);
			if(on_off==0)
			{
				output_state[opid-1]=1;//update the outputstatus for home screen display
			}
			else
			{
				output_state[opid-1]=0;//update the outputstatus for home screen display
			}
			break;
		case 8:
			GPIO_write(Board_DO8, on_off);
			if(on_off==0)
			{
				output_state[opid-1]=1;//update the outputstatus for home screen display
			}
			else
			{
				output_state[opid-1]=0;//update the outputstatus for home screen display
			}
			break;
		case 9:
			GPIO_write(Board_DO9, on_off);
			if(on_off==0)
			{
				output_state[opid-1]=1;//update the outputstatus for home screen display
			}
			else
			{
				output_state[opid-1]=0;//update the outputstatus for home screen display
			}
			break;
		case 10:
			GPIO_write(Board_DO10, on_off);
			if(on_off==0)
			{
				output_state[opid-1]=1;//update the outputstatus for home screen display
			}
			else
			{
				output_state[opid-1]=0;//update the outputstatus for home screen display
			}
			break;
		case 11:
			GPIO_write(Board_DO11, on_off);
			if(on_off==0)
			{
				output_state[opid-1]=1;//update the outputstatus for home screen display
			}
			else
			{
				output_state[opid-1]=0;//update the outputstatus for home screen display
			}
			break;
		case 12:
			GPIO_write(Board_DO12, on_off);
			if(on_off==0)
			{
				output_state[opid-1]=1;//update the outputstatus for home screen display
			}
			else
			{
				output_state[opid-1]=0;//update the outputstatus for home screen display
			}
			break;
		case 13:
			GPIO_write(Board_DO13, on_off);
			if(on_off==0)
			{
				output_state[opid-1]=1;//update the outputstatus for home screen display
			}
			else
			{
				output_state[opid-1]=0;//update the outputstatus for home screen display
			}
			break;
		case 14:
			GPIO_write(Board_DO14, on_off);
			if(on_off==0)
			{
				output_state[opid-1]=1;//update the outputstatus for home screen display
			}
			else
			{
				output_state[opid-1]=0;//update the outputstatus for home screen display
			}
			break;
		case 15:
			GPIO_write(Board_DO15, on_off);
			if(on_off==0)
			{
				output_state[opid-1]=1;//update the outputstatus for home screen display
			}
			else
			{
				output_state[opid-1]=0;//update the outputstatus for home screen display
			}
			break;
		case 16:
			GPIO_write(Board_DO16, on_off);
			if(on_off==0)
			{
				output_state[opid-1]=1;//update the outputstatus for home screen display
			}
			else
			{
				output_state[opid-1]=0;//update the outputstatus for home screen display
			}
			break;
		default:
			break;
	}
	if(log == 1)
	{
		//Event Log
		if(on_off == 0)
		{
			memset(log_temp,0, sizeof(log_temp));
			sprintf (log_temp,"%d/%d/%d,%d:%d:%d,2,%d,1\r\n",CurTime.tm_mday,CurTime.tm_mon,CurTime.tm_year,CurTime.tm_24hr,CurTime.tm_min,CurTime.tm_sec,opid);
			strcat (log_buffer,log_temp);
			logflag=0;
		}
		else
		{
			memset(log_temp,0, sizeof(log_temp));
			sprintf (log_temp,"%d/%d/%d,%d:%d:%d,2,%d,0\r\n",CurTime.tm_mday,CurTime.tm_mon,CurTime.tm_year,CurTime.tm_24hr,CurTime.tm_min,CurTime.tm_sec,opid);
			strcat (log_buffer,log_temp);
			logflag=0;
		}
	}

	//$$NR$$//dated:15Aug15
/*
	op_state_temp = (on_off ? 0:1);

	memset(log_temp,0, sizeof(log_temp));
	sprintf (log_temp,"%d/%d/%d,%d:%d:%d,2,%d,%d\r\n",CurTime.tm_mday,CurTime.tm_mon,CurTime.tm_year,CurTime.tm_24hr,CurTime.tm_min,CurTime.tm_sec,opid, op_state_temp);
	strcat (log_buffer,log_temp);
	logflag=0;
*/
	CheckRTC = 1;
	//$$NR$$//dated:15Aug15
	return;
}

/********************************************************************/
// Function used for checking different sensor status
/********************************************************************/
int sensor_check()
{
	//yet to implement
	return 1;
}

/********************************************************************/
// Function used for switching on main pump
/********************************************************************/
//void mainpump_onoff(uint8_t on_off)
//{
//	uint8_t i=0,opid=0;
//
//	for(i=0;i<16;i++)
//	{
//		if(output[i].optype==1)//if output is a master pump
//		{
//			opid=i+1;
//			break;
//		}
//	}
//
//	OP_Update(opid,on_off);
//}
/********************************************************************/
// Function used for switching on main pump
/********************************************************************/
void fertipump_onoff(uint8_t on_off)
{
	uint8_t i=0,opid=0;

	for(i=0;i<16;i++)
	{
		if(output[i].optype==4)//if output is a fertigation pump
		{
			opid=i+1;
			break;
		}
	}

	if(on_off == 0)
	{
		if(fertip_onoff == 0)
		{
			OP_Update(opid,on_off,1);
			fertip_onoff = 1;
		}
	}
	else
	{
		if(fertip_onoff == 1)
		{
			OP_Update(opid,on_off,1);
			fertip_onoff = 0;
		}
	}
}
/********************************************************************/
// Function used for auto mode program
/********************************************************************/
void auto_mode()
{
	uint8_t stop_hh,stop_mm,stop_hh1,stop_mm1;
	uint16_t curr_val=0,compare_val=0;
	int prog_index=0;
	int i=0;

	if(cntrl_mode!=0)//If not in auto mode
	{
		return;
	}

	for(prog_index=0;prog_index<=5;prog_index++)
	{
		if(zoneprog[prog_index].status==0)
		{
			continue;
		}
		switch(zoneprog[prog_index].rundaysmode)
		{
			case 0: // 0=daywise
				if(zoneprog[prog_index].rundaysval[zoneprog[prog_index].rundaysmode]&(0x01<<(CurTime.tm_wday)))
				{
					break;
				}
				else
				{
					//return;
					continue;
				}
			case 1: // 1=oddeven
				if(((CurTime.tm_wday+1)%2)==zoneprog[prog_index].rundaysval[zoneprog[prog_index].rundaysmode])
				{
					break;
				}
				else
				{
					//return;
					continue;
				}
			case 2: // 2=custom
				if((CurTime.tm_wday%zoneprog[prog_index].rundaysval[zoneprog[prog_index].rundaysmode])==0)
				{
					break;
				}
				else
				{
					//return;
					continue;
				}
			default:
				break;
		 }
		curr_val = (CurTime.tm_24hr*60)+CurTime.tm_min;
		compare_val = (zoneprog[prog_index].starttime[0]*60) + zoneprog[prog_index].starttime[1] + shift;
		if(curr_val >= compare_val)//check if current time > prog start time
		{
			compare_val= (zoneprog[prog_index].starttime[0]*60) + zoneprog[prog_index].starttime[1];
			compare_val= compare_val + (zoneprog[prog_index].cycledelay * zoneprog[prog_index].repeatcycles);
			compare_val= compare_val + (((zoneprog[prog_index].totaltime[0]*60) + zoneprog[prog_index].totaltime[1]) * (zoneprog[prog_index].repeatcycles+1));
			compare_val= compare_val + shift;
			stop_hh1 = compare_val/60;
			stop_mm1 = compare_val%60;

			if(stop_hh1>=24)
			{
				stop_hh1 = stop_hh1-24;
			}

			if(curr_val < compare_val)//check if current time < prog stop time
			{
				if(sensor_check()==1)//check if any external sensor exceptions
				{
					if(interrupted_prog == 0)	cycle=0;

					if(interrupted_prog == (prog_index+1))
					{//calculate the cycle using interrupted time
						compare_val= (zoneprog[prog_index].cycledelay * zoneprog[prog_index].repeatcycles);
						compare_val= compare_val + (((zoneprog[prog_index].totaltime[0]*60) + zoneprog[prog_index].totaltime[1]) * (zoneprog[prog_index].repeatcycles+1));//total time
						compare_val= compare_val - remtim[prog_index];
						cycle = (zoneprog[prog_index].totaltime[0]*60) + zoneprog[prog_index].totaltime[1]+zoneprog[prog_index].cycledelay;
						cycle = (compare_val/(cycle));
					}
					cntrl_state=0;//Status Running
					cur_prog_state=0;//Program Mode Running
					cur_prog= prog_index + 1;// Number of the Running Program

					while(1)
					{
						cycle_shift = (zoneprog[prog_index].cycledelay * cycle);
						cycle_shift = cycle_shift + (((zoneprog[prog_index].totaltime[0]*60) + zoneprog[prog_index].totaltime[1]) * cycle);

						stop_hh=zoneprog[prog_index].starttime[0]+zoneprog[prog_index].totaltime[0] + (shift/60) + (cycle_shift/60);
						stop_mm=zoneprog[prog_index].starttime[1]+zoneprog[prog_index].totaltime[1] + (shift%60) + (cycle_shift%60);
						if(stop_mm>=60)
						{
							stop_hh=stop_hh+(stop_mm/60);
							stop_mm=stop_mm-((stop_mm/60)*60);
						}
						if(stop_hh>=24)
						{
							stop_hh=stop_hh-24;
						}

						if(stop_hh1<CurTime.tm_24hr)
						{
							cur_prog_elapsedtime[0] = 24;
						}
						else
						{
							cur_prog_elapsedtime[0] = 0;
						}
						if(stop_mm1>=CurTime.tm_min)
						{
							cur_prog_elapsedtime[0]= cur_prog_elapsedtime[0] + stop_hh1-CurTime.tm_24hr;
							cur_prog_elapsedtime[1]=stop_mm1-CurTime.tm_min;
						}
						else
						{
							cur_prog_elapsedtime[0]= cur_prog_elapsedtime[0] + (stop_hh1-CurTime.tm_24hr)-1;
							cur_prog_elapsedtime[1]=60-(CurTime.tm_min-stop_mm1);
						};

						if(cur_prog_elapsedtime[0]>=24)	cur_prog_elapsedtime[0]=0;//invalid values
						if(cur_prog_elapsedtime[1]>=60)	cur_prog_elapsedtime[1]=0;

						remtim[prog_index] = (cur_prog_elapsedtime[0]*60) + cur_prog_elapsedtime[1];

						zone_update(zoneprog[prog_index],1);
						mainpump_update(zoneprog[prog_index]);//switch on main pump

						if((stop_hh==CurTime.tm_24hr)&&(stop_mm==CurTime.tm_min))
						{
							if((on_zones == 0) && (cycle == (zoneprog[prog_index].repeatcycles)))
							{
								cur_prog_elapsedtime[0]=0;
								cur_prog_elapsedtime[1]=0;
								remtim[prog_index]=0;
								on_zones=0;//No of on zones for auto/manual mode
								on_op=0;//No of on op for manual mode
								interrupted_prog = 0;
								cycle_shift=0;
								cycle=0;
								shift=0;// after interrupted or delayed program execution make shift 0
								mainpump_onoff=0;
								mainpump_status=0;
								fertip_onoff=0;
								ferti_cnt=0;
								for(i=0;i<10;i++)
								{
									zone_onoff[i]=0;//0=zone is currently off 1=zone is currently on
								}
								for(i=0;i<4;i++)
								{
									fertivalve_onoff[i] = 0;
								}
								for(i=0;i<4;i++)
								{
									bf_valonoff[i]=0;
								}
//								for(i=0;i<16;i++)//switch off all bf valves as motor is off
//								{
//									if(output[i].optype==2)//if output is a flush valve
//									{
//										OP_Update((i+1),(~0),0);//switch off output
//									}
//								}
								for(i=0;i<16;i++)
								{
									op_onoff[i]=0;
									OP_Update(i+1,(~0),0);//switch off all outputs
								}

								eeprom_write_progstatus(0xff);
								eeprom_read_progstatus(0xff);
								break;
							}
							else{
								cycle++;
							}
						}
						if((cntrl_mode!=0)||(pwr_good < 900)||(CurTime.tm_mday!=currentday))//If not in auto mode or mains power not ok
						{
							//next_day=0;
							interrupted_prog = prog_index+1;
							eeprom_write_progstatus(0xff);
							eeprom_read_progstatus(0xff);
							cur_prog_elapsedtime[0]=0;
							cur_prog_elapsedtime[1]=0;
							on_zones=0;//No of on zones for auto/manual mode
							on_op=0;//No of on op for manual mode
							cur_prog_state=1;//Program Mode Standby
							cntrl_state=1;//Status Standby
							cur_prog= 0;// 0 ie no Running Program
							mainpump_onoff=0;
							mainpump_status=0;
							fertip_onoff=0;
							ferti_cnt=0;
							cycle_shift=0;
							cycle=0;
							for(i=0;i<10;i++)
							{
								zone_onoff[i]=0;//0=zone is currently off 1=zone is currently on
							}
							for(i=0;i<4;i++)
							{
								fertivalve_onoff[i] = 0;
							}
							for(i=0;i<4;i++)
							{
								bf_valonoff[i]=0;
							}
							for(i=0;i<16;i++)
							{
								op_onoff[i]=0;
								OP_Update(i+1,(~0),0);//switch off all outputs
							}
							return;
						}
						ADCHandler();

						if (logflag == 0) {
							Write_log();
							logflag = 1;
						}

						Task_sleep(20);
					}
					cur_prog= 0;
					//mainpump_onoff(~0);//switch off main pump
					cur_prog_state=1;//Program Standby
					cntrl_state=1;//Status Standby
					break;
				}
			}
		}
	}
}


void manual_mode()
{
//	uint8_t temp0,temp1;
	uint8_t stop_hh,stop_mm;
	zone_prog_data	temp_zoneprog;
	int i,j;
	if(cntrl_mode!=1)//If not in manual mode return
	{
		return;
	}

	//	Manual Mode Program ON/OFF
	for(i=0;i<6;i++)
	{
		if(manprog[i].state==1)
		{
			on_zones=0;//No of on zones for auto/manual mode
			on_op=0;//No of on op for manual mode
			for(j=0;j<10;j++)
			{
				zone_onoff[j]=0;//0=zone is currently off 1=zone is currently on
			}
			for(j=0;j<16;j++)
			{
				op_onoff[j]=0;
				OP_Update(j+1,(~0),0);//switch off all outputs
			}

			temp_zoneprog=zoneprog[i];//copy zone program to a temp structure

			//Calculate new values with current time as the start time
			for(j=0;j<10;j++)
			{
				if(((temp_zoneprog.zone[j].starttime[0]==0)&&(temp_zoneprog.zone[j].starttime[1]==0))&&((temp_zoneprog.zone[j].stoptime[0]==0)&&(temp_zoneprog.zone[j].stoptime[1]==0)))
				{
					continue;
				}
				if(temp_zoneprog.zone[j].starttime[1]>=temp_zoneprog.starttime[1])
				{
					temp_zoneprog.zone[j].starttime[0]=CurTime.tm_24hr+(temp_zoneprog.zone[j].starttime[0]-temp_zoneprog.starttime[0]);
					temp_zoneprog.zone[j].starttime[1]=CurTime.tm_min+(temp_zoneprog.zone[j].starttime[1]-temp_zoneprog.starttime[1]);
				}
				else
				{
					temp_zoneprog.zone[j].starttime[0]=CurTime.tm_24hr+(temp_zoneprog.zone[j].starttime[0]-temp_zoneprog.starttime[0]);
					if((temp_zoneprog.starttime[1]-temp_zoneprog.zone[j].starttime[1])<=CurTime.tm_min)
					{
						temp_zoneprog.zone[j].starttime[1]=CurTime.tm_min-(temp_zoneprog.starttime[1]-temp_zoneprog.zone[j].starttime[1]);
					}
					else
					{
						temp_zoneprog.zone[j].starttime[1]=(CurTime.tm_min+60)-(temp_zoneprog.starttime[1]-temp_zoneprog.zone[j].starttime[1]);
					}
				};
				if(temp_zoneprog.zone[j].stoptime[1]>=temp_zoneprog.starttime[1])
				{
					temp_zoneprog.zone[j].stoptime[0]=CurTime.tm_24hr+(temp_zoneprog.zone[j].stoptime[0]-temp_zoneprog.starttime[0]);
					temp_zoneprog.zone[j].stoptime[1]=CurTime.tm_min+(temp_zoneprog.zone[j].stoptime[1]-temp_zoneprog.starttime[1]);
				}
				else
				{
					temp_zoneprog.zone[j].stoptime[0]=CurTime.tm_24hr+(temp_zoneprog.zone[j].stoptime[0]-temp_zoneprog.starttime[0]);
					if((temp_zoneprog.starttime[1]-temp_zoneprog.zone[j].stoptime[1])<=CurTime.tm_min)
					{
						temp_zoneprog.zone[j].stoptime[1]=CurTime.tm_min-(temp_zoneprog.starttime[1]-temp_zoneprog.zone[j].stoptime[1]);
					}
					else
					{
						temp_zoneprog.zone[j].stoptime[1]=(CurTime.tm_min+60)-(temp_zoneprog.starttime[1]-temp_zoneprog.zone[j].stoptime[1]);
					}
				};
			}

			temp_zoneprog.starttime[0]= CurTime.tm_24hr;
			temp_zoneprog.starttime[1]= CurTime.tm_min;

			if(sensor_check()==1)
			{
				cntrl_state=0;//Status Running
				cur_prog_state=0;//Program Mode Running
				cur_prog= i+1;
				cycle=0;
			//	mainpump_onoff(0);//switch on main pump
				while(1)
				{
					cycle_shift = (temp_zoneprog.cycledelay * cycle);
					cycle_shift = cycle_shift + (((temp_zoneprog.totaltime[0]*60) + temp_zoneprog.totaltime[1]) * cycle);

					stop_hh= temp_zoneprog.starttime[0] + temp_zoneprog.totaltime[0] + (cycle_shift/60);
					stop_mm= temp_zoneprog.starttime[1] + temp_zoneprog.totaltime[1] + (cycle_shift%60);
					if(stop_mm>=60)
					{
						stop_hh=stop_hh+(stop_mm/60);
						stop_mm=stop_mm-((stop_mm/60)*60);
					}
					if(stop_hh>=24)
					{
						stop_hh=stop_hh-24;
					}

					if(stop_mm>=CurTime.tm_min)
					{
						cur_prog_elapsedtime[0]=stop_hh-CurTime.tm_24hr;
						cur_prog_elapsedtime[1]=stop_mm-CurTime.tm_min;
					}
					else
					{
						cur_prog_elapsedtime[0]=(stop_hh-CurTime.tm_24hr)-1;
						cur_prog_elapsedtime[1]=60-(CurTime.tm_min-stop_mm);
					};


					zone_update(temp_zoneprog,1);
					mainpump_update(temp_zoneprog);//switch on main pump

					if((stop_hh==CurTime.tm_24hr)&&(stop_mm==CurTime.tm_min))
					{
						if((on_zones == 0) && (cycle == (temp_zoneprog.repeatcycles)))
						{
							cur_prog_elapsedtime[0]=0;
							cur_prog_elapsedtime[1]=0;

							on_zones=0;//No of on zones for auto/manual mode
							on_op=0;//No of on op for manual mode

							cycle_shift=0;
							cycle=0;
							shift=0;// after interrupted or delayed program execution make shift 0
							mainpump_onoff=0;
							mainpump_status=0;
							fertip_onoff=0;
							ferti_cnt=0;
							for(j=0;j<10;j++)
							{
								zone_onoff[j]=0;//0=zone is currently off 1=zone is currently on
							}
							for(j=0;j<4;j++)
							{
								fertivalve_onoff[j] = 0;
							}
							for(j=0;j<4;j++)
							{
								bf_valonoff[j]=0;
							}

							for(j=0;j<16;j++)
							{
								op_onoff[j]=0;
								OP_Update(i+1,(~0),0);//switch off all outputs
							}

							break;
						}
						else{
							cycle++;
						}
					}
					if((cntrl_mode!=1)||(pwr_good < 900)||(CurTime.tm_mday!=currentday))//If not in auto mode or mains power not ok
					{
						eeprom_write_progstatus(0xff);
						eeprom_read_progstatus(0xff);
						cur_prog_elapsedtime[0]=0;
						cur_prog_elapsedtime[1]=0;
						on_zones=0;//No of on zones for auto/manual mode
						on_op=0;//No of on op for manual mode
						cur_prog_state=1;//Program Mode Standby
						cntrl_state=1;//Status Standby
						cur_prog= 0;// 0 ie no Running Program
						manprog[i].state=0;
						mainpump_onoff=0;
						mainpump_status=0;
						fertip_onoff=0;
						ferti_cnt=0;
						cycle_shift=0;
						cycle=0;
						for(j=0;j<10;j++)
						{
							zone_onoff[j]=0;//0=zone is currently off 1=zone is currently on
						}
						for(j=0;j<4;j++)
						{
							fertivalve_onoff[j] = 0;
						}
						for(j=0;j<4;j++)
						{
							bf_valonoff[j]=0;
						}
						for(j=0;j<16;j++)
						{
							op_onoff[j]=0;
							OP_Update(j+1,(~0),0);//switch off all outputs
						}
						return;
					}

					if (logflag == 0) {
						Write_log();
						logflag = 1;
					}
					ADCHandler();
					Task_sleep(20);
				}
				//mainpump_onoff(~0);//switch off main pump
				manprog[i].state=0;
				cur_prog_state=1;//Program Standby
				cur_prog= 0;
				cntrl_state=1;//Status Standby
				//break2: break;
				return;
			}
		}
	}

	// Manual Mode Outputs ON/OFF
	for(i=0;i<16;i++)
	{
		if(manoutput[i].state==1)
		{
			if(op_onoff[i]==0)
			{
				if(on_op>=4)//only 4 simultaneous ops allowed to operate
				{
					continue;
				}
				OP_Update((i+1),0,1);//0 is to switch on OP
				op_onoff[i]=1;
				on_op++;
			}
		}
		if(manoutput[i].state==0)
		{
			if(op_onoff[i]==1)
			{
				if(on_op<=0)
				{
					continue;
				}
				OP_Update((i+1),(~0),1);//~0 is to switch off OP
				op_onoff[i]=0;
				on_op--;
			}
		}
	}
}






void fertigation_program(zone_prog_data zoneprogram,uint8_t zoneno)
{
	uint8_t id=0,i=0;
	uint16_t curr_val=0,compare_val=0,compare_val1=0,temp=0,temp1=0;
	uint8_t start_hh=0,start_mm=0,stop_hh=0,stop_mm=0;

	id=zoneprogram.zone[zoneno].fertprogid;
	if(id == 0)	return;
	if(id >= 7)	return;
	id=id-1;



	for(i=0;i<16;i++)
	{
		if(output[i].optype == 5)//if output is a fertigation valve
		{
			fertiprog[id].valve[(output[i].typeid-1)].oppartno = i;
		}
	}



	start_mm = (shift1%60);
	start_hh = (shift1/60);

	start_hh = start_hh + (fertiprog[id].startdelay/60);
	start_hh = start_hh + zoneprogram.zone[zoneno].starttime[0];

	if(start_hh>23)
	{
		start_hh = start_hh-24;
	}

	start_mm = start_mm + (fertiprog[id].startdelay%60);
	start_mm = start_mm + zoneprogram.zone[zoneno].starttime[1];
	if(start_mm>59)
	{
		start_hh = start_hh+(start_mm/60);
		if(start_hh>23)
		{
			start_hh = start_hh-24;
		}
		start_mm = (start_mm % 60);
	}

	stop_mm = stop_mm + (shift1%60);
	stop_hh = stop_hh + (shift1/60);

	stop_hh = stop_hh +  zoneprogram.zone[zoneno].stoptime[0];

	if(stop_hh > (fertiprog[id].stopdelay/60))
	{
		stop_hh = stop_hh - (fertiprog[id].stopdelay/60);
	}
	else
	{
		stop_hh = 24 + stop_hh - (fertiprog[id].stopdelay/60);
	}

	if(stop_hh>23)
	{
		stop_hh = stop_hh-24;
	}


	stop_mm = stop_mm + zoneprogram.zone[zoneno].stoptime[1];

	if(stop_mm > (fertiprog[id].stopdelay%60))
	{
		stop_mm = stop_mm - (fertiprog[id].stopdelay%60);
	}
	else
	{
		stop_mm = 60 + stop_mm - (fertiprog[id].stopdelay%60);

		if(stop_hh!= 0)	stop_hh = stop_hh - 1;
		else stop_hh=23;
	}



	fertiprog[id].totaltime = ((stop_hh*60)+stop_mm) - ((start_hh*60)+start_mm);


	curr_val = (CurTime.tm_24hr*60)+CurTime.tm_min;
	if(fertiprog[id].type == 0)//timebased
	{
		compare_val = (start_hh*60)+start_mm;
 		if( curr_val >= compare_val)//begin fertigation prog
		{
 			compare_val = (stop_hh*60)+stop_mm;
 			if(start_hh > stop_hh)
			{
				compare_val = compare_val + 1439;
				curr_val = curr_val + 1439;
			}
 			if(curr_val < compare_val)//within stop time
 			{
				if(fertiprog[id].method == 0)// Bulk Method
				{
					for(i=0;i<fertiprog[id].valvecount;i++)
					{
						curr_val = (CurTime.tm_24hr*60)+CurTime.tm_min;
						//compare_val = ((fertiprog[id].totaltime*fertiprog[id].valve[i].starttime)/100);
						compare_val = fertiprog[id].valve[i].starttime;
						compare_val = compare_val+(start_hh*60)+start_mm;

						if(compare_val > 1439)	compare_val = compare_val - 1439;

						if(curr_val >= compare_val)
						{
							//compare_val = ((fertiprog[id].totaltime*fertiprog[id].valve[i].stoptime)/100);
							compare_val = fertiprog[id].valve[i].stoptime;
							compare_val = compare_val+(start_hh*60)+start_mm;
							if(compare_val > 1439)	compare_val = compare_val - 1439;
							if(curr_val < compare_val)
							{
								if(fertivalve_onoff[i] == 0)//if fertivalve off then switch it on
								{
									ferti_cnt++;
									OP_Update((fertiprog[id].valve[i].oppartno+1),0,1);//0 is to switch on OP
									fertivalve_onoff[i]=1;
									fertivstart_log[i] = curr_val;
									if(ferti_cnt>0)		fertipump_onoff(0);//switch on ferti pump
								}
							}
							else if(curr_val >= compare_val)
							{
								if(fertivalve_onoff[i] == 1)//if fertivalve on then switch it off
								{
									ferti_cnt--;
									OP_Update((fertiprog[id].valve[i].oppartno+1),(~0),1);//~0 is to switch off OP
									fertivalve_onoff[i]=0;
									fertivstop_log[i] = curr_val;
									fertivduration_log[i] = fertivstop_log[i]-fertivstart_log[i];

									if(ferti_cnt==0)		fertipump_onoff(~0);//switch off ferti pump
								}
							}
						}
//						compare_val = ((fertiprog[id].totaltime*fertiprog[id].valve[i].stoptime)/100);
//						compare_val = compare_val+(stop_hh*60)+stop_mm;
//						if(curr_val >= compare_val)
//						{
//							if(fertivalve_onoff[i] == 1)//if fertivalve on then switch it off
//							{
//								ferti_cnt--;
//								OP_Update((fertiprog[id].valve[i].oppartno+1),(~0));//~0 is to switch off OP
//								fertivalve_onoff[i]=0;
//							}
//						}
					}
				}

				if(fertiprog[id].method == 1)// Proportional Method
				{
					for(i=0;i<fertiprog[id].valvecount;i++)
					{
						//compare_val = ((fertiprog[id].totaltime*fertiprog[id].valve[i].starttime)/100);
						compare_val = fertiprog[id].valve[i].starttime;
						compare_val = compare_val+(start_hh*60)+start_mm;

						if(compare_val > 1439)	compare_val = compare_val - 1439;

			 			temp1 = curr_val - compare_val;//time since program start
						if(curr_val >= compare_val)
						{
							//compare_val1 = ((fertiprog[id].totaltime*fertiprog[id].valve[i].stoptime)/100);
							compare_val1 = fertiprog[id].valve[i].stoptime;
							compare_val1 = compare_val1+(start_hh*60)+start_mm;

							if(compare_val1 > 1439)	compare_val1 = compare_val1 - 1439;

							if(curr_val < compare_val1)
							{
								temp = ((compare_val-compare_val1)/(2*fertiprog[id].propcycles));// total time divided into 2x parts

								if(((temp1/temp)%2)==0)
								{
									if(fertivalve_onoff[i] == 0)//if fertivalve off then switch it on
									{
										ferti_cnt++;
										OP_Update((fertiprog[id].valve[i].oppartno+1),0,1);//0 is to switch on OP
										fertivalve_onoff[i]=1;
										fertivstart_log[i] = curr_val;
									}
								}
								if(((temp1/temp)%2)==1)
								{
									if(fertivalve_onoff[i] == 1)//if fertivalve on then switch it off
									{
										ferti_cnt--;
										OP_Update((fertiprog[id].valve[i].oppartno+1),(~0),1);//0 is to switch on OP
										fertivalve_onoff[i]=0;
										fertivstop_log[i] = curr_val;
										fertivduration_log[i] = fertivduration_log[i] +(fertivstop_log[i]-fertivstart_log[i]);
									}
								}
							}
						}
						//compare_val = ((fertiprog[id].totaltime*fertiprog[id].valve[i].stoptime)/100);
						compare_val = fertiprog[id].valve[i].stoptime;
						compare_val = compare_val+(start_hh*60)+start_mm;
						if(compare_val > 1439)	compare_val = compare_val - 1439;
						if(curr_val >= compare_val)
						{
							if(fertivalve_onoff[i] == 1)//if fertivalve on then switch it off
							{
								ferti_cnt--;
								OP_Update((fertiprog[id].valve[i].oppartno+1),(~0),1);//~0 is to switch off OP
								fertivalve_onoff[i]=0;
							}
						}
					}
				}
 			}
		}
 		compare_val = (stop_hh*60)+stop_mm;
 		if(compare_val > 1439)	compare_val = compare_val - 1439;
 		if(compare_val <= curr_val)//end fertigation prog
 		{
 			for(i=0;i<fertiprog[id].valvecount;i++)
 			{
 				if(fertivalve_onoff[i] == 1)//if fertivalve on then switch it off
				{
					ferti_cnt--;
					OP_Update((fertiprog[id].valve[i].oppartno+1),(~0),1);//~0 is to switch off OP
					fertivalve_onoff[i]=0;
					fertivstop_log[i] = curr_val;
					fertivduration_log[i] = fertivstop_log[i]-fertivstart_log[i];

					if(ferti_cnt==0)		fertipump_onoff(~0);//switch off ferti pump
				}
 			}
 			ferti_cnt=0;
 		}
	}
}





/********************************************************************/
// Function used for remaintime Updation
// Call this function on reset/at 00:00 Hrs everyday
/********************************************************************/
void Remaintime_update()
{
	int x = 0;
	// Reset all power interruption program time shifing parameters
	for(x=0;x<=5;x++)
	{
		remtim[x] = 0;//remtim must remain 0 unless program is supposed to run today
	}
	interrupted_prog = 0;
	shift = 0;


	//update the remaining time of the
	for(x=0;x<=5;x++)
	{
		if(zoneprog[x].status==0)//check if program enabled
		{
			continue;
		}
		switch(zoneprog[x].rundaysmode)// check if program supposed to run today
		{
			case 0: // 0=daywise
				if(zoneprog[x].rundaysval[zoneprog[x].rundaysmode]&(0x01<<(CurTime.tm_wday)))
				{
					break;
				}
				else
				{
					//return;
					continue;
				}
			case 1: // 1=oddeven
				if(((CurTime.tm_wday+1)%2)==zoneprog[x].rundaysval[zoneprog[x].rundaysmode])
				{
					break;
				}
				else
				{
					//return;
					continue;
				}
			case 2: // 2=custom
				if((CurTime.tm_wday%zoneprog[x].rundaysval[zoneprog[x].rundaysmode])==0)
				{
					break;
				}
				else
				{
					//return;
					continue;
				}
			default:
				break;
		 }

		remtim[x] =  (zoneprog[x].cycledelay * zoneprog[x].repeatcycles);
		remtim[x] = remtim[x] + ((zoneprog[x].totaltime[1]+(60*zoneprog[x].totaltime[0]))*(zoneprog[x].repeatcycles+1));
	}
	eeprom_write_progstatus(0xff);
	eeprom_read_progstatus(0xff);
}

void Remaintime_clear(uint8_t x)//x is the prog no
{
	remtim[x] = 0;
	if((interrupted_prog-1)== x)	interrupted_prog=0;
	eeprom_write_progstatus(0xFF);
	eeprom_read_progstatus(0xFF);
}

void Remaintime_update1(uint8_t x)//x is the prog no
{

	if(zoneprog[x].status==0)//check if program enabled
	{
		return;
	}
	switch(zoneprog[x].rundaysmode)// check if program supposed to run today
	{
		case 0: // 0=daywise
			if(zoneprog[x].rundaysval[zoneprog[x].rundaysmode]&(0x01<<(CurTime.tm_wday)))
			{
				break;
			}
			else
			{
				//return;
				return;
			}
		case 1: // 1=oddeven
			if(((CurTime.tm_wday+1)%2)==zoneprog[x].rundaysval[zoneprog[x].rundaysmode])
			{
				break;
			}
			else
			{
				//return;
				return;
			}
		case 2: // 2=custom
			if((CurTime.tm_wday%zoneprog[x].rundaysval[zoneprog[x].rundaysmode])==0)
			{
				break;
			}
			else
			{
				//return;
				return;
			}
		default:
			break;
	 }

	remtim[x] =  (zoneprog[x].cycledelay * zoneprog[x].repeatcycles);
	remtim[x] = remtim[x] + ((zoneprog[x].totaltime[1]+(60*zoneprog[x].totaltime[0]))*(zoneprog[x].repeatcycles+1));

	eeprom_write_progstatus(0xFF);
	eeprom_read_progstatus(0xFF);
}

/********************************************************************/
// Function used for Starttime Shift calculation in case of Power
// Failure
// To be called at power return after a power failure or on reset
/********************************************************************/
void Starttime_Shift()
{
	int x=0,y=0;
	int curr_val=0,cmp_val=0,cmp_val1=0,shifted_stop=0;
	curr_val = (CurTime.tm_24hr*60)+CurTime.tm_min;

	if(interrupted_prog != 0)//Check if any program was interrupted
	{
		y = interrupted_prog - 1;
		cmp_val = ((zoneprog[y].totaltime[1]+(60*zoneprog[y].totaltime[0]))*(zoneprog[y].repeatcycles+1)) + (zoneprog[y].cycledelay*zoneprog[y].repeatcycles);
		cmp_val1 = (zoneprog[y].starttime[1]+(60*zoneprog[y].starttime[0]));
		shift = cmp_val;
		shift = shift - remtim[y];
		shift = curr_val -shift ;
		shift = shift - cmp_val1 ;//all start times are to be shifted forward by this amount
	}
	else//Check if any start time was missed
	{
		for(x=0;x<=5;x++)
		{
			if(remtim[x] != 0)//Check if program is not already executed
			{
				cmp_val1 = (zoneprog[x].starttime[1]+(60*zoneprog[x].starttime[0]));//1st program start time
				break;
			}
		}
		y=0;
		//Get the earliest start that has not been executed.
		for(x=0;x<=5;x++)
		{
			if(remtim[x] != 0)//Check if program is not already executed
			{
				cmp_val = (zoneprog[x].starttime[1]+(60*zoneprog[x].starttime[0]));
				if(cmp_val1 >= cmp_val)
				{
					cmp_val1 = cmp_val;
					y=x;
				}
			}
		}

		if(remtim[y] != 0)//Check if program is not already executed
		{
			if(curr_val > cmp_val1)
			{
				shift = curr_val - cmp_val1;
			}
		}
		else
		{
			shift=0;
		}
	}

	if(remtim[y] != 0)//Check if program is not already executed
	{
		shifted_stop = shift + (zoneprog[y].starttime[1]+(60*zoneprog[y].starttime[0]));
		shifted_stop = shifted_stop + ((zoneprog[y].totaltime[1]+(60*zoneprog[y].totaltime[0]))*(zoneprog[y].repeatcycles+1));
		shifted_stop = shifted_stop + (zoneprog[y].cycledelay*zoneprog[y].repeatcycles);
	}

	if(shifted_stop >1439)//shifted stop is >23:59
	{
		shifted_stop = shifted_stop - 1439;
		shift = shift - shifted_stop;
	}
	//Check if the shifted time clashes with another start time reduce the shift or is >23:59
	for(x=0;x<=5;x++)
	{
		if((remtim[x] != 0)&&(x != y))//Check if program is not already executed
		{
			cmp_val = (zoneprog[x].starttime[1] + (60*zoneprog[x].starttime[0]));
			if(shifted_stop > cmp_val)
			{
				shifted_stop = shifted_stop - cmp_val;
				shift = shift - shifted_stop;

				if(shift < 0)		shift = 0;
			}
		}
	}
}


void Backflush_program()
{
	int i=0;
	char noofbfval=0;
	int bf_valve_opno[4]={0};


	if(bf_1min > ((backflushinterval[0]*60)+backflushinterval[1]+backflushduration+backflushdelay))
	{
		bf_1min=0;//ie if value out of range
	}


	if(backflushstatus == 0)//backflush disabled
	{
		bf_1min=0;
		return;
	}

	if((backflushduration==0)&&(((backflushinterval[0]*60)+backflushinterval[1])==0))	return;

	for(i=0;i<16;i++)
	{
		if(output[i].optype==2)//if output is a flush valve
		{
			if(noofbfval< output[i].typeid)		noofbfval = output[i].typeid;
			bf_valve_opno[(output[i].typeid-1)] = i;
		}
	}


	if(((backflushinterval[0]*60)+backflushinterval[1]) == bf_1min)
	{
		if(backflushon == 0)
		{
			 backflushon=1;
			 bf_1min = 0;
		}
	}

	if(backflushon == 1)
	{
		if(bf_valonoff[bf_valno] == 0)
		{
			bf_valonoff[bf_valno] = 1;
			OP_Update((bf_valve_opno[bf_valno]+1),0,1);//switch on output
			bf_1min = 0;
		}
		if(bf_1min == backflushduration)
		{
			if((bf_valonoff[bf_valno] == 1) && (output_state[bf_valve_opno[bf_valno]]==1))
			{
				OP_Update((bf_valve_opno[bf_valno]+1),(~0),1);//switch off output
			}
		}
		if(bf_1min == (backflushduration+backflushdelay))
		{
			if(bf_valonoff[bf_valno] == 1)
			{
				bf_valonoff[bf_valno] = 0;
				bf_valno++;
			}
			if((bf_valno+1) > noofbfval)
			{
				bf_valno=0;
				backflushon=0;
				bf_1min = 0;
			}
		}
	}
}



void nextprogstart()
{
	int curr_val=0,cmp_val1=0,cmp_val=0;
	char x=0;

	nextprog_time[0]=24;//load a invalid value

	curr_val = (CurTime.tm_24hr*60)+CurTime.tm_min;

	for(x=0;x<=5;x++)// find out a start time valid today and greater than current time
	{
		cmp_val = (zoneprog[x].starttime[1]+(60*zoneprog[x].starttime[0]));
		if(remtim[x] != 0)//Check if program is not already executed
		{
			if(curr_val <= cmp_val)
			{
				cmp_val1 = (zoneprog[x].starttime[1]+(60*zoneprog[x].starttime[0]));//program start time
				break;
			}
		}
	}

	//Get the earliest start that has not been executed.
	for(x=0;x<=5;x++)
	{
		if(remtim[x] != 0)//Check if program is not already executed
		{
			cmp_val = (zoneprog[x].starttime[1]+(60*zoneprog[x].starttime[0]));
			if((cmp_val1 >= cmp_val)&&(curr_val <= cmp_val))
			{
				cmp_val1 = cmp_val;
				nextprog_time[0] = zoneprog[x].starttime[0];
				nextprog_time[1] = zoneprog[x].starttime[1];

			}
		}
	}


}
