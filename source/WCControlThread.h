//---------------------------------------------------------------------------
#ifndef WCControlThreadH
#define WCControlThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "WCAnalysis.h"
//---------------------------------------------------------------------------
class WCControlThread : public TThread
{
private:
        WCAnalysis Analysis;

        long DevelInfo_LastLoopPerSecoundTestTime;                              // Zeit bei letzter Berechnung der Anzahl der Schleifendurchläufe pro Sekunde 
        int DevelInfo_LoopsSinceLastTest;                                       // Anzahl der Schleifendurchläufe seit dem letzten Test

        // HILFSMETHODEN
//        long GetProcessTime_in_ms();                                            // ermittelt die seit Prozessbeginn verbrauchte CPU-Zeit in ms

protected:
        void __fastcall Execute();
public:
        __fastcall WCControlThread(bool CreateSuspended);

        // VERWALTUNG
        void __fastcall CleanUp();                                              // Aufräumen
        void __fastcall ThreadTerminates();                                     // Signalisiert an das Übergeordnete Objekt, das dieser Thread sich soeben beendet

        // ANALYSE
        void _fastcall MakeAnalysis();                                          // Durchführen der Analyse

        // ENTWICKLER_INFO
        void __fastcall TestLoops();                                            // Ermittelt die Anzahl der Schleifendurchläufe pro Sekunde
};
//---------------------------------------------------------------------------
#endif
