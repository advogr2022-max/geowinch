/*
WinchControl    Jörg Müller     WCIO.cpp

Klassenimplementation Ein-/Ausgabe-Klasse

Portierbarkeit: nein
*/
//---------------------------------------------------------------------------
#ifndef WCIOH
#define WCIOH
//---------------------------------------------------------------------------

// Auswahl des verwendeten Treibers für die Advantech Karten
#define DAQNAVI_ARCH            // Verwendet den DAQNAVI-Treiber anstatt des ADSAPI-Treibers für die Advantech-Karten

#include "structs.h"
#include "VarControl.h"

//-------------------------------
#ifdef DAQNAVI_ARCH

#include "ADV_DAQNavi_PCI1716.h"
#include "ADV_DAQNavi_PCI1762.h"

#endif // DAQNAVI_ARCH
//-------------------------------
#ifndef DAQNAVI_ARCH

#include "ADSAPI\Include\Device.h"
#include "ADVANTECH_PCI-1716.h"
#include "ADVANTECH_PCI-1762.h"

#endif // DAQNAVI_ARCH
//-------------------------------
#include "TR22Protocol.h"


class WCIO
{
        private:
                        HANDLE InfoOutH;                                        // Schnittstelle zum COM-Port für die Informationsausgabe
                        bool InfoOutOk;                                         // Schnittstelle geöffnet
                        _COMMTIMEOUTS InfoOutctmoOld, InfoOutctmoNew;           // Schnittstellenparameter Timeout
                        _DCB InfoOutdcbCPortOld, InfoOutdcbCPortNew;            // schnittstellenparameter Übertragung

                        HANDLE DepthCalcH;                                      // Handle für Teufenrechner-SS
                        TR22Protocol DepthCalc;                                 // Teufenrechner Kommunikations-Klasse
                        bool DepthCalcOk;                                       // Gibt an, ob die Schnittstelle zum Teufenrechner geöffnet werden konnte
                        _COMMTIMEOUTS DepthCalcctmoOld, DepthCalcctmoNew;       // Schnittstellenparameter Timeout
                        _DCB DepthCalcdcbCPort_Old, DepthCalcdcbCPort_New;      // schnittstellenparameter Übertragung

// -----------------------------------
#ifdef DAQNAVI_ARCH
						JMDAQNaviPCI1716 PCI1716; 								// Steuerung der Advantech PCI-1716
						JMDAQNaviPCI1762 PCI1762;                               // Steuerung der Advantech PCI-1762
#endif //DAQNAVI_ARCH
// -----------------------------------
#ifndef DAQNAVI_ARCH
						float AINVoltageBuffer[8];                              // Puffer für analoge-Eingangsspannungen
						float AOutVoltageBuffer[2];                             // Puffer für analoge-Ausgangsspannungen
						bool  DIStatusBuffer[16];                               // Puffer für Digitale Eingangs-Stati
						bool  DOSetBuffer[16];                                  // Puffer für Digitale Ausgangs-Einstellungen
#endif //DAQNAVI_ARCH
// -----------------------------------
						// VERWALTUNG PRIVATE
                        HANDLE OpenCOMHandle(HANDLE hComm, char *Port,
                                          _COMMTIMEOUTS *newctmo, _DCB *newdcb,
                                          _COMMTIMEOUTS *oldctmo, _DCB *olddcb);// Öffnet die angegebene serielle Schnittstelle
                        void CloseCOMHandle(HANDLE hComm, _COMMTIMEOUTS *oldctmo, _DCB *olddcb);// Schließt die angegebene serielle Schnittstelle

        public:
                        WCIO();
                        ~WCIO();

                        // VERWALTUNG
						void Init();                                            // Initialisiert die Klasse
						void InitInfoCOM(WCGenOrganisationS *GenOrgan);         // Initialisieren der Schnittstelle für Informationsausgabe
						void InitDepthCalcCOM(WCGenOrganisationS *GenOrgan);    // Initialisieren der Schnittstelle zum Teufenrechner
						void InitDIOCard(WCGenOrganisationS *GenOrgan);         // Initialisieren der DIO-Relay-Card
						bool ConfigDIODAC(WCGenOrganisationS *GenOrgan);        // Konfiguriert die Analogkarte neu
						void CleanUp();                                         // Aufräumen

						// HARDWARE ZUGRIFF

						// TEUFENRECHNER
                        int GetDepth(bool *MagneticMark, bool *MagneticMarkDirect, int *TimeSinceLastQuery, bool *Error); // Fragt die aktuelle Teufe vom Teufenrechner ab
                        bool GetDepthCalcSWVersion(char *Version, int VersionLength); // Software-Version des Teufenrechners ermitteln
                        bool SetDepthCalcCalFact(int CalFactor);                // explizit Eichfaktor setzen
                        bool SetDepthCalcDepth(int Depth);                      // explizit Teufe setzen
                        bool SetDepthCalcMagneticMark(bool MagneticMark);       // explizit Magnetmarke setzen
                        bool SetDepthCalcParam(WCVarControl *V);                // Konfiguriert den Teufenrechner

                        // DIO-RELAY-CARD
                        bool DOPortOn(short Num);                               // setzt den angegebenen Digitalausgang auf High
                        bool DOPortOff(short Num);                              // setzt den angegebenen Digitalausgang auf Low
                        bool DOOut();                                           // Ausgabe des internen Puffers an DO-Hardware

                        bool DIPortStatus(short Num, bool FromHardware, bool *Error); // fragt den Status des angegebenen Einganges ab

                        float ADUIn(unsigned short Channel, bool FromHardware, bool *Error); // Gibt die am angegebenen Kanal anliegende Spannung zurück
                        bool ADUOut(unsigned short Channel, float Voltage, bool IgnoreChannel, bool ToHardware); // Gibt die angegebene Spannung über den DAU aus

                        // EXTERNE INFO-SS
                        char CheckInfoOutPort();                                // Fragt den Eingang des Informationsausgabeprts ab und gibt das jeweils eingelesene Byte zurück
                        bool WriteToInfoOutPort(unsigned char *InfoData, unsigned int InfoDataLength); // Sendet die angegebenen Daten an den Informations-Ausgabe-Port
};
#endif

