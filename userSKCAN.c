/**************************************************************************
MODULE:    USER
CONTAINS:  MicroCANopen Object Dictionary and Process Image implementation
COPYRIGHT: Embedded Systems Academy, Inc. 2002-2003.
           All rights reserved. www.microcanopen.com
           This software was written in accordance to the guidelines at
           www.esacademy.com/software/softwarestyleguide.pdf
DISCLAIM:  Read and understand our disclaimer before using this code!
           www.esacademy.com/disclaim.htm
LICENSE:   Users that have purchased a license for PCANopenMagic
           (www.esacademy.com/software/pcanopenmagic)
           may use this code in commercial projects.
           Otherwise only educational use is acceptable.
VERSION:   1.21, Pf/Aa/Ck 16-OCT-03
 		 $LastChangedDate: 2016-10-01 $
           $LastChangedRevision: 235 $
***************************************************************************/ 

#include <mco.h>
#include "rskcanio.h"
#include <string.h>

/*OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO*/
#include <reg515C.h>		// Pentru Keil

// Se asigurã ca numarul de TPDO si RPDO este corect
#if (NR_OF_RPDOS != 2)
  #if (NR_OF_TPDOS != 2)
#error Acest exemplu este numai pentru 2 TPDO si 2 RPDO!
  #endif
#endif

#define	ADR_NOD	4		   // adresa nodului slave CANopen

// MACROURI PENTRU INTRARILE IN OD
#define GETBYTE(val,pos) ((val >> pos) & 0xFF)
#define GETBYTES16(val) GETBYTE(val, 0), GETBYTE(val, 8)
#define GETBYTES32(val) GETBYTE(val, 0), GETBYTE(val, 8), GETBYTE(val,16), GETBYTE(val,24)
#define SDOREPLY(index,sub,len,val) 0x43 | ((4-len)<<2), GETBYTES16(index), sub, GETBYTES32(val)
#define SDOREPLY4(index,sub,len,d1,d2,d3,d4) 0x43 | ((4-len)<<2), GETBYTES16(index), sub, d1, d2, d3, d4

#define ODENTRY(index,sub,len,offset) {index, sub, len, offset}

// variabile globale

// Aceasta structura contine imaginea procesului specifica nodului
UNSIGNED8 MEM_NEAR gProcImg[PROCIMG_SIZE];

// Tabel cu raspunsurile SDO la operatiile de citire a OD
// Fiecare linie are 8 octeti:
// 		Command Specifier pentru raspunsul SDO (1 octet)
//   		bitii 2+3 contin: '4' – {number of data bytes}
// 		Object Dictionary Index (2 octeti, low first)
// 		Object Dictionary Subindex (1 byte)
// 		Data (4 bytes, lowest bytes first)
UNSIGNED8 MEM_CONST SDOResponseTable[] = {

  // [1000h,00]: Device Type
  SDOREPLY(0x1000, 0x00, 4, OD_DEVICE_TYPE), /* index, subindex, lng, valoare */

#ifdef OD_SERIAL
  // [1018h,00]: Identity Object, Number of Entries = 4		
  SDOREPLY(0x1018, 0x00, 1, 0x00000004L),	/* index, subindex, lng, valoare */
#else
  // [1018h,00]: Identity Object, Number of Entries = 3
  SDOREPLY(0x1018, 0x00, 1, 0x00000003L),	/* index, subindex, lng, valoare */
#endif

  // [1018h,01]: Identity Object, Vendor ID
  SDOREPLY(0x1018, 0x01, 4, OD_VENDOR_ID),	/* index, subindex, lng, valoare */

  // [1018h,02]: Identity Object, Product Code
  SDOREPLY(0x1018, 0x02, 4, OD_PRODUCT_CODE),	 /* index, subindex, lng, valoare */

  // [1018h,03]: Identity Object, Revision
  SDOREPLY(0x1018, 0x03, 4, OD_REVISION),  /* index, subindex, lng, valoare */

#ifdef OD_SERIAL
  // [1018h,04]: Identity Object, Serial
  SDOREPLY(0x1018, 0x04, 4, OD_SERIAL),	   /* index, subindex, lng, valoare */
#endif

  // [2018h,00]: MicroCANopen Identity Object, Number of Entries = 3
  SDOREPLY(0x2018, 0x00, 1, 0x00000003L),  /* index, subindex, lng, valoare */

  // [2018h,01]: MicroCANopen Identity Object, Vendor ID = 01455341, ESA Inc.
  SDOREPLY(0x2018, 0x01, 4, 0x01455341L),  /* index, subindex, lng, valoare */

  // [2018h,02]: MicroCANopen Identity Object, Product Code = "MCOP"
  SDOREPLY4(0x2018, 0x02, 4, 'P', 'O', 'C', 'M'),  /* index, subindex, lng, valoare */

  // [2018h,03]: MicroCANopen Identity Object, Revision = 1.20
  SDOREPLY(0x2018, 0x03, 4, 0x00010020L),	/* index, subindex, lng, valoare */

#ifdef PDO_IN_OD
  // NOTA: Aceste intrari trebuie editate manual. Parametrii trebuie sa se potriveasca  
  // cu parametrii folositi pentru a apela functiile MCO_InitRPDO and MCO_InitTPDO.

  /* RPDO1 Communication Parameter - Number of Entries */
  SDOREPLY(0x1400, 0x00, 1, 0x00000002L), /* index, subindex, lng, valoare */ 
  /* RPDO1 Communication Parameter - COB-ID */
  SDOREPLY(0x1400, 0x01, 4, 0x00000207L), /* index, subindex, lng, valoare */
  /* RPDO1 Communication Parameter - Transmission Type */
  SDOREPLY(0x1400, 0x02, 1, 0x000000FFL), /* index, subindex, lng, valoare */

  /* RPDO2 Communication Parameter - Number of Entries */ 
  SDOREPLY(0x1401, 0x00, 1, 0x00000002L),  /* index, subindex, lng, valoare */
  /* RPDO2 Communication Parameter - COB-ID */
  SDOREPLY(0x1401, 0x01, 4, 0x00000307L),  /* index, subindex, lng, valoare */
  /* RPDO1 Communication Parameter - Transmission Type */
  SDOREPLY(0x1401, 0x02, 1, 0x000000FFL),	/* index, subindex, lng, valoare */

  /* RPDO1 Mapping Parameter - Number of Entries */
  SDOREPLY(0x1600, 0x00, 1, 0x00000004L),	/* index, subindex, lng, valoare */
  /* RPDO1 Mapping Parameter - PDO Mapping Entry */
  SDOREPLY(0x1600, 0x01, 4, 0x62000108L),  /* index, subindex, lng, valoare */
  /* RPDO1 Mapping Parameter - PDO Mapping Entry */
  SDOREPLY(0x1600, 0x02, 4, 0x62000208L),	/* index, subindex, lng, valoare */
  /* RPDO1 Mapping Parameter - PDO Mapping Entry */
  SDOREPLY(0x1600, 0x03, 4, 0x62000308L),	/* index, subindex, lng, valoare */
  /* RPDO1 Mapping Parameter - PDO Mapping Entry */
  SDOREPLY(0x1600, 0x04, 4, 0x62000408L),	/* index, subindex, lng, valoare */

  /* RPDO2 Mapping Parameter - Number of Entries */
  SDOREPLY(0x1601, 0x00, 1, 0x00000002L),  /* index, subindex, lng, valoare */
  /* RPDO2 Mapping Parameter - PDO Mapping Entry */
  SDOREPLY(0x1601, 0x01, 4, 0x64110110L),  /* index, subindex, lng, valoare */
  /* RPDO2 Mapping Parameter - PDO Mapping Entry */
  SDOREPLY(0x1601, 0x02, 4, 0x64110210L),  /* index, subindex, lng, valoare */

// TPDO1 communication parameters
  /* TPDO1 Communication Parameter - Number of Entries */
  SDOREPLY(0x1800, 0x00, 1, 0x00000002L),   /* index, subindex, lng, valoare */
  /* TPDO1 Communication Parameter - COB-ID */
  SDOREPLY(0x1800, 0x01, 4, 0x40000187L),	/* index, subindex, lng, valoare */
  /* TPDO1 Communication Parameter - Transmission Type */
  SDOREPLY(0x1800, 0x02, 1, 0x000000FFL),   /* index, subindex, lng, valoare */
  /* TPDO1 Communication Parameter - Inhibit Time */
  SDOREPLY(0x1800, 0x03, 2, 0L),		    /* index, subindex, lng, valoare */
  /* TPDO1 Communication Parameter - Event Timer */
  SDOREPLY(0x1800, 0x05, 2, 100L),		    /* index, subindex, lng, valoare */

// TPDO2 Communication Parameters
  /* TPDO2 Communication Parameter - Number of Entries */
  SDOREPLY(0x1801, 0x00, 1, 0x00000002L),	/* index, subindex, lng, valoare */
  /* TPDO2 Communication Parameter - COB-ID */
  SDOREPLY(0x1801, 0x01, 4, 0x40000287L),	/* index, subindex, lng, valoare */
  /* TPDO2 Communication Parameter - Transmission Type */
  SDOREPLY(0x1801, 0x02, 1, 0x000000FFL),	/* index, subindex, lng, valoare */
  /* TPDO2 Communication Parameter - Inhibit Time */
  SDOREPLY(0x1801, 0x03, 2, 50L),		    /* index, subindex, lng, valoare */
  /* TPDO2 Communication Parameter - Event Timer */
  SDOREPLY(0x1801, 0x05, 2, 100L),			/* index, subindex, lng, valoare */

// TPDO1 Mapping Parameters
  /* TPDO1 Mapping Parameter - Number of Entries */
  SDOREPLY(0x1A00, 0x00, 1, 0x00000004L),
  /* TPDO1 Mapping Parameter - PDO Mapping Entry */
  SDOREPLY(0x1A00, 0x01, 4, 0x60000108L),
  /* TPDO1 Mapping Parameter - PDO Mapping Entry */
  SDOREPLY(0x1A00, 0x02, 4, 0x60000208L),
  /* TPDO1 Mapping Parameter - PDO Mapping Entry */
  SDOREPLY(0x1A00, 0x03, 4, 0x60000308L),
  /* TPDO1 Mapping Parameter - PDO Mapping Entry */
  SDOREPLY(0x1A00, 0x04, 4, 0x60000408L),

// TPDO2 Mapping Parameters
  /* TPDO2 Mapping Parameter - Number of Entries */
  SDOREPLY(0x1A01, 0x00, 1, 0x00000002L),
  /* TPDO2 Mapping Parameter - PDO Mapping Entry */
  SDOREPLY(0x1A01, 0x01, 4, 0x64010110L),
  /* TPDO2 Mapping Parameter - PDO Mapping Entry */
  SDOREPLY(0x1A01, 0x02, 4, 0x64010210L),

  // Nr. de intrari pentru subindex zero ale tabelelor folosite in gODProcTable
  SDOREPLY(0x6000, 0x00, 1, 0x00000004L),
  SDOREPLY(0x6200, 0x00, 1, 0x00000004L),
  SDOREPLY(0x6401, 0x00, 1, 0x00000002L),
  SDOREPLY(0x6411, 0x00, 1, 0x00000002L),
#endif // PDO_IN_OD

  // End-of-table marker
  SDOREPLY(0xFFFF, 0xFF, 0xFF, 0xFFFFFFFFL)
};

#ifdef PROCIMG_IN_OD
// Tabel cu intrarile OD pentru datele din proces
OD_PROCESS_DATA_ENTRY MEM_CONST gODProcTable[] = {

  // IN_digi
  /* mapping entries for TPDO1 [1A00] */
  ODENTRY(0x6000,0x01,1+ODRD,IN_digi_1),
  ODENTRY(0x6000,0x02,1+ODRD,IN_digi_2),
  ODENTRY(0x6000,0x03,1+ODRD,IN_digi_3),
  ODENTRY(0x6000,0x04,1+ODRD,IN_digi_4),

  // IN_ana
  /* mapping entries for TPDO2 [1A01] */
  ODENTRY(0x6401,0x01,2+ODRD,IN_ana_1),
  ODENTRY(0x6401,0x02,2+ODRD,IN_ana_2),

  // OUT_digi
  /* mapping entries for RPDO1 [1600] */
  ODENTRY(0x6200,0x01,1+ODRD+ODWR,OUT_digi_1),
  ODENTRY(0x6200,0x02,1+ODRD+ODWR,OUT_digi_2),
  ODENTRY(0x6200,0x03,1+ODRD+ODWR,OUT_digi_3),
  ODENTRY(0x6200,0x04,1+ODRD+ODWR,OUT_digi_4),

  // OUT_ana
  /* mapping entries for RPDO2 [1601] */
  ODENTRY(0x6411,0x01,2+ODRD+ODWR,OUT_ana_1),
  ODENTRY(0x6411,0x02,2+ODRD+ODWR,OUT_ana_2),

  // End-of-table marker
  ODENTRY(0xFFFF,0xFF,0xFF,0xFF)
};
#endif // PROCIMG_IN_OD

/**************************************************************************
DOES:    Aceasta functie este apelata daca a aparut o eroare fatala.
		 Codurile de eroare pentru mcohwxxx.c sunt in domeniul 0x8000 - 0x87FF.
		 Codurile de eroare pentru mco.c sunt in domeniul 0x8800 to 0x8FFF. 
         Celelalte coduri de eroare pot fi folosite de aplicatie.
     
RETURNS: nimic
**************************************************************************/
void MCOUSER_FatalError
  (
  UNSIGNED16 ErrCode  // codul de eroare
  )
{
  //afiseaza codul de eroare 
  error_state(ErrCode & 0xFF);
}

/**************************************************************************
DOES:    Functie call-back pentru resetarea aplicatiei.
         Porneste ceasul de garda si asteapta pana acesta determina un reset.
RETURNS: nimic
**************************************************************************/
void MCOUSER_ResetApplication
  (
  void
  )
{
  // Resetarea de catre WDT nu e implementata in acest exemplu
  // MCOUSER_ResetCommunication();
}

/**************************************************************************
DOES:   Aceasta functie reseteaza si initializeaza controlerul CAN si stiva CANOpen.
		Este apelata de catre stiva CANOpen, daca un mesaj NMT solicita resetarea comunicatiei. 
		This function both resets and initializes both the CAN interface
        Aceasta functie trebuie sa apeleze MCO_Init si MCO_InitTPDO/MCO_InitRPDO.
RETURNS: nothing
**************************************************************************/
void MCOUSER_ResetCommunication
  (
  void
  )
{
  UNSIGNED8 i;

  EAL = 0;

  // Initializeaza (cu 0) variabilele de proces
  for (i = 0; i < PROCIMG_SIZE; i++)
  {
    gProcImg[i] = 0;
  }

  // 125kbit, Adresa nod, 1s heartbeat
  MCO_Init(125,ADR_NOD,1000); 
  
  // RPDO1, default ID (0x200+nodeID), 4 octeti
  MCO_InitRPDO(1,0x183,4,OUT_digi_1); 

  // RPDO2, default ID (0x300+nodeID), 4 octeti
  MCO_InitRPDO(2,0x286,4,OUT_ana_1); 

  // TPDO1, default ID (0x180+nodeID), 100ms event, 0ms inhibit, 4 bytes
  MCO_InitTPDO(1,0,100,0,4,IN_digi_1);   
  
  // TPDO2, default ID (0x280+nodeID), 1250ms event, 10ms inhibit, 4 bytes
  MCO_InitTPDO(2,0,250,10,4,IN_ana_1);
}

