/**************************************************************************
MODULE:    MCOHWSKCAN
CONTAINS:  Preliminary, limited hardware driver implementation for
           Infineon C515C - tested using the MicroPac C515C board.
           This version was tested with the Keil compiler system.
           www.keil.com
           This version re-uses functions provided by
           www.esacademy.com/faq/progs
COPYRIGHT: Embedded Systems Academy, Inc. 2002-2003
           All rights reserved. www.microcanopen.com
           This software was written in accordance to the guidelines at
	   	   www.esacademy.com/software/softwarestyleguide.pdf
DISCLAIM:  Read and understand our disclaimer before using this code!
           www.esacademy.com/disclaim.htm
LICENSE:   Users that have purchased a license for PCANopenMagic
           (www.esacademy.com/software/pcanopenmagic)
           may use this code in commercial projects.
           Otherwise only educational use is acceptable.
VERSION:   1.11, Pf/Aa/Ck 13-OCT-03
		 $LastChangedDate: 2016-10-01 $
           $LastChangedRevision: 232 $
---------------------------------------------------------------------------
Known shortcoming:
Only supports a transmit queue of length "1"
If queue occupied, waits until it is clear
***************************************************************************/ 




#include <reg515C.h>
#include "mcohwSKCAN.h"

// Placa de dezvoltare suportata de acest driver
#define SKCAN  1   // SK-CAN board

// precizeaza tipul placii utilizate
#define BOARDTYPE SKCAN

// definesc offset-ul pentru CAN intern
#define CAN_OFFSET 0xFD00



// variabila globala timer/conter, incrementata la fiecare milisecunda
UNSIGNED16 MEM_NEAR gTimCnt = 0;

// Numara filtrele folosite(obiectele de tip mesaj CAN)
UNSIGNED8 MEM_FAR gCANFilter = 0;

#define CAN_OFFSET_81 0xFD00

// Structura unui obiect
struct can_con_81
{
  UNSIGNED8  BL1;      	//Bit-length register
  UNSIGNED8  BL2;      	//
  UNSIGNED8  OC;      	//Output-control register
  UNSIGNED8  BRPR;      	//Baud-rate prescaler
  UNSIGNED8  RRR1;     	//Receive-ready register
  UNSIGNED8  RRR2;     	//
  UNSIGNED8  RIMR1;    	//Receive-interrupt-mask register
  UNSIGNED8  RIMR2;    	//
  UNSIGNED8  TRSR1;     	//Transmit-request-set register
  UNSIGNED8  TRSR2;     	//
  UNSIGNED8  IMSK;      	//Interrupt-mask register
  UNSIGNED8  res0B;      	//
  UNSIGNED8  res0C;      	//
  UNSIGNED8  res0D;      	//
  UNSIGNED8  res0E;      	//
  UNSIGNED8  res0F;      	//

  UNSIGNED8  MOD;      	//Mode/status register
  UNSIGNED8  INT;      	//Interrupt register
  UNSIGNED8  CTRL;      	//Control register
  UNSIGNED8  res13;      	//
  UNSIGNED8  CC;      	//Clock-control register
  UNSIGNED8  TCEC;      	//Transmit-check error counter
  UNSIGNED8  TCD;      	//Transmit-check data register
  UNSIGNED8  res17;      	//
  UNSIGNED8  TRR1;      	//Transmit-request-reset register
  UNSIGNED8  TRR2;      	//
  UNSIGNED8  RRPR1;      	//Remote-request-pending register
  UNSIGNED8  RRPR2;      	//
  UNSIGNED8  TSCH;      	//Time-Stamp counter
  UNSIGNED8  TSCL;      	//
  UNSIGNED8  res1E;      	//
  UNSIGNED8  res1F;      	//

  UNSIGNED8  res20;      	//
  UNSIGNED8  res21;      	//
  UNSIGNED8  res22;      	//
  UNSIGNED8  res23;      	//
  UNSIGNED8  res24;      	//
  UNSIGNED8  res25;      	//
  UNSIGNED8  res26;      	//
  UNSIGNED8  res27;      	//
  struct {
              UNSIGNED8 PDR;     //port-direction register
              UNSIGNED8 PR;	     //pin register
	      UNSIGNED8 LR;	     //latch register
	      UNSIGNED8 resP;    //
         }Port[2];      //
  struct {
              UNSIGNED8 H;
              UNSIGNED8 L;
         }TSR[8];      	//Time-Stamp Register
  struct {
              UNSIGNED8 H;
              UNSIGNED8 L;
         }DR[16];       //Descriptor Register
  UNSIGNED8 res60_7F[32];      
  struct {
              UNSIGNED8 Data[8];

         }MsgObj[16];	//Data Register
  
};

#define CAN_CON ((struct can_con_81 volatile xdata *) (CAN_OFFSET_81))
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/

/**************************************************************************
DOES:    Preia urmatorul mesaj CAN receptionat si il depune intr-un buffer de receptie
RETURNS: 0 daca nu s-a primit nici un mesaj, 1 daca mesajul primit a fost copiat in buffer                                     
-corectat-
**************************************************************************/
UNSIGNED8 MCOHW_PullMessage
  (
  CAN_MSG MEM_FAR *pReceiveBuf  // pointer la un buffer pentru un singur mesaj de stocat
  )
{
  // declaratii de variabile
//  UNSIGNED32 Identifier;
//  UNSIGNED8  Length;
//  UNSIGNED8  i,j;
	UNSIGNED8  j;

  // testeaza obiectele de receptie	definite
  for (j=1; j<=gCANFilter; j++)
  {

/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
    // verifica daca a fost primit vreun mesaj
    if (1)//(((CAN_CON->mesaj[j].MCRH) & 0x03) == 0x02)	   // NEWDAT?
	{
      
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/

      // return 1 - message received
      return (1);
    }
  }

  // return 0 - no message received
  return (0);
}


/**************************************************************************
DOES:    Transmite un mesaj CAN                                  
RETURNS: 0 daca messajul nu a putut fi transmis, 1 daca mesajul a fost transmis                                
**************************************************************************/
UNSIGNED8 MCOHW_PushMessage
  (
  CAN_MSG MEM_FAR *pTransmitBuf  // pointer la buffer-ul ce contine mesajul CAN de transmis
  )
{
  // Identificatorul mesajului CAN
  UNSIGNED32 Identifier;
  // numarul de octeti de date
  UNSIGNED8  Length;
  // contor local
//  UNSIGNED8  i;

  // Pregateste DLC si ID-ul CAN
  Length     = pTransmitBuf->LEN;
  Identifier = pTransmitBuf->ID;

/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
  
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/

  // mesaj transmis (s-a solicitat transmisia)
  return 1;
}


/**************************************************************************
DOES:    Preia valoarea timerului sistemului de 1 milisecunda 
RETURNS: Valoarea curenta a timpului                                   
**************************************************************************/
UNSIGNED16 MCOHW_GetTime
  (
  void
  )
{
  UNSIGNED16 tmp;

  // dezactiveaza intreruperile
  EAL = 0;

  // citeste valoarea timerului
  tmp = gTimCnt;

  // valideaza intreruperile
  EAL = 1;

  // returneaza valoarea timerului
  return tmp;
}

/**************************************************************************
DOES:    Verifica daca un moment de timp a trecut (timpul a expirat)
RETURNS: 0 daca timpul nu a expirat inca, 1 daca timpul a expirat             
**************************************************************************/
UNSIGNED8 MCOHW_IsTimeExpired
  (
  UNSIGNED16 timestamp  // valoare timp de testat daca a expirat
  )
{
  UNSIGNED16 time_now;

  // dezactiveaza intreruperile
  EAL = 0;
  // citeste valoarea timerului
  time_now = gTimCnt;
  // valideaza intreruperile
  EAL = 1;
  timestamp++;
  // daca timestamp <= timpul curent...
  if (time_now > timestamp)
  {
    // verifica daca timestamp a expirat
    if ((time_now - timestamp) < 0x8000)
    {
      return 1;
    }
	// timestamp nu a expirat
	else
	{
      return 0;
    }
  }
  // daca timestamp >= timpul curent....
  else
  {
    // verifica daca timestamp a expirat
    if ((timestamp - time_now) > 0x8000)
    {
      return 1;
    }
	// timestamp nu a expirat
    else
    {
      return 0;
    }
  }
}

/**************************************************************************
DOES:    Functia de intrerupere a timerului                          
         Incrementeaza numaratorul global de milisecunde           
         Aceasta functie trebuie apelata o data la fiecare milisecunda 
RETURNS: nimic                                                     
**************************************************************************/
void MCOHW_TimerISR
  (
  void
  ) interrupt 1
{

/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
  // stop timer 0
  TR0 = 0;

#if (BOARDTYPE == SKCAN)
  // reincarcare pentru 1ms at 10MHz, modul 1	   // de recalculat
  TH0 = 0xF9;
  TL0 = 0x8E;
#endif
  // start timer 0
  TR0 = 1;
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/


  // incrementeaza contorul global
  gTimCnt++;
}

/**************************************************************************
DOES:    Initializeaza controlerul CAN.                           
CAUTION: Nu initializeaza filtrele - nimic nu se receptioneaza inca          
RETURNS: 0 daca initializarea esueaza, 1 pentru succes                 
**************************************************************************/

UNSIGNED8 MCOHW_Init			 
  (
  UNSIGNED16 BaudRate  // viteza dorita in kbps
  )
{
//  UNSIGNED8 i;
  UNSIGNED8 baudrateok = 0;


/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
 //  -------------- CAN Mode/Status Register ---------------
  //  start the initialization of the CAN Module
  CAN_CON[1].MOD   = 0x001;

  //  -------------- Output-Control Register ----------------
  //  Output Mode:  Normal Mode
  //  TX0: The Output is driven directly with CAN data
  //       The high and low side transistor T0P/T0N are enable
  //  TX1: The Output is driven directly with CAN data
  //       The high and low side transistor T1P/T1N are enable
  CAN_CON[1].OC    = 0x0D8;

  //  --------------- CAN Control Register ------------------
  CAN_CON[1].CTRL  = 0x000;

  //  ---------------- Interrupt Register -------------------
  CAN_CON[1].INT   = 0x000;  //reset all interrupt request bits
  CAN_CON[1].IMSK  = 0x000;  //no interrupts (Interrupt mask register) 
  
  CAN_CON[1].RRR1  = 0x000;  //reset receive-Ready Register
  CAN_CON[1].RRR2  = 0x000;  //        -"-
  CAN_CON[1].RIMR1 = 0x000;  //disable all receive Bits (Receive-Interrupt -Mask Register)
  CAN_CON[1].RIMR2 = 0x000;

  ///  ------------ Bit Timing Register ---------------------
  /// baudrate =  100.000 KBaud
  /// there are 5 time quanta before sample point
  /// there are 4 time quanta after sample point
  /// the (re)synchronization jump width is 2 time quanta 
  CAN_CON[1].BL1   = 0x034;
  CAN_CON[1].BL2   = 0x041;
  CAN_CON[1].BRPR  = 0x009;


  //  ------------- Port Control Register -------------------
  CAN_CON[1].Port[0].PDR = 0x0FF;  //Port Direction Register (direction is output)
  CAN_CON[1].Port[0].LR  = 0x0FF;  //Port Latch Register ()



  //  ================== Descriptor Registers ===============

  //  Message-Object 0
  CAN_CON[1].DR[0].H  = 0xFF;
  CAN_CON[1].DR[0].L  = 0xE0;

  //  Message-Object 1
  //   This message operates as a data frame
  //   Identifier = 0x7
  CAN_CON[1].DR[1].H  = 0xFF;	      //
  CAN_CON[1].DR[1].L  = 0xE0;        // 

  CAN_CON[1].MsgObj[1].Data[7] = 0xAA;   // set data byte 7
  CAN_CON[1].MsgObj[1].Data[6] = 0x11;   // set data byte 6
  CAN_CON[1].MsgObj[1].Data[5] = 0x22;   // set data byte 5
  CAN_CON[1].MsgObj[1].Data[4] = 0x33;   // set data byte 4
  CAN_CON[1].MsgObj[1].Data[3] = 0x44;   // set data byte 3
  CAN_CON[1].MsgObj[1].Data[2] = 0x55;   // set data byte 2
  CAN_CON[1].MsgObj[1].Data[1] = 0x66;   // set data byte 1
  CAN_CON[1].MsgObj[1].Data[0] = 0x77;   // set data byte 0


  //  Message-Object 2-15
  CAN_CON[1].DR[2].H  = 0xFF;
  CAN_CON[1].DR[2].L  = 0xE0;

  CAN_CON[1].DR[3].H  = 0xFF;
  CAN_CON[1].DR[3].L  = 0xE0;

  CAN_CON[1].DR[4].H  = 0xFF;
  CAN_CON[1].DR[4].L  = 0xE0;
		  
  CAN_CON[1].DR[5].H  = 0xFF;
  CAN_CON[1].DR[5].L  = 0xE0;

  CAN_CON[1].DR[6].H  = 0xFF;
  CAN_CON[1].DR[6].L  = 0xE0;
     
  CAN_CON[1].DR[7].H  = 0xFF;
  CAN_CON[1].DR[7].L  = 0xE0;

  CAN_CON[1].DR[8].H  = 0xFF;
  CAN_CON[1].DR[8].L  = 0xE0;

  CAN_CON[1].DR[9].H  = 0xFF;
  CAN_CON[1].DR[9].L  = 0xE0;

  CAN_CON[1].DR[10].H  = 0xFF;
  CAN_CON[1].DR[10].L  = 0xE0;

  CAN_CON[1].DR[11].H  = 0xFF;
  CAN_CON[1].DR[11].L  = 0xE0;

  CAN_CON[1].DR[12].H  = 0;
  CAN_CON[1].DR[12].L  = 0;

  CAN_CON[1].DR[13].H  = 0xFF;
  CAN_CON[1].DR[13].L  = 0xE0;

  CAN_CON[1].DR[14].H  = 0xFF;
  CAN_CON[1].DR[14].L  = 0xE0;

  CAN_CON[1].DR[15].H  = 0xFF;
  CAN_CON[1].DR[15].L  = 0xE0;


  /// ------------ CAN Control/Status Register --------------
  //  reset INIT
  //  ...
  CAN_CON[1].MOD       = 0x000;


  // initializare intrerupere T0 
  // MCOHW_TimerISR trebuie sa se execute la fiecare ms
  // stop timer 0
 // TR0     =  0;
  // mode 1
 // TMOD    |= 1;
   // reincarcare la 1ms pentru 10MHz, X2 mode
  //TH0 = 0xF9;
 // TL0 = 0x8E;

  // start timer 0
 // TR0     =  1;
  // enable timer 0
 // ET0     =  1;
/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/


  // rezultatul initializarii
  return baudrateok;
}

/**************************************************************************
DOES:    Initializeaza urmatorul filtru disponibil                    
RETURNS: 0 pentru esec, 1 pentru succes                 
**************************************************************************/
UNSIGNED8 MCOHW_SetCANFilter
  (
  UNSIGNED16 CANID  // identificatorul mesajului de receptionat
  )
{

/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/



/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/

   return 0;
}

