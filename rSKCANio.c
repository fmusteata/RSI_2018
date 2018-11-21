/**************************************************************************
NAME:    rC515Cio.c                                                   
INFO:    Modul de I/E pentru placa MicroPac cu C515C               
RIGHTS:  Embedded Systems Academy   www.esacademy.com                
---------------------------------------------------------------------------
DETAILS:                                                             
---------------------------------------------------------------------------
HISTORY: V1.01 AA 15-OCT-2003 Added support for Atmel CANopen board
         V1.00 Kl 09-OCT-2002
		 $LastChangedDate: 2016-10-01 $
         $LastChangedRevision: 45 $                                  
**************************************************************************/

#include <reg515C.h> // Pentru Keil
#include <mco.h>
#include "rskcanio.h"
#include <lcd.h>
// Placa de dezvoltare suportata de acest driver
#define SKCAN  1   // SK_CAN board

// precizeaza ce placa se utilizeaza
#define BOARDTYPE SKCAN

/**************************************************************************
DOES:    Citeste intrarile switch-urilor DIP             
RETURNS: Valoarea curenta a switch-urilor DIP                    
**************************************************************************/
UNSIGNED8 read_dip_switches
  (
  void
  )
{
  UNSIGNED8 dip;

#if (BOARDTYPE == SKCAN)
  // citeste switch-urile
  dip = P1 & 0x1F;
#endif

  // returneaza valoarea citita de la switch-uri
  return (dip);
}

/**************************************************************************
DOES:    Scrie un octet in portul de iesire conectat la LED-uri                 
RETURNS: nimic                                                     
**************************************************************************/
void switch_leds
  (
  UNSIGNED8 errr  // codul de eroare ce va fi afisat
  )
{
	LCD_vDisplayString("Eroare la :");
	LCD_vDisplayCharacter(errr);
}


/**************************************************************************
DOES:   Daca un program detecteaza o eroare fatala, aceasta rutina poate fi
	    folosita pentru a afisa codul de eroare.
		ACEASTA FUNCTIE BLOCHEAZA EXECUTIA PROGRAMULUI. 
		NUMAI UN RESET REIA FUNCTIONAREA PLACII!                                                 
RETURNS: nimic                                          
**************************************************************************/
void error_state
  (
  UNSIGNED8 error  // valoarea erorii ce va fi afisata
   )
{
	UNSIGNED8 i;

  // dezactiveaza intreruperile
  EAL = 0;

  // bucla infinita	- eroarea poate fi afisata pe LCD
	 if(error>=8000)
	 { 
	 	 switch_leds(error);
	    for (i = 0; i <= 50000; i++){}	//delay
		MCOUSER_ResetApplication();
   	 }
  
}

