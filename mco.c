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
           $LastChangedDate: 2015-06-15 $
           $LastChangedRevision: 232 $
***************************************************************************/ 

#include <mco.h>
#include <mcohwSKCAN.h>
#include <string.h>

/**************************************************************************
VARIABILE GLOBALE
***************************************************************************/ 

// aceasta structura contine toata configuratia specifica nodului
MCO_CONFIG MEM_FAR gMCOConfig;

#if NR_OF_TPDOS > 0
// aceasta structura contine toata configuratia a maximum 4 TPDO
TPDO_CONFIG MEM_FAR gTPDOConfig[NR_OF_TPDOS];
#endif

// acesta este urmatorul TPDO de testat in MCO_ProcessStack
UNSIGNED8 MEM_FAR gTPDONr = NR_OF_TPDOS;

#if NR_OF_RPDOS > 0
// aceasta structura contine toata configuratia a maximum 4 RPDO
RPDO_CONFIG MEM_FAR gRPDOConfig[NR_OF_RPDOS];
#endif

// aceasta structura contine ultimul mesaj CAN receptionat
CAN_MSG MEM_FAR gRxCAN;

// aceasta structura contine mesajul CAN pentru raspunsuri SDO sau aborts
CAN_MSG MEM_FAR gTxSDO;

// imaginea procesului definita in user_C515C.c
extern UNSIGNED8 MEM_NEAR gProcImg[];

// tabel cu raspunsurile SDO la cererile de citire a OD- definit in user_C515C.c
extern UNSIGNED8 MEM_CONST SDOResponseTable[];

#ifdef PROCIMG_IN_OD
// tabel cu intrarile OD la variabilele de proces
extern OD_PROCESS_DATA_ENTRY MEM_CONST gODProcTable[];
#endif

/**************************************************************************
FUNCTII LOCALE
***************************************************************************/

// Mesaje SDO Abort
#define SDO_ABORT_UNSUPPORTED     0x06010000UL
#define SDO_ABORT_NOT_EXISTS      0x06020000UL
#define SDO_ABORT_READONLY        0x06010002UL
#define SDO_ABORT_TYPEMISMATCH    0x06070010UL
#define SDO_ABORT_UNKNOWN_COMMAND 0x05040001UL
#define SDO_ABORT_UNKNOWNSUB      0x06090011UL


/**************************************************************************
DOES:    Cautã un anumit index si subindex in tabelul cu raspunsuri SDO.
RETURNS: 255 daca nu a fost gasit, altfel, numarul inregistrarii gasite
         (incepe de la zero)
**************************************************************************/
UNSIGNED8 MCO_Search_OD
  (
  UNSIGNED16 index,   // Indexul intrarii OD cautate
  UNSIGNED8 subindex // Subindexul intrarii OD cautate
  )
{
  UNSIGNED8 data i;
  UNSIGNED8 data i_hi, hi;
  UNSIGNED8 data i_lo, lo;
  UNSIGNED8 const code *p;
  UNSIGNED8 const code *r;

  i = 0;
  i_hi = (UNSIGNED8) (index >> 8);
  i_lo = (UNSIGNED8) index;
  r = &(SDOResponseTable[0]);
  while (i < 255)
  {
    p = r;
    // pune r pe urmatoarea inregistrare din tabel
    r += 8;
    // sare peste octetul de comanda
    p++;
	lo = *p;
	p++;
	hi = *p;
    // daca indexul in tabel este 0xFFFF, s-a terminat tabelul
    if ((lo == 0xFF) && (hi == 0xFF))
    {
	  return 255;
	}
    if (lo == i_lo)
    { 
      if (hi == i_hi)
      { 
        p++;
        // intrare gasita?
        if (*p == subindex)
        {
          return i;
        }
      }
    }
    i++;
  }

  // nu a fost gasita
  return 255;
}

#ifdef PROCIMG_IN_OD
/**************************************************************************
DOES:    Cauta in gODProcTable definit in user_C515C.c o intrare cu un anumit index si subindex.
RETURNS: 255 daca nu a fost gasita, altfel numarul inraegistrarii gasite
         (incepand cu 0)
**************************************************************************/
UNSIGNED8 MCO_SearchODProcTable
  (
  UNSIGNED16 index,    // Indexul intrarii OD cautate
  UNSIGNED8 subindex  // Subindexul intrarii OD cautate
  )
{
  UNSIGNED8 j = 0;
  UNSIGNED16 compare;
  // pointer la inregistrarile OD
  OD_PROCESS_DATA_ENTRY MEM_CONST *pOD;

  // initializeaza pointerul
  pOD = &(gODProcTable[0]);
  // executa pana la sfarsitul tabelului
  while (j < 0xFF)
  {
    compare = pOD->idx;
    // sfarsit tabel? 
    if (compare == 0xFFFF)
    {
      return 0xFF;
    }
    // index gasit?
    if (compare == index)
    {
      // subindex gasit?
      if (pOD->subidx == subindex)
      {
        return j;
      }
    }
    // incrementeaza cu SIZEOF(OD_PROCESS_DATA_ENTRY)
    pOD++;
    j++;
  }

  // nu a fost gasita
  return 0xFF;
}

/**************************************************************************
DOES:    Functie de iesire pentru SDO_Handler. 
         Transmite un raspuns SDO cu lungime variabila (1-4 octeti).
         Se bazeaza pe faptul ca gTxSDO.ID, LEN si BUF[1-3] sunt deja incarcati.
RETURNS: 1 daca raspunsul a fost transmis 
**************************************************************************/
UNSIGNED8 MCO_ReplyWith
  (
  UNSIGNED8 *pDat,  // pointer la SDO data
  UNSIGNED8 len     // numarul de octeti de date in SDO
  )
{
  signed char k; // pentru numaratorul buclei

  // expedited, len of data
  gTxSDO.BUF[0] = 0x43 | ((4-len) << 2);
  // copiaza in ordine descrescatoare a adreselor pentru a incepe cu LSB
  for (k = (len - 1); k >= 0; k--)
  {
    gTxSDO.BUF[4+k] = *pDat;
    pDat++;
  }

  // transmite mesajul
  if (!MCOHW_PushMessage(&gTxSDO))
  {
    // transmisie esuata
    MCOUSER_FatalError(0x8801);
  }

  // mesaj transmis cu succes
  return 1;
}
#endif // PROCIMG_IN_OD

/**************************************************************************
DOES:    Genereaza un raspuns SDO Abort
RETURNS: nimic
**************************************************************************/
void MCO_Send_SDO_Abort
  (
  UNSIGNED32 ErrorCode  // 4 octeti cod eroare SDO abort
  )
{
  UNSIGNED8 i;

  // construieste mesajul
  gTxSDO.BUF[0] = 0x80;
  for (i=0;i<4;i++)
  {
    gTxSDO.BUF[4+i] = ErrorCode;
    ErrorCode >>= 8;
  }

  // transmite mesajul
  if (!MCOHW_PushMessage(&gTxSDO))
  {
    // transmisie esuata
    MCOUSER_FatalError(0x8801);
  }
}

/**************************************************************************
DOES:    Gestioneaza o cerere SDO primita.
RETURNS: 1 daca SDO a fost accesat cu succes, 0 daca s-a generat un SDO abort
**************************************************************************/
UNSIGNED8 MCO_Handle_SDO_Request 
  (
  UNSIGNED8 *pData  // pointer la 8 octeti de date cu cererea SDO
  )
{
  // octetul de comanda al cererii SDO
  UNSIGNED8 cmd;
  // indexul intrarii cerute de SDO
  UNSIGNED16 index;
  // subindexul intrarii cerute de SDO
  UNSIGNED8 subindex;
  // rezultatul cautarii in OD
  UNSIGNED8 found;
#ifdef PROCIMG_IN_OD
  UNSIGNED8 len;
  // pointer la o intrare in gODProcTable
  OD_PROCESS_DATA_ENTRY MEM_CONST *pOD;
#endif 

  // initializare variabile
  // cei mai semnificativi 3 biti contin comanda
  cmd = *pData & 0xE0;
  // extrage MSB al indexului
  index = pData[2];
  // adauga LSB al indexului
  index = (index << 8) + pData[1];
  // extrage subindexul
  subindex = pData[3];

  // copiaza multiplexorul in raspuns
  // index low
  gTxSDO.BUF[1] = pData[1];
  // index high
  gTxSDO.BUF[2] = pData[2];
  // subindex
  gTxSDO.BUF[3] = pData[3];

  // este o comanda de citire sau scriere OD?
  if ((cmd == 0x40) || (cmd == 0x20)) 
  {

#ifdef PROCIMG_IN_OD
    // trateaza accesul la variabilele de proces
    found = MCO_SearchODProcTable(index,subindex);
    // intrare gasita?
    if (found != 0xFF)
    {
	  pOD = &(gODProcTable[found]);
      // comanda de citre OD?
      if (cmd == 0x40)
      {
        // citire permisa?
        if ((pOD->len & ODRD) != 0) // Verifica daca bitul RD bit e setat
        {
          return MCO_ReplyWith(&(gProcImg[pOD->offset]),(pOD->len & 0x0F));
        }
		// citirea nu este permisa
        else
        {
          MCO_Send_SDO_Abort(SDO_ABORT_UNSUPPORTED);
          return 0;
        }
      }
      // comanda de citire?
      else
      {
        // este setat bitul WR? - atunci scrierea e permisa
        if ((pOD->len & ODWR) != 0)
        {
		  // pentru scrieri: bitii 2 si 3 ai *pData sunt numarul de octeti fara date
          len = 4 - ((*pData & 0x0C) >> 2); 
          // lungimea este buna?
          if (len != (pOD->len & 0x0F))
          {
            MCO_Send_SDO_Abort(SDO_ABORT_TYPEMISMATCH);
            return 0;
          }
          // extrage data din cererea de scriere SDO si o copie in imaginea procesului
          while (len > 0)
          {
            len--;
            gProcImg[pOD->offset+len] = gRxCAN.BUF[4+len];
          }
		  // scrie raspunsul
          gTxSDO.BUF[0] = 0x60;
          if (!MCOHW_PushMessage(&gTxSDO))
          {
            MCOUSER_FatalError(0x8808);
          }
		  return 1;
        }
        // scrierea nu este permisa
        else
        {
          MCO_Send_SDO_Abort(SDO_ABORT_UNSUPPORTED);
          return 0;
        }
      }
	}
#endif // PROCIMG_IN_OD

    // cauta in tabelul constant 
    found = MCO_Search_OD(index,subindex);
    // intrare gasita?
    if (found < 255)
    {
	  // comanda de citire?
      if (cmd == 0x40)
      {
        memcpy(&gTxSDO.BUF[0],&SDOResponseTable[(found*8)],8);
        if (!MCOHW_PushMessage(&gTxSDO))
        {
          MCOUSER_FatalError(0x8802);
        }
        return 1;
      }
      // comanda de scriere?
      MCO_Send_SDO_Abort(SDO_ABORT_READONLY);
      return 0;
    }
    if ((index == 0x1001) && (subindex == 0x00))
    {
      // comanda de citire
      if (cmd == 0x40)
      {
	    // expedited, 1 octet de date
        gTxSDO.BUF[0] = 0x4F;
        gTxSDO.BUF[4] = gMCOConfig.error_register;
        if (!MCOHW_PushMessage(&gTxSDO))
        {
          MCOUSER_FatalError(0x8802);
        }
        return 1;
      }
      // comanda de scriere
      MCO_Send_SDO_Abort(SDO_ABORT_READONLY);
      return 0;
    }

#ifdef DYNAMIC_HEARTBEAT
    // tratare HEARTBEAT dinamic (accese de citire/scriere)
    // acces la [1017,00] - heartbeat time
    if ((index == 0x1017) && (subindex == 0x00))
    {
      // comanda de citire?
      if (cmd == 0x40)
      {
	    // expedited, 2 bytes of data
        gTxSDO.BUF[0] = 0x4B;
        gTxSDO.BUF[4] = (UNSIGNED8) gMCOConfig.heartbeat_time;
        gTxSDO.BUF[5] = (UNSIGNED8) (gMCOConfig.heartbeat_time >> 8);
        if (!MCOHW_PushMessage(&gTxSDO))
        {
          MCOUSER_FatalError(0x8802);
        }
        return 1;
      }
      // comanda de scriere expedited cu 2 octeti de date
      if (*pData == 0x2B)
      {
        gMCOConfig.heartbeat_time = pData[5];
        gMCOConfig.heartbeat_time = (gMCOConfig.heartbeat_time << 8) + pData[4];
        // scrie raspunsul
        gTxSDO.BUF[0] = 0x60;
		// necesar pentru a trece testul de conformanta: sterge octetii neutilizati
        gTxSDO.BUF[4] = 0;
        gTxSDO.BUF[5] = 0;
        gTxSDO.BUF[6] = 0;
        gTxSDO.BUF[7] = 0;
        if (!MCOHW_PushMessage(&gTxSDO))
        {
          MCOUSER_FatalError(0x8802);
        }
        return 1;
      }
      MCO_Send_SDO_Abort(SDO_ABORT_UNSUPPORTED);
      return 0;
    }
#endif // DYNAMIC HEARTBEAT

    // Intrarea OD ceruta nu a fost gasita
    if (subindex == 0)
    {
      MCO_Send_SDO_Abort(SDO_ABORT_NOT_EXISTS);
    }
    else
    {
      MCO_Send_SDO_Abort(SDO_ABORT_UNKNOWNSUB);
    }
    return 0;
  }
  // ignora cererea SDO Abort; orice alta cerere produce o eroare
  if (cmd != 0x80)
  {
    MCO_Send_SDO_Abort(SDO_ABORT_UNKNOWN_COMMAND);
    return 0;
  }
  return 1;
}


#if NR_OF_TPDOS > 0
/**************************************************************************
DOES:    Apelata cand se intra in modul operational.
         Pregateste toate TPDO pentru functionare.
RETURNS: nimic
**************************************************************************/
void MCO_Prepare_TPDOs 
  (
    void
  )
{
UNSIGNED8 i;

  i = 0;
  // pregateste toate TPDO pentru transmisie
  while (i < NR_OF_TPDOS)
  {
    // este folosit acest TPDO?
	if (gTPDOConfig[i].CAN.ID != 0)
    {
      // copiaza variabilele de proces curente
      memcpy(&gTPDOConfig[i].CAN.BUF[0],&(gProcImg[gTPDOConfig[i].offset]),gTPDOConfig[i].CAN.LEN);
#ifdef USE_EVENT_TIME
      // reseteaza timerul event pentru transmisie imediata
      gTPDOConfig[i].event_timestamp = MCOHW_GetTime() - 2;
#endif
#ifdef USE_INHIBIT_TIME
      gTPDOConfig[i].inhibit_status = 2; // marcheaza ca gata de transmisie
      // reseteaza timerul inhibit pentru transmisie imediata
      gTPDOConfig[i].inhibit_timestamp = MCOHW_GetTime() - 2;
#endif
    }
  i++;
  }
  // se asigura ca MCO_ProcessStack incepe cu TPDO1
  gTPDONr = NR_OF_TPDOS;
}

/**************************************************************************
DOES:    Apelata cand un TPDO trebuie transmis
RETURNS: nimic
**************************************************************************/
void MCO_TransmitPDO 
  (
  UNSIGNED8 PDONr  // Numarul TPDO-ului de transmis
  )
{
#ifdef USE_INHIBIT_TIME
  // porneste un nou timer inhibit
  gTPDOConfig[PDONr].inhibit_status = 1;
  gTPDOConfig[PDONr].inhibit_timestamp = MCOHW_GetTime() + gTPDOConfig[PDONr].inhibit_time;
#endif
#ifdef USE_EVENT_TIME
  gTPDOConfig[gTPDONr].event_timestamp = MCOHW_GetTime() + gTPDOConfig[gTPDONr].event_time; 
#endif
  if (!MCOHW_PushMessage(&gTPDOConfig[PDONr].CAN))
  {
    MCOUSER_FatalError(0x8801);
  }
}
#endif // NR_OF_TPDOS > 0

/**************************************************************************
FUNCTII PUBLICE
***************************************************************************/ 

/**************************************************************************
DOES:    Initializeaza stiva MicroCANopen
         Trebuie apelata din MCOUSER_ResetApplication
RETURNS: nimic
**************************************************************************/
void MCO_Init 
  (
  UNSIGNED16 Baudrate,  // CAN baudrate in kbit (1000,800,500,250,125,50,25 sau 10)
  UNSIGNED8 Node_ID,   // CANopen node ID (1-126)
  UNSIGNED16 Heartbeat  // Heartbeat time in ms (0 daca nu se foloseste)
  )
{
  UNSIGNED8 i;

  // Initializeaza variabilele globale
  gMCOConfig.Node_ID = Node_ID;
  gMCOConfig.error_code = 0;
  gMCOConfig.Baudrate = Baudrate;
  gMCOConfig.heartbeat_time = Heartbeat;
  gMCOConfig.heartbeat_msg.ID = 0x700+Node_ID;
  gMCOConfig.heartbeat_msg.LEN = 1;

  // starea curenta NMT a acestui nod = bootup
  gMCOConfig.heartbeat_msg.BUF[0] = 0;
  gMCOConfig.error_register = 0;
 
  // Initializeaza mesajul SDO Response/Abort
  gTxSDO.ID = 0x580+gMCOConfig.Node_ID;
  gTxSDO.LEN = 8;
   
#if NR_OF_TPDOS > 0
  i = 0;
  // initializeaza TPDO-urile
  while (i < NR_OF_TPDOS)
  {
    gTPDOConfig[i].CAN.ID = 0;
    i++;
  }
#endif
#if NR_OF_RPDOS > 0
  i = 0;
  // initializeaza RPDO-urile
  while (i < NR_OF_RPDOS)
  {
    gRPDOConfig[i].CANID = 0;
    i++;
  }
#endif

  // initializeaza controlerul CAN
  if (!MCOHW_Init(Baudrate))
  {
    MCOUSER_FatalError(0x8802);
  }
  // pentru mesajul NMT master
  if (!MCOHW_SetCANFilter(0))
  {
    MCOUSER_FatalError(0x8803);
  }
  // pentru cererile SDO
  if (!MCOHW_SetCANFilter(0x600+Node_ID))
  {
    MCOUSER_FatalError(0x8803);
  }

  // semnaleaza stivei MCO_ProcessStack: nodul s-a initializat
  gTPDONr = 0xFF;
}  

#if NR_OF_RPDOS > 0
/**************************************************************************
DOES:   Aceasta functie initializeaza un RPDO. Odata initializat, stiva MicroCANopen actualizeaza automat datele la offset.
		NOTA:  Din motive de consistenta a datelor, aplicatia nu trebuie sa citeasca datele in timp ce se executa MCO_ProcessStack.
RETURNS: nimic
**************************************************************************/
void MCO_InitRPDO
  (
  UNSIGNED8 PDO_NR,       // Numarul RPDO (1-4)
  UNSIGNED16 CAN_ID,       // Identificatorul CAN identifier ce trebuie folosit (daca e 0 se utilizeaza cel implicit)
  UNSIGNED8 len,          // Numarul de octeti de date in RPDO
  UNSIGNED8 offset        // Offset-ul cu amplasarea datelor in imaginea procesului
  )
{

#ifdef CHECK_PARAMETERS	
  // verifica domeniul PDO si al ID-ului nodului (1 - 127)
  if (((PDO_NR < 1)             || (PDO_NR > NR_OF_RPDOS))      || 
      ((gMCOConfig.Node_ID < 1) || (gMCOConfig.Node_ID > 127)))
  {
    MCOUSER_FatalError(0x8804);
  }
  // offset-ul este inafara imaginii procesului?
  if (offset >= PROCIMG_SIZE)   
  { 
    MCOUSER_FatalError(0x8904);
  }
#endif
  PDO_NR--;
  gRPDOConfig[PDO_NR].len = len;
  gRPDOConfig[PDO_NR].offset = offset;
  if (CAN_ID == 0)
  {
    gRPDOConfig[PDO_NR].CANID = 0x200 + (0x100 * ((UNSIGNED16)(PDO_NR))) + gMCOConfig.Node_ID;
  }
  else
  {
    gRPDOConfig[PDO_NR].CANID = CAN_ID;
  }
  if (!MCOHW_SetCANFilter(gRPDOConfig[PDO_NR].CANID))
  {
    MCOUSER_FatalError(0x8805);
  }
}
#endif // NR_OF_RPDOS > 0


#if NR_OF_TPDOS > 0
/**************************************************************************
DOES:    Aceasta functie initializeaza un TPDO. Odata initializata, 
		 stiva MicroCANopen gestioneaza automat transmisia PDO
         Aplicatia poate modifica datele in orice moment.
NOTE:    Pentru consistenta datelor, aplicatia nu trebuie sa scrie date noi
		 in timp ce se executa MCO_ProcessStack executes.
RETURNS: nimic
**************************************************************************/
void MCO_InitTPDO
  (
  UNSIGNED8 PDO_NR,        // Numarul TPDO (1-4)
  UNSIGNED16 CAN_ID,        // Identificatorul CAN ce va fi folosit (daca e 0 se foloseste cel implicit)
  UNSIGNED16 event_time,    // Transmis la fiecare event_tim ms 
  UNSIGNED16 inhibit_time,  // Inhibit time in ms pentru transmisie la schimbarea starii
                      // (pus pe 0 daca se foloseste numai event_tim)
  UNSIGNED8 len,           // Numarul de octeti de date in TPDO
  UNSIGNED8 offset         // Offset-ul la locatia datelor in imaginea proceselor
  )
{

#ifdef CHECK_PARAMETERS
  // verifica domeniul PDO, node id, domeniul len 1 - 8 si event time sau inhibit time
  if (((PDO_NR < 1)             || (PDO_NR > NR_OF_TPDOS))     ||
      ((gMCOConfig.Node_ID < 1) || (gMCOConfig.Node_ID > 127)) ||
      ((len < 1)                || (len > 8))                  ||
      ((event_time == 0)        && (inhibit_time == 0)))
  {
    MCOUSER_FatalError(0x8806);
  }
  // offset-ul depaseste imaginea procesului?
  if (offset >= PROCIMG_SIZE)   
  { 
    MCOUSER_FatalError(0x8906);
  }
#endif
  PDO_NR--;
  if (CAN_ID == 0)
  {
    gTPDOConfig[PDO_NR].CAN.ID = 0x180 + (0x100 * ((UNSIGNED16)(PDO_NR))) + gMCOConfig.Node_ID;
  }
  else
  {
    gTPDOConfig[PDO_NR].CAN.ID = CAN_ID;
  }
  gTPDOConfig[PDO_NR].CAN.LEN = len;
  gTPDOConfig[PDO_NR].offset = offset;
#ifdef USE_EVENT_TIME
  gTPDOConfig[PDO_NR].event_time = event_time;
#endif
#ifdef USE_INHIBIT_TIME
  gTPDOConfig[PDO_NR].inhibit_time = inhibit_time;
#endif
}
#endif // NR_OF_TPDOS > 0


/**************************************************************************
DOES:    TAceasta functie implementeaza stiva MicroCANopen. 
         Trebuie apelata frecvent pentru a asigura functionarea corecta a stivei. 
         De obicei, este apelata dintr-o bucla infinita while(1) in main.
RETURNS: 0 daca nu s-a intamplat nimic, 1 daca s-a primit sau s-a transmis un mesaj CAN.
**************************************************************************/
UNSIGNED8 MCO_ProcessStack
  (
  void
  )
{
  UNSIGNED8 i;
  UNSIGNED8 ret_val = 0;

  // verifica daca este imediat dupa initializare
  // acesta a fost pus de MCO_Init
  if (gTPDONr == 0xFF)
  {
    // initializeaza heartbeat time
    gMCOConfig.heartbeat_timestamp = MCOHW_GetTime() + gMCOConfig.heartbeat_time;
    // transmite mesajul boot-up  
    if (!MCOHW_PushMessage(&gMCOConfig.heartbeat_msg))
    {
      MCOUSER_FatalError(0x8801);
    }
#ifdef AUTOSTART
// intra automat in starea operationala
    gMCOConfig.heartbeat_msg.BUF[0] = 0x05;
#if NR_OF_TPDOS > 0 
	MCO_Prepare_TPDOs();
#endif

#else
    // intra in starea pre-operationala
    gMCOConfig.heartbeat_msg.BUF[0] = 0x7F;
#endif
    // return value to default
    gTPDONr = NR_OF_TPDOS;
    return 1;
  }
 
  // trateaza urmatoarele mesaje receptionate
  // daca este un mesaj primit
  if (MCOHW_PullMessage(&gRxCAN))
  {
    // este un mesaj NMT master?
    if (gRxCAN.ID == 0)
    {
      // Mesajul NMT este pentru acest nod sau pentru toate nodurile
      if ((gRxCAN.BUF[1] == gMCOConfig.Node_ID) || (gRxCAN.BUF[1] == 0))
      {
        switch (gRxCAN.BUF[0])
        {
          // porneste nodul
          case 1:
            gMCOConfig.heartbeat_msg.BUF[0] = 5;
#if NR_OF_TPDOS > 0          
            MCO_Prepare_TPDOs();
#endif
            break;

          // opreste nodul
          case 2:
            gMCOConfig.heartbeat_msg.BUF[0] = 4;
            break;

          // intra in modul pre-operational
          case 128:
            gMCOConfig.heartbeat_msg.BUF[0] = 127;
            break;
   
          // reseteaza nodul
          case 129:
            MCOUSER_ResetApplication();
            break;

          // reseteaza comunicatia nodului
          case 130:
            MCOUSER_ResetCommunication();
            break;

          // comanda necunoscuta
          default:
            break;
        }

        return 1;
      } // Mesaj NMT master destinat acestui nod
    } // Mesaj NMT master primit
    
    // daca nodul nu este oprit...
    if (gMCOConfig.heartbeat_msg.BUF[0] != 4)
    {
      // este mesajul o cerere SDO pentru acest nod?
      if (gRxCAN.ID == gMCOConfig.Node_ID+0x600)
      {
        // trateaza cererea SDO - valoare returnata neutilizata in aceasta versiune
        i = MCO_Handle_SDO_Request(&gRxCAN.BUF[0]);
        return 1;
      }
    }

#if NR_OF_RPDOS > 0
    // nodul este operational?
    if (gMCOConfig.heartbeat_msg.BUF[0] == 5)
    {
      i = 0;
      // testeaza RPDO-urile
      while (i < NR_OF_RPDOS)
      {
        // este unul dintre RPDO-urile nodului?
        if (gRxCAN.ID == gRPDOConfig[i].CANID)
        {
          // copie datele din RPDO in imaginea procesului
          memcpy(&(gProcImg[gRPDOConfig[i].offset]),&(gRxCAN.BUF[0]),gRPDOConfig[i].len);
          // iese din bucla
          i = NR_OF_RPDOS;
          ret_val = 1;
        }
        i++;
      } // pentru toate RPDO-urile
    } // nodul este operational
#endif // NR_OF_RPDOS > 0
  } // Mesaj primit

#if NR_OF_TPDOS > 0
  // nodul este operational?
  if (gMCOConfig.heartbeat_msg.BUF[0] == 5)
  {
    // verifica urmatorul TPDO pentru transmisie
    gTPDONr++;
    if (gTPDONr >= NR_OF_TPDOS)
    {
      gTPDONr = 0;
    }
    // este TPDO-ul 'gTPDONr' in uz?
    if (gTPDOConfig[gTPDONr].CAN.ID != 0)
    {
#ifdef USE_EVENT_TIME
      // TPDO-ul foloseste event timer si event timer a expirat? daca da, trebuie transmis mesajul acum
      if ((gTPDOConfig[gTPDONr].event_time != 0) && 
          (MCOHW_IsTimeExpired(gTPDOConfig[gTPDONr].event_timestamp)) )
      {
        // preia datele din imaginea procesului si transmite
        memcpy(&(gTPDOConfig[gTPDONr].CAN.BUF[0]),&(gProcImg[gTPDOConfig[gTPDONr].offset]),gTPDOConfig[gTPDONr].CAN.LEN);
        MCO_TransmitPDO(gTPDONr);
        return 1;
      }
#endif // FOLOSESTE_EVENT_TIME
#ifdef USE_INHIBIT_TIME
      // TPDO-ul foloseste inhibit time? - transmisie change-of-state
      if (gTPDOConfig[gTPDONr].inhibit_time != 0)
      {
        // timerul inihibit functioneaza?
        if (gTPDOConfig[gTPDONr].inhibit_status > 0)
        {
          // timerul inhibit a expirat?
          if (MCOHW_IsTimeExpired(gTPDOConfig[gTPDONr].inhibit_timestamp))
          {
            // este un nou mesaj in asteptarea transmisiei?
		    if (gTPDOConfig[gTPDONr].inhibit_status == 2)
			{ 
              // transmite acum
              MCO_TransmitPDO(gTPDONr);
              return 1;
            }
          }
          // nici un mesaj nu asteapta, dar timerul a expirat
          else 
		  {
		    gTPDOConfig[gTPDONr].inhibit_status = 0;
		  }
		}
        // starea inhibit e 0 sau 1?
        if (gTPDOConfig[gTPDONr].inhibit_status < 2)
        {
          // s-au modificat datele aplicatiei?
          if ((memcmp(&gTPDOConfig[gTPDONr].CAN.BUF[0],&(gProcImg[gTPDOConfig[gTPDONr].offset]),gTPDOConfig[gTPDONr].CAN.LEN) != 0))
          {
            // copiaza noile date 
			memcpy(&gTPDOConfig[gTPDONr].CAN.BUF[0],&(gProcImg[gTPDOConfig[gTPDONr].offset]),gTPDOConfig[gTPDONr].CAN.LEN);
            // a expirat inhibit time?
            if (gTPDOConfig[gTPDONr].inhibit_status == 0)
            {
              // transmite acum
              MCO_TransmitPDO(gTPDONr);
              return 1;
            }
            // starea inhibit este 1
 			else
            {
              // asteapta ca inhibit time sa expire 
              gTPDOConfig[gTPDONr].inhibit_status = 2;
			}
          }
        }
      } // Inhibit Time != 0
#endif // USE_INHIBIT_TIME
    } // PDO activ (CAN_ID != 0)  
  } // daca nodul este operational
#endif // NR_OF_TPDOS > 0
  
  // trebuie produs un heartbeat?
  if (gMCOConfig.heartbeat_time != 0)
  {
    // a expirat timpul pentru heartbeat?
    if (MCOHW_IsTimeExpired(gMCOConfig.heartbeat_timestamp))
    {
      // transmite mesaj heartbeat
      if (!MCOHW_PushMessage(&gMCOConfig.heartbeat_msg))
      {
        MCOUSER_FatalError(0x8801);
      }
      // pregateste un nou heartbeat time pentru urmatoarea transmisie
      gMCOConfig.heartbeat_timestamp = MCOHW_GetTime() + gMCOConfig.heartbeat_time;
      ret_val = 1;
    }
  }
  return ret_val;
}

