/**************************************************************************
MODULE:    MAIN
CONTAINS:  Example application using MicroCANopen
           Written for MicroPac C515C board
COPYRIGHT: Embedded Systems Academy, Inc. 2002 - 2003
           All rights reserved. www.microcanopen.com
           This software was written in accordance to the guidelines at
	   	   www.esacademy.com/software/softwarestyleguide.pdf
DISCLAIM:  Read and understand our disclaimer before using this code!
           www.esacademy.com/disclaim.htm
LICENSE:   Users that have purchased a license for PCANopenMagic
           (www.esacademy.com/software/pcanopenmagic)
           may use this code in commercial projects.
           Otherwise only educational use is acceptable.
VERSION:   1.20, Pf/Aa/Ck/DM 13-OCT-03
		 $LastChangedDate: 2016-10-01 $
           $LastChangedRevision: 232 $
***************************************************************************/ 

#include <mco.h>
#include "rskcanio.h"
#include <reg515C.h>   // Pentru Keil
#include <mcohwskcan.h>

/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
#include <lcd.h>

#define B4 P5_0
#define B3 P5_1
#define B2 P5_6
#define B1 P5_7
/*OOOOOOOOOOOOOOOOOOOOOOOOOOO OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
void my_reverse(char str[], int len)
{
    int start, end;
    char temp;
    for(start=0, end=len-1; start < end; start++, end--) {
        temp = *(str+start);
        *(str+start) = *(str+end);
        *(str+end) = temp;
    }
}
char* my_itoa(int num, char* str, int base)
{
    int i = 0;
    char isNegative = 0;
  
    /* A zero is same "0" string in all base */
    if (num == 0) {
        str[i] = '0';
        str[i + 1] = '\0';
        return str;
    }
  
    /* negative numbers are only handled if base is 10 
       otherwise considered unsigned number */
    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }
  
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'A' : rem + '0';
        num = num/base;
    }
  
    /* Append negative sign for negative numbers */
    if (isNegative){
        str[i++] = '-';
    }
  
    str[i] = '\0';
 
    my_reverse(str, i);
  
    return str;
}

// declaratie externa pentru tabelul cu imaginea procesului
extern UNSIGNED8 MEM_NEAR gProcImg[];
	char a = 255;
/**************************************************************************
DOES:    Functia main
RETURNS: nimic
**************************************************************************/
void main
  (
  void
  )
{	
	UNSIGNED16 ii = 0;
	UNSIGNED16 ana1 = 0;
	UNSIGNED16 ana2 = 0;

	char ana11[5];
	char ana22[5];
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
 	// Reseteaza/Initializeaza SK-CAN
	LCD_vInit();
	

	MCOHW_Init(125);
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
	
  	// Reseteaza/Initializeaza communicatia CANopen
  	MCOUSER_ResetCommunication();
	
		
    // Valideaza intreruperile
	               
	



    EAL = 1;

  	// Bucla infinita
  	while(1)
  	{
    	// Actualizeaza datele de proces
    	// Primele intrari numerice sunt citite de la switch-uri
		//    gProcImg[IN_digi_1] = read_dip_switches();	
    	// Comanda primele iesiri digitale (pe led-uri)
		//   switch_leds(gProcImg[OUT_digi_1]); 
		
		ii++;  
	
	    // digitale
		gProcImg[IN_digi_1] = B1;
	    gProcImg[IN_digi_2] = B2;
	    gProcImg[IN_digi_3] = B3;
	    gProcImg[IN_digi_4] = B4;
	
		if((ii % 200) == 0)
		{			
		 	gProcImg[IN_ana_1]   = ana1++;	
	
		}

		if((ii % 2000) == 0)
		{
	
			gProcImg[IN_ana_2]   = ana2++;
		}

		LCD_vWrite_ubyteXY(0x0,0x0,gProcImg[OUT_digi_1]);
		LCD_vWrite_ubyteXY(0x4,0x0,gProcImg[OUT_digi_2]);
		LCD_vWrite_ubyteXY(0x8,0x0,gProcImg[OUT_digi_3]);
		LCD_vWrite_ubyteXY(0xC,0x0,gProcImg[OUT_digi_4]);

		//LCD_vUword2Hexstring(gProcImg[OUT_ana_1], &ana11, 5);
		//LCD_vUword2Hexstring(gProcImg[OUT_ana_2], &ana22, 5);

		LCD_vDisplayStringPos(2,0, my_itoa(gProcImg[OUT_ana_1], &ana11, 10));
		LCD_vDisplayStringPos(2,5, my_itoa(gProcImg[OUT_ana_2], &ana22, 10));
		//LCD_vDisplayStringPos(2,8, "BALA");

	    // Lanseaza prelucarea specifica retelei CANOpen
	    MCO_ProcessStack();

  	} // end of while(1)
} // end of main

