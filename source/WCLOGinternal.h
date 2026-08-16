//---------------------------------------------------------------------------
#ifndef WCLOGinternalH
#define WCLOGinternalH

#include <fstream.h>
#include <time.h>
#include "structs.h"
#include "VarControl.h"
//---------------------------------------------------------------------------
class WCLOGinternal
{
        private:
                        ofstream InternalLOGFile;                               // Datei-Handle für Maximal-Zugkraft-Datei

                        time_t oldTime;                                         // die beim letzten Schreib-Vorgang vorhandene Zeit

                        // Hilfsvariablen
                        int HV_LastMaxThreshold;                                // m.H. dieser Var. wird sichergestellt, dass ein Maximalzugkraftwert-Eintrag nur bei Änderung des aktuellen Wertes zum vorhergehenden Wert getätigt wird
                        bool HV_OilStrainerIsOn;                                // m.H. dieser Var. wird sichergestellt, dass ein Ölfiltereintrag nur bei Änderung des Zustandes ein/aus getätigt wird
                        bool HV_AutomotiveErrorIsOn;                            // m.H. dieser Var. wird sichergestellt, dass ein Fahrzeug-Fehler-Eintrag nur bei Änderung des Zustandes ein/aus getätigt wird
                        bool HV_HighTemperature_DepthCalcIsOn;                  // m.H. dieser Var. wird sichergestellt, dass ein Eintrag für "Temperaturüberschreitung Ein/Aus" nur bei Änderung von aus auf ein und umgekehrt protokolliert wird.
                        int  HV_LastHighTemperature_DepthCalc;                  // m.H. dieser Var. wird sichergestellt, dass eine Temperaturüberschreitung nur dann Protokolliert wird, wenn sich der zu hohe Wert ändert
                        bool HV_LOGSleepIsOn;                                   // m.H. dieser Var. wird sichergestellt, dass ein Eintrag für "Protokollierung abschalten" nur bei Änderung des Zustandes ein/aus getätigt wird
                        bool HV_LOGinternalSleepIsOn;                           // m.H. dieser Var. wird sichergestellt, dass ein Eintrag für "interne Protokollierung abschalten" nur bei Änderung des Zustandes ein/aus getätigt wird
                        bool HV_DepthCalcCommErrorOn;                           // m.H. dieser Var. wird sichergestellt, dass ein Eintrag für "Fehler bei der Kommunikation mit dem Teufenrechner" nur bei Änderung des Zustandes ein/aus getätigt wird
                        bool HV_ControllerErrorMonitIsOff;                      // m.H. dieser Var. wird sichergestellt, dass ein Eintrag für Umschalten der Steuergerät-Fehlerüberwachung nur bei Änderung des Zustandes ein/aus getätigt wird
                        bool HV_IncEncErrorMonitIsOff;                          // m.H. dieser Var. wird sichergestellt, dass ein Eintrag für Umschalten der IGR-Fehlerüberwachung nur bei Änderung des Zustandes ein/aus getätigt wird
                        bool HV_GearShiftErrorIsOn;                             // m.H. dieser Var. wird sichergestellt, dass ein Eintrag für das Auftreten einer korrupten Schaltgetriebestellung nur bei Änderung des Zustandes ein/aus getätigt wird
                        bool HV_LostMotionIsOn;                                 // m.H. dieser Var. wird sichergestellt, dass ein Eintrag für den Zustand des Freilaufs nur bei Änderung des Zustandes ein/aus getätigt wird
                        bool HV_UnloadCableIsOn;                             	// m.H. dieser Var. wird sichergestellt, dass ein Eintrag für "Kabel entladen" nur bei Änderung des Zustandes ein/aus getätigt wird

        public:
                        WCLOGinternal();                                        // Konstruktor
                        ~WCLOGinternal();                                       // Destruktor

                        // DATEI ÖFFNEN UND SCHLIEßEN
                        bool OpenInternalLOGFile(char *LOGFilePathName);        // Öffnet eine neue LOG-Datei
                        bool CloseInternalLOGFile();                            // Schließt eine geöffnete LOG-Datei

                        // DATEN SCHREIBEN
                        bool WriteMaxThresholdInFile(int Value);                // Schreiben in die Maximal-Zugkraft-LOG-Datei
                        bool WriteCloseFile();                                  // Schreiben des Kommentars zum Schließen der Datei
                        bool WriteOpenFile();                                   // Schreiben des Kommentars zum Öffnen der Datei
                        bool WriteOilStrainerLampON();                          // Öl-Filter Lampe ist angegangen
                        bool WriteOilStrainerLampOFF();                         // Öl-Filter Lampe ist erloschen
                        bool WriteAutomotiveErrorON();                          // Fahrzeug-Fehler ausgelöst
                        bool WriteAutomotiveErrorOFF();                         // Fahrzeug-Fehler aus
                        bool WriteHighTemperature_DepthCalcOn(int Value);       // Protokollieren, wenn Temperatur auf dem Teufenrechner zu hoch
                        bool WriteHighTemperature_DepthCalcOff();               // Protokollieren, wenn Temperatur auf dem Teufenrechner im Normalbereich
                        bool WriteLOGSleepOn(__int64 Value);                    // Protokollierung bei Value eingestellt
                        bool WriteLOGSleepOff(__int64 Value);                   // Protokollierung bei Value wieder aufgenommen
                        bool WriteLOGinternalSleepOn(__int64 Value);            // interne Protokollierung bei Value eingestellt
                        bool WriteLOGinternalSleepOff(__int64 Value);           // interne Protokollierung bei Value wieder aufgenommen
                        bool WriteDepthCalcCommErrorON();                       // Fehler bei der Kommunikation mit dem Teufenrechner aufgetreten
                        bool WriteDepthCalcCommErrorOFF();                      // Fehler bei der Kommunikation mit dem Teufenrechner wieder verschwunden
                        bool WriteControllerErrorMonitON();                     // Protokollierung, wenn Steuergerät Fehlerüberwachung abgeschalten wurde
                        bool WriteControllerErrorMonitOFF();                         // Protokollierung, wenn Steuergerät Fehlerüberwachung wieder eingeschalten wurde
                        bool WriteIncEncErrorMonitON();                         // Protokollierung, wenn Teufen-Encoder Fehlerüberwachung abgeschalten wurde
                        bool WriteIncEncErrorMonitOFF();                        // Protokollierung, wenn Teufen-Encoder Fehlerüberwachung wieder eingeschalten wurde
                        bool WriteGearShiftErrorON();                           // Protokollierung, wenn die Schaltstellung des Schaltgetriebes fehlerhaft ist
                        bool WriteGearShiftErrorOFF();                          // Protokollierung, wenn die Schaltstellung des Schaltgetriebes wieder i.O. ist
                        bool WriteLostMotionON();                               // Protokollierung, wenn Freilauf eingeschaltet wird
                        bool WriteLostMotionOFF();                              // Protokollierung, wenn Freilauf ausgeschaltet wird
						bool WriteUnloadCableON();                           	// Protokollierung, wenn "Kabel entladen" eingeschaltet wird
                        bool WriteUnloadCableOFF();                          	// Protokollierung, wenn "Kabel entladen" ausgeschaltet wird
};

#endif
