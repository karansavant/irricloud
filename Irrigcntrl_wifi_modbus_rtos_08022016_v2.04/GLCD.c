//*****************************************************************************
//
// GLCD.c -
//
// 
// Brief description of various user-defined function subroutines used in the code.
//
// GLCD_ON() : This function turns the display on. This can be done by sending the command 3Fh to both the controllers. So, while sending this command, both CS1 and CS2 must be pulled low. Similarly the RS pin should be low too as the byte sent is an instruction.
// Set_Start_Line() :  This function changes the line number to be displayed at the top of the screen. You can set it to be any number between 0 to 63. It does not affect the data in the display RAM, it just scrolls the display up and down.
// goto_column() : Moves the cursor to specified column (0-127).
// GOTO_ROW() : Moves the cursor to specified row (0-63).
// goto_page() : Moves the cursor to specified page number (0-7).
// goto_xy() : Moves the cursor to specified row and column.
// GLCD_write_byte() : Writes a byte of data to the current location.
// GLCD_read_byte() : Returns a byte read from the current display location. If you see the code for this subroutine, you will see there are two read operations involved. The first one is a dummy read during which the data is fetched from the display RAM is latched in to the output register of KS0108B. In the second read, the microcontroller can get the actual data.
// GLCD_Clrln() : Clears a specified row (0-7).
// GLCD_clear_screen() : Clears the whole screen (all 8 pages).
// Draw_Point() : Plots a dark or light color point at a specified position.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/tm4c1294ncpdt.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"

//#include "IRQ.h"
#include "Arial_Narrow15x16.h"
#include "font5x8.h"
#include "GLCDConst.h"

//*****************************************************************************

// Glcd commands / status
#define DISPLAY_SET_Y       	0x40
#define DISPLAY_SET_X       	0xB8
#define DISPLAY_START_LINE  	0xC0
#define DISPLAY_ON_CMD			0x3E
#define ON						0x01
#define OFF						0x00
#define DISPLAY_STATUS_BUSY		0x80

// Glcd module connections

#define GLCD_RS (1U << 2)	// PA2
#define GLCD_RW (1U << 3)	// PA3
#define GLCD_EN (1U << 6)	// PA6
#define GLCD_RST (1U << 7)	// PA7//N1
#define GLCD_CS1 (1U << 1)	// PP1
#define GLCD_CS2 (1U << 4)	// PK4

#define GLCD_Data	GPIO_PORTM_DATA_R
#define GLCD_DEN	GPIO_PORTM_DEN_R
#define GLCD_Dir	GPIO_PORTM_DIR_R

#define GLCD_CONTROL_PA_BIT (GPIO_PORTA_AHB_DATA_BITS_R)
#define GLCD_CONTROL_PN_BIT (GPIO_PORTN_DATA_BITS_R)
#define GLCD_CONTROL_PP_BIT (GPIO_PORTP_DATA_BITS_R)
#define GLCD_CONTROL_PK_BIT (GPIO_PORTK_DATA_BITS_R)

#define GLCD_RS_ON()		GLCD_CONTROL_PA_BIT[GLCD_RS] |= GLCD_RS;
#define GLCD_RS_OFF()		GLCD_CONTROL_PA_BIT[GLCD_RS] &= ~GLCD_RS;
#define GLCD_RW_ON()		GLCD_CONTROL_PA_BIT[GLCD_RW] |= GLCD_RW;
#define GLCD_RW_OFF()		GLCD_CONTROL_PA_BIT[GLCD_RW] &= ~GLCD_RW;
#define GLCD_EN_ON()		GLCD_CONTROL_PA_BIT[GLCD_EN] |= GLCD_EN;
#define GLCD_EN_OFF()		GLCD_CONTROL_PA_BIT[GLCD_EN] &= ~GLCD_EN;
#define GLCD_RST_ON()		GLCD_CONTROL_PA_BIT[GLCD_RST] |= GLCD_RST;
#define GLCD_RST_OFF()		GLCD_CONTROL_PA_BIT[GLCD_RST] &= ~GLCD_RST;
#define GLCD_CS1_ON()		GLCD_CONTROL_PP_BIT[GLCD_CS1] |= GLCD_CS1;
#define GLCD_CS1_OFF()		GLCD_CONTROL_PP_BIT[GLCD_CS1] &= ~GLCD_CS1;
#define GLCD_CS2_ON()		GLCD_CONTROL_PK_BIT[GLCD_CS2] |= GLCD_CS2;
#define GLCD_CS2_OFF()		GLCD_CONTROL_PK_BIT[GLCD_CS2] &= ~GLCD_CS2;

//
// End Glcd module connections
//
#define lowerbitmask(bitno)		((1<<bitno) - 1);
#define higherbitmask(bitno)	~((1<<bitno) - 1);
//
//
void GLCD_Screen_Update();
void GLCD_MsgLine_Update(uint8_t y_start, uint8_t z_start, uint8_t dy, uint8_t dz);
//
//
volatile uint8_t screen_map[8][128] ={0};
volatile uint8_t CURRENT_X = 0, CURRENT_Y = 0, CURRENT_Z = 0;
volatile uint8_t SCRN_MAP_CUR_X = 0, SCRN_MAP_CUR_Y = 0, SCRN_MAP_CUR_Z = 0;

//
//
//*****************************************************************************
// Delay function
//*****************************************************************************
void delay_glcd(uint16_t count)
{
uint16_t i;
	for(i = 0; i < count; i++);
}

//*****************************************************************************
//
//*****************************************************************************

uint8_t GLCD_ReadByteFromROMMemory(uint8_t * ptr)
{
	return *(ptr);
}

//*****************************************************************************
//  function
//*****************************************************************************

void enable_pulse()
{
	GLCD_EN_ON();	//EN high
//	delay_usec(5);	//delay of 5us
	delay_glcd(200);

	GLCD_EN_OFF();	//EN low
//	delay_usec(5);	//delay of 5us
	delay_glcd(200);
}

//*****************************************************************************
//  function
//*****************************************************************************

void select_chip(uint8_t _CS)
{
	switch(_CS)
	{
		case 1:
			GLCD_CS1_ON();
			GLCD_CS2_OFF();
			break;
		case 2:
			GLCD_CS2_ON();
			GLCD_CS1_OFF();
			break;
		case 3:
			GLCD_CS1_ON();
			GLCD_CS2_ON();
			break;
	}
}

//*****************************************************************************
//  function
//*****************************************************************************

void GLCD_send_cmd(uint8_t cmd)
{
	GLCD_RS_OFF();	//RW low --> command
	GLCD_RW_OFF();	//RW low --> write

	GLCD_Data = cmd;
	enable_pulse();

	return;
}

//*****************************************************************************
//
//*****************************************************************************

void GLCD_write_byte(uint8_t data_byte)
{
uint8_t i;

	GLCD_RS_ON();				//RS high --> data
	GLCD_RW_OFF();				//RW low --> write

	GLCD_Data = data_byte;		//put data on data port

    for(i = 0; i < 4; i++);		//delay

	enable_pulse();
}

//*****************************************************************************
//
//*****************************************************************************
/*
uint16_t GLCD_read_byte(uint16_t column)
{
	unit8_t i=0;
	uint16_t read_data = 0; //Read data here

	GLCD_Dir = 0x00;	//PORTD as Input
	GLCD_RW_ON(); 		//Read
	GLCD_RS_ON();		//Data

	if (column < 64)
	{
		GLCD_CS1_ON();	GLCD_CS2_OFF();	//Disable CS2
	}
	else
	{
		GLCD_CS1_OFF();	GLCD_CS2_ON();	//Enable CS2
	}

//	delay_us(1); //tasu
    for(i = 0; i < 10; i++);


	GLCD_EN_ON(); //Latch RAM data into ouput register

//	delay_us(1); //twl + tf   //Dummy read
    for(i = 0; i < 10; i++);

	GLCD_EN_OFF(); //Low Enable

//	delay_us(5);
    for(i = 0; i < 50; i++);


	GLCD_EN_ON(); //latch data from output register to data bus

//	delay_us(1); //tr + td(twh)
    for(i = 0; i < 10; i++);

	read_data = GLCD_Data; //Input data

	GLCD_EN_OFF(); //Low Enable to remove data from the bus

//	delay_us(1); //tdhr
    for(i = 0; i < 10; i++);


	GLCD_Dir = 0xFF; //Output again

    for(i = 0; i < 10; i++);

	return read_data;
}
*/
//*****************************************************************************
// IO_init() - All pins are Enabled/initiliazed
//*****************************************************************************

void GLCD_Init(void)
{
	/*Setting Outputs*/
    GPIO_PORTD_AHB_DATA_R = 0x00;
    GLCD_Data = 0x00;

	// Activate both chips
	select_chip(3);
	GLCD_RST_ON();

	GLCD_send_cmd (0x3F);	// Display ON command
}

//*****************************************************************************
//  function
//*****************************************************************************

void turn_on_GLCD()
{
	//Activate both chips
	select_chip(3);

	GLCD_RS_OFF();	//RW low --> command
	GLCD_RW_OFF();	//RW low --> write

	GLCD_Data = 0x3F;				//Display ON command
	enable_pulse();
}

//*****************************************************************************
//  function
//*****************************************************************************

void turn_off_GLCD()
{
	//Activate both chips
	select_chip(3);

	GLCD_RS_OFF();	//RW low --> command
	GLCD_RW_OFF();	//RW low --> write

	GLCD_Data = 0x3E;				//Display ON command
	enable_pulse();
}

//*****************************************************************************
//  Set Start line 0-63 (Set Z Address)
//*****************************************************************************

void set_start_line(uint8_t _CS, uint8_t line)
{
uint8_t cmd;

	//Activate both chips
	select_chip(_CS);

	if(line<64)
	{
		cmd = 0xC0 | line;			// Set Start Line command
		GLCD_send_cmd (cmd);
	}
}

//*****************************************************************************
//  Set Column Number 0-63 (Set Y Address)
//  Moves the cursor to position y horizontally
//*****************************************************************************

void goto_column(uint8_t y)
{
uint8_t cmd;

	if(y<64)				//left section
	{
		select_chip(1); 		//select chip 1
		cmd = 0x40 | (y);		//put column address on data port
		GLCD_send_cmd (cmd);
	}
	else if (y>=64 && y<=127)	//right section
	{
		select_chip(2); 		//select chip 2
		cmd = 0x40 | (y-64);	//put column address on data port
		GLCD_send_cmd (cmd);
	}
}

//*****************************************************************************
//  Set Page Number 0-7 (Set X Address)
//*****************************************************************************

void goto_page(uint8_t x)
{
uint8_t cmd;

	if(x<8)
	{
		cmd = 0xB8 | (x);		//put page address on data port
		GLCD_send_cmd (cmd);

//		goto_column(0);			// Set Column Number = 0 (Set Y Address)
		CURRENT_X = x;
	}
	else
	{
		cmd = 0xB8 | (0);	//put page address on data port
		GLCD_send_cmd (cmd);

//		goto_column(0);			// Set Column Number = 0 (Set Y Address)
		CURRENT_X = 0;
	}

}
//*****************************************************************************
//  Set Next Page (Set Next X Address)
//*****************************************************************************
/*
void goto_next_page(uint8_t x)
{
	CURRENT_X++;
	CURRENT_Y = 0;
	goto_page(CURRENT_X);
}
*/
//*****************************************************************************
//
//*****************************************************************************

void goto_xy(uint8_t x, uint8_t y)
{
	CURRENT_X = x;	CURRENT_Y = y;	CURRENT_Z = x*8;

	goto_column(y);
	goto_page(x);
}

//*****************************************************************************

void goto_yz(uint8_t y, uint8_t z)
{
//uint8_t _cs;

	CURRENT_X = z / 8;	CURRENT_Y = y;	CURRENT_Z = z;

//	if (y<64)
//		_cs = 1;
//	else if ((y>=64) && (y<=127))
//		_cs = 2;

	goto_column(y);
//	set_start_line(_cs, z);
}

//*****************************************************************************
//
//*****************************************************************************

void goto_screen_map_xy(uint8_t x, uint8_t y)
{
	SCRN_MAP_CUR_X = x;	SCRN_MAP_CUR_Y = y;	SCRN_MAP_CUR_Z = x*8;
}

//*****************************************************************************

void goto_screen_map_yz(uint8_t y, uint8_t z)
{
	SCRN_MAP_CUR_X = z / 8;	SCRN_MAP_CUR_Y = y;	SCRN_MAP_CUR_Z = z;
}

//*****************************************************************************
//
//*****************************************************************************

void GLCD_fill_screen(uint8_t datatofill)
{
	memset(screen_map, datatofill, sizeof(screen_map[0][0]) * 128 * 8);

	GLCD_MsgLine_Update(0, 0, GLCD_SCREEN_WIDTH-1, GLCD_SCREEN_HEIGHT-1);
}

//*****************************************************************************
//
//*****************************************************************************

void GLCD_clear_screen(bool screen_update_flag)
{
	memset(screen_map, 0x00, sizeof(screen_map[0][0]) * 128 * 8);

	if (screen_update_flag)
		GLCD_Screen_Update();
}

//--------------------------------------------------------------------------------
// Clear GLCD Screen Line and update screen map data
//--------------------------------------------------------------------------------

void GLCD_clear_page(uint8_t z_start, uint8_t dz, bool screen_update_flag)
{
uint8_t x_addr;

	for (x_addr=(z_start/8); x_addr<=(dz/8); x_addr++)
	{
		memset((void *)screen_map[x_addr], 0x00, sizeof(screen_map[0][0]) * 128);
	}

	if (screen_update_flag)
		GLCD_Screen_Update(0, z_start, GLCD_SCREEN_WIDTH-1, dz);
}

//--------------------------------------------------------------------------------
// Clear GLCD Screen Line and update screen map data
//--------------------------------------------------------------------------------

void GLCD_clear_part_of_screen(uint8_t z_start, uint8_t y_start, uint8_t dz, uint8_t dy, bool screen_update_flag)
{
uint8_t x_addr, y_addr, z_addr, x_start, x_end, tmpdata = 0;

	x_start = (z_start / 8);	x_end = (dz / 8);

	if (z_start % 8)
	{
		x_addr = (z_start / 8);		z_addr = (z_start % 8);
		tmpdata = ((1 << (z_addr - 1)) - 1);

		for(y_addr=y_start; y_addr<=dy; y_addr++)
			screen_map[x_addr][y_addr] &= tmpdata;

		x_start++;
	}

	if (dz % 8)	x_end--;

	tmpdata = (dy - y_start) + 1;
	for (x_addr=x_start; x_addr<= x_end; x_addr++)
	{
		memset((void *)(&screen_map[x_addr][y_start]), 0x00, sizeof(screen_map[0][0]) * tmpdata);
	}

	if ((dz % 8) && (x_start < x_end))
	{
		x_addr = x_end + 1;		z_addr = (dz % 8) + 1;
		tmpdata = ~((1 << z_addr) - 1);

		for(y_addr=y_start; y_addr<=dy; y_addr++)
			screen_map[x_addr][y_addr] &= tmpdata;

		x_start++;
	}

	if (screen_update_flag)
		GLCD_Screen_Update(0, z_start, GLCD_SCREEN_WIDTH-1, dz);
}

//--------------------------------------------------------------------------------
// update screen map data and then clear GLCD Screen
//--------------------------------------------------------------------------------

void GLCD_Screen_Update()
{
uint8_t x_addr, y_addr, data;

	for (x_addr=0; x_addr<8; x_addr++)
	{
		goto_xy(x_addr, 0); //At start of line of left side
		for(y_addr=0; y_addr<=63; y_addr++)
		{
			data = screen_map[x_addr][y_addr];
			GLCD_write_byte(data);
		}

		goto_xy(x_addr, 64); //At start of line of right side (Problem)
		for(y_addr=0; y_addr<=63; y_addr++)
		{
			data = screen_map[x_addr][y_addr + 64];
			GLCD_write_byte(data);
		}
	}
}

//--------------------------------------------------------------------------------
// update screen map data and then clear GLCD Screen
//--------------------------------------------------------------------------------

void GLCD_MsgLine_Update(uint8_t y_start, uint8_t z_start, uint8_t dy, uint8_t dz)
{
uint8_t x_addr, y_addr, data;

	for (x_addr=(z_start/8); x_addr<=(dz/8); x_addr++)
	{
		goto_xy(x_addr, 0); //At start of line of left side
		for(y_addr=0; y_addr<=63; y_addr++)
		{
			data = screen_map[x_addr][y_addr];
			GLCD_write_byte(data);
		}

		goto_xy(x_addr, 64); //At start of line of right side (Problem)
		for(y_addr=0; y_addr<=63; y_addr++)
		{
			data = screen_map[x_addr][y_addr + 64];
			GLCD_write_byte(data);
		}
	}
}

void GLCD_WriteData(uint8_t dataToWrite, bool data_invert_flag)
{
uint8_t x_addr, z_addr, data, map_data;

	x_addr = (SCRN_MAP_CUR_Z / 8);
	z_addr = (SCRN_MAP_CUR_Z % 8);	// horizontal line number for current page

	if (data_invert_flag) dataToWrite = ~dataToWrite;

	if (z_addr == 0)		// ((z address % 8) == 0) -> z address = 0/8/16/24/32/40/48/56
	{
		data = dataToWrite;

		screen_map[x_addr - 1][SCRN_MAP_CUR_Y] &= 0x7F;
		if (data_invert_flag)	screen_map[x_addr - 1][SCRN_MAP_CUR_Y] |= 0x80;
	}
	else
	{
		//lowerbitmask(z_addr-1)
		map_data = (screen_map[x_addr][SCRN_MAP_CUR_Y] & ((1 << (z_addr - 1)) - 1));

		data = map_data | (dataToWrite << z_addr);
		if (data_invert_flag)	data |= (1 << (z_addr - 1));

		if ((x_addr == 7) && (SCRN_MAP_CUR_Z >= 56))
			data = (screen_map[x_addr][SCRN_MAP_CUR_Y]) | (0 << z_addr);

		if (x_addr < 7)
		{
			// higherbitmask(z_addr)
			map_data = (screen_map[x_addr + 1][SCRN_MAP_CUR_Y] & ~((1 << z_addr) - 1));

			screen_map[x_addr + 1][SCRN_MAP_CUR_Y] = map_data | (dataToWrite >> (8 - z_addr));
		}
	}

	screen_map[x_addr][SCRN_MAP_CUR_Y] = data;

	SCRN_MAP_CUR_Y++;

}

//-------------------------------------------------------------------------------------------------
// Writes char to screen memory
//-------------------------------------------------------------------------------------------------

void GLCD_writesymbol(uint8_t charToWrite, bool data_invert_flag)
{
int8_t i;

	charToWrite -= 32;
	for(i = 0; i < 5; i++)
	  GLCD_WriteData(GLCD_ReadByteFromROMMemory((uint8_t *)(font5x8 + (5 * charToWrite) + i)), data_invert_flag);

	GLCD_WriteData(0x00, data_invert_flag);
}

//-------------------------------------------------------------------------------------------------
// Writes char to screen memory
//-------------------------------------------------------------------------------------------------

void GLCD_writechar(char charToWrite, bool data_invert_flag)
{
int8_t i;

	charToWrite -= 32;
	for(i = 0; i < 5; i++)
	  GLCD_WriteData(GLCD_ReadByteFromROMMemory((uint8_t *)(font5x8 + (5 * charToWrite) + i)), data_invert_flag);

	GLCD_WriteData(0x00, data_invert_flag);
}

//-------------------------------------------------------------------------------------------------
// Write null-terminated string to screen memory
//-------------------------------------------------------------------------------------------------
void GLCD_WriteString(char * stringToWrite, bool data_invert_flag)
{
	GLCD_WriteData(0x00, data_invert_flag);
	GLCD_WriteData(0x00, data_invert_flag);

	while(*stringToWrite)
		GLCD_writechar(*stringToWrite++, data_invert_flag);

	SCRN_MAP_CUR_Z += (GLCD_CHAR_HEIGHT + GLCD_LINE_SPACE);
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------

void GLCD_write_titlebar(char * stringToWrite)
{
uint8_t y_addr, strlength;

	for (y_addr=0; y_addr<=127; y_addr++)
	{
//		screen_map[0][y_addr] |= 0xFF;
		screen_map[1][y_addr] |= (1 << (TITLEBAR_HEIGHT % 8)) - 1;
	}
	memset((void *)screen_map[0], 0xFF, sizeof(screen_map[0][0]) * 128);

	strlength = strlen((const char *)stringToWrite);
	y_addr = (((GLCD_CHAR_PER_LINE - strlength) * 6) / 2) + 1;

	goto_screen_map_yz(y_addr, TITLEBAR_STR_TOPLINE);

	while(*stringToWrite)
	  GLCD_writechar(*stringToWrite++, true);

	SCRN_MAP_CUR_Z += (GLCD_CHAR_HEIGHT + TITLEBAR_BOTTOM_SPACE + GLCD_LINE_SPACE);
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------

void GLCD_write_messagebar(char * stringToWrite)
{
uint8_t y_addr, strlength;

	for (y_addr=0; y_addr<=127; y_addr++)
	{
		screen_map[6][y_addr] |= 0xC0;
//		screen_map[7][y_addr] |= 0xFF;
	}
	memset((void *)screen_map[7], 0xFF, sizeof(screen_map[0][0]) * 128);

	strlength = strlen((const char *)stringToWrite);
	y_addr = (((GLCD_CHAR_PER_LINE - strlength) * 6) / 2) + 1;

	goto_screen_map_yz(y_addr, MSGBAR_STR_TOPLINE);

	while(*stringToWrite)
	  GLCD_writechar(*stringToWrite++, true);

	SCRN_MAP_CUR_Z += (GLCD_CHAR_HEIGHT + MSGBAR_BOTTOM_SPACE);
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------

void GLCD_WriteDataLargeFont(uint8_t dataToWriteH, uint8_t dataToWriteL, bool data_invert_flag)
{
uint8_t x_addr;//, z_addr;//, data, map_data;

	x_addr = (SCRN_MAP_CUR_Z / 8);
//	z_addr = (SCRN_MAP_CUR_Z % 8);	// horizontal line number for current page

	if (data_invert_flag)
	{
		dataToWriteH = ~dataToWriteH;
		dataToWriteL = ~dataToWriteL;
	}

	screen_map[x_addr][SCRN_MAP_CUR_Y]	 = dataToWriteH;
	screen_map[x_addr+1][SCRN_MAP_CUR_Y] = dataToWriteL;

	SCRN_MAP_CUR_Y++;
}

//-------------------------------------------------------------------------------------------------
// Writes char to screen memory
//-------------------------------------------------------------------------------------------------

void GLCD_writecharLargeFont(char charToWrite, bool data_invert_flag)
{
int8_t i, data_h, data_l, count = 24;

	if (charToWrite < ':')	count = 20;
	if ((charToWrite == ' ') || (charToWrite == ':'))	count = 14;

	charToWrite -= 32;
	for(i = 0; i < count; i++)
	{
		data_h = GLCD_ReadByteFromROMMemory((uint8_t *)(Arial_Narrow15x16 + (31 * charToWrite + 1) + i));	i++;
		data_l = GLCD_ReadByteFromROMMemory((uint8_t *)(Arial_Narrow15x16 + (31 * charToWrite + 1) + i));

		GLCD_WriteDataLargeFont(data_h, data_l, data_invert_flag);
	}

//	GLCD_WriteDataLargeFont(0x00, 0x00, data_invert_flag);
}

//-------------------------------------------------------------------------------------------------
// Write null-terminated string to screen memory
//-------------------------------------------------------------------------------------------------

void GLCD_WriteStringLargeFont(char * stringToWrite, bool data_invert_flag)
{
	while(*stringToWrite)
		GLCD_writecharLargeFont(*stringToWrite++, data_invert_flag);
}

//-------------------------------------------------------------------------------------------------
// Write point data to screen memory
//-------------------------------------------------------------------------------------------------

void Draw_Point(uint16_t y_addr,uint16_t z_addr, uint16_t color)
{
	uint16_t x_addr, Col_Data;

	x_addr = (z_addr / 8);
	goto_xy(x_addr, y_addr);

	switch (color)
	{
	case 0: //Light spot
//		Col_Data = ~(1<<x_addr) & GLCD_read_byte(y_addr);
		break;

	case 1: //Dark spot
		if ((z_addr % 8) == 0)
		{
			Col_Data = 1;
		}
		else
		{
			Col_Data = (screen_map[x_addr][y_addr]) | (1 << (z_addr % 8));
		}

		screen_map[x_addr][y_addr] = Col_Data;
		break;
	}

	GLCD_write_byte(Col_Data);
}

//****************************************************************************
