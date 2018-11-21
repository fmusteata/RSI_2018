/**************************************************************************
MODULE:    MCO
CONTAINS:  MicroCANopen implementation
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
VERSION:   2.00, Pf/Aa/Ck 21-OCT-03
		   $LastChangedDate: 2016-10-01 $
           $LastChangedRevision: 232 $
***************************************************************************/ 

#ifndef _MCO_H
#define _MCO_H

/**************************************************************************
DEFINES: MEMORY TYPES USED
**************************************************************************/

// Intrari constante in OD - de pus in memoria program
#define MEM_CONST const code
// Date din proces si variabile utilizate frecvent 
#define MEM_NEAR data
// Variabile utilizate mai rar
#define MEM_FAR xdata

/**************************************************************************
DEFINES: INTRARI CONSTANTE IN OBJECT DICTIONARY
Modificati aceste valori in functie de aplicatie
**************************************************************************/

#define OD_DEVICE_TYPE   0x000F0191L 
#define OD_VENDOR_ID     0x00455341L
#define OD_PRODUCT_CODE  0x00010002L
#define OD_REVISION      0x00010020L

// Urmatorul este optional si poate fi lasat "undefined"
#define OD_SERIAL        0xFFFFFFFFL

/**************************************************************************
DEFINES: Defineste imaginea procesului
Modificati aceste valori in functie de aplicatie
**************************************************************************/

// Defineste dimensiunea imaginii procesului
#define PROCIMG_SIZE 16

// Defineste variabilele de proces: offset-uri in imaginea procesului 
// Digital Input 1
#define IN_digi_1 0x00
// Digital Input 2
#define IN_digi_2 0x01
// Digital Input 3
#define IN_digi_3 0x02
// Digital Input 4
#define IN_digi_4 0x03

// Analog Input 1
#define IN_ana_1 0x04
// Analog Input 2
#define IN_ana_2 0x06

// Digital Output 1
#define OUT_digi_1 0x08
// Digital Output 2
#define OUT_digi_2 0x09
// Digital Output 3
#define OUT_digi_3 0x0A
// Digital Output 4
#define OUT_digi_4 0x0B

// Analog Output 1
#define OUT_ana_1 0x0C
// Analog Output 2
#define OUT_ana_2 0x0E

/**************************************************************************
DEFINES: VALIDEAZA/DEZACTIVEAZA OPTIUNI ALE CODULUI
**************************************************************************/

// Numarul maxim de TPDO (0 to 4)
#define NR_OF_TPDOS 2

// Numarul maxim de RPDO (0 to 4)
#define NR_OF_RPDOS 2

// Daca este definit, 1 sau mai multe TPDO folosesc event timer
#define USE_EVENT_TIME

// If defined, 1 or more TPDOs are change-of-state and use the inhibit timer
#define USE_INHIBIT_TIME

// Daca este definit, parametrii PDO sunt adaugati in Object Dictionary
// Intrarile trebuie sa fie adaugate in SDOResponseTable in user_xxxx.c
#define PDO_IN_OD

// Daca este definit, datele PDO sunt accesibile prin cereri SDO
// Intrarile trebuie adaugate in ODProcTable in user_xxxx.c
#define PROCIMG_IN_OD

// Daca este definit, OD entry [1017,00] poate fi accesat cu cereri SDO de citire/scriere
// Este si un exemplu pentru implementarea intrarilor OD dinamice/variabile.
#define DYNAMIC_HEARTBEAT

// If defined, node starts up automatically (does not wait for NMT master)
#define AUTOSTART

// Daca este definit, toti parametrii functiilor sunt verificati pentru valabilitate. 
// In caz de esec, este apelata functia MCOUSER_FatalError.
#define CHECK_PARAMETERS

/**************************************************************************
AICI SE TERMINA ZONA DE CONFIGURARE A STIVEI - A NU SE MODIFICA NIMIC MAI JOS
**************************************************************************/

#ifdef PROCIMG_IN_OD
// Definiti pentru tipurile de acces la intrarile OD
#define ODRD 0x10
#define ODWR 0x20
#endif // PROCIMG_IN_OD

#ifndef USE_EVENT_TIME
  #ifndef USE_INHIBIT_TIME
#error Cel putin unul dintre USE_EVENT_TIME sau USE_INHIBIT_TIME trebuie sa fie definit!
  #endif
#endif

#if (NR_OF_RPDOS == 0)
  #if (NR_OF_TPDOS == 0)
#error Cel putin un PDO trebuie definit!
  #endif
#endif

#if ((NR_OF_TPDOS > 4) || (NR_OF_RPDOS > 4))
#error Aceasta configuratie nu a fost testata de ESAcademy!
#endif

/**************************************************************************
DEFINITII GLOBALE
**************************************************************************/

// Tipuri de date standard
#define UNSIGNED8  unsigned char
#define UNSIGNED16  unsigned int
#define UNSIGNED32 unsigned long

// Expresii booleene
#define BOOLEAN unsigned char
#define TRUE 0xFF
#define FALSE 0

// Structura de date pentru un singur mesaj CAN 
typedef struct
{
  UNSIGNED16 ID;                 // ID mesaj CAN 
  UNSIGNED8 LEN;                 // Lungime date (0-8) 
  UNSIGNED8 BUF[8];              // Buffer date 
} CAN_MSG;

// Aceasta structura contine intreaga configuratie specifica unui nod
typedef struct
{
  UNSIGNED8 Node_ID;             // ID-ul nodului (1-126)
  UNSIGNED8 error_code;          // Biti: 0=RxQueue 1=TxQueue 3=CAN
  UNSIGNED16 Baudrate;            // Rata de bit curenta (in kbit/sec)
  UNSIGNED16 heartbeat_time;      // Intervalul Heartbeat in ms
  UNSIGNED16 heartbeat_timestamp; // Timestamp pentru ultimul heartbeat
  CAN_MSG heartbeat_msg;    	  // Continutul mesajului Heartbeat
  UNSIGNED8 error_register;      // Registrul de erori pentru intrarea OD [1001,00]
} MCO_CONFIG;

// Aceasta structura contine intreaga configuratie pentru un TPDO
typedef struct 
{
#ifdef USE_EVENT_TIME
  UNSIGNED16 event_time;          // Event timer in ms (0 pentru operarea numai de tip change-of-state)
  UNSIGNED16 event_timestamp;     // Daca se foloseste event timer, acesta este momentul (timestamp) 
                            	  // pentru urmatoarea transmisie a mesajului heartbeat
#endif
#ifdef USE_INHIBIT_TIME
  UNSIGNED16 inhibit_time;        // Inhibit timer in ms (0 daca nu se foloseste change-of-state)
  UNSIGNED16 inhibit_timestamp;   // Daca se foloseste inhibit timer, acesta este momentul (timestamp) 
                            	  //  pentru urmatoarea transmisie
  UNSIGNED8 inhibit_status;       // 0: Inhibit timer nu este pornit sau a expirat
                            	  // 1: Inhibit timer pornit
                            	  // 2: Exista un mesaj de transmis care asteapta expirarea inhibit time
#endif
  UNSIGNED8 offset;              // Offset la datele aplicatiei in process image
  CAN_MSG CAN;              	// Mesajul curent sau ultimul mesaj de transmis
} TPDO_CONFIG;

// Aceasta structura contine intreaga configuratie pentru un RPDO
typedef struct 
{
  UNSIGNED16 CANID;               // ID-ul mesajului 
  UNSIGNED8 len;                  // Lungimea mesajului (0-8) 
  UNSIGNED8 offset;               // Pointer la destinatia datelor 
} RPDO_CONFIG;

#ifdef PROCIMG_IN_OD
// Aceasta structura contine intreaga configuratie pentru o intrare de date de proces in OD
typedef struct 
{
  UNSIGNED16 idx;                 // Indexul intrarii in OD
  UNSIGNED8 subidx;              // Subindexul intrarii in OD 
  UNSIGNED8 len;                 // Lungimea datelor in octeti (1-4), plus biti ODRD, ODWR, RMAP/WMAP
  UNSIGNED8 offset;              // Offset la datele de proces in process image
} OD_PROCESS_DATA_ENTRY;
#endif // PROCIMG_IN_OD

/**************************************************************************
FUNCTII GLOBALE
**************************************************************************/

/**************************************************************************
DOES:    Initializeaza stiva MicroCANopen
         Trebuie apelata din MCOUSER_ResetApplication
RETURNS: nothing
**************************************************************************/
void MCO_Init 
  (
  UNSIGNED16 Baudrate,  // CAN baudrate in kbit/sec (1000,800,500,250,125,50,25 or 10)
  UNSIGNED8 Node_ID,    // CANopen node ID (1-126)
  UNSIGNED16 Heartbeat  // Heartbeat time in ms (0 daca nu se foloseste heartbeat)
  );

/**************************************************************************
DOES:    Aceasta functie initializeaza un TPDO.
		 Odata initializat, stiva MicroCANopen gestioneaza automat transmisia PDO.
         Aplicatia poate modifica datele direct, in orice moment.
NOTE:    Pentru a asigura consistenta datelor, aplicatia nu trebuie sa scrie date noi 
		 in timp ce se executa MCO_ProcessStack executes.
RETURNS: nothing
**************************************************************************/
void MCO_InitTPDO
  (
  UNSIGNED8 PDO_NR,       // Numarul TPDO-ului (1-4)
  UNSIGNED16 CAN_ID,       // ID-ul CAN ce trebuie folosit (daca este pus 0,
  							// se va folosi cel predefinit)
  UNSIGNED16 event_tim,    // TPDO este transmis la fiecare event_tim milisecunde 
                     		// (trebuie pus pe 0 daca se utilizeaza NUMAI inhibit_tim)
  UNSIGNED16 inhibit_tim,  // Inhibit time in ms pentru transmisie change-of-state
                     	// (trebuie pus pe 0 daca se utilizeaza NUMAI event_tim)
  UNSIGNED8 len,          // Numarul de octeti de date in TPDO
  UNSIGNED8 offset        // Offset la locatia datelor in process image
  );

/**************************************************************************
DOES:    Aceasta functie initializeaza un RPDO.
		 Odata initializat, stiva MicroCANopen actualizeaza automat datele
		 primit ein RPDO la offset-ul din process image.
NOTE:    Pentru a asigura consistenta datelor, aplicatia nu trebuie sa
		 citeasca datele in timp ce se executa functia MCO_ProcessStack.
RETURNS: nothing
**************************************************************************/
void MCO_InitRPDO
  (
  UNSIGNED8 PDO_NR,       // Numarul RPDO-ului (1-4)
  UNSIGNED16 CAN_ID,       // ID-ul CAN care trebuie folosit (daca este pus
  						// pe 0 se va utiliza COB ID-ul din setul predefinit)
 
  UNSIGNED8 len,          // Numarul de octeti de date din RPDO
  UNSIGNED8 offset        // Offset la locatia datelor in process image
  );

/**************************************************************************
DOES:    Aceasta functie implementeaza stiva de protocoale MicroCANopen. 
         Ea trebuie apelata frecvent pentru a asigura functionarea corecta
		 a stivei de comunicatie.
		 De obicei, ea este apelata din bucla infinita while(1) din main.
RETURNS: 0 daca nu s-a procesat nimic
		 1 daca a fost transmis sau a fost primit un mesaj CAN.
**************************************************************************/
UNSIGNED8 MCO_ProcessStack
  (
  void
  );

/**************************************************************************
FUNCTII APELATE DIN STIVA MICROCANOPEN (CALL_BACK)
Acestea trebuie implementate de aplicatie.
**************************************************************************/

/**************************************************************************
DOES:    Functie call-back pentru resetarea aplicatiei.
         Porneste WDT si asteapta pana cand acesta determina un RESET.
RETURNS: nothing
**************************************************************************/
void MCOUSER_ResetApplication
  (
  void
  );

/**************************************************************************
DOES:    Aceasta functie reseteaza si initializeaza interfata CAN si 
		 stiva MicroCANopen. Este apelata din stiva MicroCANopen, daca este primit
		 un mesaj NMT care solicita "Reset Communication".
         Aceasta functie trebuie sa apeleze MCO_Init si MCO_InitTPDO/MCO_InitRPDO.
RETURNS: nimic
**************************************************************************/
void MCOUSER_ResetCommunication
  (
  void
  );

/**************************************************************************
DOES:    Aceasta functie este apelata daca apare o eroare fatala. 
         Codurile de eroare pentru mcohwxxx.c sunt in intervalul 0x8000 - 0x87FF.
         Codurile de eroare pentru mco.c sunt in intervalul 0x8800 - 0x8FFF. 
         Celelalte coduri de eroare pot fi folosite de aplicatie.
RETURNS: nimic
**************************************************************************/
void MCOUSER_FatalError
  (
  UNSIGNED16 ErrCode // Pentru depanare, cautati in codul sursa codul de eroare
  					// intalnit
  );

#endif
