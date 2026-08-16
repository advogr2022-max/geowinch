/*
WinchControl    Jörg Müller    ADVANTECH_PCI-1716.h

Enthält Funktionen zum speziellen problemorientierten Zugriff auf die DAQ-Karte

                        Advantech PCI-1716

Portierbarkeit: nein
*/

#ifndef __ADVANT1716_H
#define __ADVANT1716_H

#include "constants.h"
#include "ADSAPI\Include\Device.h"

// Globale Variablen, Struktueren usw.
const           PCI1716_MAXCHANNELNUM = 8;                                      // Maximale Kanalanzahl
const           PCI1716_STARTCHANNELNUM = 0;                                    // Startkanal bei Konfiguration
const           PCI1716_MAXOUTCHANNELNUM = 2;                                   // Anzahl der Ausgangskanäle

PT_MAIConfig    PCI1716_MAIConfig;                                              // Konfigurationsstruktur für analogen Eingang
unsigned short  PCI1716_GainArray[PCI1716_MAXCHANNELNUM];                       // Verstärkungs-Code-Puffer (Teil der Konfigurationsstruktur für analogen Eingang)
PT_AOConfig     PCI1716_AOConf;                                                 // Konfigurationsstruktur für analogen Ausgang
PT_MAIVoltageIn PCI1716_MAIVoltageIn;                                           // Eingangsverstärkungseinstellungen für AIn
float           PCI1716_VoltageArray[PCI1716_MAXCHANNELNUM];                    // Puffer für Eingangsspannungen

long            PCI1716_DevH = 0;                                               // Handle für die Mess-Karte
unsigned long   PCI1716_DevNum = 0xFFFFFFFF;                                    // Nummer der Karte, wie vom Treiber erkannt
// -----------------------------------------------------------------------------
// Initialisieren der Variablen/Handles
void _fastcall PCI1716_VarInit()
{
        PCI1716_DevNum = 0xFFFFFFFF;                                            // noch keine Karte detektiert
        PCI1716_DevH = 0;
};

// -----------------------------------------------------------------------------
// Suchen nach der richtigen Karte
// Rückgabewert:    true ...    passende Karte gefunden und Gerätenummer gesetzt
//                  false ...   passende Karte nicht gefunden,
//                              PCI..._DevNum bereits gesetzt,
//                              PCI..._DevH bereits gesetzt,
//                              PCI..._DevNum noch nicht initialisiert oder
//                              PCI..._DevH noch nicht initialisiert gesetzt
bool _fastcall PCI1716_CardFind()
{
    DEVFEATURES DevFeatures;
    PT_DeviceGetFeatures DevGetFeatures;
    DevGetFeatures.buffer = &DevFeatures;
    DevGetFeatures.size = sizeof(DevFeatures);

    if ((PCI1716_DevNum != 0xFFFFFFFF) ||
        (PCI1716_DevH != 0)) return false;  //Variable schon belegt

    // Nacheinander versuchen verschiedene Karten zu öffnen
    for (PCI1716_DevNum = 0; PCI1716_DevNum < MAXNUMBEROFADVANTECHCARDS; ++PCI1716_DevNum) {


        if (DRV_DeviceOpen(PCI1716_DevNum, &PCI1716_DevH) == SUCCESS) {

            // Anhand der Board-ID die passende Karte identifizieren
            if (DRV_DeviceGetFeatures(PCI1716_DevH, &DevGetFeatures) == SUCCESS) {

                DRV_DeviceClose(&PCI1716_DevH);
                PCI1716_DevH = 0; // Handle wieder als unbenutzt kennzeichnen

                // 0x74 ist die Advantech PCI-1716 Board-ID,
                // die den Typ der Karte angibt
                // PCI..._DevNum bleibt markiert, da so beim Öffnen und
                // Initialisieren der Karte diese lokalisiert wird
                if (DevFeatures.dwBoardID == 0x74) return true; // Karte gefunden
            }

            DRV_DeviceClose(&PCI1716_DevH);
        }
    }

    PCI1716_VarInit();

    // Kein Gerät gefunden
    return false;
};

// -----------------------------------------------------------------------------
// Karte initialisieren (öffnen)
// Rückgabewert: true ...   i.O.
//               false ...  fehlgeschlagen, weil
//                          keine Kartenposition bekannt,
//                          Kartenzugang schon geöffnet oder
//                          Öffnen fehlgeschlagen
bool _fastcall PCI1716_CardOpen()
{
    DEVFEATURES DevFeatures;
    PT_DeviceGetFeatures DevGetFeatures;
    DevGetFeatures.buffer = &DevFeatures;
    DevGetFeatures.size = sizeof(DevFeatures);

    if ((PCI1716_DevNum == 0xFFFFFFFF) ||
        (PCI1716_DevH != 0)) return false;  // Kartenzugang schon geöffnet

    if (DRV_DeviceOpen(PCI1716_DevNum, &PCI1716_DevH) == SUCCESS) {

        // Anhand der Board-ID Karte auf Typ prüfen
        if (DRV_DeviceGetFeatures(PCI1716_DevH, &DevGetFeatures) == SUCCESS) {
            if (DevFeatures.dwBoardID == 0x74) return true;
        }
        DRV_DeviceClose(&PCI1716_DevH);
    }

    PCI1716_VarInit(); // Karte als nicht vorhanden markieren
    return false;
}
/* am 28.04.2011 durch neue Fkt. PCI1762_CardInit() ersetzt
// -----------------------------------------------------------------------------
// Karte initialisieren
// Rückgabewert: true, wenn i.O.
//               false, wenn fehlgeschlagen
// Error_MSG:    Fehlerdefinition (oder-verknüpft)
//               0x0000 kein Fehler
//               0x0001 Karte nicht gefunden
//               0x0002 Karte konnte nicht geöffnet werden
bool _fastcall PCI1716_CardInit(short *Error_MSG)
{
//        short OutEntries;                                                       // Anzahl der gefundenen Karten
//        DEVLIST DeviceList[MAXNUMBEROFADVANTECHCARDS];
        DEVFEATURES DevFeatures;
        PT_DeviceGetFeatures DevGetFeatures;
        DevGetFeatures.buffer = &DevFeatures;
        DevGetFeatures.size = sizeof(DevFeatures);

        // noch keine Karte detektiert
        PCI1716_DevNum = 0xFFFFFFFF;
        PCI1716_DevH = 0;

        try {
            // Suchen nach Advatech PCI-1716
//  Diese Variante funktioniert nicht zu 100% bei Win XP Embedded System mit UNO3072L-DIO Treiber
//    am 21.04.2011 deaktiviert
//            DRV_DeviceGetList (DeviceList, MAXNUMBEROFADVANTECHCARDS, &OutEntries);
//
//            // alle gefundenen Karten überprüfen, ob die gewünschten dabei sind?
//            for (short i = 0; i < OutEntries; ++i)
//            {
//                // Karte gefunden ?
//                if (strncmp(DeviceList[i].szDeviceName, DIODACCardName, strlen(DIODACCardName)) == 0) {
//                    PCI1716_DevNum = DeviceList[i].dwDeviceNum;
//                    break;
//                }
//            }
//            if (PCI1716_DevNum == 0xFFFFFFFF) {
//                *Error_MSG = 0x0001;                                                // Karte nicht gefunden
//                return false;
//            }
//
//            PCI1716_DevNum = 1;
//            // Karte öffnen
//            if (DRV_DeviceOpen(PCI1716_DevNum, &PCI1716_DevH) != SUCCESS) {
//                PCI1716_DevNum = 0xFFFFFFFF;
//                PCI1716_DevH = 0;
//
//                *Error_MSG = 0x0002;                                                // Karte konnte nicht geöffnet werden
//                return false;
//            }
//
            // Nacheinander versuchen verschiedene Karten zu öffnen
            for (short i = 0; i < MAXNUMBEROFADVANTECHCARDS; ++i) {

                PCI1716_DevNum = i;
                PCI1716_DevNum = 1;

                if (DRV_DeviceOpen(PCI1716_DevNum, &PCI1716_DevH) == SUCCESS) {

                    // Anhand der Board-ID die passende Karte identifizieren
                    if (DRV_DeviceGetFeatures(PCI1716_DevH, &DevGetFeatures) == SUCCESS) {
                        if (DevFeatures.dwBoardID == 0x74) break;
                    }

                    DRV_DeviceClose(&PCI1716_DevH);
                }

                PCI1716_DevNum = 0xFFFFFFFF;
                PCI1716_DevH = 0;

            }

            if (PCI1716_DevNum == 0xFFFFFFFF)
            {
                *Error_MSG = 0x0001;                                            // Karte nicht gefunden
                return false;
            }

        } catch (...) {
            *Error_MSG = 0x0001;                                                // Karte nicht gefunden
            return false;
        }

        return true;
};
*/
// -----------------------------------------------------------------------------
// Karte konfigurieren
// *AINGains            Feld mit Verstärkungs-Codes
// AINGainSize          Anzahl der Felder von Gains
// *AOUTCHAN            Feld mit Analogausgangskanälen
// *AOUTRefSrc          Feld mit Analogausgangsreferenzquellen
// *AOUTMINVAL          Feld mit Analogausgangsmindestwerten
// *AOUTMAXVAL          Feld mit Analogausgangsmaximalwerten
// AOUTNUM              Feldgröße der AOUT...
// Rückgabewert: true  ... wenn alles i.O.
//               fasle ... wenn Verstärkungseinstellung fehlgeschlagen
bool _fastcall PCI1716_Config(short *AINGains, short AINGainSize,
                              unsigned short *AOUTCHAN, unsigned short *AOUTRefSrc,
                              float *AOUTMINVAL, float *AOUTMAXVAL, short AOUTNUM)
{
        bool OK = true;

        // Analog-Eingang
        // --------------

        PCI1716_MAIConfig.NumChan = PCI1716_MAXCHANNELNUM;
        PCI1716_MAIConfig.StartChan = PCI1716_STARTCHANNELNUM;

        // Standard-Verstärkung einstellen (notwendig, wenn GainSize < PCI1716_MAXCHANNELNUM)
        for (int i = 0; i < PCI1716_MAXCHANNELNUM; ++i) PCI1716_GainArray[i] = 0;

        // Übergebene Verstärkungen einstellen
        for (int i = 0; i < AINGainSize; ++i)
        {
                if (i >= PCI1716_MAXCHANNELNUM) break;                          // Maximalkanalanzahl nicht überschreiten
                PCI1716_GainArray[i] = AINGains[i];
        };
        PCI1716_MAIConfig.GainArray = PCI1716_GainArray;

        if (DRV_MAIConfig(PCI1716_DevH, &PCI1716_MAIConfig) != SUCCESS)
        {
                OK = false;
        };

        // Vorbereiten der ADU-In-Informationen
        for (int i = 0; i < PCI1716_MAXCHANNELNUM; ++i) PCI1716_VoltageArray[i] = 0;

        PCI1716_MAIVoltageIn.NumChan = PCI1716_MAIConfig.NumChan;
        PCI1716_MAIVoltageIn.StartChan = PCI1716_MAIConfig.StartChan;
        PCI1716_MAIVoltageIn.GainArray = PCI1716_MAIConfig.GainArray;
        PCI1716_MAIVoltageIn.TrigMode = 0;                                      // interner Trigger
        PCI1716_MAIVoltageIn.VoltageArray = PCI1716_VoltageArray;

        // Analog-Ausgang
        // --------------

        // Ausgang Geschwindigkeitssteuerung
        for (int i = 0; i < AOUTNUM; ++i)
        {
                PCI1716_AOConf.chan = AOUTCHAN[i];
                PCI1716_AOConf.RefSrc = AOUTRefSrc[i];
                PCI1716_AOConf.MinValue = AOUTMINVAL[i];
                PCI1716_AOConf.MaxValue = AOUTMAXVAL[i];

                if (DRV_AOConfig(PCI1716_DevH, &PCI1716_AOConf) != SUCCESS)
                {
                        OK = false;
                };
        };

        return OK;
};
// -----------------------------------------------------------------------------
// Karte schließen
// Rückgabewert: true  ... wenn alles i.O.
//               false ... Fehlgeschlagen
bool _fastcall PCI1716_Close()
{
        PT_DioWritePortByte DOPortByte;
        if (PCI1716_DevH != 0)
        {
                // alle gesetzten Ports zurücksetzen
                for (int i = 0; i < 2; ++i)
                {
                        DOPortByte.port = i;
                        DOPortByte.mask = 0xFFFF;
                        DOPortByte.state = 0;
                        DRV_DioWritePortByte(PCI1716_DevH, &DOPortByte);
                };

                // Port schließen
                DRV_DeviceClose(&PCI1716_DevH);
        };

        return true;
};
// -----------------------------------------------------------------------------
// Liest die Spannungen aus dem ADU
// Rückgabewert: true  ... wenn alles i.O.
//               fasle ... Fehlgeschlagen
// Voltages     Spannungen, die eingelesen wurden [0]... Kanal 0, [1]... Kanal 2 usw.
// VoltagesSize Größe des Felds Voltages
bool _fastcall PCI1716_AIN(float *Voltages, short VoltagesSize)
{
        if (PCI1716_DevH == 0)
        {
                return false;
        };

        if (DRV_MAIVoltageIn(PCI1716_DevH, &PCI1716_MAIVoltageIn) == SUCCESS)
        {
                for (unsigned short Channel = 0; Channel < PCI1716_MAXCHANNELNUM; ++Channel)
                {
                        if (Channel >= VoltagesSize) break;                           // Überlaufsicherung
                        Voltages[Channel] = PCI1716_MAIVoltageIn.VoltageArray[Channel];
                };
                return true;
        };

        return false;
};

// -----------------------------------------------------------------------------
// Gibt eine Spannung aus
// Rückgabewert: true  ... wenn alles i.O.
//               fasle ... Fehlgeschlagen
// Voltages     Spannungen, die eingelesen wurden [0]... Kanal 0, [1]... Kanal 1 usw.
// VoltagesSize Größe des Felds Voltages
bool _fastcall PCI1716_AOUT(float *Voltages, short VoltagesSize)
{
        bool OK = false;

        if (PCI1716_DevH == 0) {
                return OK;
        };

        // Kein MultiOut möglich
        PT_AOVoltageOut AnalogOut;

        OK = true;
        for (unsigned short Channel = 0; Channel < PCI1716_MAXOUTCHANNELNUM; ++Channel)
        {
                if (Channel >= VoltagesSize) break;                             // Überlaufsicherung
                AnalogOut.chan = Channel;
				AnalogOut.OutputValue = Voltages[Channel];
                if (DRV_AOVoltageOut(PCI1716_DevH, &AnalogOut) != SUCCESS) OK = false;
        };

        return OK;
};
/*
TODO
29.01.2007 deaktiviert, weil durch Neuimplementierung mit anderer Funktionsweise ersetzt
kann nach bewähren gelöscht werden
bool _fastcall PCI1716_AOUT(unsigned short Channel, float Voltage)
{
        if (PCI1716_DevH == 0)
        {
                return false;
        };

        PT_AOVoltageOut AnalogOut;
        AnalogOut.chan = Channel;
        AnalogOut.OutputValue = Voltage;

        if (DRV_AOVoltageOut(PCI1716_DevH, &AnalogOut) == SUCCESS) return true;

        return false;
};
*/
#endif // __ADVANT1716_H
