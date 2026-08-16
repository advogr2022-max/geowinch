//---------------------------------------------------------------------------
#ifndef WCSoundThreadH
#define WCSoundThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>

// -----------------------------
// Definition Alarmschemen
// Alarmschema 1
// getakteter Piepton
const Alarm1_AcousticFrequency = 4000;                                          // Tonfrequenz
const Alarm1_OnDuration = 1000;                                                 // Dauer des Tons
const Alarm1_OffDuration = 1000;                                                // Pause zwischen den Tönen

// -----------------------------
// Alarmschema 2
// getakteter Piepton
const Alarm2_AcousticFrequency = 8000;                                          // Tonfrequenz
const Alarm2_OnDuration = 2000;                                                 // Dauer des Tons
const Alarm2_OffDuration = 1000;                                                // Pause zwischen den Tönen

// -----------------------------
// Alarmschema 3
// Ton mit aufsteigender Frequenz
const Alarm3_AcousticStartFrequency1 = 4000;                                    // Tonfrequenz des ersten Schrittes
const Alarm3_AcousticStartFrequency2 = 3000;                                    // Tonfrequenz des zweiten Schrittes
const Alarm3_OnDuration1 = 1000;                                                // Dauer des ersten Tons
const Alarm3_OnDuration2= 1000;                                                 // Dauer des zweiten Tons

// -----------------------------

//---------------------------------------------------------------------------
class WCSoundThread : public TThread
{
private:

        long DevelInfo_LastLoopPerSecoundTestTime;                              // Zeit bei letzter Berechnung der Anzahl der Schleifendurchläufe pro Sekunde
        int DevelInfo_LoopsSinceLastTest;                                       // Anzahl der Schleifendurchläufe seit dem letzten Test

		int InternalAlarmScheme;

protected:
        void __fastcall Execute();
        void __fastcall GetAlarmScheme();                                       // Überprüfen des gesetzten Alarmschemas
        void __fastcall WCSoundThread::ThreadTerminates();                      // Signalisiert an das Übergeordnete Objekt, das dieser Thread sich soeben beendet
        
public:
        __fastcall WCSoundThread(bool CreateSuspended);
		int AlarmScheme;                                                        // Gibt aus, welches Alarm-Schema über den Lautsprecher ausgegeben werden soll

		        // ENTWICKLER_INFO
		void __fastcall TestLoops();                                            // Ermittelt die Anzahl der Schleifendurchläufe pro Sekunde

};
//---------------------------------------------------------------------------
#endif
