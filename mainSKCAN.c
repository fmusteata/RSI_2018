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

/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
 	// Reseteaza/Initializeaza SK-CAN
	LCD_vInit();
	LCD_vDisplayString("Init SKCAN - Done");

	MCOHW_Init(125);
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
	
  	// Reseteaza/Initializeaza communicatia CANopen
  	MCOUSER_ResetCommunication();
	LCD_vDisplayString("Init MCO - Done");
		
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

    	/*trimite in ecou toate celelalte valori de la intrare la iesire
	
	    // digitale
	    gProcImg[IN_digi_2] = gProcImg[OUT_digi_2];
	    gProcImg[IN_digi_3] = gProcImg[OUT_digi_3];
	    gProcImg[IN_digi_4] = gProcImg[OUT_digi_4];
	
	    // analogice
	    gProcImg[IN_ana_1]   = gProcImg[OUT_ana_1];
	    gProcImg[IN_ana_1+1] = gProcImg[OUT_ana_1+1];
	    gProcImg[IN_ana_2]   = gProcImg[OUT_ana_2];
	    gProcImg[IN_ana_2+1] = gProcImg[OUT_ana_2+1];	*/
	
	    // Lanseaza prelucarea specifica retelei CANOpen
	    MCO_ProcessStack();
		LCD_vDisplayString("Init MCO - Done");

  	} // end of while(1)
} // end of main

