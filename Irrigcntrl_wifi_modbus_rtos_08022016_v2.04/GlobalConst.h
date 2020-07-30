/******************************************************************************/
/* GlobalConst.h: GLOBAL Constants And Declarations                           */
/******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GlobalConst_H
#define __GlobalConst_H

/******************************************************************************/

#define SoftwareVersionID "0.01"				//  Software Version.

// Default Passwords
//#define PassWordLength 4		// Does not include terminating NULL.
#define pwAL1			"1111"
#define pwAL2			"2222"
#define pwAL3			"3333"
#define pwAL4			"4444"

// Panel Mode by Access Level Flags
#define alNorm			0
#define alAdmin			1
#define alEngr			2
#define alOwner			3
#define alOperator		4
#define alTestMode		5
#define alNoMode		10

// Panel State Flags
#define psHome			0x00
#define psPassword		0x01
#define psSetup			0x02
#define psManual	 	0x03
#define psProgram		0x04
#define psLogs		 	0x05
#define psDiagnostic 	0x06
#define psInitSetup 	0x07

/*
// Event Priority State Flags
#define epsNoEvent			0x00
#define epsEvent			0x01
#define epsWarning			0x02
#define epsFault			0x04

// Event Screen State Flags
#define essNoEvent			0x00
#define essViewEvent 		0x01
#define essWarning			0x02
#define essFault			0x03
*/
#endif

/******************************************************************************/
