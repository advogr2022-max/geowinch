//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "WCSoundThread.h"
#include "GUIMain.h"
#pragma package(smart_init)

//---------------------------------------------------------------------------
__fastcall WCSoundThread::WCSoundThread(bool CreateSuspended)
        : TThread(CreateSuspended)
{
		Priority = tpIdle;
//		Priority = tpLower;
//		Priority = tpHighest;
        FreeOnTerminate = true;

        DevelInfo_LoopsSinceLastTest = -1;

		InternalAlarmScheme = 0;
}
//---------------------------------------------------------------------------
void __fastcall WCSoundThread::Execute()
{

	while(!Terminated) {

			Synchronize(GetAlarmScheme);

			// Alarmschema auswählen
			switch (InternalAlarmScheme) {

				case 1: // Alarmschema 1
						// getakteter Piepton
						Beep(Alarm1_AcousticFrequency, Alarm1_OnDuration);
						Sleep(Alarm1_OffDuration);
						break;

				case 2: // Alarmschema 2
						// getakteter Piepton
						Beep(Alarm2_AcousticFrequency, Alarm2_OnDuration);
						Sleep(Alarm2_OffDuration);
						break;

				case 3: // Alarmschema 3
						// Zwei sich abwechslende Töne
						Beep(Alarm3_AcousticStartFrequency1, Alarm3_OnDuration1);
						Sleep(100);
						Beep(Alarm3_AcousticStartFrequency2, Alarm3_OnDuration2);
						Sleep(100);
						break;

				case 0:
						// Alarmschema 0 (Standard)
						// Alarm aus

				default:
						// Alarmschema 0
						// Alarm aus
						Sleep(2000);
						break;
			};

			Synchronize(TestLoops);
//		};
	};

	Synchronize(ThreadTerminates);

}
//---------------------------------------------------------------------------

// ==========================================================================
// VERWALTUNG
// ==========================================================================
// Signalisiert an das Übergeordnete Objekt, das dieser Thread sich soeben beendet
void __fastcall WCSoundThread::ThreadTerminates()
{
        MainForm->SoundThreadTerminates = true;
};

//---------------------------------------------------------------------------
// Überprüfen des gesetzten Alarmschemas
void __fastcall WCSoundThread::GetAlarmScheme()
{
        InternalAlarmScheme = AlarmScheme;
};

// ==========================================================================
// ENTWICKLER_INFO
// ==========================================================================
// Ermittelt die Anzahl der Schleifendurchläufe pro Sekunde
void __fastcall WCSoundThread::TestLoops()
{
        if (MainForm->VarControl.devel() == _OFF) return;

        long CurrTime, TimeDiff;

        CurrTime = GetProcessTime_in_ms();

        ++DevelInfo_LoopsSinceLastTest;

        TimeDiff = CurrTime - DevelInfo_LastLoopPerSecoundTestTime;
        if (TimeDiff < 10000) return;

        MainForm->GenOrgan.SoundThreadLoopsPerSecound = (float)(DevelInfo_LoopsSinceLastTest) * (float)(CLOCKS_PER_SEC) / (float)(TimeDiff);

        DevelInfo_LoopsSinceLastTest = 0;
        DevelInfo_LastLoopPerSecoundTestTime = CurrTime;
};

