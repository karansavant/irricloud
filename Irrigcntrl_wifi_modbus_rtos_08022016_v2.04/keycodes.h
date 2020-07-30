/********************************************************/
/*  				Keyboard Constants					*/
/********************************************************
 * Key codes by scan sequence with Key Legends on *
 * PCB and with logical Key Names.                *
 **************************************************/

#ifndef KEY_SCAN_CODES
#define KEY_SCAN_CODES

// Scan codes of the keys recieved from TCA8424:
#define NO_SW_PRESS		0X00

//#define SW24			201//0XAE		// System Operating KEY GROUP
//#define AUTO			199//0XBD
//#define ADV				200//0XBC
//#define MAN				202//0XAD
//#define LOG				203//0XAC
//
//#define SWUP			206//0XBE		// Arrow KEY GROUP
//#define SWDOWN			184//0XCC
//#define SWLEFT			204//0XCD
//#define SWRIGHT			186//0XCB
//
//#define ENT				205//0XCE
//#define SET 			185//0XBB		// OTHERS
//#define HOME			190//0XAB
//
//#define SW1 			174//0XC7		// NUM KEY GROUP
//#define SW2 			170//0XB7
//#define SW3 			168//0XA7
//#define SW4				173//0XC8
//#define SW5 			171//0XB8
//#define SW6 			167//0XA8
//#define SW7 			188//0XC9
//#define SW8 			183//0XB9
//#define SW9 			169//0XA9
//#define BACK			187//0XCA
//#define SW0 			189//0XBA
//#define ENTER			172//0XAA

#define SW24			201//0XAE		// System Operating KEY GROUP
#define AUTO			199//0XBD
#define ADV				204//0XBC
#define MAN				206//0XAD
#define LOG				205//0XAC

#define SWUP			200//0XBE		// Arrow KEY GROUP
#define SWDOWN			183//0XCC
#define SWLEFT			203//0XCD
#define SWRIGHT			186//0XCB

#define ENT				202//0XCE
#define SET 			184//0XBB		// OTHERS
#define HOME			185//0XAB

#define SW1 			174//0XC7		// NUM KEY GROUP
#define SW2 			172//0XB7
#define SW3 			171//0XA7
#define SW4				173//0XC8
#define SW5 			169//0XB8
#define SW6 			170//0XA8
#define SW7 			188//0XC9
#define SW8 			190//0XB9
#define SW9 			168//0XA9
#define BACK			187//0XCA
#define SW0 			189//0XBA
#define ENTER			167//0XAA

#define SW_VIRTUAL1		0XF1
#define SW_VIRTUAL2		0XF2
#define SW_VIRTUAL3		0XF3
#define SW_VIRTUAL4		0XF4
#define SW_VIRTUAL5		0XF5		//$$NR$$//dated:01Nov15


// Logical Key Codes:
#define kbNoKey			NO_SW_PRESS

#define kbWaterOnOff	SW24		// System Operating Keys
#define kbManual 		MAN
#define kbProgram		AUTO
#define kbLogs			LOG
#define kbDiagnostic	ADV

#define kbUp			SWUP		// Arrow Keys
#define kbDown			SWDOWN
#define kbLeft			SWLEFT
#define kbRight			SWRIGHT

#define kbEnter			ENT
#define kbSetup			SET		// System Operating Keys
#define kbHome			HOME

#define kbNum1			SW1		// Num keys
#define kbNum2			SW2
#define kbNum3			SW3
#define kbNum4			SW4
#define kbNum5			SW5
#define kbNum6			SW6
#define kbNum7			SW7
#define kbNum8			SW8
#define kbNum9 			SW9
#define kbDot			BACK
#define kbNum0			SW0
#define kbClear			ENTER

#define kbVirtual		SW_VIRTUAL1
#define kbVirtualViewLog SW_VIRTUAL2
#define kbVirtualEdit	SW_VIRTUAL3
#define kbVirtualHome	SW_VIRTUAL4
#define kbVirtualTest	SW_VIRTUAL5		//$$NR$$//dated:01Nov15

#define kbNext			kbRight
#define kbBack			kbLeft
#define kbSelect		kbEnter

#define kbEsc			kbBack


/*
// Scan codes of the keys:
#define NO_KEY		0x00
#define K01			0XC7
#define K02			0x02
#define K03			0x03
#define K04			0x04
#define K05			0x05
#define K06			0x06
#define K07			0x07
#define K08			0x08
#define K09			0x09
#define K10			0x0A
#define K11			0x0B
#define K12			0x0C
#define K13			0x0D
#define K14			0x0E
#define K15			0x0F
#define K16			0x10
#define K17			0x11
#define K18			0x12
#define K19			0x13
#define K20			0x14
#define K21			0x15
#define K22			0x16
#define K23			0x17
#define K24			0x18
#define K25			0x19
#define K26			0x1A
*/

#endif


