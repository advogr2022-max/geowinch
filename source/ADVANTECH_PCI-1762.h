/*
WinchControl    Jörg Müller    ADVANTECH_PCI-1762.h

Enthält Funktionen zum speziellen problemorientierten Zugriff auf die DIO-Karte

                        Advantech PCI-1762

Portierbarkeit: nein
*/

#ifndef __ADVANT1762_H
#define __ADVANT1762_H

#include "constants.h"
#include "ADSAPI\Include\Device.h"


long            PCI1762_DevH = 0;                                               // Handle für die Steuer-Karte
unsigned long   PCI1762_DevNum = 0xFFFFFFFF;                                    // Nummern der IO-Karten

// -----------------------------------------------------------------------------
// Initialisieren der Variablen/Handles
void _fastcall PCI1762_VarInit()
{
        PCI1762_DevNum = 0xFFFFFFFF;                                            // noch keine Karte detektiert
        PCI1762_DevH = 0;
};

// -----------------------------------------------------------------------------
// Suchen nach der richtigen Karte
// Rückgabewert:    true ...    passende Karte gefunden und Gerätenummer gesetzt
//                  false ...   passende Karte nicht gefunden,
//                              PCI..._DevNum bereits gesetzt,
//                              PCI..._DevH bereits gesetzt,
//                              PCI..._DevNum noch nicht initialisiert oder
//                              PCI..._DevH noch nicht initialisiert gesetzt
bool _fastcall PCI1762_CardFind()
{
    DEVFEATURES DevFeatures;
    PT_DeviceGetFeatures DevGetFeatures;
    DevGetFeatures.buffer = &DevFeatures;
    DevGetFeatures.size = sizeof(DevFeatures);

    if ((PCI1762_DevNum != 0xFFFFFFFF) ||
        (PCI1762_DevH != 0)) return false;  //Variable schon belegt

    // Nacheinander versuchen verschiedene Karten zu öffnen
    for (PCI1762_DevNum = 0; PCI1762_DevNum < MAXNUMBEROFADVANTECHCARDS; ++PCI1762_DevNum) {


        if (DRV_DeviceOpen(PCI1762_DevNum, &PCI1762_DevH) == SUCCESS) {

            // Anhand der Board-ID die passende Karte identifizieren
            if (DRV_DeviceGetFeatures(PCI1762_DevH, &DevGetFeatures) == SUCCESS) {

                DRV_DeviceClose(&PCI1762_DevH);
                PCI1762_DevH = 0; // Handle wieder als unbenutzt kennzeichnen

                // 0x80 ist die Advantech PCI-1762 Board-ID,
                // die den Typ der Karte angibt
                // PCI..._DevNum bleibt markiert, da so beim Öffnen und
                // Initialisieren der Karte diese lokalisiert wird
                if (DevFeatures.dwBoardID == 0x80) return true; // Karte gefunden
            }

            DRV_DeviceClose(&PCI1762_DevH);
        }
    }

    PCI1762_VarInit();

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
bool _fastcall PCI1762_CardOpen()
{
    DEVFEATURES DevFeatures;
    PT_DeviceGetFeatures DevGetFeatures;
    DevGetFeatures.buffer = &DevFeatures;
    DevGetFeatures.size = sizeof(DevFeatures);

    if ((PCI1762_DevNum == 0xFFFFFFFF) ||
        (PCI1762_DevH != 0)) return false;  // Kartenzugang schon geöffnet

    if (DRV_DeviceOpen(PCI1762_DevNum, &PCI1762_DevH) == SUCCESS) {

        // Anhand der Board-ID Karte auf Typ prüfen
        if (DRV_DeviceGetFeatures(PCI1762_DevH, &DevGetFeatures) == SUCCESS) {
            if (DevFeatures.dwBoardID == 0x80) return true;
        }
        DRV_DeviceClose(&PCI1762_DevH);
    }

    PCI1762_VarInit(); // Karte als nicht vorhanden markieren
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
bool _fastcall PCI1762_CardInit(short *Error_MSG)
{
//        short OutEntries;                                                       // Anzahl der gefundenen Karten
//        DEVLIST DeviceList[MAXNUMBEROFADVANTECHCARDS];
        DEVFEATURES DevFeatures;
        PT_DeviceGetFeatures DevGetFeatures;
        DevGetFeatures.buffer = &DevFeatures;
        DevGetFeatures.size = sizeof(DevFeatures);

        // noch keine Karte detektiert
        PCI1762_DevNum = 0xFFFFFFFF;
        PCI1762_DevH = 0;

        try {
            // Suchen nach Advatech PCI-1762
//  Diese Variante funktioniert nicht zu 100% bei Win XP Embedded System mit UNO3072L-DIO Treiber
//    am 21.04.2011 deaktiviert
//
//            DRV_DeviceGetList (DeviceList, MAXNUMBEROFADVANTECHCARDS, &OutEntries);
//            // alle gefundenen Karten überprüfen, ob die gewünschten dabei sind ?
//            for (short i = 0; i < OutEntries; ++i)
//            {
//                ShowMessage(DeviceList[i].szDeviceName);
//                // Karte gefunden ?
//                if (strncmp(DeviceList[i].szDeviceName, DIORelayCardName, strlen(DIORelayCardName)) == 0)
//                {
//                        PCI1762_DevNum = DeviceList[i].dwDeviceNum;
//                        break;
//                }
//            }
//
//            if (PCI1762_DevNum == 0xFFFFFFFF)
//            {
//                *Error_MSG = 0x0001;                                            // Karte nicht gefunden
//                return false;
//            }
//
//
//            // Karte öffnen
//            if (DRV_DeviceOpen(PCI1762_DevNum, &PCI1762_DevH) != SUCCESS)
//            {
//                PCI1762_DevNum = 0xFFFFFFFF;
//                PCI1762_DevH = 0;
//
//                *Error_MSG = 0x0002;                                            // Karte konnte nicht geöffnet werden
//                return false;
//            }
//
//
            // Nacheinander versuchen verschiedene Karten zu öffnen
            for (short i = 0; i < MAXNUMBEROFADVANTECHCARDS; ++i) {

                PCI1762_DevNum = i;
                PCI1762_DevNum = 2;

                if (DRV_DeviceOpen(PCI1762_DevNum, &PCI1762_DevH) == SUCCESS) {

                    // Anhand der Board-ID die passende Karte identifizieren
                    if (DRV_DeviceGetFeatures(PCI1762_DevH, &DevGetFeatures) == SUCCESS) {
                        if (DevFeatures.dwBoardID == 0x80) break;
                    }

                    DRV_DeviceClose(&PCI1762_DevH);
                }

                PCI1762_DevNum = 0xFFFFFFFF;
                PCI1762_DevH = 0;

            }

            if (PCI1762_DevNum == 0xFFFFFFFF)
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
// Karte schließen
// Rückgabewert: true  ... wenn alles i.O.
//               fasle ... Fehlgeschlagen
bool _fastcall PCI1762_Close()
{
        PT_DioWritePortByte DOPortByte;

        // Ports zu den Mess-/Steuer-Karten schließen
        if (PCI1762_DevH != 0)                                                  // DIO-Relay-Karte
        {
                // alle gesetzten Ports zurücksetzen
                for (int i = 0; i < 2; ++i)
                {
                        DOPortByte.port = i;
                        DOPortByte.mask = 0xFFFF;
                        DOPortByte.state = 0;
                        DRV_DioWritePortByte(PCI1762_DevH, &DOPortByte);
                };

                // Port schließen
                DRV_DeviceClose(&PCI1762_DevH);
        };

        return true;
};

// -----------------------------------------------------------------------------
// Liest alle Port-Zustände ein
// Rückgabewert: true  ... wenn alles i.O.
//               false ... Fehlgeschlagen
// Buffer       ... Rückabepuffer
// Size         ... Puffergröße
// Ist die Puffergröße größer als 16, dann werden alle Elemente ab dem 17 ignoriert.
// Ist die Puffergröße kleiner als 16, dann werden die fehlenden Bits auf 0 gesetzt.
bool _fastcall PCI1762_DIN(bool *Buffer, int size)
{
        PT_DioReadPortByte DIPortByte;                                          // Puffer für Information
        unsigned short value = 0xAA;

        unsigned short WaitingInPorts = 16;                                     // Die Anzahl der noch nicht abgearbeiteten Ausgabeports (max. 16, weil Karte nur so viel kann)
        if (size < WaitingInPorts) WaitingInPorts = size;

        DIPortByte.value = &value;

        // alle Ports zurücksetzen
        for (int i = 0; i < size; ++i) Buffer[i] = false;

        for (unsigned short us = 0; us < 2; ++us)                               // Byte-weiser Zugriff auf Hardware
        {
                DIPortByte.port = us;                                           // Byte feslegen
                if (DRV_DioReadPortByte(PCI1762_DevH, &DIPortByte) == SUCCESS)
                {
                        // aktuelles DI-Byte auswerten
                        for (int i = (us * 8); i < ((us + 1) * 8); ++i)         // Transformieren der Port-Bits auf die Kanal-Nummern
                        {
                                if (WaitingInPorts <= 0) break;                 // Puffergröße beachten

                                Buffer[i] = value & 0x01;                       // Transfer in internen Puffer
                                value >>= 1;

                                --WaitingInPorts;
                        };
                }else return false;

        };

        return true;
};

// -----------------------------------------------------------------------------
// Setzt die Ports
// Rückgabewert: true  ... wenn alles i.O.
//               false ... Fehlgeschlagen
// Buffer       ... Eingabepuffer
// Size         ... Puffergröße
// Ist die Puffergröße größer als 16, dann werden alle Elemente ab dem 17 ignoriert.
// Ist die Puffergröße kleiner als 16, dann werden die fehlenden Bits auf 0 gesetzt.
bool _fastcall PCI1762_DOUT(bool *Buffer, int size)
{
        unsigned short WaitingOutPorts = 16;                                    // Die Anzahl der noch nicht abgearbeiteten Ausgabeports (max. 16, weil Karte nur so viel kann)
        if (size < WaitingOutPorts) WaitingOutPorts = size;

        PT_DioWritePortByte DOByte;

        if (PCI1762_DevH == 0)
        {
                return false;
        };

        DOByte.mask = 0xFFFF;                                                   // Ausgabe-Byte preparieren

        for (unsigned short us = 0; us < 2; ++us)
        {
                DOByte.port = us;                                               // Port festlegen
                DOByte.state = 0;                                               // Status rücksetzen
                for (unsigned short us2 = 0; us2 < 8; ++us2)                    // Bits des internen Puffer in den Treiberstruktur transferieren
                {
                        if (WaitingOutPorts <= 0) break;                        // Puffergröße beachten

                        DOByte.state |= (unsigned short)(Buffer[(us * 8) + us2]) << us2;

                        --WaitingOutPorts;
                };

                if (DRV_DioWritePortByte(PCI1762_DevH, &DOByte) != SUCCESS) return false;;
        };

        return true;
};

#endif //__ADVANT1762_H
