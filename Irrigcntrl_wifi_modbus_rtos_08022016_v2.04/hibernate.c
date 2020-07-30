//*****************************************************************************
//
// hibernate.c - Hibernation Example.
//
// This is part of revision 2.1.0.12573 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "inc/hw_gpio.h"
#include "inc/hw_hibernate.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
//#include "driverlib/debug.h"
//#include "driverlib/gpio.h"
#include "driverlib/hibernate.h"
//#include "driverlib/interrupt.h"
//#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
//#include "driverlib/uart.h"
//#include "driverlib/systick.h"
#include "utils/ustdlib.h"
//#include "utils/uartstdio.h"
#include "utils/cmdline.h"
#include <xdc/runtime/System.h>

#include "database_struct.h"

//*****************************************************************************
/*
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
*/
//*****************************************************************************
//! <h1>RTC Example (hibernate)</h1>
//!
//! The program displays wall time and date by making use of calendar function
//! of the Hibernate module.  User can modify the date and time if so desired.
//*****************************************************************************

bool ReadDateTime(struct tm *sTime);
void WriteDateTime(userdef_tm WriteTime);

void SetDefaultDateTime(userdef_tm *DfltTime);

uint8_t GetDaysInMonth(uint8_t ui8Year, uint8_t ui8Mon);
//uint8_t ComputeDayOfWeek(uint8_t month, uint8_t day, uint8_t year);
//uint8_t ComputeDayOfWeek(uint16_t month, uint16_t day, uint16_t year)
uint8_t ComputeDayOfWeek(userdef_tm WriteTime);
bool ValidateDateTime(userdef_tm ValidTime);

void rtc_init (void);
bool GetDateTime(void);
//bool SetDateTime(userdef_tm NewTime);
void get_updated_datetime (void);

//*****************************************************************************
// Variables that keep track of the date and time.
//*****************************************************************************

extern volatile bool CheckRTC;
extern volatile bool rtc_update;

extern char dateformat;
extern char disp_clockformat;

//extern char disp_rtcdaysofweek[4];
//extern char rtctime[9], disp_rtctime[6], rtctimeslot;
//extern char rtcdate[11], disp_rtcdate[9];

//*****************************************************************************
// Flag that informs that date and time have to be set.
//*****************************************************************************
volatile bool g_bSetDefaultDate = false;//,  g_bSetDate = false;
volatile uint8_t g_bSetDate = 0x00;
static uint8_t ui8SecsPrev = 0xFF, ui8MinsPrev = 0xFF;		//$$NR$$//dated:16Aug15

userdef_tm CurTime, SetTime;
//struct ud_time CurTime, SetTime;

//*****************************************************************************
// The error routine that is called if the driver library encounters an error.
//*****************************************************************************

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
// This function reads the current date and time from the calendar logic of the
// hibernate module.  Return status indicates the validity of the data read.
// If the received data is valid, the 24-hour time format is converted to
// 12-hour format.
//*****************************************************************************

bool ReadDateTime(struct tm *sTime)
{
uint8_t ui8MonthDays;

	// Get the latest time.
	HibernateCalendarGet(sTime);	//(&sTime);

    // Is valid data read?
	if(((sTime->tm_hour < 0) || (sTime->tm_hour > 23)) ||
	   ((sTime->tm_min < 0) || (sTime->tm_min > 59)) ||
       ((sTime->tm_sec < 0) || (sTime->tm_sec > 59)) ||
       ((sTime->tm_mday < 1) || (sTime->tm_mday > 31)) ||
       ((sTime->tm_mon < 0) || (sTime->tm_mon > 11)) ||
       ((sTime->tm_year < 100) || (sTime->tm_year > 199))// ||
//       ((sTime->tm_wday < 0) || (sTime->tm_wday > 6))	//$$NR$$//dated:19Oct15
       )
	{
        // No - Let the application know the same by returning relevant message.
        return false;
    }

    // Since different months have varying number of days, get the number of
    // days for the current month and year.
    ui8MonthDays = GetDaysInMonth((uint8_t)sTime->tm_year, (uint8_t)sTime->tm_mon);

    if(((uint8_t)sTime->tm_mday) > ui8MonthDays)		return false;

    // Return that new data is available so that it can be displayed.
    return true;
}

//*****************************************************************************
// This function writes the requested date and time to the calendar logic of
// hibernation module.
//*****************************************************************************

//void WriteDateTime(void)
void WriteDateTime(userdef_tm WriteTime)
{
struct tm sTime;
uint8_t	tmp_wday;

	// Get the latest date and time.  This is done here so that unchanged
	// parts of date and time can be written back as is.
	HibernateCalendarGet(&sTime);

	// Set the date and time values that are to be updated.
	sTime.tm_hour	= (int)WriteTime.tm_hour;		//g_ui32HourIdx;
    if ((disp_clockformat == 0) && (WriteTime.tm_slot) && (sTime.tm_hour < 12))  	sTime.tm_hour	+= 12;		//g_ui32HourIdx;

	sTime.tm_min	= (int)WriteTime.tm_min;		//g_ui32MinIdx;
	sTime.tm_mday	= (int)WriteTime.tm_mday;		//g_ui32DayIdx;
	sTime.tm_mon	= (int)(WriteTime.tm_mon - 1);	//g_ui32MonthIdx-1;
	sTime.tm_year	= (int)(WriteTime.tm_year + 100);//100 + g_ui32YearIdx;

	tmp_wday = ComputeDayOfWeek(WriteTime);
	sTime.tm_wday	= (int)(tmp_wday);

	// Update the calendar logic of hibernation module with the requested data.
	HibernateCalendarSet(&sTime);
}

//*****************************************************************************
// This function sets time to the default system time (23:58:00) and
// date to the default system date (31/12/2014).
//*****************************************************************************

void SetDefaultDateTime(userdef_tm *DfltTime)
{
	// Set default date and time valuee.
	DfltTime->tm_mday	= 31;
	DfltTime->tm_mon	= 12;
	DfltTime->tm_year	= 14;

	DfltTime->tm_slot	= 1;

	DfltTime->tm_hour	= 11;
	//add, if 24 hours clock format
    if (disp_clockformat == 1)	DfltTime->tm_hour	+= 12;

	DfltTime->tm_min	= 58;
}

//*****************************************************************************
// This function returns the number of days in a month including for a leap year.
//*****************************************************************************

uint8_t GetDaysInMonth(uint8_t ui8Year, uint8_t ui8Mon)
{
    // Return the number of days based on the month.
    if(ui8Mon == 1)
    {
        // For February return the number of days based on the year being a
        // leap year or not.
        if((ui8Year % 4) == 0)
        {
            // If leap year return 29.
            return 29;
        }
        else
        {
            // If not leap year return 28.
            return 28;
        }
    }
    else if((ui8Mon == 3) || (ui8Mon == 5) || (ui8Mon == 8) || (ui8Mon == 10))
    {
        // For April, June, September and November return 30.
        return 30;
    }

    // For all the other months return 31.
    return 31;
}

//*****************************************************************************
// This function returns the day of week.
//*****************************************************************************

// Computes the Day of the week and returns a number representing it
// Sunday = 1, Monday = 2, Tuesday = 3...
// Sunday = 0, Monday = 1, Tuesday = 2...

//uint8_t ComputeDayOfWeek(uint8_t month, uint8_t day, uint8_t year)
//uint8_t ComputeDayOfWeek(uint16_t month, uint16_t day, uint16_t year)
/*
uint8_t ComputeDayOfWeek(userdef_tm WriteTime)
{
uint16_t month, day, year;
uint32_t day_of_week;

	month	= WriteTime.tm_mon;
	day		= WriteTime.tm_mday;
	year	= WriteTime.tm_year + 2000;

	if (month < 3)
	{
		month += 12;
		--year;
	}

	day_of_week = day + (13 * month - 27) / 5 + year + year/4 - year/100 + year/400;

//	return ((uint8_t)(day_of_week % 7) + 1);
	return ((uint8_t)(day_of_week % 7));
}
*/
//*****************************************************************************
// This function returns the day of week.
//*****************************************************************************

//unsigned char computeDayOfWeek(int y, int m, int d)
uint8_t ComputeDayOfWeek(userdef_tm WriteTime)
{
int month, day, year;
int h, j, k;

month	= WriteTime.tm_mon;
day		= WriteTime.tm_mday;
year	= WriteTime.tm_year + 2000;

	//January and February are counted as months 13 and 14 of the previous year
	if(month <= 2)
	{
		month += 12;
		year -= 1;
	}

	//J is the century
	j = year / 100;
	//K the year of the century
	k = year % 100;

	//Compute H using Zeller's congruence
	h = day + (26 * (month + 1) / 10) + k + (k / 4) + (5 * j) + (j / 4);

	//Return the day of the week
//	return ((h + 5) % 7) + 1;	//$$NR$$//dated:19Oct15
	return ((h + 6) % 7);		//$$NR$$//dated:19Oct15
}


//int main1(void)
//{
//unsigned char tmpday;
//	tmpday = computeDayOfWeek(2015, 1, 1);
//
//	System_printf("Day = %d", tmpday);
//	/* SysMin will only print to the console when you call flush or exit */
//	System_flush();
//}

//*****************************************************************************
// This function validates the date and time.
//*****************************************************************************

bool ValidateDateTime(userdef_tm ValidTime)
{
uint8_t ui8MonthDays;

    // Is valid data read?
	if((ValidTime.tm_hour > 23) || (ValidTime.tm_min > 59) || (ValidTime.tm_sec > 59) ||
       (ValidTime.tm_mday > 31) || (ValidTime.tm_mon > 12) || (ValidTime.tm_year > 99))
    {
        // No - Let the application know the same by returning relevant message.
        return false;
    }

//  if ((disp_clockformat == 0) && (((rtctimeslot == 0) && (ValidTime.tm_hour > 11)) || ((rtctimeslot == 1) && (ValidTime.tm_hour > 12))))		return false;
    if ((disp_clockformat == 0) && (((ValidTime.tm_slot == 0) && (ValidTime.tm_hour > 11)) || ((ValidTime.tm_slot == 1) && (ValidTime.tm_hour > 12))))		return false;

    // Since different months have varying number of days, get the number of
    // days for the current month and year.
    ui8MonthDays = GetDaysInMonth(ValidTime.tm_year, ValidTime.tm_mon);

    if(ValidTime.tm_mday > ui8MonthDays)		return false;

    // Return that new data is available so that it can be displayed.
    return true;
}

//*****************************************************************************
// This function converts time from string to decimal format.
//*****************************************************************************

bool DateTimeStrToDec(char *temp_str, uint8_t type)
{
uint8_t temp_data[3], ui8MonthDays;

	if (type == 1)			// time
	{
		temp_data[0]	= (*temp_str - '0')*10;		temp_str++;			//hh
		temp_data[0]	+= (*temp_str - '0');		temp_str+=2;
		temp_data[1]	= (*temp_str - '0')*10;		temp_str++;			//mm
		temp_data[1]	+= (*temp_str - '0');		//temp_str+=2;
		temp_data[2]	= 0;

	    // Is valid data?
		if((temp_data[0] > 23) || (temp_data[1] > 59) || (temp_data[2] > 59))       return false;

	    if ((disp_clockformat == 0) && (temp_data[0] > 12))		return false;

	    // set valid time
	    SetTime.tm_hour	= temp_data[0];
	    SetTime.tm_min	= temp_data[1];
	    SetTime.tm_sec	= temp_data[2];

	    // Return that new data is available so that it can be displayed.
	    return true;
	}
	else if (type == 2)		// date
	{
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

	    // set valid time
	    SetTime.tm_mday	= temp_data[0];
	    SetTime.tm_mon	= temp_data[1];
	    SetTime.tm_year	= temp_data[2];

	    // Return that new data is available so that it can be displayed.
	    return true;
	}

	return false;
}

//*****************************************************************************
// This example also demonstrates the RTC
// calendar function that keeps track of date and time.
//*****************************************************************************

void rtc_init (void)
{
uint32_t ui32SysClock=120000000, ui32Status, ui32HibernateCount;

    // Enable the hibernate module.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);

    // Initialize these variables before they are used.
    ui32Status = 0;
    ui32HibernateCount = 0;

    // Check to see if Hibernation module is already active, which could mean
    // that the processor is waking from a hibernation.
    if(HibernateIsActive())
    {
        // Read the status bits to see what caused the wake.  Clear the wake
        // source so that the device can be put into hibernation again.
        ui32Status = HibernateIntStatus(0);
        HibernateIntClear(ui32Status);
    }

    // Configure Hibernate module clock.
    HibernateEnableExpClk(ui32SysClock);

    // Store the hibernation count message into the respective char buffer.
//  usnprintf(g_pcHibBuf, sizeof(g_pcHibBuf), "Hibernate count = %u", ui32HibernateCount);

    // Enable RTC mode.
    HibernateRTCEnable();

    // Configure the hibernate module counter to 24-hour calendar mode.
    HibernateCounterMode(HIBERNATE_COUNTER_24HR);

    // If hibernation count is very large, it may be that there was already
    // a value in the hibernate memory, so reset the count.
    ui32HibernateCount = (ui32HibernateCount > 10000) ? 0 : ui32HibernateCount;

//    rtc_update = true;

//    get_datetime_update();
}

//*****************************************************************************
// This function formats valid new date and time to be displayed. It also
// indicates if valid new data is available or not.  If date and time is
// invalid, this function sets the date and time to default value.
//*****************************************************************************

bool GetDateTime(void)
{
	//static uint8_t ui8SecsPrev = 0xFF, ui8MinsPrev = 0xFF;	//$$NR$$//dated:16Aug15
    static uint8_t homescrns_refresh_timecnt = 10;  //sec       //$$NR$$//dated:08Feb16
	struct tm sTime;

//	if (rtc_update==false)	return;
//
//	rtc_update = false;

    // Get the latest date and time and check the validity.
    if(ReadDateTime(&sTime) == false)
    {
//    	userdef_tm DefaultTime;
//		// Set the date to the default values.
//		SetDefaultDateTime(&DefaultTime);
//		WriteDateTime(DefaultTime);

    	g_bSetDefaultDate = true;
    	return false;
    }

    // If date and time is valid, check if seconds have updated from previous visit.
    // No - Return false to indicate no information to display.
    if(ui8SecsPrev == (uint8_t)sTime.tm_sec)		return false;

    // If valid new date and time is available, update a local variable to keep
    // track of seconds to determine new data for next visit.
    ui8SecsPrev = (uint8_t)sTime.tm_sec;

    // Get the time (sec) value that are to be updated.
    CurTime.tm_sec	= (uint8_t)sTime.tm_sec;

    //$$NR$$//dated:08Feb16
    // set flag for date and time update on display
    homescrns_refresh_timecnt--;
    if (homescrns_refresh_timecnt == 0)
    {
        CheckRTC = 1;
        homescrns_refresh_timecnt = 10;
    }
    //$$NR$$//dated:08Feb16

    // If date and time is valid, check if minutes have updated from previous visit.
    // No - Return false to indicate no information to display.
    if(ui8MinsPrev == (uint8_t)sTime.tm_min)		return true;

    // If valid new date and time is available, update a local variable to keep
    // track of minutes to determine new data for next visit.
    ui8MinsPrev = (uint8_t)sTime.tm_min;

    // Get the date and time values that are to be updated.
    CurTime.tm_24hr	= (uint8_t)sTime.tm_hour;
    CurTime.tm_hour	= (uint8_t)sTime.tm_hour;
	CurTime.tm_min	= (uint8_t)sTime.tm_min;
	CurTime.tm_mday	= (uint8_t)sTime.tm_mday;
	CurTime.tm_mon	= (uint8_t)(sTime.tm_mon + 1);
	CurTime.tm_year	= (uint8_t)(sTime.tm_year - 100);
	CurTime.tm_wday	= (uint8_t)sTime.tm_wday;

	CurTime.tm_slot = 0;
    if (disp_clockformat == 0)
    {
//    	CurTime.tm_slot = 1;								//rtctimeslot
//   	if (CurTime.tm_hour < 12)	CurTime.tm_slot = 0;	//rtctimeslot
    	if (CurTime.tm_hour >= 12)	CurTime.tm_slot = 1;	//rtctimeslot
    	if (CurTime.tm_hour > 12)	CurTime.tm_hour -= 12;	//ui32tempHrs
    }

//    strcpy(disp_rtcdaysofweek, g_ppcDay[sTime.tm_wday]);

    //$$NR$$//dated:08Feb16
//    // set flag for date and time update on display
//    CheckRTC = 1;
    //$$NR$$//dated:08Feb16

	// Return true to indicate new information to display.
    return true;
}

//*****************************************************************************
// This function formats valid new date and time to be displayed. It also
// indicates if valid new data is available or not.  If date and time is
// invalid, this function sets the date and time to default value.
//*****************************************************************************
/*
bool SetDateTime(userdef_tm NewTime)
{

//	if (rtc_update==false)	return;
//
//	rtc_update = false;

	// Set the date to the default values.
	WriteDateTime(NewTime);

	// Return true to indicate new information to display.
    return true;
}
*/
//*****************************************************************************
//
//*****************************************************************************

void get_updated_datetime (void)
{
//bool bUpdate;

	if (g_bSetDate)
	{
		// Clear the flag.
		g_bSetDate = 0x00;//false;

		// Set the date to the default values.
		WriteDateTime(SetTime);

		// clear flag of set date & time to default
		g_bSetDefaultDate = false;

		//$$NR$$//dated:16Aug15
//		1. set rtc_update
//		2. set ui8SecsPrev = 0xFF, ui8MinsPrev = 0xFF; (then no need to set CheckRTC = 1;)

		rtc_update = true;
		ui8SecsPrev = 0xFF;		ui8MinsPrev = 0xFF;
		//$$NR$$//dated:16Aug15
	}


	if (rtc_update==false)	return;

	rtc_update = false;

	// Check the flag which indicates that an invalid time is in hibernate
	// module.  If disp then force setting to the default time.
	if(g_bSetDefaultDate)
	{
    	userdef_tm DefaultTime;

		// Set the date to the default values.
		SetDefaultDateTime(&DefaultTime);
		WriteDateTime(DefaultTime);

		// Clear the flag.
		g_bSetDefaultDate = false;
	}
//	else if(g_bSetDate)
//	{
//		// Clear the flag.
//		g_bSetDate = false;
//
//		// Set the date to the default values.
//		WriteDateTime(SetTime);
//	}

	// Update the buffer that displays date and time on the main screen.
//	bUpdate = GetDateTime();
	GetDateTime();

	// Is a new value of date and time available to be displayed?
//	if(bUpdate == true)
//	{
//		System_printf("The current date and time is: \n");
//		System_printf("\tDate: %02d/%02d/%02d	", CurTime.tm_mday, CurTime.tm_mon, CurTime.tm_year);
//		System_printf("\tTime: %02d:%02d:%02d ", CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec);
//
//		if (disp_clockformat == 0)	System_printf((CurTime.tm_slot ? " PM":" AM"));
////		System_printf("	State = %d\n", g_bSetDate);
//		System_printf("\n", g_bSetDate);
//
//		/* SysMin will only print to the console when you call flush or exit */
//		System_flush();
//	}
}

//*****************************************************************************
// This function updates individual buffers with valid date and time to be
// displayed on the date screen so that the date and time can be updated.
//*****************************************************************************
/*
bool DateTimeUpdateGet(void)
{
    struct tm sTime;

    // Get the latest date and time and check the validity.
    if(ReadDateTime(&sTime) == false)
    {
        // Invalid - Return here with false as no information to update.  So
        // use default values.
        SetDefaultDateTime();
        return false;
    }

    // If date and time is valid, copy the date and time values into respective indexes.
    g_ui32MonthIdx = sTime.tm_mon;
    g_ui32DayIdx = sTime.tm_mday;
    g_ui32YearIdx = sTime.tm_year - 100;
    g_ui32HourIdx = sTime.tm_hour;
    g_ui32MinIdx = sTime.tm_min;

    // Return true to indicate new information has been updated.
    return true;
}
*/
//*****************************************************************************
// This function returns the date and time value that is written to the
// calendar match register.  5 seconds are added to the current time.  Any
// side-effects due to this addition are handled here.
//*****************************************************************************
/*
void GetCalendarMatchValue(struct tm* sTime)
{
    uint32_t ui32MonthDays;

    // Get the current date and time and add 5 secs to it.
    HibernateCalendarGet(sTime);
    sTime->tm_sec += 5;

    // Check if seconds is out of bounds.  If so subtract seconds by 60 and
    // increment minutes.
    if(sTime->tm_sec > 59)
    {
        sTime->tm_sec -= 60;
        sTime->tm_min++;
    }

    //
    // Check if minutes is out of bounds.  If so subtract minutes by 60 and
    // increment hours.
    //
    if(sTime->tm_min > 59)
    {
        sTime->tm_min -= 60;
        sTime->tm_hour++;
    }

    //
    // Check if hours is out of bounds.  If so subtract minutes by 24 and
    // increment days.
    //
    if(sTime->tm_hour > 23)
    {
        sTime->tm_hour -= 24;
        sTime->tm_mday++;
    }

    //
    // Since different months have varying number of days, get the number of
    // days for the current month and year.
    //
    ui32MonthDays = GetDaysInMonth(sTime->tm_year, sTime->tm_mon);

    // Check if days is out of bounds for the current month and year.  If so
    // subtract days by the number of days in the current month and increment
    // months.
    if(sTime->tm_mday > ui32MonthDays)
    {
        sTime->tm_mday -= ui32MonthDays;
        sTime->tm_mon++;
    }

    // Check if months is out of bounds.  If so subtract months by 11 and
    // increment years.
    if(sTime->tm_mon > 11)
    {
        sTime->tm_mon -= 11;
        sTime->tm_year++;
    }

    // Check if years is out of bounds.  If so subtract years by 100.
    if(sTime->tm_year > 99)
    {
        sTime->tm_year -= 100;
    }
}
*/

