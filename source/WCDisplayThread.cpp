//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "WCDisplayThread.h"
#include "GUIMain.h"
#include "tools.h"

#pragma package(smart_init)

//---------------------------------------------------------------------------
__fastcall WCDisplayThread::WCDisplayThread(bool CreateSuspended)
        : TThread(CreateSuspended)
{
//		Priority = tpLowest;
//		Priority = tpLower;
//		Priority = tpHigher;
//        Priority = tpTimeCritical;
        FreeOnTerminate = true;

        LOG_OldDepth = -1000000;
        LOG_OldTime = 0;
        LOG_GetDiskFree_LastTime = 0;
        LOG_GetDiskFree_FreeSpace = 0;
        LOGinternal_GetDiskFree_FreeSpace = 0;

        DevelInfo_LoopsSinceLastTest = -1;

        // Instanzierung LOG-Handle
        LOGH = 0;
        try
        {
                LOGH = new WCLog(&MainForm->GenOrgan, &MainForm->VarControl);
        }catch(...)
        {
                LOGH = 0;
		};

        // Interne LOG-Dateien
        // absolut zulässige Maximal-Zugkraft
#ifndef __DEMO_MODE
		LOGinternal.OpenInternalLOGFile(MainForm->GenOrgan.LOGPathinternal);
#endif

}

//---------------------------------------------------------------------------
void __fastcall WCDisplayThread::Execute()
{
	clock_t prevtime, cntdwn;							// Hilfsvariablem für Taktgeber


	IsTimeElapsed(&prevtime, &cntdwn, 150, 0, true);	// Taktgeber initialisieren

	while(!Terminated) {
		if (IsTimeElapsed(&prevtime, &cntdwn, 100, 0, false)) {
			Synchronize(ThreadProtected);
			Application->ProcessMessages();
		};
//		Sleep(0);	// Verlassen der Zeitschleife für anderen Thread gleicher Priorität
	};

	// Aufräumen
	//if (LOGH != 0) delete LOGH;
	Synchronize(ActLOGClose);
	LOGinternal.CloseInternalLOGFile();
	SetFileAttributes(MainForm->GenOrgan.LOGPathinternal, FILE_ATTRIBUTE_HIDDEN);        // Nach dem Schließen der Protokolldatei wird das Dateiattribut auf versteckt gesetzt

	Synchronize(ThreadTerminates);
}
// ==========================================================================
// HILFSMETHODEN
// ==========================================================================
// ermittelt die seit Prozessbeginn verbrauchte CPU-Zeit in ms
/*long WCDisplayThread::GetProcessTime_in_ms()
{
		return ((float)(clock()) * (float)(1000) / (float)(CLOCKS_PER_SEC));
};
*/
// ==========================================================================
// VERWALTUNG
// ==========================================================================
// Signalisiert an das Übergeordnete Objekt, das dieser Thread sich soeben beendet
void __fastcall WCDisplayThread::ThreadTerminates()
{
		MainForm->DisplayThreadTerminates = true;
};

//---------------------------------------------------------------------------
// Führt alle Actionen "Thread-sicher" aus
void __fastcall WCDisplayThread::ThreadProtected()
{
		bool Hbool = MainForm->VarControl.GetInfo_MagneticMarkStatic();         // Das Rücksetzen muss von diesem Eintrag abhängig gemacht werden, damit alle evtl. zuständigen Methoden die Magnetmarke auswerten können, bevor "MainForm->VarControl.SetInfo_MagneticMarkStaticOff()" aufgerufen wird

		// Hauptkontrollanzeige
		ActualizeMainView();
		ActualizeNormalView();
		ActualizeDevelInfos();

		// Sperren aktualisieren
		ActualizeLock();

		// Tasten-Ansicht
		ActualizeButtonView();

		// LOG-Datei bearbeiten
		ActGetDiskFree();                                                       // ActGetDiskFree unbedingt vor dem ersten Aufruf von ActLOG() ausführen, damit nicht bereits mit einer Protokoll-Datei begonnen wird, obwohl kein Platz dafür vorhanden ist (keine leeren, nutzlosen LOG-Dateien schreiben)
		ActLOG();
		ActLOGinternal();

		// Schleifendurchläufe testen
		TestLoops();

		// Nach der Abarbeitung kann das Magnetmarkenanzeige-Flag wieder zurückgesetzt werden
		// sofern diese detektiert wurde
		if (Hbool)
		{
				MainForm->VarControl.SetInfo_MagneticMarkStaticOff();
		};
};


// ==========================================================================
// DARSTELLUNG
// ==========================================================================

// Aktualisiert die Hauptansicht
void __fastcall WCDisplayThread::ActualizeMainView()
{
		// Programm-Modus
		MainForm->ShowProgramMode();

		// aktuelle Teufe
		MainForm->ShowCurrDepth();

		// Magnetmarke
		MainForm->ShowMagneticMark(MainForm->VarControl.GetInfo_MagneticMarkStatic());

		// aktuelle Geschwindigkeit
		MainForm->ShowCurrVelocity();

		// aktueller Status der internen automatischen Windenbremse
		MainForm->ShowCurrWinchBrakeInternAutoStatus();

		//aktuelle Fahrtrichtung
		MainForm->ShowCurrDirection();

		// aktuelle Zugkraft
		MainForm->ShowCurrTraction();

		// Momentane Maximal-Zugkraft
		MainForm->ShowCurrTensionLimits();

		// Zugkraft-Limits
		MainForm->ShowMaxTractionLimits();

		// Antriebsinformationen
		MainForm->ShowEngineCoolantTemp();
        MainForm->ShowEngineSpeed();

		// Hydraulikzustand
		MainForm->ShowHydrWrkPressure();
		MainForm->ShowHydrChargePressure();
        MainForm->ShowHydrLvl();

		// Alarmsignalisierung aktualisieren
		MainForm->PlayAlarmSound();
};

// --------------------------------------------------------------------------
// Aktualisiert die Normalansicht
void __fastcall WCDisplayThread::ActualizeNormalView()
{
		// Mindest-Teufe
//        MainForm->ShowMinDepth();

		// Maximal-Teufe
//        MainForm->ShowMaxDepth();

		// Soll Geschwindigkeit
//        MainForm->ShowVelocity();

		// Momentane Maximal-Zugkraft
//        MainForm->ShowCurrMaxTractionValue();

		// Not-Aus-Signalisierung
		MainForm->ShowEmergencySwitchState();

		// Ölfilter austauschen
		MainForm->ShowOilFilterState();

		// Fahrzeugfehler-Anzeige
		MainForm->ShowAutomotiveError();

		// Nebenantriebsanzeige
		MainForm->ShowHydraulicOKState();

		// Dynamische Zugkraftberechnung momentan ein/aus
		MainForm->ShowCalcCurrTractionsSignalisation();

		// Magnetmarkenüberwachung
		MainForm->ShowMagneticMarkStatus();

		// Power Fail Überwachung
		MainForm->ShowPowerFailStatus();

		// Programmautomatik Ablaufprogrammpunkt
		MainForm->ShowAutoRunCurrProgramIndex();

		// Kalibrierung
		// Hinweise für Teufenkalibrierung
		MainForm->ShowCalibDepthInfo();

		// SONDERPOSTEN
		// Die Temperaturanzeige befindet sich im Menü "System", soll aber trotzdem ständig aktuell sein
		MainForm->ShowTemperatures(false);

};
// --------------------------------------------------------------------------
// Aktualisiert die Informationen, die nur im Entwicklermodus angezeigt werden
void __fastcall WCDisplayThread::ActualizeDevelInfos()
{
		if (MainForm->VarControl.devel() == _OFF) return;
		MainForm->ShowDevelInfo();

		Application->ProcessMessages();
};

// --------------------------------------------------------------------------
// Aktualisiert Sperren für Steuerelemente
void __fastcall WCDisplayThread::ActualizeLock()
{
		// ---------------------------------------------------------------------
		// Menü-Tasten
		// CalibrateOn
		if (MainForm->VarControl.CalibrateOn_Locked) MainForm->CalibrateButton->Enabled = false;
		else MainForm->CalibrateButton->Enabled = true;

		// UnloadCable
		if (MainForm->VarControl.UnloadCable_Locked) MainForm->UnloadCableButton->Enabled = false;
		else MainForm->UnloadCableButton->Enabled = true;

        // FileAdministration
        if (MainForm->VarControl.FileAdministration_Locked) MainForm->FileAdminButton->Enabled = false;
		else MainForm->FileAdminButton->Enabled = true;

        // ParameterMenu
        if (MainForm->VarControl.ParameterMenu_Locked) MainForm->SetParameterButton->Enabled = false;
        else MainForm->SetParameterButton->Enabled = true;

        // SystemMenu
        if (MainForm->VarControl.SystemMenu_Locked) MainForm->SystemButton->Enabled = false;
        else MainForm->SystemButton->Enabled = true;

        // ---------------------------------------------------------------------
        // Steuertasten
        // AutoRunOn
        if (MainForm->VarControl.AutoRunOn_Locked && MainForm->VarControl.UnloadCableActive_Locked)
                MainForm->StartStopButton->Enabled = false;
        else    MainForm->StartStopButton->Enabled = true;

        // ClearAfterThreshold
        if (MainForm->VarControl.Reset_Locked) MainForm->ResetButton->Enabled = false;
        else MainForm->ResetButton->Enabled = true;

        // EngineOn
		if (MainForm->VarControl.EngineStart_Locked && !MainForm->VarControl.GetControl_EngineStart()) MainForm->EngineButton->Enabled = false;
        else MainForm->EngineButton->Enabled = true;

        // EngineOff
        if (MainForm->VarControl.EngineStop_Locked) MainForm->EngineStopButton->Enabled = false;
        else MainForm->EngineStopButton->Enabled = true;

        // FastRun
        if (MainForm->VarControl.FastRun_Locked) MainForm->FastRunButton->Enabled = false;
        else MainForm->FastRunButton->Enabled = true;

        // LostMotionImpulse
        if (MainForm->VarControl.LostMotionImpulse_Locked) MainForm->LostMotionImpulseButton->Enabled = false;
        else MainForm->LostMotionImpulseButton->Enabled = true;

		// SetAutoVelocity...Buttons in SpeedView
		if (MainForm->VarControl.SetAutoVelocityXButton_Locked) MainForm->SpeedView->AutoSpeedModifyEnabled = false;
		else MainForm->SpeedView->AutoSpeedModifyEnabled = true;

		// WinchBrakeOff
		if (MainForm->VarControl.WinchBrakeOff_Locked) MainForm->WinchBrakeOffSpeedButton->Enabled = false;
		else MainForm->WinchBrakeOffSpeedButton->Enabled = true;

        // ---------------------------------------------------------------------
        // Parameter-Panel-Eingaben
#ifndef __BLOCK_WINCH_CHOISE
        // Winden-Nummer
        if (MainForm->VarControl.WinchNo_Locked) MainForm->PWinchCfgListBox->Enabled = false;
		else MainForm->PWinchCfgListBox->Enabled = true;
#endif

		// Mindest-Teufe
        if (MainForm->VarControl.MinDepth_Locked) MainForm->PMinDepthEdit->Enabled = false;
		else MainForm->PMinDepthEdit->Enabled = true;

        // Maximal-Teufe
		if (MainForm->VarControl.MaxDepth_Locked) MainForm->PMaxDepthEdit->Enabled = false;
        else MainForm->PMaxDepthEdit->Enabled = true;

        // Soll-Geschwindigkeit
		if (MainForm->VarControl.Velocity_Locked) MainForm->PVelocityEdit->Enabled = false;
        else MainForm->PVelocityEdit->Enabled = true;

        // Fahrtrichtung
		if (MainForm->VarControl.Direction_Locked) {
			MainForm->PChangeDirectionButton->Enabled = false;
			MainForm->SpeedView->AutoDirModifyEnabled = false;
		} else {
			MainForm->PChangeDirectionButton->Enabled = true;
			MainForm->SpeedView->AutoDirModifyEnabled = true;
		}


		// Windenbremse Software-Intern gesteuert
		if (MainForm->VarControl.WinchBrakeInternAutoReleaseHysteresisPerc_Locked) MainForm->PWinchBrakeInternalAutoCheckBox->Enabled = false;
		else MainForm->PWinchBrakeInternalAutoCheckBox->Enabled = true;

        // Motordrehzahländerung Ein/Aus
        if (MainForm->VarControl.EngineRevolutionChangeable_Locked) MainForm->PEngineRevolutionChangeableCheckBox->Enabled = false;
        else MainForm->PEngineRevolutionChangeableCheckBox->Enabled = true;

        // Motor-Start-Stop Funktion aktivierbar
        if (MainForm->VarControl.EngineStartStopEnable_Locked) MainForm->PEngineStartStopEnableCheckBox->Enabled = false;
        else MainForm->PEngineStartStopEnableCheckBox->Enabled = true;

		// Freilauffunktion Verfügbar
		if (MainForm->VarControl.LostMotionEnable_Locked) MainForm->PLostMotionEnableCheckBox->Enabled = false;
		else MainForm->PLostMotionEnableCheckBox->Enabled = true;

		// Bremse manuell abschaltbar verfügbar
		if (MainForm->VarControl.BrakeOffEnable_Locked) MainForm->PBrakeOffEnableCheckBox->Enabled = false;
		else MainForm->PBrakeOffEnableCheckBox->Enabled = true;

        // Maximal-Zugkraft
		if (MainForm->VarControl.MaxTraction_Locked) MainForm->PMaxTensionEdit->Enabled = false;
        else MainForm->PMaxTensionEdit->Enabled = true;

        // Differenz-Zeit

        // Dynamische Maximal-Zugkraft-Ermittlung
        if (MainForm->VarControl.CalcCurrTractions_Locked) MainForm->DynamicTensionOnOffButton->Enabled = false;
        else MainForm->DynamicTensionOnOffButton->Enabled = true;
        MainForm->DynamicTensionOnOffButton->Flat = !MainForm->DynamicTensionOnOffButton->Enabled;

        // Teufe im Teufenrechner setzen (StartDepth)
		if (MainForm->VarControl.SetDepth_DC_Locked ||
			MainForm->VarControl.StartDepth_Locked)
				MainForm->PSetDepthEdit->Enabled = false;
        else MainForm->PSetDepthEdit->Enabled = true;

		// Teufenumkehrung
		if (MainForm->VarControl.ReverseDepth_Locked) {
			MainForm->PReverseDepthCheckBox->Enabled = false;
		} else MainForm->PReverseDepthCheckBox->Enabled = true;

        // Magnetmarken-Überwachung ein/aus
        if (MainForm->VarControl.MagneticMarkOn_Locked) MainForm->MagneticMarkOnOffButton->Enabled = false;
        else MainForm->MagneticMarkOnOffButton->Enabled = true;
        MainForm->MagneticMarkOnOffButton->Flat = !MainForm->MagneticMarkOnOffButton->Enabled;

		// Magnetmarken-Zwangssynchronisation
		if (MainForm->VarControl.MMSyncForceRangeLim_Locked ||
			MainForm->VarControl.MMSyncForceOn_Locked)
			MainForm->PMMSyncForceRangeLimEdit->Enabled = false;
		else MainForm->PMMSyncForceRangeLimEdit->Enabled = true;

        // Format der cm-Impulse
        if (MainForm->VarControl.FormatCMImpulse_Locked) MainForm->Ext_cm_Imp_GroupBox->Enabled = false;
        else MainForm->Ext_cm_Imp_GroupBox->Enabled = true;

        // Eichwerte direkt eingeben
        // Teufe
		// Eichfaktor im Teufenrechner setzen
		if (MainForm->VarControl.CalFactDepth_Locked ||
			MainForm->VarControl.SetDepthCalFactor_DC_Locked)
			MainForm->PDepthCalFactorEdit->Enabled = false;
		else MainForm->PDepthCalFactorEdit->Enabled = true;

        // Sondertastenmodus auswählen
        if  (MainForm->VarControl.SpecialSignalMode_Locked)
        {
                MainForm->PSpecialSignalMode0RadioButton->Enabled = false;
                MainForm->PSpecialSignalMode1RadioButton->Enabled = false;
                MainForm->PSpecialSignalMode2RadioButton->Enabled = false;
				MainForm->PSpecialSignalMode3RadioButton->Enabled = false;
				MainForm->PSpecialSignalMode4RadioButton->Enabled = false;
				MainForm->PSpecialSignalMode5RadioButton->Enabled = false;
		}else
		{
				MainForm->PSpecialSignalMode0RadioButton->Enabled = true;
				MainForm->PSpecialSignalMode1RadioButton->Enabled = true;
				MainForm->PSpecialSignalMode2RadioButton->Enabled = true;
				MainForm->PSpecialSignalMode3RadioButton->Enabled = true;
				MainForm->PSpecialSignalMode4RadioButton->Enabled = true;
				MainForm->PSpecialSignalMode5RadioButton->Enabled = true;
		};

		// Kabel-/Seilöler Dauerbetrieb
		if (MainForm->VarControl.LineLubPermanentOperation_Locked)
			MainForm->PLineLubPermanentOperationCheckBox->Enabled = false;
		else
			MainForm->PLineLubPermanentOperationCheckBox->Enabled = true;

		// Kabel-/Seilöler Intensität
		if (MainForm->VarControl.LineLubIntensity_Locked)
			MainForm->PLineLubIntensitySlider->Enabled = false;
		else
			MainForm->PLineLubIntensitySlider->Enabled = true;

		// Steuergerätefehler
		if (MainForm->VarControl.ControllerErrorMonitAtSM_Locked) {
			MainForm->PControllerErrorAtSMCheckBox->Enabled = false;
		} else {
			MainForm->PControllerErrorAtSMCheckBox->Enabled = true;
		}

        // Zugkraft
		if (MainForm->VarControl.CalibTractionX_Locked ||
			MainForm->VarControl.CalibTractionY_Locked ||
			MainForm->VarControl.CalibTractionZero_Locked)
        {
				MainForm->PCalibTensionXEdit->Enabled = false;
				MainForm->PCalibTensionYEdit->Enabled = false;
				MainForm->PCalibTensionZeroEdit->Enabled = false;
		}else
		{
				MainForm->PCalibTensionXEdit->Enabled = true;
				MainForm->PCalibTensionYEdit->Enabled = true;
				MainForm->PCalibTensionZeroEdit->Enabled = true;
        };

        // Kabel entladen
        // Soll-Zugkraft
        if (MainForm->VarControl.UnloadCableTraction_Locked ||
            MainForm->UnloadCabelStretchCheckBox->Checked)
				MainForm->UnloadCableSetTensionEdit->Enabled = false;
        else    MainForm->UnloadCableSetTensionEdit->Enabled = true;


        // Stretching ein/aus
        if (MainForm->VarControl.UnloadCableStrech_Locked)
                MainForm->UnloadCabelStretchCheckBox->Enabled = false;
        else    MainForm->UnloadCabelStretchCheckBox->Enabled = true;

        // Anfangslänge für Stretching
        if (MainForm->VarControl.UnloadCableBeginLenght_Locked)
				MainForm->UnloadCableStretchStartLenghtEdit->Enabled = false;
		else    MainForm->UnloadCableStretchStartLenghtEdit->Enabled = true;

		// Endteufe für Stretching
		if (MainForm->VarControl.UnloadCableEndLenght_Locked)
				MainForm->UnloadCableStretchEndLenghtEdit->Enabled = false;
		else    MainForm->UnloadCableStretchEndLenghtEdit->Enabled = true;

        // Anfangszugkraft für Stretching
        if (MainForm->VarControl.UnloadCableStartTraction_Locked)
				MainForm->UnloadCableStretchStartTensionEdit->Enabled = false;
        else    MainForm->UnloadCableStretchStartTensionEdit->Enabled = true;

        // Endzugkraft für Stretching
        if (MainForm->VarControl.UnloadCableEndTraction_Locked)
				MainForm->UnloadCableStretchEndTensionEdit->Enabled = false;
        else    MainForm->UnloadCableStretchEndTensionEdit->Enabled = true;

        // Programmautomatik Ein/Aus
        if (MainForm->VarControl.AutoRunProgramableOn_Locked)
                MainForm->PProgramAutomCheckBox->Enabled = false;
        else    MainForm->PProgramAutomCheckBox->Enabled = true;

        // Programmautomatik Programmauswahl
        if (MainForm->VarControl.AutoRunProgramFileName_Locked)
                MainForm->PProgramAutomFileListBox->Enabled = false;
        else    MainForm->PProgramAutomFileListBox->Enabled = true;

        // Programmautomatik Ablaufprogrammpunktauswahls
        if (MainForm->VarControl.SetAutoRunProgramableIndex_Locked)
                MainForm->AutoRunProgramItemListListBox->Enabled = false;
        else    MainForm->AutoRunProgramItemListListBox->Enabled = true;

        // LOG-Datei Typauswahl
        if (MainForm->VarControl.LOGFileType_Locked)
                MainForm->LOGFileTypeGroupBox->Enabled = false;
        else MainForm->LOGFileTypeGroupBox->Enabled = true;

        // LOG-Datei Namenseingabe
		if (MainForm->LOGFileTypeLAS2RadioButton->Checked) {
			MainForm->LOGFileNameLabel->Enabled = false;
			MainForm->LOGFileNameMaskEdit->Enabled = false;
		} else {
			MainForm->LOGFileNameLabel->Enabled = true;
			MainForm->LOGFileNameMaskEdit->Enabled = true;
		}

		// LOG-Datei Kommentar-Eingabe
		if (MainForm->LOGFileTypeLAS2RadioButton->Checked) {
			MainForm->LOGFileCommentsGroupBox->Enabled = false;
			MainForm->LOGFileCommentsMemo->Enabled = false;
		} else {
			MainForm->LOGFileCommentsGroupBox->Enabled = true;
			MainForm->LOGFileCommentsMemo->Enabled = true;
		}

		// LOG-Datei Trupp-Eingabe
		if (MainForm->LOGFileTypeLAS2RadioButton->Checked) {
			MainForm->LOGFileTeamLabel->Enabled = false;
			MainForm->LOGFileTeamMaskEdit->Enabled = false;
		} else {
			MainForm->LOGFileTeamLabel->Enabled = true;
			MainForm->LOGFileTeamMaskEdit->Enabled = true;
		}

		// LOG-Datei Windennr.-Eingabe
		if (MainForm->LOGFileTypeLAS2RadioButton->Checked) {
			MainForm->LOGFileWinchMachineryLabel->Enabled = false;
			MainForm->LOGFileWinchMachineryMaskEdit->Enabled = false;
		} else {
			MainForm->LOGFileWinchMachineryLabel->Enabled = true;
			MainForm->LOGFileWinchMachineryMaskEdit->Enabled = true;
		}


		// ----------------------------------------------------------
        // Kalibrieren
		// Teufe

        // Eingabe Kabellänge
		if (MainForm->VarControl.CalFactDepth_Earmarked_Locked) MainForm->CalibDepth_DepthEdit->Enabled = false;
		else MainForm->CalibDepth_DepthEdit->Enabled = true;

		// Übernehmen
        if (MainForm->VarControl.CalibDepthSetParm_Locked) MainForm->CalibDepthAcceptButton->Enabled = false;
        else MainForm->CalibDepthAcceptButton->Enabled = true;

        // Zugkraft
        // Messung 1
		if (MainForm->VarControl.CalibTension_GetVal1_Locked) MainForm->CalibTractionMessure1Button->Enabled = false;
        else MainForm->CalibTractionMessure1Button->Enabled = true;

        // Zugkraft bei Messung 1
		if (MainForm->VarControl.CalibTension_GetVal1_Locked) MainForm->CalibTension_Tension1Edit->Enabled = false;
        else MainForm->CalibTension_Tension1Edit->Enabled = true;

        // Messung 2
		if (MainForm->VarControl.CalibTension_GetVal2_Locked) MainForm->CalibTractionMessure2Button->Enabled = false;
        else MainForm->CalibTractionMessure2Button->Enabled = true;

        // Zugkraft bei Messung 2
		if (MainForm->VarControl.CalibTension_GetVal2_Locked) MainForm->CalibTension_Tension2Edit->Enabled = false;
        else MainForm->CalibTension_Tension2Edit->Enabled = true;

        // Werte speichern
        if (MainForm->VarControl.CalibTractionSetParam_Locked) MainForm->CalibTractionCompleteSetButton->Enabled = false;
        else MainForm->CalibTractionCompleteSetButton->Enabled = true;

		// ----------------------------------------------------------
		// FunktionsPanel-Standard-Tasten (Ok, Cancel, usw...)
/*        if (MainForm->VarControl.ParameterOKButton_Locked)
		{
		}else
		{
		};
*/
        if (MainForm->VarControl.CancelButton_Locked)
        {
                MainForm->CalibTractionAbortButton->Enabled = false;
                MainForm->ChoiceAbortButton->Enabled = false;

// Sonderreglung  MainForm->UnloadCableChancelButton->Enabled = false;
                MainForm->LOGFileCancelButton->Enabled = false;
                MainForm->SuperVisionTabSheetExitButton->Enabled = false;
                MainForm->SystemCancelButton->Enabled = false;
        } else
        {
                MainForm->CalibTractionAbortButton->Enabled = true;
                MainForm->ChoiceAbortButton->Enabled = true;
// Sonderreglung  MainForm->UnloadCableChancelButton->Enabled = true;
                MainForm->LOGFileCancelButton->Enabled = true;
                MainForm->SuperVisionTabSheetExitButton->Enabled = true;
                MainForm->SystemCancelButton->Enabled = true;
        };

        // Sonderreglungen
        // Kabel entladen: Abbrechen
        if (MainForm->VarControl.CancelButton_Locked ||
            MainForm->VarControl.UnloadCable_Locked)
        {
                MainForm->UnloadCableExitButton->Enabled = false;
        } else
        {
                MainForm->UnloadCableExitButton->Enabled = true;
        };

        // ---------------------------------------------------------------------
        // SystemPanel-Elemente

        // System herunterfahren
        if (MainForm->VarControl.SystemShutdownButton_Locked)
        {
                MainForm->SystemShutdownButton->Enabled = false;
        }else
        {
                MainForm->SystemShutdownButton->Enabled = true;
        };

        // System neu starten
		if (MainForm->VarControl.SystemRestartButton_Locked)
        {
                MainForm->SystemRestartButton->Enabled = false;
        }else
        {
                MainForm->SystemRestartButton->Enabled = true;
        };

        // Programm beenden
        if (MainForm->VarControl.TerminationButton_Locked)
        {
                MainForm->SystemProgramTerminationButton->Enabled = false;
        }else
        {
                MainForm->SystemProgramTerminationButton->Enabled = true;
        };

        // Datum, Uhrezeit setzen
        if (MainForm->VarControl.SystemDateTimeAcceptButton_Locked)
        {
                MainForm->SystemDateTimeAcceptButton->Enabled = false;
        }else
        {
                MainForm->SystemDateTimeAcceptButton->Enabled = true;
        };

        // ---------------------------------------------------------------------
        // Message-Panel-Elemente
// ToDo: Nach bewähren der neuen Methode "AcousticSignalOn"-Flag vollständig entfernen
//        if (MainForm->GenOrgan.AcousticSignalOn) {
		if ((MainForm->GenOrgan.Evnt.UnconfirmedEvents(EVNTOBJTYPE_ERROR) > 0) ||
			(MainForm->GenOrgan.Evnt.UnconfirmedEvents(EVNTOBJTYPE_WARNING) > 0))
				MainForm->AlarmSoundOnOffButton->Enabled = true;
		else MainForm->AlarmSoundOnOffButton->Enabled = false;

};
// ==========================================================================
// ZUSTAENDE TASTEN
// ==========================================================================
// Aktualisiert das Aussehen von Tasten (für die Bedienung)
void __fastcall WCDisplayThread::ActualizeButtonView()
{
        MainForm->SetClearAfterThreshold();
        MainForm->SetFastRun();
        MainForm->SetLostMotionImpulse();
        MainForm->SetStartStop();
        MainForm->SetWinchBrakeOffButton();

        MainForm->ShowErrorState();
};

// ==========================================================================
// LOG-FILE
// ==========================================================================
// Prüfen des noch verfügbaren Speicherplatzes
void __fastcall WCDisplayThread::ActGetDiskFree()
{
        unsigned char Device;
        clock_t Countdown; // nur Dummy

        // Taktung und Überlauftest
        // Bei Überlauf wird sofort der Speicherplatz ausgelesen, was sich nur darin auswirkt,
        // dass die Überprüfungszeit für einen Durchgang verringert wird
        if (!IsTimeElapsed(&LOG_GetDiskFree_LastTime, &Countdown, LOG_DiskSpace_Test_Clock, 0, false)) return; // Initialisierung von LOG_GetDiskFree_LastTime erfolgte im Konstruktor

        // Feststellen des Platzes auf dem Datenträger für offizielle LOG-Datei
        if (MainForm->GenOrgan.LOGPath != 0)
        {
                // LW-Buchstaben in den für DiskFree gültigen Wert wandeln (A = 1, B = 2, C = 3)
                if (MainForm->GenOrgan.LOGPath[0] < 'a') Device = MainForm->GenOrgan.LOGPath[0] - 'A' + 1;          // LW-Buchstaben groß geschr.
                else Device = MainForm->GenOrgan.LOGPath[0] - 'a' + 1;                            // LW-Buchstaben klein geschr.
                LOG_GetDiskFree_FreeSpace = DiskFree(Device);
                if (MainForm->GenOrgan.LOGDiskFree == true)                     // bei Bedarf Abschalten
                {
					if ((LOG_GetDiskFree_FreeSpace == -1) || (LOG_GetDiskFree_FreeSpace < LOG_DiskSpace_sleep))
                    {
						MainForm->GenOrgan.LOGDiskFree = false;
						MainForm->GenOrgan.Evnt << 20;                 			// Fehler 20 ein
					};
				} else                                                          // wenn möglich wieder zuschalten
				{
					if (LOG_GetDiskFree_FreeSpace >= LOG_DiskSpace_wakeup)
					{
//                        MainForm->GenOrgan.LOGDiskFree = true;
						MainForm->GenOrgan.Evnt >> 20; 							// Fehler 20 aus
                    };
                };
        };

        // Feststellen des Platzes auf dem Datenträger für interne LOG-Datei
        if (MainForm->GenOrgan.LOGPathinternal != 0)
        {
                // LW-Buchstaben in den für DiskFree gültigen Wert wandeln (A = 1, B = 2, C = 3)
                if (MainForm->GenOrgan.LOGPath[0] < 'a') Device = MainForm->GenOrgan.LOGPath[0] - 'A' + 1;          // LW-Buchstaben groß geschr.
                else Device = MainForm->GenOrgan.LOGPath[0] - 'a' + 1;                            // LW-Buchstaben klein geschr.
                LOGinternal_GetDiskFree_FreeSpace = DiskFree(Device);
                if (MainForm->GenOrgan.LOGInternalDiskFree == true)             // bei Bedarf Abschalten
                {
                    if ((LOGinternal_GetDiskFree_FreeSpace == -1) || (LOGinternal_GetDiskFree_FreeSpace < LOGinternal_DiskSpace_sleep))
                    {
						MainForm->GenOrgan.LOGInternalDiskFree = false;
						MainForm->GenOrgan.Evnt << 21;							// Fehler 21 ein
					};
				} else                                                          // wenn möglich wieder zuschalten
				{
					if (LOGinternal_GetDiskFree_FreeSpace >= LOGinternal_DiskSpace_wakeup)
					{
						MainForm->GenOrgan.LOGInternalDiskFree = true;
						MainForm->GenOrgan.Evnt >> 21;							// Fehler 21 aus
                    };
                };
        };
};

// --------------------------------------------------------------------------
// Aktion betreffend der LOG-Datei
void __fastcall WCDisplayThread::ActLOG()
{
        if (LOGH == 0) return;
        time_t CurrTime;
        time(&CurrTime);

        bool Hbool = MainForm->VarControl.GetInfo_MagneticMarkStaticLOG();

        // Protokollierung unterbrechen
        if (MainForm->GenOrgan.Evnt[20])
        {
                MainForm->GenOrgan.NewLOG = true;                               // Neue Protokolldatei nötig (darauf wird reagiert, sobald wieder ausreichend Speicherplatz zur Verfügung steht)
                LOGH->StopLOG();

                // Bei Fehler Magnetmarken-Status zurücksetzen
                // sofern diese detektiert wurde
                if (Hbool) MainForm->VarControl.SetInfo_MagneticMarkStaticLOGOff();

                return;
        };

        if (MainForm->GenOrgan.NewLOG)
        {
                LOGH->StartLOG();
                MainForm->GenOrgan.NewLOG = false;
                LOG_OldTime = CurrTime - 1;                                     // festlegen des ersten Zeitpunktes
        };

		if (((MainForm->VarControl.GetParam_LOGFileType() == LOGFILE_LAS2) ||
             (MainForm->VarControl.GetParam_LOGFileType() == LOGFILE_GEOSYSV2) ||
             (LOG_OldDepth != (MainForm->VarControl.GetInfo_CurrDepth() / LOG_DEPTH_INTERVAL))) &&
            ((CurrTime - LOG_OldTime) >= LOG_TIME_INTERVAL))
        {
				if (!LOGH->WriteDataSet()) MainForm->GenOrgan.Evnt << 13;		// Fehler 13 ein
				else MainForm->GenOrgan.Evnt >> 13;                          	// Fehler 13 aus
		}else return;

        // Nach der Abarbeitung kann das Magnetmarkenanzeige-Flag wieder zurückgesetzt werden
        // sofern diese detektiert wurde
        if (Hbool) MainForm->VarControl.SetInfo_MagneticMarkStaticLOGOff();

        LOG_OldDepth = MainForm->VarControl.GetInfo_CurrDepth() / LOG_DEPTH_INTERVAL; // wird derzeit nur für GEOSYS-Standard-Format verwendet.

        // LOG-Format-abhängiges Merken des letzten Zeitpunktes für Datensatz
        if (MainForm->VarControl.GetParam_LOGFileType() == LOGFILE_LAS2)        // Wenn "LAS 2.0"-Format
        {
                LOG_OldTime += LOG_TIME_INTERVAL;
        } else                                                                  // ansonsten (GEOSYS-Standard-Format)
        {
                LOG_OldTime = CurrTime;                                         // würde es hier wie LAS 2.0 verwaltet, würden alle verfallenen Sekunden nachgeholt -> keine Speicherplatz-Ersparnis
        };
};

// --------------------------------------------------------------------------
void __fastcall WCDisplayThread::ActLOGClose()
{
        LOGH->StopLOG();                                                        // Protokollieren sauber einstellen
        if (LOGH != 0) delete LOGH;
};

// --------------------------------------------------------------------------
void __fastcall WCDisplayThread::ActLOGinternal()
{

        // ---------------------------------------------------------------------
        // Dieser Eintrag MUSS als erstes abgearbeitet werden!!!
        // ist zu wenig Speicherplatz für die interne Protokollierung vorhanden, wird nur noch dieser Eintrag protokolliert
		if (MainForm->GenOrgan.Evnt[21])
		{
                LOGinternal.WriteLOGinternalSleepOn(LOGinternal_GetDiskFree_FreeSpace);
                return;
		} else LOGinternal.WriteLOGinternalSleepOff(LOGinternal_GetDiskFree_FreeSpace);

        // ---------------------------------------------------------------------
        // Wenn die absolut zulässige Maximal-Zugkraft überschritten wird, dann "Loggen"
        if (MainForm->GenOrgan.internal_LOG_AbsoluteMaxTraction > ABSOLUTE_MAX_TRACTION)
                LOGinternal.WriteMaxThresholdInFile(MainForm->GenOrgan.internal_LOG_AbsoluteMaxTraction);
        // ---------------------------------------------------------------------
        // Dieser Wert wurde geschrieben -> Zurücksetzen der Zugkraft
        MainForm->GenOrgan.internal_LOG_AbsoluteMaxTraction = 0;

        // ---------------------------------------------------------------------
        // Öl-Filter
        if (MainForm->VarControl.GetInfo_OilStrainer() == _ON) LOGinternal.WriteOilStrainerLampON();
        else LOGinternal.WriteOilStrainerLampOFF();

        // ---------------------------------------------------------------------
        // Fahrzeug-Fehler
        if (MainForm->VarControl.GetInfo_AutomotiveError() == _ON) LOGinternal.WriteAutomotiveErrorON();
        else LOGinternal.WriteAutomotiveErrorOFF();

        // ---------------------------------------------------------------------
        // Temperaturüberschreitung auf Teufenrechner
        if (MainForm->VarControl.GetInfo_TemperatureSensorDepthCalc() >= TEMPERATURE_SENSOR_DEPTHCALC_CRITICAL) LOGinternal.WriteHighTemperature_DepthCalcOn(MainForm->VarControl.GetInfo_TemperatureSensorDepthCalc());
        else LOGinternal.WriteHighTemperature_DepthCalcOff();

        // ---------------------------------------------------------------------
        // Protokollierung aufgrund von Mangel an Festplattenspeicher eingestellt
        if (MainForm->GenOrgan.Evnt[20]) LOGinternal.WriteLOGSleepOn(LOG_GetDiskFree_FreeSpace);
        else LOGinternal.WriteLOGSleepOff(LOG_GetDiskFree_FreeSpace);

        // ---------------------------------------------------------------------
        // Kommunikationsfehler zwischen STR und TR
        if (MainForm->GenOrgan.Evnt[2]) LOGinternal.WriteDepthCalcCommErrorON();
        else LOGinternal.WriteDepthCalcCommErrorOFF();

        // ---------------------------------------------------------------------
        // Steuergerät Fehlerüberwachung abgeschaltet ?
        // ACHTUNG HIER IST DAS EREIGNIS "ON", wenn MONITORING "OFF" und umgekehrt!!!
        if (MainForm->VarControl.GetParam_ControllerErrorMonit() == _OFF) LOGinternal.WriteControllerErrorMonitON();
		else LOGinternal.WriteControllerErrorMonitOFF();

        // ---------------------------------------------------------------------
        // IGR Fehlerüberwachung abgeschaltet ?
        // ACHTUNG HIER IST DAS EREIGNIS "ON", wenn MONITORING "OFF" und umgekehrt!!!
        if (MainForm->VarControl.GetParam_IncEncErrorMonit() == _OFF) LOGinternal.WriteIncEncErrorMonitON();
        else LOGinternal.WriteIncEncErrorMonitOFF();

        // ---------------------------------------------------------------------
        // Schaltstellung Schaltgetriebe nicht korrekt ?
        if (MainForm->GenOrgan.Evnt[22]) LOGinternal.WriteGearShiftErrorON();
        else LOGinternal.WriteGearShiftErrorOFF();

        // ---------------------------------------------------------------------
        // Freilauf EIN/AUS
        if ((MainForm->VarControl.GetInfo_LostMotion() == _ON) && (MainForm->VarControl.GetControl_WinchBrakeOff() == _OFF)) LOGinternal.WriteLostMotionON();
        else LOGinternal.WriteLostMotionOFF();

        // ---------------------------------------------------------------------
        // Kabel entladen aktiv/inaktiv
        if (MainForm->VarControl.GetControl_UnloadCableActive() == _ON) LOGinternal.WriteUnloadCableON();
        else LOGinternal.WriteUnloadCableOFF();

};

// ==========================================================================
// ENTWICKLER_INFO
// ==========================================================================
// Ermittelt die Anzahl der Schleifendurchläufe pro Sekunde
void __fastcall WCDisplayThread::TestLoops()
{
        if (MainForm->VarControl.devel() == _OFF) return;

        long CurrTime, TimeDiff;

        CurrTime = GetProcessTime_in_ms();

        ++DevelInfo_LoopsSinceLastTest;

        TimeDiff = CurrTime - DevelInfo_LastLoopPerSecoundTestTime;
        if (TimeDiff < 10000) return;

        MainForm->GenOrgan.DisplayThreadLoopsPerSecound = (float)(DevelInfo_LoopsSinceLastTest) * (float)(CLOCKS_PER_SEC) / (float)(TimeDiff);

        DevelInfo_LoopsSinceLastTest = 0;
        DevelInfo_LastLoopPerSecoundTestTime = CurrTime;
};

