//****************************************************************************
// @Module        LCD
// @Filename      LCD.H
// @Project       Hands on training
//----------------------------------------------------------------------------
// @Controller    Siemens C500 Family
//
// @Compiler      Keil C51
//                
//
// @Author        Adapted by Dany Nativel (originally C166 drivers by Michael Knese)
//
// @Description   This file contains all function prototypes and macros for LCD
//                
//
//----------------------------------------------------------------------------
// @Date          01/26/99 10:00:00 AM
//
//****************************************************************************



//****************************************************************************
// @Prototypes of global functions
//****************************************************************************

void LCD_vInit(void);                                                       //initalization 
bit  LCD_bBusyFlag (void);						    //status of busy Flag
void LCD_vClear (void);							    //clear display
void LCD_vHome (void);  						    //cursor to home position
void LCD_vSetCursor (unsigned char cursor);				    //Corsur on/off/blink
void LCD_vMoveShift (unsigned char move);                                   //Move/shift cursor/display left/right
void LCD_vGotoXY (unsigned char X, unsigned char Y);                        //Set cursor position (x,y)
void LCD_vCursorPos (char row, char column);				    //        -"-      row, col
void LCD_vWrite_ubyteXY(unsigned char x, unsigned char y, unsigned char in_Data);                      //write a ubyte as string
void LCD_vLong2String (unsigned long input, char *str, char numdigits);     //convert
void LCD_vLong2StringLZ (unsigned long input, char *str, char numdigits);   //	"
void LCD_vUword2Hexstring (unsigned int input, char *str, char numdigits);  //	"
unsigned int LCD_uwHexstring2uword (char *str);				    //	"
void LCD_vDisplayStringPos (char row, char column, char *string);	    //write string a position
void LCD_vDisplayString (char *s);                                         //write a string at the current position
void LCD_vDisplayStringCentered (char row, char *string);                   //write string (centerd)
void LCD_vDisplayScreen (char *ptr);					    //write the whole display
void LCD_vWipeOnLR (char *ptr);						    //	       "
void LCD_vWipeOnRL (char *ptr);						    //	       "
void LCD_vWipeOffLR (void);						    //delete the whole display
void LCD_vWipeOffRL (void);						    //	       "
void LCD_vDisplayRow (char row, char *string);                              //write a whole row
void LCD_vCursorLeft (void);
void LCD_vCursorRight (void);
void LCD_vDefineChar (char address, const unsigned char *pattern);          //self definably characters 
// void LCD_vWriteData(unsigned char data);					    //write a characters to curent position (in consideration of the busy signal) 
void LCD_vDisplayCharacter(unsigned char in_data);					    //write a characters to curent position (in consideration of the busy signal) 
void LCD_text (void);  						            //write an example text (for the HOT)



// USER CODE BEGIN (LCDHeader,1)

// USER CODE END


//****************************************************************************
// @Macros
//****************************************************************************


// USER CODE BEGIN (LCDHeader,2)

// USER CODE END


//****************************************************************************
// @Defines
//****************************************************************************
 
#ifndef TRUE
  #define TRUE 1
  #define FALSE !TRUE
#endif


// just put the right address for LCD_OFFSET depending of the jumpers configuration
//
// CS1 from StarterKit : LCD_OFFSET=0xFD00H => JP11:2+3
// CS2 from StarterKit : LCD_OFFSET=0xFE00H => JP11:2+4
// CS3 from StarterKit : LCD_OFFSET=0xFF00H => JP11:1+2 

#define LCD_OFFSET 0x0FF00L  

#define Befehle			((unsigned char volatile xdata *) 0)[LCD_OFFSET]
#define READ_BusyF_and_Adress	((unsigned char volatile xdata *) 0)[LCD_OFFSET+0x40]
#define WRITE_Data 		((unsigned char volatile xdata *) 0)[LCD_OFFSET+0x80]
#define READ_Data 		((unsigned char volatile xdata *) 0)[LCD_OFFSET+0xC0]




// !!! #define LCD_DisplayCharacter(c)	(LCD_vWriteData(c))

#define LCD_Home                (Befehle    = 0x02)
#define LCD_Clear               (Befehle    = 0x01)
#define LCD_CursorLeft          (Befehle    = 0x10)
#define LCD_CursorRight         (Befehle    = 0x14)
#define LCD_CursorON            (Befehle    = 0x0d)
#define LCD_CursorOFF           (Befehle    = 0x0e)
#define LCD_CursorBlink	        (Befehle    = 0x0f)
#define LCD_DisplayOFF          (Befehle    = 0x08)
#define LCD_DisplayON           (Befehle    = 0x0c)


// USER CODE BEGIN (LCDHeader,3)

// USER CODE END





