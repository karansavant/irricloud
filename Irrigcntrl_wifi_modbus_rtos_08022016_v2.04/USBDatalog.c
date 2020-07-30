
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SDSPI.h>
#include <ti/drivers/USBMSCHFatFs.h>

/* Example/Board Header files */
#include "Board.h"
#include "database_struct.h"
#include "MenuDefs.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

extern void alert_status_update(uint8_t index, uint8_t state);

/* Global Variables*/

extern uint8_t log_buffer[1000];
extern userdef_tm CurTime;
extern	char ouput_buffer[20000];

/* Buffer size used for the file copy process */
#define LOGFILE_BUFF_SIZE       20000

/* String conversion macro */
#define STR_(n)             #n
#define STR(n)              STR_(n)

/* Drive number used for FatFs */
#define USB_DRIVE_NUM          1

char usb_status=0; //0=not working 1=working
char fileusb[50] = {0};

char logfile_buff[LOGFILE_BUFF_SIZE]={0};
uint16_t logfile_size, logfile_rddata_size=0;

void Write_log();
uint8_t Read_log(uint16_t read_startloc);
void Read_log_portal();
void USBlog_init();


/*
 * 	USBlog_init()
 *  Initialize and mount the usb pendrive
 */
void USBlog_init()
{
	USBMSCHFatFs_Handle usbmschfatfsHandle;
	USBMSCHFatFs_Params usbmschfatfsParams;

	/* Mount and register the USB Drive */
	USBMSCHFatFs_Params_init(&usbmschfatfsParams);
	usbmschfatfsParams.servicePriority = 1;//1;
	usbmschfatfsHandle = USBMSCHFatFs_open(Board_USBMSCHFatFs0,
										   USB_DRIVE_NUM,
										  &usbmschfatfsParams);
	if (usbmschfatfsHandle == NULL) {
		//System_abort("Error starting the USB Drive\n");
		 usb_status=0;// error
	}
	else {
		//System_printf("Drive %u is mounted\n", USB_DRIVE_NUM);
		 usb_status=1;
	}

	/* Need to block until a USB Drive has been enumerated */
	if(usb_status == 1)
	{
		if (!USBMSCHFatFs_waitForConnect(usbmschfatfsHandle, 10000)) {
			//System_abort("No USB drive present, aborting...\n");
			 usb_status=0;// No usb drive
		}
	}
	if( usb_status==0)
	{
		//Raise alert that usb is not functioning
		alert_status_update(USB_Error,1);
	}
}


/*
 * 	Write_log()
 *  Write data from log buffer to the USB Mass Storage device
 */
void Write_log()
{
	/* Variables for the CIO functions */
	FILE *src ;

	/* Variables to keep track of the file copy progress */
//	unsigned int bytesRead = 0;
//	unsigned int bytesWritten = 0;
//	unsigned int filesize;
//	unsigned int totalBytesCopied = 0;
	if(usb_status == 0)//usb not active
		return;

	sprintf (fileusb,"fat:"STR(USB_DRIVE_NUM)":%d-%d-%d.csv",CurTime.tm_mday,(CurTime.tm_mon),(CurTime.tm_year));
	/* Try to open the source file */

	////System_printf("Create a new file if not present \"%s\"...", fileusb);

	/* Open file for appending */
	src = fopen(fileusb, "a+");
	if ( !src ) {
		//System_printf("Error: \"%s\" could not be created or opened\n",
		//		fileusb);
	//	System_abort("Aborting...\n");
	}

	fwrite(log_buffer, 1,strlen( (const char *)log_buffer), src);
	memset(log_buffer,0, sizeof(log_buffer));
	fflush(src);

	//System_printf("done\n");

	/* Get the filesize of the source file */
	//fseek(src, 0, SEEK_END);
	//filesize = ftell(src);
	//rewind(src);

	/* Close file */
	fclose(src);
}

//$$NR$$//dated:08Nov15

/*************************************************************/
//  Write test file to the USB Mass Storage device
/*************************************************************/

uint8_t usb_test_write(char *write_data)
{
// Variables for the CIO functions
FILE *src;
//uint16_t testfile_size;

	// Variables to keep track of the file copy progress
//	unsigned int bytesRead = 0;
//	unsigned int bytesWritten = 0;
//	unsigned int filesize;
//	unsigned int totalBytesCopied = 0;

	// usb not active
	if(usb_status == 0)	return (1);

//	sprintf (fileusb,"fat:"STR(USB_DRIVE_NUM)":%d-%d-%d.csv",CurTime.tm_mday,(CurTime.tm_mon),(CurTime.tm_year));
	sprintf (fileusb,"fat:"STR(USB_DRIVE_NUM)":usb_test.txt");

	// Try to open the source file
//	System_printf("Create a new file if not present \"%s\"...", fileusb);

	// Open file for appending
	src = fopen(fileusb, "w");
	if ( !src ) {
//		System_printf("Error: \"%s\" could not be created or opened\n",	fileusb);
//		System_abort("Aborting...\n");
		return (2);
	}

	fwrite(write_data, 1, strlen( (const char *)write_data), src);
//	memset(write_data, 0, sizeof(write_data));
//	memset(write_data, 0, strlen(write_data));
	fflush(src);

	//System_printf("done\n");

	// Get the filesize of the source file
	fseek(src, 0, SEEK_END);
	//testfile_size = ftell(src);
	rewind(src);

	// Close file
	fclose(src);

	//check file size
//	if (testfile_size != sizeof(write_data))	return (3);
//	if (testfile_size != strlen(write_data))	return (3);

	return (0);
}


/*************************************************************/
//  Read test file from the USB Mass Storage device
/*************************************************************/

uint8_t usb_test_read(char *read_data)
{
// Variables for the CIO functions
FILE *src ;
uint16_t testfile_size, read_data_size;
char read_usb_buff[20];

//int temp=0;
//unsigned int bytesRead = 0;

	// usb not active
	if(usb_status == 0)	return (1);

//	sprintf (fileusb,"fat:"STR(USB_DRIVE_NUM)":%d-%d-%d.csv",CurTime.tm_mday,(CurTime.tm_mon),(CurTime.tm_year));
	sprintf (fileusb,"fat:"STR(USB_DRIVE_NUM)":usb_test.txt");

	// Open file for reading
	src = fopen(fileusb, "r");

	if (!src) {
		//System_printf("Error opening \"%s\"\n", fileusb);
		//System_abort("Aborting...\n");
		return (2);
	}

	// set end position for getting size of file
	fseek(src, 0, SEEK_END);

	//read file size
	testfile_size = ftell(src);
//	rewind(src);

	// set starting position for reading data from file
	fseek(src, 0, SEEK_SET);

//	memset(logfile_buff, 0, sizeof(logfile_buff));
//	bytesRead = fread(ouput_buffer, 1, LOGFILE_BUFF_SIZE, src);
//	logfile_rddata_size = fread(logfile_buff, 1, LOGFILE_BUFF_SIZE, src);

//	memset(read_usb_buff, 0, testfile_size);
//	fwrite(write_data, 1, strlen( (const char *)write_data), src);
	read_data_size = fread(read_usb_buff, 1, testfile_size, src);

	// Close file
	fclose(src);

	//check file size
//	if (read_data_size != sizeof(read_data))	return (3);
	if (read_data_size != strlen(read_data))	return (3);

	//check file data
//	if (strcmp(read_usb_buff, (const char *)read_data) != 0)	return (4);

	return (0);
}

//$$NR$$//dated:08Nov15


/*
 * 	Read_log()
 *  Read data from USB Mass Storage device into an specified buffer
 */
uint8_t Read_log(uint16_t read_startloc)
{
uint16_t temp_size;

	/* Variables for the CIO functions */
	FILE *src ;
	//int temp=0;
//	unsigned int bytesRead = 0;

	if(usb_status == 0)//usb not active
			return 0;

	sprintf (fileusb,"fat:"STR(USB_DRIVE_NUM)":%d-%d-%d.csv",CurTime.tm_mday,(CurTime.tm_mon),(CurTime.tm_year));

	/* Open file for reading */
	src = fopen(fileusb, "r");

	if (!src) {
		//System_printf("Error opening \"%s\"\n", fileusb);
		//System_abort("Aborting...\n");
		return 0;
	}

	// set end position for getting size of file
	fseek(src, 0, SEEK_END);

	//read file size
	logfile_size = ftell(src);

//	if (logfile_size < LOGFILE_BUFF_SIZE)
	if ((logfile_size - read_startloc) < LOGFILE_BUFF_SIZE)
		temp_size = logfile_size - read_startloc;
	else
		temp_size = LOGFILE_BUFF_SIZE;

	// set starting position for reading data from file
	fseek(src, read_startloc, SEEK_SET);

//	memset(logfile_buff, 0, sizeof(logfile_buff));
	//bytesRead = fread(ouput_buffer, 1, LOGFILE_BUFF_SIZE, src);
//	logfile_rddata_size = fread(logfile_buff, 1, LOGFILE_BUFF_SIZE, src);

	memset(logfile_buff, 0, temp_size);
	logfile_rddata_size = fread(logfile_buff, 1, temp_size, src);

	/* Close file */
	fclose(src);

	if (logfile_size == 0)		return (0);

	return (1);
}


void Read_log_portal()
{
	/* Variables for the CIO functions */
	FILE *src ;
	char day=0,month=0,year=0;

	if(usb_status == 0)		return;

	if(CurTime.tm_mday == 1)
	{
		// Return the number of days based on the month.
		if(CurTime.tm_mon == 1)
		{
			month = 12;
			day = 31;
			if(CurTime.tm_year >= 1)
			{
				year = CurTime.tm_year -1;
			}
			else
			{
				year = 0;
			}
		}
		else
		{
			month = CurTime.tm_mon -1;
			year = CurTime.tm_year;
			if(month == 2)
			{
				// For February calculate day based on the year being a
				// leap year or not.
				if((CurTime.tm_year % 4) == 0)
				{
					// If leap year return 29.
					day = 29;
				}
				else
				{
					// If not leap year return 28.
					day = 28;
				}
			}
			else if((month == 4) || (month == 6) || (month == 9) || (month == 11))
			{
				// For April, June, September and November return 30.
				day = 30;
			}
			else
			{
				// For all the other months return 31.
				day = 31;
			}
		}
	}
	else
	{
		day = CurTime.tm_mday-1;
		month = CurTime.tm_mon;
		year = CurTime.tm_year;
	}

	sprintf (fileusb,"fat:"STR(USB_DRIVE_NUM)":%d-%d-%d.csv",day,month,year);

	/* Open file for reading */
	src = fopen(fileusb, "r");

	if (!src) {
		//System_printf("Error opening \"%s\"\n", fileusb);
		//System_abort("Aborting...\n");
		return;
	}

	memset(logfile_buff,0, sizeof(logfile_buff));
	//bytesRead = fread(ouput_buffer, 1, LOGFILE_BUFF_SIZE, src);
	logfile_rddata_size = fread(logfile_buff, 1, LOGFILE_BUFF_SIZE, src);


	/* Close file */
	fclose(src);
}



//Void taskFxn(UArg arg0, UArg arg1)
//{
//
//
//    /* Now output the outputfile[] contents onto the console */
//    dst = fopen(outputfileusb, "r");
//    if (!dst) {
//        //System_printf("Error opening \"%s\"\n", outputfileusb);
//        System_abort("Aborting...\n");
//    }
//
//    /* Print file contents */
//    while (true) {
//        /* Read from output file */
//        bytesRead = fread(logfile_buff, 1, LOGFILE_BUFF_SIZE, dst);
//        if (bytesRead == 0) {
//            break; /* Error or EOF */
//        }
//        /* Write output */
//        //System_printf("%s", logfile_buff);
//    }
//
//    /* Close the file */
//    fclose(dst);
//
//
//    /* Stopping the USB Drive */
//    USBMSCHFatFs_close(usbmschfatfsHandle);
//    //System_printf("Drive %u unmounted\n", USB_DRIVE_NUM);
//}
