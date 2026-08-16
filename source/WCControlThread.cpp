//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "WCControlThread.h"
#include "GUIMain.h"
#pragma package(smart_init)

//---------------------------------------------------------------------------
//  Wichtig: Methoden und Eigenschaften von VCL-Objekten können nur in
//  Methodenaufrufen unter Verwendung von Synchronize benutzt werden, z.B.:
//
//      Synchronize(UpdateCaption);
//
//  wobei UpdateCaption so aussehen könnte:
//
//      void __fastcall WCControlThread::UpdateCaption()
//      {
//        Form1->Caption := 'Aktualisiert im Thread';
//      }
//---------------------------------------------------------------------------

__fastcall WCControlThread::WCControlThread(bool CreateSuspended)
        : TThread(CreateSuspended)
{
//		Priority = tpLower;
//        Priority = tpHighest;
//        Priority = tpTimeCritical;
        FreeOnTerminate = true;

        DevelInfo_LoopsSinceLastTest = -1;
        Analysis.Init(&MainForm->VarControl, &MainForm->GenOrgan);
}
//---------------------------------------------------------------------------
void __fastcall WCControlThread::Execute()
{
	clock_t prevtime, cntdwn;							// Hilfsvariablem für Taktgeber


	IsTimeElapsed(&prevtime, &cntdwn, 20, 0, true);	// Taktgeber initialisieren

		while(!Terminated)
		{
				if (IsTimeElapsed(&prevtime, &cntdwn, 20, 0, false)) {
					Synchronize(MakeAnalysis);
					// Schleifendurchläufe testen
					Synchronize(TestLoops);
//					Application->ProcessMessages();
				};

            	Sleep(0);	// Verlassen der Zeitschleife für anderen Thread gleicher Priorität
//				Application->ProcessMessages();

		};

		Analysis.CleanUp();
		Synchronize(ThreadTerminates);

}
// ==========================================================================
// HILFSMETHODEN
// ==========================================================================
// ermittelt die seit Prozessbeginn verbrauchte CPU-Zeit in ms
/*long WCControlThread::GetProcessTime_in_ms()
{
        return ((float)(clock()) * (float)(1000) / (float)(CLOCKS_PER_SEC));
};
*/
// ==========================================================================
// VERWALTUNG
// ==========================================================================
// Aufräumen
void __fastcall WCControlThread::CleanUp()
{
        Analysis.CleanUp();
};

//---------------------------------------------------------------------------
// Signalisiert an das Übergeordnete Objekt, das dieser Thread sich soeben beendet
void __fastcall WCControlThread::ThreadTerminates()
{
        MainForm->ControlThreadTerminates = true;
};

// ==========================================================================
// ANALYSE
// ==========================================================================
// Durchführen der Analyse
void _fastcall WCControlThread::MakeAnalysis()
{
		Analysis.Analyze();
};

// ==========================================================================
// ENTWICKLER_INFO
// ==========================================================================
// Ermittelt die Anzahl der Schleifendurchläufe pro Sekunde
void __fastcall WCControlThread::TestLoops()
{
        if (MainForm->VarControl.devel() == _OFF) return;

        long CurrTime, TimeDiff;

        CurrTime = GetProcessTime_in_ms();

        ++DevelInfo_LoopsSinceLastTest;

        TimeDiff = CurrTime - DevelInfo_LastLoopPerSecoundTestTime;
        if (TimeDiff < 10000) return;

        MainForm->GenOrgan.ControlThreadLoopsPerSecound = (float)(DevelInfo_LoopsSinceLastTest) * (float)(CLOCKS_PER_SEC) / (float)(TimeDiff);

        DevelInfo_LoopsSinceLastTest = 0;
        DevelInfo_LastLoopPerSecoundTestTime = CurrTime;
};

