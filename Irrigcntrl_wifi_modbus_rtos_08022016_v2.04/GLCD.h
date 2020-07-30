//*****************************************************************************
//
// GLCD.h - Prototypes for the GLCD driver.
//
//*****************************************************************************

#ifndef __DRIVERLIB_GLCD_H__
#define __DRIVERLIB_GLCD_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern void GLCD_Init(void);
extern void GLCD_Screen_Update();
extern void GLCD_MsgLine_Update(uint8_t y_start, uint8_t z_start, uint8_t dy, uint8_t dz);
extern void GLCD_fill_screen(uint8_t datatofill);
extern void GLCD_clear_screen(bool screen_update_flag);
extern void GLCD_clear_page(uint8_t z_start, uint8_t dz, bool screen_update_flag);
extern void GLCD_clear_part_of_screen(uint8_t z_start, uint8_t y_start, uint8_t dz, uint8_t dy, bool screen_update_flag);

extern void set_start_line(uint8_t _CS, uint8_t line);
extern void goto_yz(uint8_t y, uint8_t z);
extern void goto_screen_map_xy(uint8_t x, uint8_t y);
extern void goto_screen_map_yz(uint8_t y, uint8_t z);
extern void GLCD_writesymbol(uint8_t charToWrite, bool data_invert_flag);
extern void GLCD_writechar(char charToWrite, bool data_invert_flag);
extern void GLCD_WriteString(char * stringToWrite, bool data_invert_flag);
extern void GLCD_write_titlebar(char * stringToWrite);
extern void GLCD_write_messagebar(char * stringToWrite);

extern void GLCD_WriteStringLargeFont(char * stringToWrite, bool data_invert_flag);

extern volatile uint8_t SCRN_MAP_CUR_X, SCRN_MAP_CUR_Y, SCRN_MAP_CUR_Z;

extern void Draw_Point(uint16_t y_addr,uint16_t z_addr, uint16_t color);	//$$NR$$//dated:01Nov15
//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __DRIVERLIB_GLCD_H__
