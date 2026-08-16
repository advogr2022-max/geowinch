//---------------------------------------------------------------------------
#ifndef WCDisplayThreadH
#define WCDisplayThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "constants.h"
#include "WCLog.h"
#include "WCLOGinternal.h"
//---------------------------------------------------------------------------
class WCDisplayThread : public TThread
{
private:
        WCLog *LOGH;                                                            // Handle für die LOG-Verwaltung
        long DevelInfo_LastLoopPerSecoundTestTime;                              // Zeit bei letzter Berechnung der Anzahl der Schleifendurchläufe pro Sekunde
        int DevelInfo_LoopsSinceLastTest;                                       // Anzahl der Schleifendurchläufe seit dem letzten Test

        // LOG-Datei Verwaltung
        long LOG_GetDiskFree_LastTime;                                          // Hilfsvariable, für Taktung der Speicherplatzprüfung für LOG-Dateien
        __int64 LOG_GetDiskFree_FreeSpace;                                      // Variable zum Aufnehmen des aktuellen freien Speichers, der die Protokollierung beeinflusst
        __int64 LOGinternal_GetDiskFree_FreeSpace;                              // Variable zum Aufnehmen des aktuellen freien Speichers, der die interne Protokollierung beeinflusst

        int LOG_OldDepth;                                                       // Die zuletzt "geloggte" Teufe
        time_t LOG_OldTime;                                                     // Die zuletzt "geloggte" Geschwindigkeit

        // interne LOG-Datei Verwaltung
        WCLOGinternal LOGinternal;                                              // Handler für interne LOG-Datei

        // HILFSMETHODEN
//        long GetProcessTime_in_ms();                                            // ermittelt die seit Prozessbeginn verbrauchte CPU-Zeit in ms

protected:
        void __fastcall Execute();
public:
        __fastcall WCDisplayThread(bool CreateSuspended);

        // VERWALTUNG
        void __fastcall ThreadTerminates();                                     // Signalisiert an das Übergeordnete Objekt, das dieser Thread sich soeben beendet

        void __fastcall ThreadProtected();                                      // Führt alle Actionen "Thread-sicher" aus

        // DARSTELLUNG
        void __fastcall ActualizeMainView();                                    // Aktualisiert die Hauptansicht
        void __fastcall ActualizeNormalView();                                  // Aktualisiert die Normalansicht
        void __fastcall ActualizeDevelInfos();                                  // Aktualisiert die Informationen, die nur im Entwicklermodus angezeigt werden

        // SPERREN
        void __fastcall ActualizeLock();                                        // Aktualisiert Sperren für Steuerelemente

        // ZUSTAENDE TASTEN
        void __fastcall ActualizeButtonView();                                  // Aktualisiert das aussehen von Tasten (für die Bedienung)

        // LOG-FILE
        void __fastcall ActGetDiskFree();                                       // Prüfen des noch verfügbaren Speicherplatzes
        void __fastcall ActLOG();                                               // Aktion betreffend der LOG-Datei
        void __fastcall ActLOGClose();
        void __fastcall ActLOGinternal();

        // ENTWICKLER_INFO
        void __fastcall TestLoops();                                            // Ermittelt die Anzahl der Schleifendurchläufe pro Sekunde
};
//---------------------------------------------------------------------------
#endif
