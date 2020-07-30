/******************************************************************************/
/* GLCDConst.h: GLCD Constants And Declarations                           */
/******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GLCDConst_H
#define __GLCDConst_H

/******************************************************************************/
// Glcd module details
#define GLCD_SCREEN_WIDTH		128
#define GLCD_SCREEN_HEIGHT		64

#define GLCD_CHAR_WIDTH			5
#define GLCD_CHAR_HEIGHT		8
#define GLCD_CHAR_SPACE			1
#define GLCD_LINE_SPACE			2
#define GLCD_CHAR_PER_LINE		21

#define GLCD_LINE_HEIGHT		(GLCD_CHAR_HEIGHT + GLCD_LINE_SPACE)

#define TITLEBAR_TOP_SPACE		2
#define TITLEBAR_BOTTOM_SPACE	1
#define TITLEBAR_HEIGHT			(TITLEBAR_TOP_SPACE + GLCD_CHAR_HEIGHT + TITLEBAR_BOTTOM_SPACE)
#define TITLEBAR_STR_TOPLINE	TITLEBAR_TOP_SPACE

#define MIDDLE_PAGE_TOP			TITLEBAR_HEIGHT + GLCD_LINE_SPACE

#define MSGBAR_TOP_SPACE		1
#define MSGBAR_BOTTOM_SPACE		1
#define MSGBAR_HEIGHT			(MSGBAR_TOP_SPACE + GLCD_CHAR_HEIGHT + MSGBAR_BOTTOM_SPACE)
#define MSGBAR_STR_TOPLINE		(GLCD_SCREEN_HEIGHT - GLCD_CHAR_HEIGHT - MSGBAR_BOTTOM_SPACE)


#endif

/******************************************************************************/
