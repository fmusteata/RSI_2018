//****************************************************************************
// @Module        LCD
// @Filename      LCD.C
// @Project       Hands On Training
//----------------------------------------------------------------------------
// @Controller    Siemens C500 Family
//
// @Compiler      Keil C51
//                
//
// @Author        Adapted by Dany Nativel (originally C166 drivers by Michael Knese)
//
// @Description   This file contains all functions that use the LCD Display
//                
//
//----------------------------------------------------------------------------
// @Date         01/26/99 10:00:00 AM
//
//****************************************************************************

//****************************************************************************
// @Project Includes
//****************************************************************************

#include <ctype.h>
#include <string.h>
#include <LCD.H>



// USER CODE BEGIN (LCD_General,1)

// USER CODE END


//***************************************************************************
// @Global Variables
//***************************************************************************

// USER CODE BEGIN (LCD_General,2)

// USER CODE END


//***************************************************************************
// @Define user-defined character dot patterns
//***************************************************************************
const unsigned char LCD_CHAR_BAR0[] = {
  0x0F,   // ---01111
  0x0F,   // ---01111
  0x0F,   // ---01111
  0x0F,   // ---01111
  0x0F,   // ---01111
  0x0F,   // ---01111
  0x0F,   // ---01111
  0x0F    // ---01111
};
const unsigned char LCD_CHAR_BAR1[] = {
  0x07,   // ---00111
  0x07,   // ---00111
  0x07,   // ---00111
  0x07,   // ---00111
  0x07,   // ---00111
  0x07,   // ---00111
  0x07,   // ---00111
  0x07    // ---00111
};
const unsigned char LCD_CHAR_BAR2[] = {
  0x03,   // ---00011
  0x03,   // ---00011
  0x03,   // ---00011
  0x03,   // ---00011
  0x03,   // ---00011
  0x03,   // ---00011
  0x03,   // ---00011
  0x03    // ---00011
};
const unsigned char LCD_CHAR_BAR3[] = {
  0x01,   // ---00001
  0x01,   // ---00001
  0x01,   // ---00001
  0x01,   // ---00001
  0x01,   // ---00001
  0x01,   // ---00001
  0x01,   // ---00001
  0x01    // ---00001
};
const unsigned char LCD_CHAR_BAR4[] = {
  0x10,   // ---10000
  0x10,   // ---10000
  0x10,   // ---10000
  0x10,   // ---10000
  0x10,   // ---10000
  0x10,   // ---10000
  0x10,   // ---10000
  0x10    // ---10000
};
const unsigned char LCD_CHAR_BAR5[] = {
  0x18,   // ---11000
  0x18,   // ---11000
  0x18,   // ---11000
  0x18,   // ---11000
  0x18,   // ---11000
  0x18,   // ---11000
  0x18,   // ---11000
  0x18    // ---11000
};
const unsigned char LCD_CHAR_BAR6[] = {
  0x1c,   // ---11100
  0x1c,   // ---11100
  0x1c,   // ---11100
  0x1c,   // ---11100
  0x1c,   // ---11100
  0x1c,   // ---11100
  0x1c,   // ---11100
  0x1c    // ---11100
};
const unsigned char LCD_CHAR_BAR7[] = {
  0x1e,   // ---11110
  0x1e,   // ---11110
  0x1e,   // ---11110
  0x1e,   // ---11110
  0x1e,   // ---11110
  0x1e,   // ---11110
  0x1e,   // ---11110
  0x1e    // ---11110
};
const unsigned char LCD_CHAR_BAR8[] = {
  0x1f,   // ---11111
  0x1f,   // ---11111
  0x1f,   // ---11111
  0x1f,   // ---11111
  0x1f,   // ---11111
  0x1f,   // ---11111
  0x1f,   // ---11111
  0x1f    // ---11111
};
const unsigned char LCD_CHAR_BAR9[] = {
  0x00,   // ---00000
  0x00,   // ---00000
  0x00,   // ---00000
  0x00,   // ---00000
  0x00,   // ---00000
  0x00,   // ---00000
  0x00,   // ---00000
  0x00    // ---00000
};
const unsigned char LCD_CHAR_UP_ARROW[] = {
  0x1f,	  // ---11111
  0x1b,	  // ---11011
  0x11,	  // ---10001
  0x0a,	  // ---01010
  0x1b,	  // ---11011
  0x1b,	  // ---11011
  0x1b,	  // ---11011
  0x1f 	  // ---11111
};
const unsigned char LCD_CHAR_DOWN_ARROW[] = {
  0x1f,	  // ---11111
  0x1b,	  // ---11011
  0x1b,	  // ---11011
  0x1b,	  // ---11011
  0x0a,	  // ---01010
  0x11,	  // ---10001
  0x1b,	  // ---11011
  0x1f 	  // ---11111
};
const unsigned char LCD_CHAR_TRADEMARK_T[] = {
  0x1f,	  // ---11111
  0x04,	  // ---00100
  0x04,	  // ---00100
  0x04,	  // ---00100
  0x00,	  // ---00000
  0x00,	  // ---00000
  0x00,	  // ---00000
  0x00 	  // ---00000
};
const unsigned char LCD_CHAR_TRADEMARK_M[] = {
  0x11,	  // ---10001
  0x1b,	  // ---11011
  0x15,	  // ---10101
  0x11,	  // ---10001
  0x00,	  // ---00000
  0x00,	  // ---00000
  0x00,	  // ---00000
  0x00 	  // ---00000
};



//****************************************************************************
// @Function      void LCD_vInit(void)
//
//----------------------------------------------------------------------------
// @Description   This function initializes the LCD component. It effects all
//                necessary configurations of the LCD-Register, depending on 
//                the selected operating mode. 
//----------------------------------------------------------------------------
// @Returnvalue   none
//
//----------------------------------------------------------------------------
// @Parameters    none
//
//----------------------------------------------------------------------------
// @Date          11/16/98 12:44:55 PM
//
//****************************************************************************

void LCD_vInit(void)
{
  unsigned long i;

   for(i=0x0D2;i>0;i--);   	/* wait more than 15ms  				*/
   Befehle = 0x30;		/* init							*/


  for(i=0x032;i>0;i--);	/* wait more than 4.1ms	  		     		*/
  Befehle = 0x30;		/* init							*/


  for(i=0x05;i>0;i--);	/* wait more than 0.1ms	  				 	*/
  Befehle = 0x30;		/* init							*/

  while (LCD_bBusyFlag()==1);      	/* wait till LCD ready  */
         	

  Befehle = 0x38;		/* 8 bit bus, 2 lines, 5x7 dots				*/
  
  while (LCD_bBusyFlag()==1);      	/* wait till LCD ready  */
             

  Befehle = 0x08;		/* display off						*/

  while (LCD_bBusyFlag()==1);   
       

  Befehle = 0x01;		/* display clear					*/

  while (LCD_bBusyFlag()==1);         
               
  Befehle = 0x06;		/* entry mode set,  cursor move direction: increase	*/


  while (LCD_bBusyFlag()==1);       

  Befehle = 0x0C;         	/* display on						*/



  //
  // load user-defined characters into LCD
  //
  LCD_vDefineChar (0, LCD_CHAR_BAR0);
  LCD_vDefineChar (1, LCD_CHAR_BAR1);
  LCD_vDefineChar (2, LCD_CHAR_BAR2);
  LCD_vDefineChar (3, LCD_CHAR_BAR3);
  LCD_vDefineChar (4, LCD_CHAR_BAR4);
  LCD_vDefineChar (5, LCD_CHAR_BAR5);
  LCD_vDefineChar (6, LCD_CHAR_BAR6);
  LCD_vDefineChar (7, LCD_CHAR_BAR7);


  // USER CODE BEGIN (LCD_Init,1)

  // USER CODE END

}



/********************************************************************************/
/* 	wait till Busy Flag = 0		     					*/
/********************************************************************************/
bit LCD_bBusyFlag (void)

{ 

   if ((READ_BusyF_and_Adress & 0x80) == 0x80)      	/* test busy flag               */
       return (1);                                      /* LCD is busy                  */
   else                                    		
       return (0);                                      /* LCD is ready                 */

}


/********************************************************************************/
/*	Clears display and returns cursor to home position (DD RAM address = 0)	*/
/********************************************************************************/
void LCD_vClear (void)

{

   while (LCD_bBusyFlag()==1);
   Befehle = 0x01;		/* display clear 		*/


}


/********************************************************************************/
/*	Sets DD RAM address 0 in address counter. Also returns display being 	*/
/*	shifted to original position. DD RAM contents remain unchanged		*/
/********************************************************************************/
void LCD_vHome (void)

{
    while (LCD_bBusyFlag()==1);
    Befehle = 0x02;

}


/********************************************************************************/
/*	Cursor ON/OFF , blink ON/OFF						*/
/*										*/
/*	0 ==>> Cursor OFF ,  blink OFF						*/
/*	1 ==>> Cursor ON  ,  blink OFF						*/
/*	2 ==>> Cursor ON  ,  blink ON						*/
/*										*/
/********************************************************************************/
void LCD_vSetCursor (unsigned char cursor)

{

   if 	   (cursor == 0)
     {
	while (LCD_bBusyFlag()==1);
	Befehle = 0x0C;
     }

   else if (cursor == 1)
     {
	while (LCD_bBusyFlag()==1);
	Befehle = 0x0E;
     }

   else if (cursor == 2)
     {
	while (LCD_bBusyFlag()==1);
	Befehle = 0x0F;
     }

}

/********************************************************************************/
/*	Moves cursor and shifts display without changing DD RAM contents	*/
/*										*/
/*	0 ==>> moves cursor left						*/
/*	1 ==>> shifts display to the right 					*/
/*	2 ==>> shifts display to the left					*/
/*	3 ==>> moves cursor right               				*/
/********************************************************************************/
void LCD_vMoveShift (unsigned char move)

{              

   if      (move == 0)
     {
	while (LCD_bBusyFlag()==1);
	Befehle = 0x10;
     }

   else if (move == 1)
     {
	while (LCD_bBusyFlag()==1);
	Befehle = 0x1C;
     }

   else if (move == 2)
     {
	while (LCD_bBusyFlag()==1);
	Befehle = 0x18;
     }

   else if (move == 3)
     {
	while (LCD_bBusyFlag()==1);
	Befehle = 0x14;
     }


}

 

/********************************************************************************/
/*	Set CURSOR-POSITION						        */
/*      Author :   Michael Knese						*/
/*										*/
/*	parameter: X: X-Position (00..27h)                                      */ 
/*                 Y: Y-Position (0 = 1.line ;   1 = 2.line)                    */
/*                 ==> DD RAM address (00..27h => 1.line;    40..67h => 2.line   */
/********************************************************************************/

void LCD_vGotoXY (unsigned char X, unsigned char Y)

{
     while (LCD_bBusyFlag()==1);  
     Befehle = 0x80 | ( Y ? (X+0x40) : X );
}



/*
** LCD_Cursor: Position the LCD cursor at "row", "column".
*/
void LCD_vCursorPos (char row, char column)
{
	while (LCD_bBusyFlag()==1);
	switch (row) {
		case 1: Befehle = (0x80 + column - 1); break;
		case 2: Befehle = (0xc0 + column - 1); break;
		//case 3: Befehle = (0x94 + column - 1); break;
		//case 4: Befehle = (0xd4 + column - 1); break;
		default: break;
	}
}


//////////////////////////////////////////////////////////////////////////////////
//	Write a unsined byte on the display             		        //
//      Author :   Michael Knese						//
//										//
//	parameter: X: X-Position (00..27h)                                      // 
//                 Y: Y-Position (0 = 1.line ;   1 = 2.line)                    //
//                 ==> DD RAM address (00..27h => 1.line;    40..67h => 2.line   //
//                 Data: 							//
//////////////////////////////////////////////////////////////////////////////////

void LCD_vWrite_ubyteXY(unsigned char x, unsigned char y, unsigned char in_Data)
{

    LCD_vGotoXY(x,y);	                                //go to position (x,y)

    while (LCD_bBusyFlag()==1);                        	// wait till LCD ready          
    WRITE_Data = (in_Data/100) + '0' ;    in_Data %= 100;
    while (LCD_bBusyFlag()==1);                        	// wait till LCD ready          
    WRITE_Data = (in_Data/ 10) + '0' ;    in_Data %=  10;
    while (LCD_bBusyFlag()==1);                        	// wait till LCD ready          
    WRITE_Data = (in_Data    ) + '0' ; 

}




/*
*********************************************************************************************************
* long_to_string()
*
* Description : Convert a "long" to a null-terminated string
*               (base = decimal)
* Arguments   : input = number to be converted
*               str = pointer to string (i.e. display buffer)
*               numdigits = number of digits to display
* Returns     : none
*********************************************************************************************************
*/
void LCD_vLong2String (unsigned long input, char *str, char numdigits)
{
   char digit;
   char blank = TRUE;

   LCD_vLong2StringLZ(input, str, numdigits);

   for (digit=0; digit < numdigits-1; digit++) {
      if (str[digit] == '0') {
         if (blank == TRUE) str[digit] = ' ';
         }
      else {
          blank = FALSE;
      }
   }
}


/*
*********************************************************************************************************
* long_to_string_lz()
*
* Description : Convert a "long" to a null-terminated string, with leading zeros
*               (base = decimal)
* Arguments   : input = number to be converted
*               str = pointer to string (i.e. display buffer)
*               numdigits = number of digits to display
* Returns     : none
*********************************************************************************************************
*/
void LCD_vLong2StringLZ (unsigned long input, char *str, char numdigits)
{
    char digit;

    for (digit=numdigits; digit > 0; digit--) {
      str[digit-1] = (input % 10) + '0';
      input = input / 10;
    }
    str[numdigits] = 0;    // null-terminate the string
}



/*
*********************************************************************************************************
* uword_to_hexstring()
*
* Description : Convert an unsigned int to a null-terminated string
*               (base = hexidecimal)
* Arguments   : input = number to be converted
*               str = pointer to string (i.e. display buffer)
*               numdigits = number of digits to display
* Returns     : none
*********************************************************************************************************
*/
void LCD_vUword2Hexstring (unsigned int input, char *str, char numdigits)
{
     char digit;

     for (digit=numdigits; digit > 0; digit--) {
         char temp = input % 0x10;
         if (temp <= 0x09)
            str[digit-1] = temp + '0';
         else
            str[digit-1] = temp - 0x0a + 'A';
         input = input / 0x10;
     }
     str[numdigits] = 0;    // null-terminate the string
}


/*
*********************************************************************************************************
* hexstring_to_uword()
*
* Description : Convert a null-terminated string (base = hexidecimal)
*               to an unsigned int (16 bits);
* Arguments   : str = pointer to string
* Returns     : result
* Note        : Adapted from a function in the "Snippets" collection, by Bob Stout.
*********************************************************************************************************
*/
unsigned int LCD_uwHexstring2uword (char *str)
{
      unsigned int i, j = 0;

      while (str && *str && isxdigit(*str)) {
         i = *str++ - '0';
         if (9 < i)
            i -= 7;
         j <<= 4;
         j |= (i & 0x0f);
      }
      return(j);
}



/*
** LCD_DisplayString: Display a string at the specified row and column.
*/
void LCD_vDisplayStringPos (char row, char column, char *string)
{
	LCD_vCursorPos (row, column);
	while (*string)
		LCD_vDisplayCharacter (*string++);
}



/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
  ³    function: LCD / write string                                          ³
  ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
  ³      author: Michael Knese                                               ³
  ³        date: 28.12.1997                                                  ³
  ³     version: 1.1                                                         ³
  ³ last change: 28.06.1998                                                  ³
  ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
  ³      return: /                                                           ³
  ³          in: s, string to be displayed  (Zero terminated string)         ³
  ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
  ³ description:                                                             ³
  ³ write string to dataregister                                             ³
  ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

void LCD_vDisplayString (char *s)
{
  while (*s)
  {                                                     // transmit all characters
    while (LCD_bBusyFlag()==1);
    WRITE_Data = (*s);
    s++;                                                // next character
    }

}



/*
** LCD_DisplayStringCentered: Display a string centered on the specified row.
*/
void LCD_vDisplayStringCentered (char row, char *string)
{
	char len = strlen (string);

	if (len <= 16) {
		// if the string is less than one line, center it ...
		char i;
		LCD_vCursorPos (row, 1);
		for (i=0; i<16; i++)
			LCD_vDisplayCharacter (' ');
		LCD_vDisplayStringPos(row,((16 - len) / 2)+1,string);
	}
	else {
		// if the string is more than one line, display first 20 characters
		char temp[17];
		strncpy(temp, string, 16);
		temp[16] = 0;
		LCD_vDisplayStringPos(row,1,temp);
	}
}



/*
** LCD_DisplayScreen: Display an entire screen (80 characters).
**
**  inputs: ptr = pointer to a string containing the entire screen
**
**  example:
**		char screen[] = "01234567890123456789"
**				" This is a test of  "
**				"LCD_DisplayScreen()."
**				"   How's it look?   ";
**		DisplayScreen(screen);
**
*/
void LCD_vDisplayScreen (char *ptr)
{
	LCD_vDisplayRow(1,ptr+ 0);
	LCD_vDisplayRow(2,ptr+16);
	//LCD_DisplayRow(3,ptr+40);
	//LCD_DisplayRow(4,ptr+60);
}


/*
** LCD_WipeOnLR: Display an entire screen (80 characters) by
**               "wiping" it on (left to right).
**
**  inputs: ptr = pointer to a string containing the entire screen
**
*/
void LCD_vWipeOnLR (char *ptr)
{
	// "wipe" on new screen
	char i;
	for (i=0; i<16; i++) {
		LCD_vCursorPos(1,i+1);
		LCD_vDisplayCharacter(*(ptr+ 0+i));
		LCD_vCursorPos(2,i+1);
		LCD_vDisplayCharacter(*(ptr+16+i));
		//LCD_vCursorPos(3,i+1);
		//LCD_DisplayCharacter(*(ptr+40+i));
		//LCD_vCursorPos(4,i+1);
		//LCD_DisplayCharacter(*(ptr+60+i));
	}
}


/*
** LCD_WipeOnLR: Display an entire screen (80 characters) by
**               "wiping" it on (right to left).
**
**  inputs: ptr = pointer to a string containing the entire screen
**
*/
void LCD_vWipeOnRL (char *ptr)
{
	// "wipe" on new screen
	char i;
	for (i=16; i>0; i--) {
		LCD_vCursorPos(1,i);
		LCD_vDisplayCharacter(*(ptr+ 0+i-1));
		LCD_vCursorPos(2,i);
		LCD_vDisplayCharacter(*(ptr+16+i-1));
		//LCD_vCursorPos(3,i);
		//LCD_DisplayCharacter(*(ptr+40+i-1));
		//LCD_vCursorPos(4,i);
		//LCD_DisplayCharacter(*(ptr+60+i-1));
	}
}


/*
** LCD_WipeOffLR: "Wipe" screen left-to-right.
*/
void LCD_vWipeOffLR (void)
{
	// "wipe" off old screen (left to right)
	char i;
	for (i=1; i<17; i++) {
		#define BLOCK 0xff
		LCD_vCursorPos(1,i);
		LCD_vDisplayCharacter(BLOCK);
		LCD_vCursorPos(2,i);
		LCD_vDisplayCharacter(BLOCK);
		//LCD_vCursorPos(3,i);
		//LCD_DisplayCharacter(BLOCK);
		//LCD_vCursorPos(4,i);
		//LCD_DisplayCharacter(BLOCK);
	}
}


/*
** LCD_WipeOffRL: "Wipe" screen right-to-left.
*/
void LCD_vWipeOffRL (void)
{
	// "wipe" off old screen (right to left)
	char i;
	for (i=16; i>0; i--) {
		#define BLOCK 0xff
		LCD_vCursorPos(1,i);
		LCD_vDisplayCharacter(BLOCK);
		LCD_vCursorPos(2,i);
		LCD_vDisplayCharacter(BLOCK);
		//LCD_vCursorPos(3,i);
		//LCD_DisplayCharacter(BLOCK);
		//LCD_vCursorPos(4,i);
		//LCD_DisplayCharacter(BLOCK);
	}
}



/*
** LCD_DisplayRow: Display a string at the specified row.
*/
void LCD_vDisplayRow (char row, char *string)
{
	char i;
	LCD_vCursorPos (row, 1);
	while (LCD_bBusyFlag()==1);
	for (i=0; i<16; i++)
		LCD_vDisplayCharacter (*string++);
}


/*
** LCD_CursorLeft: Move the cursor left by one character.
*/
void LCD_vCursorLeft (void)
{
	while (LCD_bBusyFlag()==1);
	Befehle = (0x10);
}


/*
** LCD_CursorRight: Move the cursor right by one character.
*/
void LCD_vCursorRight (void)
{
	while (LCD_bBusyFlag()==1);
	Befehle = (0x14);
}




/*
** LCD_DefineCharacter: Define dot pattern for user-defined character.
**
**  inputs: address = address of character (0x00-0x07)
**          pattern = pointer to 8-byte array containing the dot pattern
*/
void LCD_vDefineChar (char address, const unsigned char *pattern)
{
	char i;

	while (LCD_bBusyFlag()==1);
	Befehle = (0x40 + (address << 3));
	for (i=0; i<8; i++) {
		//LCD_vWriteData(*pattern++);
                LCD_vDisplayCharacter(*pattern++);
	}
}


//
//write data to the LCD display 
//

// void LCD_vWriteData(ubyte data)
// {
//	while (LCD_bBusyFlag()==1);
//	WRITE_Data = data;
//}


//
//write data to the LCD display 
//

void LCD_vDisplayCharacter(unsigned char in_data)
{
	while (LCD_bBusyFlag()==1);
	WRITE_Data = in_data;
}




//////////////////////////////////////////////////////////////////////////////////
//	Write a text on the display             		                //
//	text:        SIEMENS  HOT                 				//
//                   Data:
//	parameter: 								//
//////////////////////////////////////////////////////////////////////////////////

void LCD_text (void)
{                                  
        LCD_vClear();

        LCD_vDisplayStringPos(1,1,"SIEMENS  HOT");
	    LCD_vDisplayStringPos(2,1,"Data:");
 }


// USER CODE BEGIN (LCD_General,3)

// USER CODE END
