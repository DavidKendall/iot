/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : drv_glcd.h
 *    Description : Graphic LCD driver include file
 *
 *    History :
 *    1. Date        : December 2, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: #2 $
 **************************************************************************/
#ifndef  __DRV_GLCD_H
#define  __DRV_GLCD_H

#include <drv_glcd_cnfg.h>
#include <arm_comm.h>
#include <glcd_ll.h>

#define DEF_X_LEFT         	0
#define DEF_Y_UP           	0
#define DEF_X_RIGHT        	GLCD_HORIZONTAL_SIZE
#define DEF_Y_DOWN         	GLCD_VERTICAL_SIZE
#define DEF_FONT_COLOUR    	0x000000
#define DEF_BACKGND_COLOUR 	0x00FF0
#define TEXT_DEF_TAB_SIZE  	4

/*
 * Some 12-bit color definitions
 */
#define WHITE 0xFFF
#define BLACK 0x000
#define RED 0xF00
#define GREEN 0x0F0
#define BLUE 0x00F
#define CYAN 0x0FF
#define MAGENTA 0xF0F
#define YELLOW 0xFF0
#define BROWN 0xB22
#define ORANGE 0xFA0
#define PINK 0xF6A


/*
 * External function - generate BEL signal
 */
#define TEXT_BEL1_FUNC()


/*
 * Set window macro
 */
#define LCD_SET_WINDOW(xl,xr,yu,yd) \
{\
Int32U Temp;\
  Temp = xl | (xr<<8);\
  GLCD_SendCmd(CASET,(pInt8U)&Temp,0);\
  Temp = yu | (yd<<8);\
  GLCD_SendCmd(RASET,(pInt8U)&Temp,0);\
}


/*
 * Send pixel macro
 */
#define LCD_WRITE_PIXEL(c) GLCD_SendCmd(RAMWR,(pInt8U)&c,2)

/*
 * Flush pixels macro
 */
#define LCD_FLUSH_PIXELS() GLCD_SendCmd(NOP,NULL,0)

#define GLCD_MANF_ID 0x45
#define GLCD_MOD_ID  0x03

#define WAITED_PIXEL 0x80000000

#define PIXEL_CONV(Pixel,Order) (!Order)?(((Pixel >> 4) & 0xFF) | ((Pixel << 12) & 0xF000)):(((Pixel << 16) & 0xFF0000) | ((Pixel) & 0xF00))

/*
 * PCF8833 status register values
 */
#define PCF8833_ST_BOOSTER_ON     (1UL <<  7)
#define PCF8833_ST_Y_ADDR_ODR     (1UL <<  6)
#define PCF8833_ST_X_ADDR_ODR     (1UL <<  5)
#define PCF8833_ST_ADDR_MODE      (1UL <<  4)
#define PCF8833_ST_LINE_ADDR_ORD  (1UL <<  3)
#define PCF8833_ST_RGB_ORD        (1UL <<  2)
#define PCF8833_ST_PIXEL_FORM     (7UL << 12)
#define PCF8833_ST_IDLE_MODE      (1UL << 11)
#define PCF8833_ST_PARTIAL_MODE   (1UL << 10)
#define PCF8833_ST_SLEEP_MODE     (1UL <<  9)
#define PCF8833_ST_NORM_MODE      (1UL <<  8)
#define PCF8833_ST_V_SCROLL_MODE  (1UL << 23)
#define PCF8833_ST_INV_MODE       (1UL << 21)
#define PCF8833_ST_ALL_PIXELS_ON  (1UL << 20)
#define PCF8833_ST_ALL_PIXELS_OFF (1UL << 19)
#define PCF8833_ST_DISPLAY_MODE   (1UL << 18)
#define PCF8833_ST_TEARING_MODE   (1UL << 17)


/*
 * GLCD command interpreter type
 */
typedef enum _GLCD_ISS_t {
	GLCD_ISS_0 = 0, GLCD_ISS_1
} GLCD_ISS_t;

/*
 * Execution status
 */
typedef enum _GLCD_Status_t {
	GLCD_OK = 0, GLCD_ID_ERROR, GLCD_UNSUPPORTED, GLCD_ERROR
} GLCD_Status_t;


/*
 * GCLD command set
 */
typedef enum _GLCD_Cmd_t {
	// Equal commands
	NOP, SLPIN, SLPOUT, PTLOUT, INVOFF, INVON,
	DISPOFF, DISPON, CASET, RASET, RAMWR, RGBSET, VSCSAD,
	IDMOFF, IDMON, RDID1, RDID2, RDID3, FRMSEL, FRM8SEL, TMPRNG,
	TMPHIS, TMPREAD, EPWRIN, EPWROUT, RDEV, RDRR,
  // Adapted commands
  PTIN, 	// ISS=0 equal on PTLAR + PTIN
  SCRLAR,	// ISS=1 ASCSET mode 0
  DATCTR,	// ISS=0 equal on MADCTR + COLMOD
  SETCON, //
  // Commands applicable only when ISS=1
  OSCON, OSCOFF, RMWIN, RMWOUT, VOLUP, VOLDOWN, ASCSET, DISCTR,
  PWRCTR, VOLCTR,
  // Commands applicable only when ISS=0
  CLKINT, CLKEXT, RDDST, SWRESET, RDDID, PTLAR, PTLON, MADCTR,
  COLMOD, APOFF, APON, BSTROFF, BSTRON,
	GLCD_CMD_NUMB
} GLCD_Cmd_t;


/*
 * Command direction
 */
typedef enum _GLCD_CmdDir_t {
	GLCD_WRITE = 0, GLCD_READ
} GLCD_CmdDir_t;


/*
 * Data flag
 */
#define GLCD_DATA   0x100

/*
 * Invalid command (some commands don't have analog in other
 * command mode interpreter)
 */
#define GLCD_INV_CMD  ((Int32U)(-1))

/*
 * Contrast limits
 */
#define GLCD_MAX_COTRAST   63
#define GLCD_MIN_COTRAST  -64


typedef struct _GLCD_CmdCtrl_t {
	Int32U        	 Cmd;
	GLCD_CmdDir_t    Dir;
	Boolean 				 Pulse;
	Int32U 				   Param;
} GLCD_CmdCtrl_t, *pGLCD_CmdCtrl_t;

#pragma pack(1)
typedef struct _ASCSET_Data_t {
	Int8U Ssl;
	Int8U Sel;
	Int8U Sfl;
	Int8U Smd;
} ASCSET_Data_t, *pASCSET_Data_t;

typedef struct _DATCTR_Data_t {
	union
	{
		Int8U RC_AddMode;
		struct
		{
			Int8U MY : 1;
			Int8U MX : 1;
			Int8U MV : 1;
			Int8U ML : 1;
			Int8U 	 : 4;
		};
	};
	union
	{
		Int8U RGB_Mode;
		struct
		{
			Int8U RGB : 1;
			Int8U 	 	: 7;
		};
	};
	union
	{
		Int8U GrayScale;
		struct
		{
			Int8U GS  : 3;
			Int8U 	 	: 5;
		};
	};
} DATCTR_Data_t, *pDATCTR_Data_t;

typedef struct _MADCTR_Data_t {
	union
	{
		Int8U MemDataAccCtrl;
		struct
		{
			Int8U 	 	: 3;
			Int8U RGB : 1;
			Int8U ML  : 1;
			Int8U MV  : 1;
			Int8U MX  : 1;
			Int8U MY  : 1;
		};
	};	
} MADCTR_Data_t, *pMADCTR_Data_t;

typedef struct _COLMOD_Data_t {
	union
	{
		Int8U ColourMode;
		struct
		{
			Int8U Mode : 3;
			Int8U 		 : 5;
		};
	};	
} COLMOD_Data_t, *pCOLMOD_Data_t;
#pragma pack()

typedef Int32U LcdPixel_t, *pLcdPixel_t;

typedef struct _FontType_t {
  Int32U H_Size;
  Int32U V_Size;
  Int32U CharacterOffset;
  Int32U CharactersNuber;
  pInt8U pFontStream;
  pInt8U pFontDesc;
} FontType_t, *pFontType_t;

typedef struct _Bmp_t {
  Int32U  H_Size;
  Int32U  V_Size;
  Int32U  BitsPP;
  Int32U  BytesPP;
  pInt32U pPalette;
  pInt32U pPicStream;
  pInt8U  pPicDesc;
} Bmp_t, *pBmp_t;



/*************************************************************************
 * Function Name: GLCD_SendCmd
 * Parameters: GLCD_Cmd_t Cmd, pInt8U pData, Int32U Size
 * Return: GLCD_Status_t
 *
 * Description: Send command to the Graphic LCD
 *
 *************************************************************************/
GLCD_Status_t GLCD_SendCmd (GLCD_Cmd_t Cmd, pInt8U pData, Int32U Size);

/*************************************************************************
 * Function Name: GLCD_PowerUpInit
 * Parameters: pInt8U pInitData
 * Return: GLCD_Status_t
 *         GLCD_OK - init pass
 *			 	 GLCD_ID_ERROR - unsupported driver type
 * Description: Power-up initialization of Graphic LCD
 *
 *************************************************************************/
//GLCD_Status_t GLCD_PowerUpInit (pInt8U pInitData);
GLCD_Status_t LCDPowerUpInit (pInt8U);
/*************************************************************************
 * Function Name: LCDSetFont
 * Parameters: pFontType_t pFont, LcdPixel_t Color
 *              LcdPixel_t BackgndColor
 *
 * Return: none
 *
 * Description: Set current font, font color and background color
 *
 *************************************************************************/
void LCDSetFont(pFontType_t pFont,
                LcdPixel_t Color, LcdPixel_t BackgndColor);

/*************************************************************************
 * Function Name: LCDSetWindow
 * Parameters: Int32U X_Left, Int32U Y_Up,
 *             Int32U X_Right, Int32U Y_Down
 *
 * Return: none
 *
 * Description: Set draw window XY coordinate in pixels
 *
 *************************************************************************/
void LCDSetWindow(Int32U X_Left, Int32U Y_Up,
                    Int32U X_Right, Int32U Y_Down);

/*************************************************************************
 * Function Name: LCDTextSetPos
 * Parameters: Int32U X_UpLeft, Int32U Y_UpLeft,
 *             Int32U X_DownLeft, Int32U Y_DownLeft
 *
 * Return: none
 *
 * Description: Set text X,Y coordinate in characters
 *
 *************************************************************************/
void LCDTextSetPos(Int32U X, Int32U Y);

/*************************************************************************
 * Function Name: GLCD_TextSetTabSize
 * Parameters: Int32U Size
 *
 * Return: none
 *
 * Description: Set text tab size in characters
 *
 *************************************************************************/
void LCDTextSetTabSize(Int32U Size);

/*************************************************************************
 * Function Name: GLCD_TextCalcWindow
 * Parameters: pInt32U pXL, pInt32U pXR,
 *             pInt32U pYU, pInt32U pYD,
 *             pInt32U pH_Size, pInt32U pV_Size
 *
 * Return: Boolean
 *          FALSE - out of window coordinate aren't valid
 *          TRUE  - the returned coordinate are valid
 *
 * Description: Calculate character window
 *
 *************************************************************************/
static
Boolean GLCD_TextCalcWindow (pInt32U pXL, pInt32U pXR,
                             pInt32U pYU, pInt32U pYD,
                             pInt32U pH_Size, pInt32U pV_Size);

/*************************************************************************
 * Function Name: putchar
 * Parameters: int c
 *
 * Return: none
 *
 * Description: Put char function
 *
 *************************************************************************/
int putchar (int c);


/*
 * Set LCD contrast
 */
void LCDSetContrast( Int8U Contrast );


/*
 * Functions to print an 8- , 16- or 32-bit unsigned value
 */
void LCDPutHex8U( Int8U byte);
void LCDPutHex16U( Int16U data );
void LCDPutHex32U( Int32U data );

/*
 * Some API graphics functions
 * Dimensions in Int32U
 */
void LCDDrawFilledRectangle ( Int32U x0, Int32U y0, Int32U x1, Int32U y1, LcdPixel_t Color );
void LCDDrawRectangle( Int32U x0, Int32U y0, Int32U x1, Int32U y1, LcdPixel_t Colour ) ;
void LCDDrawFilledCircle( Int32U x0, Int32U y0, Int32U radius, LcdPixel_t Colour );
void LCDDrawCircle( Int32U x0, Int32U y0, Int32U radius, LcdPixel_t Colour );
void LCDDrawLine( Int32U x0, Int32U y0, Int32U x1, Int32U y1, LcdPixel_t Color );
void LCDDrawPixel ( Int32U x, Int32U y, LcdPixel_t Color );

#endif  /* __DRV_GLCD_H */
