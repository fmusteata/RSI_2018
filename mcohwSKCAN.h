/**************************************************************************
MODULE:    MCOHW
CONTAINS:  Hardware driver specification for MicroCANopen implementation
           The specific implementations are named mcohwXXX.c, where
           XXX represents the CAN hardware used.
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
VERSION:   1.10, Pf/Aa/Ck 27-MAY-03
		   $LastChangedDate: 2016-10-01 $
           $LastChangedRevision: 232 $
***************************************************************************/ 

#include <mco.h>
// Biti de stare pentru functia MCOHW_GetStatus
#define INIT 0x01
#define CERR 0x02
#define ERPA 0x04
#define RXOR 0x08
#define TXOR 0x10
#define BOFF 0x80


/**************************************************************************
DOES:    Returneaza o variabila globala de stare.
CHANGES: Starea poate fi modificata in orice moment de acest modul, de
         exemplu dintr-o rutina de intreruperi sau de orice alta functie din
		 acest modul
BITS:    0: INIT - pus pe 1 dupa terminarea initializarii
                   este 0 daca modull nu s-a initializat inca sau
				   initializarea a esuat
         1: CERR - pus pe 1 daca a aparut o eroare de bit sau incadrare CAN
         2: ERPA - pus pe 1 daca nodul a trecut in starea "error passive"
         3: RXOR - pus pe 1 daca a aparut o eroare de suprapunere la receptie
         4: TXOR - pus pe 1 daca a aparut o eroare de suprapunere la transmisie
         5: Rezervat
         6: Rezervat
         7: BOFF - pus pe 1 daca nodul a trecut in starea "bus off"
**************************************************************************/
UNSIGNED8 MCOHW_GetStatus
  (
  void
  );

/**************************************************************************
DOES:    Initializeaza interfata CAN.
RETURNS: 1 daca initializarea s-a terminat cu succes 
         0 daca initializarea a esuat, bitul INIT din octetul returnat de 
		 MCOHW_GetStatus este 0
**************************************************************************/
UNSIGNED8 MCOHW_Init 
  (  
  UNSIGNED16 BaudRate   // Valori permise: 1000, 800, 500, 250, 125, 50, 25, 10
  );

#define IN 0
#define OUT 1
/**************************************************************************
DOES:    Initializeaza un filtru hardware CAN.
RETURNS: 1 daca filtrul a fost setat 
         2 daca acest controler CAN nu suporta filtre 
           (in acest caz HW va primi toate mesajele CAN)
         0 daca nici un filtru nu mai este disponibil
**************************************************************************/
UNSIGNED8 MCOHW_SetCANFilter 
  (
  UNSIGNED16 CANID      // ID-ul CAN care va fi acceptat de catre filtru
  );

/**************************************************************************
DOES:    Aceasta functie implementeaza o coada de transmisie CAN.
		 Fiecare apel al functiei adauga un mesaj in coada.
RETURNS: 1 Mesajul a fost adaugat in coada de transmisie
         0 Daca coada este plina, mesajul nu a fost adaugat, iar bitul
           TXOR din octetul returnat de MCOHW_GetStatus este setat
NOTES:   Stiva MicroCANopen nu va incerca sa adauge in coada mai multe
		 mesaje dintr-o data. Cu fiecare apel la MCO_ProcessStack, maxim
		 un mesaj poate fi adaugat. Pentru multe aplicatii, o coada de
		 lungime 1 va fi suficienta. Numai aplicatiiloe cu o incarcare
		 mare a magistralei CAN sau cu rate foarte mici de comunicatie
		 poate avea nevoie de o coada de dimensiune 3 sau mai mult.
**************************************************************************/
UNSIGNED8 MCOHW_PushMessage 
  (
  CAN_MSG MEM_FAR *pTransmitBuf // Structura de date cu mesajul de transmis
  );

/**************************************************************************
DOES:    Aceasta functie implementeaza o coada de receptie CAN. Cu fiecare
		 apel un singur mesaj poate fi extras din coada.
RETURNS: 1 a fost scos un mesaj din coada de receptie
         0 coada este goala, nici un mesaj nu a fost primit
NOTES:   Implementarea acestei functii variaza mult de la un controler CAN
		 la altul.
         La un controler stil SJA1000, poate fi folosita chiar coada HW
		 din controller.
         Controlerele CAN care au doar un buffer de receptie au nevoie de
		 o coada de receptie mai mare.
         Controlerele de tip "Full CAN" ar putea implementa mai multe
		 obiecte de tip mesaj, cate unul pentru fiecare ID primit (folosind
		 functia MCOHW_SetCANFilter).
**************************************************************************/
UNSIGNED8 MCOHW_PullMessage
  (
  CAN_MSG MEM_FAR *pTransmitBuf // Structura de date cu mesajul receptionat
  );

/**************************************************************************
DOES:    Functia returneaza continutul unui ceas care numara milisecundele.
		 Ceasul este de tip UNSIGNED16 si trebuie incrementat la fiecare
		 milisecunda.
RETURNS: Numarul de milisecunde curent
NOTES:   Trebuie asigurata consistenta datelor (In sistemele de 8 biti,
		 trebuie dezactivata intreruperea de timer pe durata citirii ceasului)
**************************************************************************/
UNSIGNED16 MCOHW_GetTime 
  (
  void
  );

/**************************************************************************
DOES:    Functia compara un moment de timp (timestamp) UNSIGNED16 cu ceasul
		 intern si returneaza 1 daca momentul de timp a trecut.
RETURNS: 1 daca momentul de timp a trecut
         0 daca momentul de timp inca nu a fost atins.
NOTES:   Intervalul maxim care poate fi masurat este 0x8000 (circa 32 secunde);
         pentru utilizarea functiei in MicroCANopen este suficient. 
**************************************************************************/
UNSIGNED8 MCOHW_IsTimeExpired 
  (
  UNSIGNED16 timestamp // Timestamp de verificat pentru expirare
  );
/**************************************************************************
// Implementare recomandata pentru aceasta functie (8051 version):         
{
UNSIGNED16 time_now;

  EA = 0; // Diezactiveaza intreruperile
  time_now = gTimCnt;
  EA = 1; // Valideaza intreruperile
  timestamp++; // pentru a asigura intervalul minim (1 ms)
  if (time_now > timestamp)
  {
    if ((time_now - timestamp) < 0x8000)
      return 1;
    else
      return 0;
  }
  else
  {
    if ((timestamp - time_now) > 0x8000)
      return 1;
    else
      return 0;
  }
}
**************************************************************************/

