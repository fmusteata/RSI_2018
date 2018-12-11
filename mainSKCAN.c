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
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/

// declaratie externa pentru tabelul cu imaginea procesului
extern UNSIGNED8 MEM_NEAR gProcImg[];

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
		gProcImg[IN_digi_1] = P5_7;
	    gProcImg[IN_digi_2] = P5_6;
	    gProcImg[IN_digi_3] = P5_1;
	    gProcImg[IN_digi_4] = P5_0;
	
		if((ii % 200) == 0)
		{			
		 	gProcImg[IN_ana_1]   = ana1++;	
	
		}

		if((ii % 2000) == 0)
		{
	
			gProcImg[IN_ana_2]   = ana2++;
		}

	    // analogice
	  //  gProcImg[IN_ana_1]   = gProcImg[OUT_ana_1];
	   // gProcImg[IN_ana_1+1] = gProcImg[OUT_ana_1+1];
	   // gProcImg[IN_ana_2]   = gProcImg[OUT_ana_2];
	  //  gProcImg[IN_ana_2+1] = gProcImg[OUT_ana_2+1];	
	
	    // Lanseaza prelucarea specifica retelei CANOpen
	    MCO_ProcessStack();

  	} // end of while(1)
} // end of main

