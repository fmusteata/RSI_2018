/**************************************************************************
NAME:    rC515Cio.h                                                   
INFO:    Header pentru placa de dezvoltare MicroPac cu C515C         
RIGHTS:  Embedded Systems Academy   www.esacademy.com               
---------------------------------------------------------------------------
DETAILS:                                                             
---------------------------------------------------------------------------
HISTORY: V1.01 AA 15-OCT-2003 Added support for Atmel CANopen board
         V1.00 Kl 09-OCT-2002
		 $LastChangedDate: 2016-10-01 $
         $LastChangedRevision: 45 $                                      
**************************************************************************/

#include <mco.h>


/**************************************************************************
DOES:    Citeste starea switch-urilor DIP              
RETURNS: Valoarea curenta a switch-urilor DIP                    
**************************************************************************/
UNSIGNED8 read_dip_switches
  (
  void
  );

/**************************************************************************
DOES:    Comanda starea LED-urilor                  
RETURNS: nothing                                                     
**************************************************************************/
void switch_leds
  (
  UNSIGNED8 on_off  // seteaza/reseteaza bitii pentru a aprinde/stinge led-urile
  );

/**************************************************************************
DOES:    Daca un program detecteaza o eroare fatala, aceasta rutina poate fi
		 folosita pentru a afisa un cod de eroare pentru utilizator.
RETURNS: nimic                                          
**************************************************************************/
void error_state
  (
  UNSIGNED8 error  // codul de eroare de afisat
  );

