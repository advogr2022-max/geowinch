/*
WinchControl    Jörg Müller    VarControl.cpp/VarControl.h

Klassenimplementation für die Organisation der Kontroll-
und Informationsvariablen

Portierbarkeit: vollständig, wenn Präprozessordirektiven angepasst
*/

// Anzupassende Zeilen sind mit "SYS" gekennzeichnet

//---------------------------------------------------------------------------
// SYS
#include <vcl.h>

//SYS
#pragma hdrstop

#include "VarControl.h"
//---------------------------------------------------------------------------
// SYS
#pragma package(smart_init)

//===========================================================================

#include "UnicodeCoder.h"

// ACHTUNG JEDE ÄNDERUNG EINES EINTRAGS MUSS MIT VERSCHIEDENEN
// METHODENIMPLEMENTIERUNGEN ABGEGLICHEN WERDEN!
// JEDE ZU BEACHTENDE METHODENIMPLEMENTIERUNG IST MIT FOLGENDEM KOMMENTAR
// MARKIERT:

// DEPENDENT ON PARAMETER

// Klassen-Verwaltung

// DEPENDENT ON PARAMETER
// Konstruktor
WCVarControl::WCVarControl()
{
        // Vornehmen der Grundeinstellungen
        DataFileName = 0;
        InternalDataFileName = 0;
        TempDataFileName = 0;
        BackupDataFileName  = 0;
		SetStandards();
        InitOK = _YES;                                                          // Initialisierung erfolgreich, Klasseninstanz ist einsatzbereit

        // Keine Blockierungen
        AutoRunOn_Locked = false;
        WinchWork_Locked = false;
        Reset_Locked = false;
        FastRun_Locked = false;
        EngineStart_Locked = false;
        EngineStop_Locked = false;
		UnloadCable_Locked = false;
		UnloadCableActive_Locked = false;
        UnloadCablePassive_Locked = false;
        CalibrateOn_Locked = false;
        CalibrateDepth_Locked = false;
        CalibrateTraction_Locked = false;
        GetDepthCalcSWVersion_Locked = false;
		LostMotionImpulse_Locked = false;
		SetDepth_DC_Locked = false;
		ReverseDepth_Locked = false;
        SetDepthCalcParam_Locked = false;
		SetDepthCalFactor_DC_Locked = false;
		WinchBrakeOff_Locked = false;

		CalibTension_GetVal1_Locked = false;
		CalibTension_GetVal2_Locked = false;
		CalibTractionCalculateParam_Locked = false;
        CalibTractionSetParam_Locked = false;
        CalibTractionSetZero_Locked = false;
		CalibDepthSetParm_Locked = false;

		CalibTSensorDepthCalc_CaractSlope_Locked = false;
		CalibTSensorDepthCalc_CaractZero_Locked = false;
		CalibTSensorA_CaractSlope_Locked = false;
		CalibTSensorA_CaractZero_Locked = false;
		CalibTSensorB_CaractSlope_Locked = false;
		CalibTSensorB_CaractZero_Locked = false;

		SavableParamChanged = false;

		WinchNo_Locked = false;
//        WinchCnt_Locked = false;
		MinDepth_Locked = false;
		MaxDepth_Locked = false;
		StartDepth_Locked = false;
		Velocity_Locked = false;
		Direction_Locked = false;
		EngineRevolutionChangeable_Locked = false;
		EngineStartStopEnable_Locked = false;
		LostMotionEnable_Locked = false;
		BrakeOffEnable_Locked = false;
		FormatCMImpulse_Locked = false;
		MaxTraction_Locked = false;
		MinTraction_Locked = false;
		MMSyncForceOn_Locked = false;
		MMSyncForceRangeLim_Locked = false;
		MagneticMarkOn_Locked = false;
		UnloadCableTraction_Locked = false;
		UnloadCableMinTraction_Locked = false;
		UnloadCableStrech_Locked = false;
		UnloadCableBeginLenght_Locked = false;
		UnloadCableEndLenght_Locked = false;
		UnloadCableStartTraction_Locked = false;
		UnloadCableEndTraction_Locked = false;
		CalcCurrTractions_Locked = false;
		TractionViewDimensionUnit_Locked = false;
        CalFactDepth_Locked = false;
        CalibTractionX_Locked = false;
        CalibTractionY_Locked = false;
        CalibTractionZero_Locked = false;

        CalibTractionValue1_Locked = false;
		CalibTractionValue2_Locked = false;
		CalibTension_MeasuringVal_Locked = false;
        CalibTension_Tension_Locked = false;

    	ControllerErrorMonitAtSM_Locked = false;
		ControllerErrorLactive_Locked = false;
        AutoRunProgramableOn_Locked = false;
        AutoRunProgramFileName_Locked = false;
    	VelocityDimensionUnit_Locked = false;
		WinchBrakeInternAuto_Locked = false;
    	WinchBrakeInternAutoReleasePerc_Locked = false;
		WinchBrakeInternAutoReleaseHysteresisPerc_Locked = false;

    	LOGFileTensionUnit_Locked = false;
		LOGFileType_Locked = false;
		SpecialSignalMode_Locked = false;

		LineLubPermanentOperation_Locked = false;
        LineLubIntensity_Locked = false;

//        ParameterOKButton_Locked = false;
        CancelButton_Locked = false;
        FileAdministration_Locked = false;
        ParameterMenu_Locked = false;
        SystemMenu_Locked = false;
        SystemShutdownButton_Locked = false;
        SystemRestartButton_Locked = false;
        TerminationButton_Locked = false;
        SystemDateTimeAcceptButton_Locked = false;
		SetAutoVelocityXButton_Locked = false;
        SetAutoRunProgramableIndex_Locked = false;

        VelocityLock_Active = false;

};

//---------------------------------------------------------------------------

// Destruktor
WCVarControl::~WCVarControl()
{
        CleanUp();
};

//---------------------------------------------------------------------------

// Initialisieren der Klasse und setzen von Standardwerten
bool WCVarControl::Init(char *filename, char *tmpfilename, char *backupfile, char *internalfilename)
{
        SetStandards();

        InitOK = _YES;                                                          // Initialisierung erfolgreich, Klasseninstanz ist einsatzbereit

        // Dateinamen übernehmen
        // -----------------------
        // Normale Parameterdatei
        if (DataFileName != 0) delete DataFileName;                             // Speicherfreigabe
        DataFileName = 0;

        // Speicher für Namen reservieren
        try
        {
                DataFileName = new char[strlen(filename) + 1];
                strcpy(DataFileName, filename);
        }catch(...)
        {
                DataFileName = 0;
        };

        // temporären Dateinamen übernehmen
        // -----------------------
        if (TempDataFileName != 0) delete TempDataFileName;                     // Speicherfreigabe
        TempDataFileName = 0;

        // Speicher für Namen reservieren
        try
        {
                TempDataFileName = new char[strlen(tmpfilename) + 1];
                strcpy(TempDataFileName, tmpfilename);
        }catch(...)
        {
                TempDataFileName = 0;
        };

        // Backup Datendatei
        // -----------------------
        if (BackupDataFileName != 0) delete BackupDataFileName;                 // Speicherfreigabe
        BackupDataFileName = 0;

        // Speicher für Namen reservieren
        try
        {
                BackupDataFileName = new char[strlen(backupfile) + 1];
                strcpy(BackupDataFileName, backupfile);
        }catch(...)
        {
                BackupDataFileName = 0;
        };

        // -----------------------
        // Spezielle Parameterdatei
        if (InternalDataFileName != 0) delete InternalDataFileName;
        InternalDataFileName = 0;

        // Speicher für Namen reservieren
        try
        {
                InternalDataFileName = new char[strlen(internalfilename) + 1];
                strcpy(InternalDataFileName, internalfilename);
        }catch(...)
        {
                InternalDataFileName = 0;
        };

        // -----------------------
        // Auslesen der Parameterdatei
        if (new_ReadData() == _BAD)                                             // wenn Fehler beim Einlesen der Daten
        {
//                SetStandards();
                return _BAD;
        };

        return _GOOD;
};
//---------------------------------------------------------------------------

// Aufräumen
void WCVarControl::CleanUp()
{
        if (DataFileName != 0)
        try
        {
                delete DataFileName;
                DataFileName = 0;
        }catch(...)
        {
        };

        if (TempDataFileName != 0)
        try
        {
                delete TempDataFileName;
                TempDataFileName = 0;
        }catch(...)
        {
        };

        if (BackupDataFileName != 0)
        try
        {
                delete BackupDataFileName;
                BackupDataFileName = 0;
        }catch(...)
        {
        };

        if (InternalDataFileName != 0)
        try
        {
                delete InternalDataFileName;
                InternalDataFileName = 0;
        }catch(...)
        {
        };

        CloseFile();
};
//===========================================================================
// Status-Abfragen

// Gibt den Status der Initialisierung zurück
bool WCVarControl::Is_Initialized()
{
        return InitOK;
};
//---------------------------------------------------------------------------

// DEPENDENT ON PARAMETER
// Standardwerte setzen
void WCVarControl::SetStandards()
{
        //Programmsteuerung
        ControlVar.AutoRunOn = STD_AutoRunOn;                                   // Automatische Teufenfahrt (true) oder manuelle Teufenfahrt (false)
        ControlVar.WinchWork = STD_WinchWork;                                   // Winde ein (true), angehalten (false)
        ControlVar.CalcCurrTractions = STD_CalcCurrTractions;                   // momentane Zugkraft in Abhängigkeit der Teufe berechnen
        ControlVar.CalcCurrTractions = STD_CalcCurrTractionsActive;             // momentane Zugkraft in Abhängigkeit der Teufe berechnen tatsächlich aktiv
        ControlVar.ThresholdExceedenceStopDelayOn = STD_ThresholdExceedenceStopDelayOn; // Verzögerung des Ausschaltens vom Steuergerät bei Zugkraft-/ Teufenüberschreitung (gleichzeitig Haltedauer der Mittenspannung durch den STR)
        ControlVar.ClearAfterThresholdDepth = STD_ClearAfterThresholdDepth;     // Freigabe nach Grenzwertüberschreitung aktiv (true) oder nicht (false)
        ControlVar.ClearAfterThresholdTraction = STD_ClearAfterThresholdTraction;// Freigabe nach Grenzwertüberschreitung aktiv (true) oder nicht (false)
        ControlVar.ResetSpecialSignalError1Flag = STD_ResetSpecialSignalErrorFlags; // Bestätigen von ausgewählten durch Sondersignale ausgelösten Fehlern
        ControlVar.ResetSpecialSignalError2Flag = STD_ResetSpecialSignalErrorFlags; // Bestätigen von ausgewählten durch Sondersignale ausgelösten Fehlern
        ControlVar.FastRun = STD_FastRun;                                       // Schnelle (true) oder langsame (false) Fahrt
        ControlVar.EngineRevolutionUpDown = STD_EngineRevolutionUpDown;         // Motordrehzahl erhöhen (+1), verringern (-1), belassen (0)
        ControlVar.EngineRevolutionAutoUpDown = STD_EngineRevolutionAutoUpDown; // Motordrehzahl erhöhen (+1), verringern (-1), belassen (0); Variable für programminterne Automatik
		ControlVar.Hello_WinchClear = STD_Hello_WinchClear;						// Hallo über das Signal WinchClear zur externen Steuerelektronik (forciertes kurzes Auslösen des Signals)
		ControlVar.MagneticMarkOn = STD_MagneticMarkOn;                         // Magnetmarken ein (true) oder aus (false)
		ControlVar.MMSyncForceActive = STD_MMSyncForceActive;					// MM-Zwangssynchronisation aktiv (true) oder inaktiv (false)
		ControlVar.MMSyncForceCounter = STD_MMSyncForceCounter;                 // MM Teufenzähler für MM-Zwangssynchronisation
		ControlVar.devel = STD_devel;                                           // Entwicklermodus ein (true), aus (false)
		ControlVar.UnloadCable = STD_UnloadCable;                         		// Kabel entladen ein (true) oder aus (false)
		ControlVar.UnloadCableActive = STD_UnloadCableActive;                   // Aktiv Kabel entladen ein (true) oder aus (false)
		ControlVar.UnloadCablePassive = STD_UnloadCablePassive;                 // Passiv Kabel entladen ein (true) oder aus (false)
		ControlVar.CalibrateOn = STD_CalibrateOn;                               // Kalibrieren ein (true) oder aus (false)
		ControlVar.CalibrateDepth = STD_CalibrateDepth;                         // Kalibrieren des Teufenrechners (true) - Eichwertbestimmung
		ControlVar.CalibrateTraction = STD_CalibrateTraction;                   // Kalibrieren des Zugraftmesser-Moduls (true) - Anstieg- und Nullwertbestimmung
		ControlVar.GetCtlUnitSWVersion = STD_GetCtlUnitSWVersion;				// Gibt an, ob die Software-Version des Steuergeräts ausgelesen werden soll
		ControlVar.GetDepthCalcSWVersion = STD_GetDepthCalcSWVersion;           // Gibt an, ob die Software-Version des Teufenrechners ausgelesen werden soll
		ControlVar.LostMotionImpulse = STD_LostMotionImpulse;                   // Gibt an, ob der Freilaufimpuls noch auftreten muss oder nicht
		ControlVar.SetDepth = STD_SetDepth;                                  	// Gibt an, ob die Start-Teufe neu an das Steuergerät übergeben werden soll (true)
		ControlVar.SetDepth_DC = STD_SetDepth;                                  // Gibt an, ob die Start-Teufe neu an den Teufenrechner übergeben werden soll (true)
		ControlVar.SetDepthCalcParam = STD_SetDepthCalcParam;                   // Gibt an, ob die Parameter an den Teufenrechner übergeben werden sollen (true)
		ControlVar.SetDepthCalFactor = STD_SetDepthCalFactor;                	// Gibt an, ob der Kalibrierfaktor neu an das Steuergerät übergeben werden soll (true)
		ControlVar.SetDepthCalFactor_DC = STD_SetDepthCalFactor;                // Gibt an, ob der Eichfaktor neu an den Teufenrechner übergeben werden soll (true)
		ControlVar.SetFormatCMImpulse = STD_SetFormatCMImpulse;                 // Gibt an, ob der Wert in FormatCMImpulse an den Teufenrechner übergeben werden soll (true) oder nicht (false)
		ControlVar.SetMagneticMark = STD_SetMagneticMark;                       // Gibt an, ob der Wert in MagneticMarkOn an den Teufenrechner übergeben werden soll (true) oder nicht (false)
		ControlVar.WinchBrakeOff = STD_WinchBrakeOff;                           // Gibt an, ob die Bremse der Winde gelöst werden soll (NICHT GLEICH FREILAUF)
		ControlVar.WinchBrakeOffImpulse = STD_WinchBrakeOffImpulse;             // Gibt an, ob ein Impuls zur Steuerung der Bremslösung bzw. Anzug kommen soll
		ControlVar.CalibTension_GetVal1 = STD_CalibTension_GetVal1;     	    // Gibt an, ob der erste Spannungswert für die Zugkraftkalibrierung ermittelt werden soll
		ControlVar.CalibTension_GetVal2 = STD_CalibTension_GetVal2;		        // Gibt an, ob der zweite Spannungswert für die Zugkraftkalibrierung ermittelt werden soll
		ControlVar.CalibTractionCalculateParam = STD_CalibTractionCalculateParam;// Gibt an, ob die Parameter zur Zugkraftermittlung berechnet werden sollen
		ControlVar.CalibTractionSetParam = STD_CalibTractionSetParam;           // Gibt an, ob die Parameter zur Zugkraftermittlung übernommen werden sollen
		ControlVar.CalibTractionSetZero = STD_CalibTractionSetZero;             // Gibt an, ob ein Nullableich für den Zugkraftmesser durchegführt werden soll
		ControlVar.SignalHornOn = STD_SignalHornOn;                             // Gibt an, ob das Signal-Horn betätigt ist (true) oder nicht (false)
		ControlVar.EngineStart = STD_EngineStart;                               // Gibt an, ob der Motor gestartet werden soll (true)
		ControlVar.EngineStop = STD_EngineStart;                                // Gibt an, ob der Motor ausgeschaltet werden soll (true)
		ControlVar.SpecialFunc1 = STD_SpecialFunc1;								// Sonderfunktion 1
		ControlVar.PowerFail =  STD_PowerFail;                                  // Gibt an, ob die Stromversorgung des STR durch den SST-Schlüsselschalte oder die Kfz-Zündung abgeschaltet (true) wurde

		//Statusinformationen
		StatusInfoVar.CurrDepth = STD_CurrDepth;                                // momentane Teufe
		StatusInfoVar.CurrDirection = STD_CurrDirection;                        // momentane Fahrtrichtung; abwärts (false) / aufwärts (true)
		StatusInfoVar.CurrVelocity = STD_CurrVelocity;                          // momentane Sondengeschwindigkeit
		StatusInfoVar.AnnouncedVelocity = STD_AnnouncedVelocity;                // angezeigte Sondengeschwindigkeit
		StatusInfoVar.CurrWinchRevMessVoltage = STD_CurrWinchRevMessVoltage;    // Gemessene momentane Windengeschwindigkeitssteuer-Spannung in mV
		StatusInfoVar.CurrTensionLimit_Lower = STD_CurrTensionLimit_Lower;      // momentane Mindestzugkraft
		StatusInfoVar.CurrTensionLimit_Upper = STD_CurrTensionLimit_Upper;      // momentane Maximalzugkraft
		StatusInfoVar.CurrTraction = STD_CurrTraction;                          // momentane Zugkraft
		StatusInfoVar.CurrTractionDifference = STD_CurrTractionDifference;      // momentane Zugkraftdifferenz
		StatusInfoVar.CurrTractionReference = STD_CurrTractionReference;        // momentane Soll-Zugkraft
		StatusInfoVar.CurrTimeDifference = STD_CurrTimeDifference;              // momentane Zeitdifferenz
		StatusInfoVar.CurrTractionVoltage = STD_CurrTractionVoltage;            // momentane Spannung vom Zugkraftmesser
		StatusInfoVar.MagneticMark = STD_MagneticMark;                          // Magnetmarke tritt auf (true)
		StatusInfoVar.MagneticMarkStatic = STD_MagneticMark;                    // Magnetmarke tritt auf (true)
		StatusInfoVar.MagneticMarkStaticLOG = STD_MagneticMark;                 // Magnetmarke tritt auf (true)
		StatusInfoVar.MagneticMarkDirect = STD_MagneticMark;                    // Magnetmarke tritt auf (true)
		StatusInfoVar.ControllerError = STD_ControllerError;                    // Steuergerät signalisiert Fehler (true)
		StatusInfoVar.LostMotion = STD_LostMotion;                              // Leerlauf aktiv (true), inaktiv (false)
		StatusInfoVar.EngineCoolantTemp = STD_EngineCoolantTemp;				// Temperatur der Antriebsmotorkühlflüssigkeit
		StatusInfoVar.EngineOn = STD_EngineOn;                                  // Motor ein(true)/aus(false)
		StatusInfoVar.EngineSpeed = STD_EngineSpeed;							// Antriebsmotordrehzahl in rpm
		StatusInfoVar.OilStrainer = STD_OilStrainer;                            // Ölfilter ein/aus
		StatusInfoVar.OilStrainer2 = STD_OilStrainer;                           // Ölfilter 2 ein/aus
		StatusInfoVar.HydraulicOK = STD_HydraulicOK;                      	    // Hydraulik OK
		StatusInfoVar.HydrWrkPressure = STD_HydrWrkPressure;					// Aktueller Arbeitsdruck der Hydraulik
        StatusInfoVar.HydrChargePressure = STD_HydrChargePressure;				// Speisedruck der Hydraulik
    	StatusInfoVar.HydrLvl = STD_HydrLvl;									// Aktueller Hydraulikfüllstand
		StatusInfoVar.EmergencySwitch = STD_EmergencySwitch;                    // Notschalter betätigt ein/aus
		StatusInfoVar.ProgramMode = STARTUP_ProgramMode;                        // Programm-Modus beim Programmstart
		StatusInfoVar.DepthThresholdExceeded = STD_DepthThresholdExceeded;      // Teufenbereich verlassen
		StatusInfoVar.TractionThresholdExceeded = STD_TractionThresholdExceeded;// Zugkraftschwelle überschritten
		StatusInfoVar.LeverOn = STD_LeverOn;                                    // Fahrhebel aktiv (Manuelle Fahrtrichtungs-/Geschwindigkeitseinstellung)
		StatusInfoVar.HWCtrlSignalisation = STD_HWCtrlSignalisation;            // Signalisiert den tatsächlichen Hardware-Kontroll-Status
		StatusInfoVar.AutomotiveError = STD_AutomotiveError;                    // Signalisiert einen Fahrzeugfehler (Warnung für z.B. Motor-Öldruck, Motor-Temperatur)
		StatusInfoVar.SpecialSignal01 = STD_SpecialSignal01;                    // Konfigurierbares Sondersignal zur Signalisierung verschiedener Ereignisse
		StatusInfoVar.SpecialSignal02 = STD_SpecialSignal02;                    // Konfigurierbares Sondersignal zur Signalisierung verschiedener Ereignisse
		StatusInfoVar.SpecialSignalExtBrake = STD_SpecialSignalExtBrake;        // Durch ein Sondersignal überwachte externe Bremse

		StatusInfoVar.CalibTractionValue1Ok = STD_CalibTractionValue1Ok;        // Gibt an, ob der erste Spannungswert erfolgreich ermittelt wurde
		StatusInfoVar.CalibTractionValue2Ok = STD_CalibTractionValue2Ok;        // Gibt an, ob der zweite Spannungswert erfolgreich ermittelt wurde
		StatusInfoVar.CalibDepthAllowed = STD_CalibDepthAllowed;				// Gibt an, ob das Kalibrieren der Teufe im Aktuellen Zustand generell möglich ist
		StatusInfoVar.CalibDepthCableOk = STD_CalibDepthCableOk;                // Gibt an, ob das Kabel eine bestimmte Länge herausgezogen wurde und nun die Teufe angegeben werden kann
		strcpy(StatusInfoVar.CtlUnitSWVersion, STD_CtlUnitSWVersion);       	// Pufferspeicher für den Versionsstring des Steuergeräts
		strcpy(StatusInfoVar.DepthCalcSWVersion, STD_DepthCalcSWVersion);       // Pufferspeicher für den Versionsstring des Teufenrechners
		StatusInfoVar.TemperatureSensorDepthCalc = STD_TemperatureSensorDepthCalc; // Enthält die Temperatur des T.-Sensors auf dem Teufenrechner in °C
		StatusInfoVar.TemperatureSensorA = STD_TemperatureSensorA;              // Enthält die Temperatur des T.-Sensors A in °C
		StatusInfoVar.TemperatureSensorB = STD_TemperatureSensorB;              // Enthält die Temperatur des T.-Sensors B in °C
		StatusInfoVar.TemperatureOilTank = STD_TemperatureOilTank;				// Öltanktemperatur in °C

		// Programmeinstellungen
		ParameterVar.WinchNo = STD_WinchNo;                                     // Nummer der aktuellen Winde
//        ParameterVar.WinchCnt = STD_WinchCnt;                                   // Anzahl der zu verwaltenden Winden
		ParameterVar.HydraulicBrakeOn = STD_HydraulicBrakeOn;                   // Hydraulische Bremse aktiv (true) / inaktiv (false)
		ParameterVar.UnloadCableStretch = STD_UnloadCableStretch;         		// Stretching beim entladen des Kabels ein (true) oder aus (false)
		ParameterVar.MinDepth = STD_MinDepth;                                   // einstellbare Mindestteufe
		ParameterVar.MaxDepth = STD_MaxDepth;                                   // einstellbare Maximalteufe
		ParameterVar.StartDepth = STD_StartDepth;                               // einstellbare Startteufe
		ParameterVar.ReverseDepth = STD_ReverseDepth;							// Einstellung, ob die Teufe umgekehrt angezeigt werden soll (aus - wird + und umgekehrt)
		ParameterVar.DepthThresholdForAuto = STD_DepthThresholdForAuto;         // Teufe, ab wann Automatikbetrieb möglich ist
		ParameterVar.DepthDistanceForBreak = STD_DepthDistanceForBreak;         // Abstand vorm Endewert, ab welchem die Winde abgebremst werden soll
		ParameterVar.MMSyncForceOn = STD_MMSyncForceOn;							// MM-Zwangssynchr. Ein/Aus
		ParameterVar.MMSyncForceRangeLim = STD_MMSyncForceRangeLim;				// MM-Zwangssynchr. Teufenbereichsgrenze in Meter
/*		ParameterVar.UnloadCable_PID_TD = STD_UnloadCable_PID_TD;         		// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Vorhaltezeit
		ParameterVar.UnloadCable_PID_TD_SAVE = STD_UnloadCable_PID_TD;    		// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Vorhaltezeit (zum Speichern)
		ParameterVar.UnloadCable_PID_TI = STD_UnloadCable_PID_TI;         		// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Nachstellzeit
		ParameterVar.UnloadCable_PID_TI_SAVE = STD_UnloadCable_PID_TI;    		// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Nachstellzeit (zum Speichern)
		ParameterVar.UnloadCable_PID_KR = STD_UnloadCable_PID_KR;         		// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Verstärkungsfaktor
		ParameterVar.UnloadCable_PID_KR_SAVE = STD_UnloadCable_PID_KR;    		// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Verstärkungsfaktor (zum Speichern)
		ParameterVar.UnloadCable_PID_TD_2 = STD_UnloadCable_PID_TD_2;     		// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Vorhaltezeit
		ParameterVar.UnloadCable_PID_TD_2_SAVE = STD_UnloadCable_PID_TD_2;		// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Vorhaltezeit (zum Speichern)
		ParameterVar.UnloadCable_PID_TI_2 = STD_UnloadCable_PID_TI_2;     		// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Nachstellzeit
		ParameterVar.UnloadCable_PID_TI_2_SAVE = STD_UnloadCable_PID_TI_2;		// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Nachstellzeit (zum Speichern)
		ParameterVar.UnloadCable_PID_KR_2 = STD_UnloadCable_PID_KR_2;     		// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Verstärkungsfaktor
		ParameterVar.UnloadCable_PID_KR_2_SAVE = STD_UnloadCable_PID_KR_2;		// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Verstärkungsfaktor (zum Speichern)
		ParameterVar.UnloadCable_ExtReg_MaxTension = STD_UnloadCable_ExtReg_MaxTension; // Kabel Entladen mit externem Regler Kennliniendef. Maximalzugkraft in N
		ParameterVar.UnloadCable_ExtReg_MaxVoltage = STD_UnloadCable_ExtReg_MaxVoltage; // Kabel Entladen mit externem Regler Kennliniendef. Maximalspannung in mV
		ParameterVar.UnloadCable_ExtReg_MinTension =  STD_UnloadCable_ExtReg_MinVoltage; // Kabel Entladen mit externem Regler Kennliniendef. Mindestzugkraft
		ParameterVar.UnloadCable_ExtReg_MinVoltage = STD_UnloadCable_ExtReg_MinVoltage; // Kabel Entladen mit externem Regler Kennliniendef. Mindestspannung
//		ParameterVar.UnloadCable_ExtReg_MaxPosOffset = STD_UnloadCable_ExtReg_MaxPosOffset; // Kabel Entladen mit externem Regler max. positiver Offset der Kennlinie in mV
//		ParameterVar.UnloadCable_ExtReg_MaxNegOffset = STD_UnloadCable_ExtReg_MaxNegOffset; // Kabel Entladen mit externem Regler max. negativer Offset der Kennlinie in mV
//        ParameterVar.UnloadCable_ExtReg_Delay = STD_UnloadCable_ExtReg_Delay;	// Kabel Entladen mit externem Regler Verzögerung beim Abfahren (Rampe) in ms
//        ParameterVar.UnloadCable_ExtReg_P = STD_UnloadCable_ExtReg_P;			// Kabel Entladen mit externem Regler P-Anteil
		ParameterVar.UnloadCable_ExtReg_I = STD_UnloadCable_ExtReg_I;			// Kabel Entladen mit externem Regler I-Anteil
		ParameterVar.UnloadCable_ExtReg_ReturnRotVoltage = STD_UnloadCable_ExtReg_ReturnRotVoltage;		// Kabel Entladen mit externem Regler Windenansteuerspannung zum Kabel straffen in mV
*/
		ParameterVar.TractionViewDimensionUnit = STD_TractionViewDimensionUnit; // Darstellung der Zugkraft (0=1N|1=1kN|2=1kp)
		ParameterVar.Velocity = STD_Velocity;                                   // einstellbare Geschwindigkeit
        ParameterVar.Velocity_PrgAutom = STD_Velocity_PrgAutom;                 // einstellbare Geschwindigkeit für Programmautomatik (wird je nach Programmpunkt gesetzt)
        ParameterVar.Direction = STD_Direction;                                 // einstellbare Fahrtrichtung (false = abwärts, true = aufwärts)
        ParameterVar.Direction_PrgAutom = STD_Direction_PrgAutom;               // einstellbare Fahrtrichtung (false = abwärts, true = aufwärts) für Programmautomatik (wird je nach Programmpunkt gesetzt)
        ParameterVar.EngineRevolutionChangeable = STD_EngineRevolutionUpDown;   // Drehzahl Fahrzeugmotor veränderbar
        ParameterVar.EngineStartStopEnable = STD_EngineStartStopEnable;         // Antriebsmotor Starten / Stoppen möglich
        ParameterVar.LostMotionEnable = STD_LostMotionEnable;					// Freilaufbedienung möglich
		ParameterVar.BrakeOffEnable = STD_BrakeOffEnable;
        ParameterVar.ExternalBrakeAvailable = STD_ExternalBrakeAvailable;		// Externe Bremse verfügbar
		ParameterVar.FormatCMImpulse = STD_FormatCMImpulse;                     // eingestelltes Format für den cm-Impuls (0=nichts | 1=KARAT | 2=ONYX | 3=KEDR)
		ParameterVar.IncEncErrorMonitOn = STD_IncEncErrorMonitOn;               // Inkrementalgeber-Fehler-Überwachung
		ParameterVar.ControllerErrorMonitOn = STD_ControllerErrorMonitOn;       // Steuergerät - Fehler-Überwachung
        ParameterVar.ControllerErrorMonitOnAtSM = STD_ControllerErrorMonitOnAtSM;// Steuergerät - Fehler-Überwachung in der langsamen Fahrstufe aktiv
		ParameterVar.ControllerErrorLactive = STD_ControllerErrorLactive;		// Steuergerät - Fehler L-aktiv
		ParameterVar.UnloadCableBeginLenght = STD_UnloadCableBeginLenght; 		// Anfangslänge beim Entladen des Kabels
		ParameterVar.UnloadCableEndLenght = STD_UnloadCableEndLenght;     		// Endlänge beim Entladen des Kabels
		ParameterVar.UnloadCableMinTraction = STD_UnloadCableMinTraction; 		// Mindestzugkraft zum Entladen des Kabels für aktiven Zustand (Regime 1)
        for (unsigned int i = 0; i < _WinchCnt; ++i)
        {
				ParameterVar.MinTraction[i] = STD_MinTraction;	                // einstellbare Mindestzugkraft
				ParameterVar.MaxTraction[i] = STD_MaxTraction;                  // einstellbare Maximalzugkraft
				ParameterVar.RelTensionLimitLower[i]                            // einstellbare untere Zugkraftgerenze, abhängig von der aktuell höchsten Zugkraft (wichtig bei Abwärtsfahrt)
				= STD_RelTensionLimitLower;
				ParameterVar.RelTensionLimitUpper[i]                            // einstellbare obere Zugkraftgerenze, abhängig von der aktuell niedrigsten Zugkraft (wichtig bei Abwärtsfahrt)
				= STD_RelTensionLimitUpper;
				ParameterVar.UnloadCableTraction[i]
				= STD_UnloadCableTraction;  	                              	// einstellbare Zugkraft zum Entladen des Kabels für aktiven Zustand (Regime 1)
				ParameterVar.UnloadCableStartTraction[i]
				= STD_UnloadCableStartTraction; 	                          	// einstellbare Anfangszugkraft beim Entladen des Kabels
				ParameterVar.UnloadCableEndTraction[i]
				= STD_UnloadCableEndTraction;	                             	// einstellbare Endzugkraft beim Entladen des Kabels
				ParameterVar.CalFactDepth[i] = STD_CalFactDepth;	            // Eichfaktor für den Teufenrechner
				ParameterVar.CalibTractionX[i] = STD_CalibTractionX;	        // Zugkraftkalibrierung X-Achse für Anstiegsberechnung
				ParameterVar.CalibTractionY[i] = STD_CalibTractionY;	        // Zugkraftkalibrierung Y-Achse für Anstiegsberechnung
                ParameterVar.CalibTractionZero[i] = STD_CalibTractionZero; 	    // Nullwert für Zugkraftkalibrierung
        };

        ParameterVar.CalFactDepth_Earmarked = STD_CalFactDepth_Earmarked;		// Teufenkalibrierfaktor, ermittelt bei der Kalibrierung und vorgemerkt für die Übernahme
		ParameterVar.CalibTractionValue1 = STD_CalibTractionValue1;             // Die mit dem, bei der Zugkraftkalibrierung ermittelte, ersten Spannungswert korespondierende Zugkraft
		ParameterVar.CalibTractionValue2 = STD_CalibTractionValue2;             // Die mit dem, bei der Zugkraftkalibrierung ermittelte, zweiten Spannungswert korespondierende Zugkraft
		for (int i = 0; i < CalibTension_NumOfCalMeasuringPoints; ++i) {
			ParameterVar.CalibTension_MeasuringVal[i] = STD_CalibTension_MeasuringVal; // Der jeweilig mit der Zugkraft korrespondierende Messwert
			ParameterVar.CalibTension_Tension[i] = STD_CalibTension_Tension;    // Die jeweilig mit dem Messwert korrespondierende Zugkraft
		}
		ParameterVar.CalibTSensorDepthCalc_CaractSlope = STD_CalibTSensorDepthCalc_CaractSlope; // Anstieg der Kennlinie des T.-Sensors des Teufenrechners in °C/V
        ParameterVar.CalibTSensorDepthCalc_CaractZero = STD_CalibTSensorDepthCalc_CaractZero; // Nulldurchgang der Kennlinie des T.-Sensors des Teufenrechners in °C
		ParameterVar.CalibTSensorA_CaractSlope = STD_CalibTSensorA_CaractSlope; // Anstieg der Kennlinie des T.-Sensors A in °C/V
        ParameterVar.CalibTSensorA_CaractZero = STD_CalibTSensorA_CaractZero;   // Nulldurchgang der Kennlinie des T.-Sensors A in °C
        ParameterVar.CalibTSensorB_CaractSlope = STD_CalibTSensorB_CaractSlope; // Anstieg der Kennlinie des T.-Sensors B in °C/V
        ParameterVar.CalibTSensorB_CaractZero = STD_CalibTSensorB_CaractZero;   // Nulldurchgang der Kennlinie des T.-Sensors B in °C
        ParameterVar.AutoRunProgramableOn = STD_AutoRunProgramableOn;           // Programmautomatik Ein/Aus
        wcsncpy(ParameterVar.AutoRunProgramFileName, STD_AutoRunProgramFileName, LIM_PAR_AutoRunPrgFileNameLength);
        ParameterVar.VelocityDimensionUnit = STD_VelocityDimensionUnit;			// Darstellung der Geschwindigkeit (0=1m/h|1=1m/min)
		ParameterVar.WinchBrakeInternAuto = STD_WinchBrakeInternAuto;			// Windenbremse intern automatisch
		ParameterVar.WinchBrakeInternAutoReleasePerc = STD_WinchBrakeInternAutoReleasePerc; // Lösen der Windenbremse bei interner Automatikbremse in Prozent
		ParameterVar.WinchBrakeInternAutoReleaseHysteresisPerc = STD_WinchBrakeInternAutoReleaseHysteresisPerc; // Jitterkompensation Lösen/Feststellen der Windenbremse bei interner Automatikbremse in Prozent

        ParameterVar.LOGFileTensionUnit = STD_LOGFileTensionUnit;			    // Zugkrafteinheit für die Protokollierung
		ParameterVar.LOGFileType = STD_LOGFileType;                             // Typ der LOG-Datei (0...Standard; 1...LAS 2.0)
		ParameterVar.SpecialSignalMode = STD_SpecialSignalMode;                 // Modus, wie die Sondersignale ausgewertet werden sollen (welche Version der Abarbeitung) (0=Aus, 1=Positionssensoren Schaltgetriebe)

		ParameterVar.LineLubAvailable = STD_LineLubAvailable;					// Kabel-/Seilöler verfügbar
		ParameterVar.LineLubPermanentOperation = STD_LineLubPermanentOperation;	// Kabel-/Seilöler Dauerbetrieb
		ParameterVar.LineLubIntensity = STD_LineLubIntensity;					// Kabel-/Seilöler Intensität


};

//===========================================================================
// Dateiarbeit
// Öffnet eine bestehende Parameter-Datei und erstellt bei Bedarf eine neue
// Rückgabewert: _GOOD bei erfolgreichem Öffnen der Datei,
//               _BAD bei Misserfolg
// read: gibt an, ob Datei zum Lesen geöffnet werden soll (true) oder schreiben (false)
bool WCVarControl::OpenFile(bool read)
{
        CloseFile();                                                            // evtl. geöffnete Datei schließen

        if (read == true)
        {
            if (DataFileName == 0) return _BAD;

                InFi.open(DataFileName);
                if (InFi.is_open())
                {
                        InFi.seekg(0);
                        return _GOOD;
                };
        }else
        {
            if (TempDataFileName == 0) return _BAD;

                OuFi.open(TempDataFileName);                                    // Achtung zum Schreiben der Parameter wird die Temporäre Datei geöffnet
                if (OuFi.is_open())
                {
                        OuFi.seekp(0);
                        return _GOOD;
                };
        };

        return _BAD;
};
//---------------------------------------------------------------------------

// Öffnet eine bestehende spezielle separate spezielle Parameter-Datei und
// erstellt bei Bedarf eine neue (für Entwicklung)
bool WCVarControl::OpenInternalFile(bool read)
{
        if (InternalDataFileName == 0) return _BAD;

        CloseFile();                                                            // evtl. geöffnete Datei schließen
        if (read)
        {
                InternalInFi.open(InternalDataFileName);
                if (InternalInFi.is_open())
                {
                        InternalInFi.seekg(0);
                        return _GOOD;
                };
        }else
        {
                InternalOuFi.open(InternalDataFileName);
                if (InternalOuFi.is_open())
                {
                        InternalOuFi.seekp(0);
                        return _GOOD;
                };
        };

        return _BAD;
};
//---------------------------------------------------------------------------

// Schließt die momentan geöffnete Datei
void WCVarControl::CloseFile()
{
        if (InFi.is_open()) InFi.close();
        if (OuFi.is_open()) OuFi.close();
        if (InternalInFi.is_open()) InternalInFi.close();
        if (InternalOuFi.is_open()) InternalOuFi.close();
};

//---------------------------------------------------------------------------
// DEPENDENT ON PARAMETER
// Liest Daten aus der Parameter-Datei
bool WCVarControl::new_ReadData()
{
		const COMMENTENTRY_CNT = 73;                                            // Anzahl der verschiedenen Kommentare

        char CommentString[200];                                                // Kommentar-String (Hilfs-String)
        char HVString[200];                                                     // Puffer für eingelesene Zeile
        int HVIndex, HV;                                                        // Hilfsindex zur Positionierung innerhalb HVString, und allgemeine Hilfsvar

        bool Status_WinchDepended;                                              // Status Windenabhängig
		int Status_ValType;                                                     // 0 ... Integer, 1 ... bool, 2 ... float, 3 ... char

        int WinchNumber;                                                        // Windennummer, auf welchen sich der aktuelle Parametereintrag bezieht (Hilfsvar.)
        int tmpint;                                                             // Hilfsvariable zur Parameteraufnahme für verschiedene Typen
        bool tmpbool;
        float tmpfloat;
        wchar_t tmpstring[LIM_PAR_AutoRunPrgFileNameLength + 1];                // Achtung, die Länge muss dem größten verwendeten Ziel entsprechen!
        wchar_t Hwchar;

        JMGS_UnicodeCoder UC;

        // ----------------------------- Vorbereitung Vollständigkeistliste
        const BufSize_ParameterReady = (COMMENTENTRY_CNT / 8) + 1;              // Größendefinition der Nachfolgenden Puffer
        unsigned char ParameterReady[_WinchCnt][BufSize_ParameterReady];        // _WinchCnt dimmensionaler Puffer, für Statusbits, welcher Parameter bereits eingelesen wurde. Dadurch möglich, Festzustellen, ob alle erforderlichen Parameter eingelesen werden konnten.
        unsigned char BufBit;                                                   // Hilfsbyte für Bit-Setzen

        // Statuspuffer vorkonfigurieren (0 entspricht Eintrag 1 usw.)
        for (int i = 0; i < BufSize_ParameterReady; ++i)
        {
                for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][i] = 0x00;// alles leer
        };
		// Ausnahmen im Statuspuffer (entspr. Bits auf 1 vorkonfigurieren)
		for (int i = 1; i <= COMMENTENTRY_CNT; ++i)
		{
				BufBit = 1;                                                     // Bit im Hilfsbyte weiterzählen
				BufBit <<= ((i-1) % 8);
				switch (i)
				{                       // nicht benötigte Ausnahmen (sind keine Parameter wie z.B. Kommentare) auf 1 setzen
                        case 1:         // nur Kommentarzeile
						case 2:         // nur Kommentarzeile
						case 4:         // 02.02.2010 HW-regler Ein/Aus wird nicht mehr gespeichert
						case 8:         // 15.02.2017 Startteufe wird nicht mehr abgespeichert
						case 11:        // 02.02.2010 Dynamische Maximalzugkraftberechnung Ein/Aus wird nicht mehr gespeichert
						case 18:        // 05.04.2018 Schleifenfänger anders als bisher organisiert, Parameter überflüssig
						case 19:        // 02.02.2010 MM-Signalisierung Ein/Aus wird nicht mehr abgespeichert
						case 24:        // 03.04.2018 Teufenstufen für Maximalzugkraftberechnung werden seit Jahren nicht mehr verwendet -> Eintrag überflüssig
						case 31:        // 17.07.2008 Regler, der Geschwindigkeit Stufenweise regelt wurde entfernt->Parameter nicht mehr notwendig
						case 32:        // 17.07.2008 Regler, der Geschwindigkeit Stufenweise regelt wurde entfernt->Parameter nicht mehr notwendig
						case 33:        // 16.07.2016 Automatik anhalten nicht mehr notwendig
						case 34:        // 16.07.2016 Automatik anhalten nicht mehr notwendig
						case 35:        // 15.07.2016 Automatik anhalten nicht mehr notwendig
						case 36:        // 15.07.2016 Automatik anhalten nicht mehr notwendig
						case 37:        // 17.07.2008 Regler, der Geschwindigkeit Stufenweise regelt wurde entfernt->Parameter nicht mehr notwendig
						case 38:        // 17.07.2008 Regler, der Geschwindigkeit Stufenweise regelt wurde entfernt->Parameter nicht mehr notwendig
						case 39:        // 17.07.2008 Regler, der Geschwindigkeit Stufenweise regelt wurde entfernt->Parameter nicht mehr notwendig
						case 40:        // 17.07.2008 Regler, der Geschwindigkeit Stufenweise regelt wurde entfernt->Parameter nicht mehr notwendig
						case 41:        // 17.07.2008 Regler, der Geschwindigkeit Stufenweise regelt wurde entfernt->Parameter nicht mehr notwendig
						case 42:        // 17.07.2008 Regler, der Geschwindigkeit Stufenweise regelt wurde entfernt->Parameter nicht mehr notwendig
						case 49:        // 14.12.2016 Windenanzahl entfernt, Parameter nicht mehr notwendig
						case 50:        // 15.07.2016 Automatik Reglung ausgelagert
						case 51:        // 15.07.2016 Automatik Reglung ausgelagert
						case 52:        // 15.07.2016 Automatik Reglung ausgelagert
						case 55:        // 15.07.2016 Automatik Reglung ausgelagert
						case 56:        // 15.07.2016 Automatik Reglung ausgelagert
						case 57:        // 15.07.2016 Automatik Reglung ausgelagert
						case 58:		// 15.02.2017 Einstellung Programmautomatikfunktion nicht mehr abspeichern
						case 59:        // 15.02.2017 Einstellung Programmautomatikfunktion nicht mehr abspeichern
/*#ifndef __ENABLE_PROGRAMAUTOMATIC
						case 58:		// Bei nicht verfügbarer Programmautomatikfunktion, Parameter ignorieren
						case 59:        // Bei nicht verfügbarer Programmautomatikfunktion, Parameter ignorieren
#endif
*/
                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][(i-1) / 8] |= BufBit;        // Bit auf 1 setzen
                                        break;
                        default:
                                        break;
                };
        };
        // Ausfüllen der nicht verwendeten Bits mit 1
        for (int i = COMMENTENTRY_CNT; i < BufSize_ParameterReady * 8; ++i)
        {
                BufBit = 1;                                                     // Bit im Hilfsbyte weiterzählen
                BufBit <<= (i % 8);
                for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][i / 8] |= BufBit; // Bit auf 1 setzen
        };
        // -----------------------------

        if (OpenFile(true) == _BAD) return _BAD;

        InFi.seekg(0);

        try
        {
        while (!InFi.eof() && !InFi.fail())
        {
                // Zeile einlesen
                InFi.getline(HVString, sizeof(HVString));

                if ((strlen(HVString) == 0) || InFi.eof() || InFi.fail()) continue; // Wenn nichts zu tun nächste Zeile (bzw. Abbruch, siehe Schleifenbedingung)

                WinchNumber = -1;

                // Alle relevanten Kommentare durchprobieren
                for (int i = 3; i <= COMMENTENTRY_CNT; ++i)
                {
                        // -----------------------------
                        // Kommentar auswählen und zugehörige Stati setzen
                        switch (i)
                        {
                                case 3:
                                                strcpy(CommentString, COMMENT_PF_ENTRY003);
                                                Status_WinchDepended = false;
                                                Status_ValType = 0;
                                                break;
/*  02.02.2010 HW-regler Ein/Aus wird nicht mehr gespeichert
                                case 4:
                                                strcpy(CommentString, COMMENT_PF_ENTRY004);
                                                Status_WinchDepended = false;
                                                Status_ValType = 1;
                                                break;
*/
                                case 5:
                                                strcpy(CommentString, COMMENT_PF_ENTRY005);
                                                Status_WinchDepended = false;
                                                Status_ValType = 1;
                                                break;
                                case 6:
                                                strcpy(CommentString, COMMENT_PF_ENTRY006);
                                                Status_WinchDepended = false;
                                                Status_ValType = 0;
                                                break;
                                case 7:
                                                strcpy(CommentString, COMMENT_PF_ENTRY007);
                                                Status_WinchDepended = false;
                                                Status_ValType = 0;
                                                break;
/*                                case 8:
												strcpy(CommentString, COMMENT_PF_ENTRY008);
												Status_WinchDepended = false;
												Status_ValType = 0;
												break;
*/
                                case 9:
                                                strcpy(CommentString, COMMENT_PF_ENTRY009);
                                                Status_WinchDepended = false;
                                                Status_ValType = 0;
                                                break;
                                case 10:
                                                strcpy(CommentString, COMMENT_PF_ENTRY010);
                                                Status_WinchDepended = false;
                                                Status_ValType = 1;
                                                break;
/*  02.02.2010 Dynamische Maximalzugkraftberechnung Ein/Aus wird nicht mehr gespeichert
                                case 11:
                                                strcpy(CommentString, COMMENT_PF_ENTRY011);
                                                Status_WinchDepended = false;
                                                Status_ValType = 1;
                                                break;
*/
                                case 12:
                                                strcpy(CommentString, COMMENT_PF_ENTRY012);
                                                Status_WinchDepended = true;
                                                Status_ValType = 0;
                                                break;
                                case 13:
                                                strcpy(CommentString, COMMENT_PF_ENTRY013);
                                                Status_WinchDepended = true;
                                                Status_ValType = 0;
                                                break;
                                case 14:
                                                strcpy(CommentString, COMMENT_PF_ENTRY014);
                                                Status_WinchDepended = true;
                                                Status_ValType = 0;
                                                break;
                                case 15:
                                                strcpy(CommentString, COMMENT_PF_ENTRY015);
                                                Status_WinchDepended = true;
                                                Status_ValType = 0;
                                                break;
                                case 16:
                                                strcpy(CommentString, COMMENT_PF_ENTRY016);
                                                Status_WinchDepended = true;
                                                Status_ValType = 0;
                                                break;
                                case 17:
                                                strcpy(CommentString, COMMENT_PF_ENTRY017);
                                                Status_WinchDepended = true;
                                                Status_ValType = 0;
                                                break;
/* 05.04.2018 Schleifenfänger anders als bisher organisiert, Parameter überflüssig
								case 18:
												strcpy(CommentString, COMMENT_PF_ENTRY018);
												Status_WinchDepended = true;
												Status_ValType = 0;
												break;
*/
/*  02.02.2010 MM-Ein/Aus wird nicht mehr gespeichert
                                case 19:
                                                strcpy(CommentString, COMMENT_PF_ENTRY019);
                                                Status_WinchDepended = false;
                                                Status_ValType = 1;
                                                break;
*/
                                case 20:
                                                strcpy(CommentString, COMMENT_PF_ENTRY020);
                                                Status_WinchDepended = true;
                                                Status_ValType = 0;
                                                break;
                                case 21:
                                                strcpy(CommentString, COMMENT_PF_ENTRY021);
                                                Status_WinchDepended = true;
                                                Status_ValType = 0;
                                                break;
                                case 22:
                                                strcpy(CommentString, COMMENT_PF_ENTRY022);
                                                Status_WinchDepended = true;
                                                Status_ValType = 0;
                                                break;
                                case 23:
                                                strcpy(CommentString, COMMENT_PF_ENTRY023);
                                                Status_WinchDepended = true;
                                                Status_ValType = 0;
                                                break;
/* 03.04.2018
								case 24:
												strcpy(CommentString, COMMENT_PF_ENTRY024);
												Status_WinchDepended = false;
												Status_ValType = 0;
												break;
*/
                                case 25:
                                                strcpy(CommentString, COMMENT_PF_ENTRY025);
                                                Status_WinchDepended = false;
                                                Status_ValType = 0;
                                                break;
                                case 26:
                                                strcpy(CommentString, COMMENT_PF_ENTRY026);
                                                Status_WinchDepended = false;
                                                Status_ValType = 0;
                                                break;
                                case 27:
                                                strcpy(CommentString, COMMENT_PF_ENTRY027);
                                                Status_WinchDepended = false;
                                                Status_ValType = 0;
                                                break;
                                case 28:
                                                strcpy(CommentString, COMMENT_PF_ENTRY028);
												Status_WinchDepended = true;
                                                Status_ValType = 0;
                                                break;
                                case 29:
                                                strcpy(CommentString, COMMENT_PF_ENTRY029);
                                                Status_WinchDepended = false;
                                                Status_ValType = 0;
                                                break;
                                case 30:
                                                strcpy(CommentString, COMMENT_PF_ENTRY030);
                                                Status_WinchDepended = false;
                                                Status_ValType = 0;
                                                break;
/* 17.07.2008 Regler, der Geschwindigkeit Stufenweise regelt wurde entfernt->Parameter nicht mehr notwendig
                                case 31:
                                                strcpy(CommentString, COMMENT_PF_ENTRY031);
                                                Status_WinchDepended = false;
                                                Status_ValType = 0;
                                                break;
                                case 32:
                                                strcpy(CommentString, COMMENT_PF_ENTRY032);
                                                Status_WinchDepended = false;
                                                Status_ValType = 0;
                                                break;
*/
/*                                case 33:
												strcpy(CommentString, COMMENT_PF_ENTRY033);
												Status_WinchDepended = false;
												Status_ValType = 0;
												break;
								case 34:
												strcpy(CommentString, COMMENT_PF_ENTRY034);
												Status_WinchDepended = false;
												Status_ValType = 0;
												break;
								case 35:
												strcpy(CommentString, COMMENT_PF_ENTRY035);
												Status_WinchDepended = false;
												Status_ValType = 0;
												break;
								case 36:
												strcpy(CommentString, COMMENT_PF_ENTRY036);
												Status_WinchDepended = false;
												Status_ValType = 0;
												break;
								case 37:
                                                strcpy(CommentString, COMMENT_PF_ENTRY037);
                                                Status_WinchDepended = false;
                                                Status_ValType = 2;
                                                break;
                                case 38:
                                                strcpy(CommentString, COMMENT_PF_ENTRY038);
                                                Status_WinchDepended = false;
                                                Status_ValType = 2;
                                                break;
                                case 39:
                                                strcpy(CommentString, COMMENT_PF_ENTRY039);
                                                Status_WinchDepended = false;
                                                Status_ValType = 2;
                                                break;
                                case 40:
                                                strcpy(CommentString, COMMENT_PF_ENTRY040);
                                                Status_WinchDepended = false;
                                                Status_ValType = 2;
                                                break;
                                case 41:
                                                strcpy(CommentString, COMMENT_PF_ENTRY041);
                                                Status_WinchDepended = false;
                                                Status_ValType = 2;
                                                break;
                                case 42:
												strcpy(CommentString, COMMENT_PF_ENTRY042);
												Status_WinchDepended = false;
												Status_ValType = 2;
												break;
*/
								case 43:
                                                strcpy(CommentString, COMMENT_PF_ENTRY043);
                                                Status_WinchDepended = false;
                                                Status_ValType = 2;
                                                break;
                                case 44:
                                                strcpy(CommentString, COMMENT_PF_ENTRY044);
                                                Status_WinchDepended = false;
                                                Status_ValType = 2;
                                                break;
                                case 45:
                                                strcpy(CommentString, COMMENT_PF_ENTRY045);
                                                Status_WinchDepended = false;
                                                Status_ValType = 2;
                                                break;
                                case 46:
                                                strcpy(CommentString, COMMENT_PF_ENTRY046);
                                                Status_WinchDepended = false;
                                                Status_ValType = 2;
                                                break;
                                case 47:
                                                strcpy(CommentString, COMMENT_PF_ENTRY047);
                                                Status_WinchDepended = false;
                                                Status_ValType = 2;
                                                break;
                                case 48:
                                                strcpy(CommentString, COMMENT_PF_ENTRY048);
                                                Status_WinchDepended = false;
                                                Status_ValType = 2;
                                                break;
/*                                case 49:
												strcpy(CommentString, COMMENT_PF_ENTRY049);
												Status_WinchDepended = false;
												Status_ValType = 0;
												break;
								case 50:
												strcpy(CommentString, COMMENT_PF_ENTRY050);
												Status_WinchDepended = false;
												Status_ValType = 0;
												break;
								case 51:
												strcpy(CommentString, COMMENT_PF_ENTRY051);
												Status_WinchDepended = false;
												Status_ValType = 0;
												break;
								case 52:
												strcpy(CommentString, COMMENT_PF_ENTRY052);
												Status_WinchDepended = false;
												Status_ValType = 0;
												break;
*/
                                case 53:
                                                strcpy(CommentString, COMMENT_PF_ENTRY053);
                                                Status_WinchDepended = false;
                                                Status_ValType = 0;
                                                break;
                                case 54:
                                                strcpy(CommentString, COMMENT_PF_ENTRY054);
                                                Status_WinchDepended = false;
                                                Status_ValType = 1;
                                                break;
/*                                case 55:
												strcpy(CommentString, COMMENT_PF_ENTRY055);
                                                Status_WinchDepended = false;
                                                Status_ValType = 0;
                                                break;
                                case 56:
                                                strcpy(CommentString, COMMENT_PF_ENTRY056);
                                                Status_WinchDepended = false;
                                                Status_ValType = 0;
                                                break;
                                case 57:
                                                strcpy(CommentString, COMMENT_PF_ENTRY057);
                                                Status_WinchDepended = false;
                                                Status_ValType = 0;
												break;
*/
/*
#ifdef __ENABLE_PROGRAMAUTOMATIC
// Bei verfügbarer Programmatomatikfunktion werden diess Parameter
// aus der Parameterdatei gelesen, andernfalls nicht.
// Weiter oben in dieser Methode muss die Statuspufferenstellung
// mit dieser Einstellung her abgestimmt sein.
								case 58:
												strcpy(CommentString, COMMENT_PF_ENTRY058);
												Status_WinchDepended = false;
												Status_ValType = 1;
												break;
								case 59:
												strcpy(CommentString, COMMENT_PF_ENTRY059);
												Status_WinchDepended = false;
												Status_ValType = 3;
												break;
#endif
*/
								case 60:
												strcpy(CommentString, COMMENT_PF_ENTRY060);
												Status_WinchDepended = false;
												Status_ValType = 1;
												break;
								case 61:
												strcpy(CommentString, COMMENT_PF_ENTRY061);
												Status_WinchDepended = false;
												Status_ValType = 1;
												break;
								case 62:
												strcpy(CommentString, COMMENT_PF_ENTRY062);
												Status_WinchDepended = false;
												Status_ValType = 0;
												break;
								case 63:
												strcpy(CommentString, COMMENT_PF_ENTRY063);
												Status_WinchDepended = false;
												Status_ValType = 1;
												break;
								case 64:
												strcpy(CommentString, COMMENT_PF_ENTRY064);
												Status_WinchDepended = false;
												Status_ValType = 0;
												break;
								case 65:
												strcpy(CommentString, COMMENT_PF_ENTRY065);
												Status_WinchDepended = false;
												Status_ValType = 1;
												break;
								case 66:
												strcpy(CommentString, COMMENT_PF_ENTRY066);
												Status_WinchDepended = false;
												Status_ValType = 1;
												break;
								case 67:
												strcpy(CommentString, COMMENT_PF_ENTRY067);
												Status_WinchDepended = false;
												Status_ValType = 0;
												break;
								case 68:
												strcpy(CommentString, COMMENT_PF_ENTRY068);
												Status_WinchDepended = false;
												Status_ValType = 1;
												break;
								case 69:
												strcpy(CommentString, COMMENT_PF_ENTRY069);
												Status_WinchDepended = false;
												Status_ValType = 1;
												break;
								case 70:
												strcpy(CommentString, COMMENT_PF_ENTRY070);
												Status_WinchDepended = false;
												Status_ValType = 1;
												break;
								case 71:
												strcpy(CommentString, COMMENT_PF_ENTRY071);
												Status_WinchDepended = false;
												Status_ValType = 1;
												break;
								case 72:
												strcpy(CommentString, COMMENT_PF_ENTRY072);
												Status_WinchDepended = false;
												Status_ValType = 1;
												break;
								case 73:
												strcpy(CommentString, COMMENT_PF_ENTRY073);
												Status_WinchDepended = false;
												Status_ValType = 0;
												break;
								default:
												continue;                       // Wert ist keinem Eintrag zugeordnet, nächsten versuchen

						};
						// -----------------------------
						// Entspricht der Beginn der aktuellen Parameterzeile dem momentanen Kommentar?
                        if (strncmpi(CommentString, HVString, strlen(CommentString)) == 0)
                        {
                                HVIndex = strlen(CommentString);                // Hinter das letzte zu vergleichende Zeichen springen

                                // Windenabhängig ? wenn ja, welche Winde
                                if (Status_WinchDepended)
                                {
                                        // alle Leerzeichen und Tabulatoren überspringen
                                        while((HVString[HVIndex] == 0x20) || (HVString[HVIndex] == 0x09)) ++HVIndex;

                                        // Wert prüfen (nur von 0 bis 9 zulässig) und Zahl zusammensetzen
                                        while ((HVString[HVIndex] >= '0') && (HVString[HVIndex] <= '9'))
                                        {
                                                if (WinchNumber >= 0) WinchNumber *= 10; // wenn schon Ziffer übernommen, dann ein Dezimalindex mehr
                                                else WinchNumber = 0;           // ansonsten vorbereiten der ersten Ziffer

                                                WinchNumber += (int)(HVString[HVIndex++]) & 0x0F;

                                                // Wenn Anzahl der max. festgelegten Winden überschritten, dann ungültig und Abbruch
                                                if (!(WinchNumber < _WinchCnt))
                                                {
                                                        WinchNumber = -1;
                                                        break;
                                                };
                                        };

                                        if (WinchNumber < 0) continue;          // erwartete Windennummer nicht gefunden -> Zeile ungültig -> nächste Zeile
                                };

                                // Wert erfassen
                                // alle Leerzeichen und Tabulatoren überspringen
                                while((HVString[HVIndex] == 0x20) || (HVString[HVIndex] == 0x09)) ++HVIndex;

                                if (Status_ValType == 0) {

                                    tmpint = atoi(&HVString[HVIndex]); // Integer

                                } else if (Status_ValType == 1) {

                                    tmpbool = (bool)(atoi(&HVString[HVIndex])); // Boolean

                                } else if (Status_ValType == 2) {                 // Float

                                    if ((atof(&HVString[HVIndex]) == _LHUGE_VAL) || (atof(&HVString[HVIndex]) == -_LHUGE_VAL)) continue; // ungültig -> nächste Zeile
                                    tmpfloat = (float)(atof(&HVString[HVIndex]));

                                }else if (Status_ValType == 3) {

									for (int i = 0; i < sizeof(tmpstring); ++i) {

                                        // Feldgrenze der Quelle erreicht ?
                                        if (HVIndex >= sizeof(HVString)) {
                                            tmpstring[i] = 0; // EK setzen
                                            break; // fertig
                                        }

                                        // nächstes UTF-8 Zeichen dekodieren
                                        if (!UC.DecodeFromUTF8(&Hwchar, &HV, &HVString[HVIndex])) {
                                            tmpstring[i] = 0; // EK setzen
                                            break; // fertig
                                        }

                                        // Zeichen eintragen
                                        HVIndex += HV;
                                        tmpstring[i] = reinterpret_cast<wchar_t>(Hwchar);
                                    }
                                    tmpstring[sizeof(tmpstring) - 1] = 0;   // EK zur Sicherheit

                                } else continue;                                // ungültig -> nächste Zeile (tritt das auf, dann liegt Programmierfehler vor)

                        // -----------------------------
                                // Wert übernehmen
                                switch (i)
                                {
                                        case 3:
                                                        if ((tmpint >= 0) && (tmpint < _WinchCnt))
                                                        {
                                                                ParameterVar.WinchNo = tmpint;
                                                                for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][0] |= 0x04; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        };
                                                        break;
/*  02.02.2010 HW-regler Ein/Aus wird nicht mehr gespeichert
                                        case 4:
                                                        break;
*/
                                        case 5:
                                                        ParameterVar.UnloadCableStretch = tmpbool;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][0] |= 0x10; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
                                        case 6:
                                                        ParameterVar.MinDepth = tmpint;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][0] |= 0x20; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
                                        case 7:
                                                        ParameterVar.MaxDepth = tmpint;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][0] |= 0x40; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
/*                                        case 8:
														ParameterVar.StartDepth = tmpint;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][0] |= 0x80; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
*/
                                        case 9:
                                                        ParameterVar.Velocity = tmpint;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][1] |= 0x01; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
                                        case 10:
                                                        ParameterVar.Direction = tmpbool;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][1] |= 0x02; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
/*  02.02.2010 Dynamische Maximalzugkraftberechnung Ein/Aus wird nicht mehr gespeichert
                                        case 11:
*/
                                        case 12:
                                                        ParameterVar.MinTraction[WinchNumber] = tmpint;
                                                        ParameterReady[WinchNumber][1] |= 0x08;
                                                        break;
                                        case 13:
                                                        ParameterVar.MaxTraction[WinchNumber] = tmpint;
                                                        ParameterReady[WinchNumber][1] |= 0x10;
                                                        break;
                                        case 14:
                                                        ParameterVar.RelTensionLimitLower[WinchNumber] = tmpint;
                                                        ParameterReady[WinchNumber][1] |= 0x20;
                                                        break;
                                        case 15:
                                                        ParameterVar.UnloadCableTraction[WinchNumber] = tmpint;
                                                        ParameterReady[WinchNumber][1] |= 0x40;
                                                        break;
                                        case 16:
														ParameterVar.UnloadCableStartTraction[WinchNumber] = tmpint;
                                                        ParameterReady[WinchNumber][1] |= 0x80;
                                                        break;
                                        case 17:
                                                        ParameterVar.UnloadCableEndTraction[WinchNumber] = tmpint;
                                                        ParameterReady[WinchNumber][2] |= 0x01;
                                                        break;
/* 05.04.2018 Schleifenfänger anders als bisher organisiert, Parameter überflüssig
										case 18:
														break;
*/
/*  02.02.2010 MM-Ein/Aus wird nicht mehr gespeichert
                                        case 19:
                                                        break;
*/
										case 20:
                                                        ParameterVar.CalFactDepth[WinchNumber] = tmpint;
                                                        ParameterReady[WinchNumber][2] |= 0x08;
                                                        break;
                                        case 21:
                                                        ParameterVar.CalibTractionX[WinchNumber] = tmpint;
                                                        ParameterReady[WinchNumber][2] |= 0x10;
                                                        break;
                                        case 22:
                                                        ParameterVar.CalibTractionY[WinchNumber] = tmpint;
                                                        ParameterReady[WinchNumber][2] |= 0x20;
                                                        break;
                                        case 23:
                                                        ParameterVar.CalibTractionZero[WinchNumber] = tmpint;
                                                        ParameterReady[WinchNumber][2] |= 0x40;
                                                        break;
/*										case 24:
														ParameterVar.CalcMaxTractionDepthStep = tmpint;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][2] |= 0x80; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
*/
                                        case 25:
														ParameterVar.UnloadCableBeginLenght = tmpint;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][3] |= 0x01; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
                                        case 26:
                                                        ParameterVar.UnloadCableEndLenght = tmpint;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][3] |= 0x02; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
                                        case 27:
                                                        ParameterVar.FormatCMImpulse = tmpint;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][3] |= 0x04; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
                                        case 28:
														ParameterVar.RelTensionLimitUpper[WinchNumber] = tmpint;
														ParameterReady[WinchNumber][3] |= 0x08;
														break;
                                        case 29:
                                                        ParameterVar.TractionViewDimensionUnit = tmpint;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][3] |= 0x10; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
                                        case 30:
														ParameterVar.LOGFileType = tmpint;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][3] |= 0x20; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
/* 17.07.2008 Regler, der Geschwindigkeit Stufenweise regelt wurde entfernt->Parameter nicht mehr notwendig
                                        case 31:
                                        case 32:
                                                        break;
*/
/*                                        case 33:
														ParameterVar.StopVelocityStep = ParameterVar.StopVelocityStepSAVE = tmpint;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][4] |= 0x01; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 34:
														ParameterVar.DriveVelocityStepClock = ParameterVar.DriveVelocityStepClockSAVE = tmpint;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][4] |= 0x02; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 35:
														ParameterVar.DriveVelocity0mph = ParameterVar.DriveVelocity0mphSAVE = tmpint;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][4] |= 0x04; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 36:
														ParameterVar.AutoRunStoppingClock = ParameterVar.AutoRunStoppingClockSAVE = tmpint;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][4] |= 0x08; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 37:
														ParameterVar.UnloadCable_PID_TD = ParameterVar.UnloadCable_PID_TD_SAVE = tmpfloat;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][4] |= 0x10; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 38:
														ParameterVar.UnloadCable_PID_TI = ParameterVar.UnloadCable_PID_TI_SAVE = tmpfloat;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][4] |= 0x20; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 39:
														ParameterVar.UnloadCable_PID_KR = ParameterVar.UnloadCable_PID_KR_SAVE = tmpfloat;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][4] |= 0x40; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 40:
														ParameterVar.UnloadCable_PID_TD_2 = ParameterVar.UnloadCable_PID_TD_2_SAVE = tmpfloat;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][4] |= 0x80; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 41:
														ParameterVar.UnloadCable_PID_TI_2 = ParameterVar.UnloadCable_PID_TI_2_SAVE = tmpfloat;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][5] |= 0x01; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 42:
														ParameterVar.UnloadCable_PID_KR_2 = ParameterVar.UnloadCable_PID_KR_2_SAVE = tmpfloat;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][5] |= 0x02; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
*/
										case 43:
														ParameterVar.CalibTSensorDepthCalc_CaractSlope = tmpfloat;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][5] |= 0x04; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 44:
														ParameterVar.CalibTSensorDepthCalc_CaractZero = tmpfloat;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][5] |= 0x08; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
                                        case 45:
                                                        ParameterVar.CalibTSensorA_CaractSlope = tmpfloat;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][5] |= 0x10; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
                                        case 46:
                                                        ParameterVar.CalibTSensorA_CaractZero = tmpfloat;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][5] |= 0x20; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
                                        case 47:
                                                        ParameterVar.CalibTSensorB_CaractSlope = tmpfloat;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][5] |= 0x40; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
                                        case 48:
                                                        ParameterVar.CalibTSensorB_CaractZero = tmpfloat;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][5] |= 0x80; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
/*                                        case 49:
														if (tmpint < 1) ParameterVar.WinchCnt = 1;
														else if (tmpint > _WinchCnt) ParameterVar.WinchCnt = _WinchCnt;   // Sinfälligkeitstest und ggf. Korrektur
														else ParameterVar.WinchCnt = tmpint;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][6] |= 0x01; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 50:
                                                        ParameterVar.AutoRunVelocityPID_Kp = ParameterVar.AutoRunVelocityPID_KpSAVE = tmpint;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][6] |= 0x02; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
                                        case 51:
                                                        ParameterVar.AutoRunVelocityPID_Ki = ParameterVar.AutoRunVelocityPID_KiSAVE = tmpint;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][6] |= 0x04; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
                                        case 52:
                                                        ParameterVar.AutoRunVelocityPID_Kd = ParameterVar.AutoRunVelocityPID_KdSAVE = tmpint;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][6] |= 0x08; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
*/
                                        case 53:
                                                        ParameterVar.SpecialSignalMode = tmpint;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][6] |= 0x10; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
                                        case 54:
                                                        ParameterVar.EngineRevolutionChangeable = tmpbool;
                                                        for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][6] |= 0x20; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
                                                        break;
/*                                        case 55:
														ParameterVar.AutoRunVelocityPID_Kp_FastRun = ParameterVar.AutoRunVelocityPID_Kp_FastRunSAVE = tmpint;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][6] |= 0x40; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 56:
														ParameterVar.AutoRunVelocityPID_Ki_FastRun = ParameterVar.AutoRunVelocityPID_Ki_FastRunSAVE = tmpint;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][6] |= 0x80; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 57:
														ParameterVar.AutoRunVelocityPID_Kd_FastRun = ParameterVar.AutoRunVelocityPID_Kd_FastRunSAVE = tmpint;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][7] |= 0x01; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
*/
/*
#ifdef __ENABLE_PROGRAMAUTOMATIC
// Bei verfügbarer Programmatomatikfunktion werden diess Parameter
// aus der Parameterdatei gelesen, andernfalls nicht.
// Weiter oben in dieser Methode muss die Statuspufferenstellung
// mit dieser Einstellung her abgestimmt sein.
										case 58:
														ParameterVar.AutoRunProgramableOn = tmpbool;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][7] |= 0x02; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 59:
														wcsncpy(ParameterVar.AutoRunProgramFileName, tmpstring, LIM_PAR_AutoRunPrgFileNameLength);
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][7] |= 0x04; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
#endif
*/
										case 60:
                                                        ParameterVar.EngineStartStopEnable = tmpbool;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][7] |= 0x08; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;

										case 61:
														ParameterVar.ReverseDepth = tmpbool;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][7] |= 0x10; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;

										case 62:
														ParameterVar.LOGFileTensionUnit = tmpint;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][7] |= 0x20; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;

										case 63:
														ParameterVar.MMSyncForceOn = tmpbool;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][7] |= 0x40; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;

										case 64:
														ParameterVar.MMSyncForceRangeLim = tmpint;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][7] |= 0x80; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 65:
														ParameterVar.LostMotionEnable = tmpbool;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][8] |= 0x01; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 66:
														ParameterVar.WinchBrakeInternAuto = tmpbool;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][8] |= 0x02; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 67:
														ParameterVar.VelocityDimensionUnit = tmpint;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][8] |= 0x04; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 68:
														ParameterVar.ControllerErrorLactive = tmpbool;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][8] |= 0x08; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 69:
														ParameterVar.BrakeOffEnable = tmpbool;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][8] |= 0x10; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 70:
														ParameterVar.ControllerErrorMonitOnAtSM = tmpbool;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][8] |= 0x20; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 71:
														ParameterVar.LineLubAvailable = tmpbool;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][8] |= 0x40; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 72:
														ParameterVar.LineLubPermanentOperation = tmpbool;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][8] |= 0x80; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										case 73:
														CheckParam_LineLubIntensity(tmpint, &tmpint);
														ParameterVar.LineLubIntensity = tmpint;
														for (int j = 0; j < _WinchCnt; ++j) ParameterReady[j][9] |= 0x01; // Nicht windenabhängig -> alle Dimmensionen gleich füllen
														break;
										default:
														break;                  // Unbekannt -> Abbruch (tritt das auf, dann liegt Programmierfehler vor)
                                };
                        // -----------------------------
                        };
                };
        };
        }
        catch(...){}; // Einfach abbrechen

        // ----------------------------- Auswertung Vollständigkeit
        // Vergleichen, ob alles erwartete eingelesen
        for (int i = 0; i < BufSize_ParameterReady; ++i)
                for (int j = 0; j < _WinchCnt; ++j)
                        if (ParameterReady[j][i] != 0xFF)
                        {
                                CloseFile();
                                return _BAD;
                        };
        // -----------------------------


        CloseFile();
        FileBackup();                                                           // Nach erfolgreichem Einlesen, ist klar, dass es sich um eine gültige Parameterdatei handelt -> diese kann gesichert werden
        return _GOOD;
};

//---------------------------------------------------------------------------
// DEPENDENT ON PARAMETER
// TODO Kopie der momentanen Parameterdatei
// Schreibt Daten in die Parameter-Datei
bool WCVarControl::WriteData()
{
//    JMGS_UnicodeCoder UC;
    char Hstring[5];

        if (OpenFile(false) == _BAD) return _BAD;

        // 1x Schreiben des Dateinamens, Versionsnummer, Compilierungsdatum und Nummer der Compilierung pro Datum
        OuFi << __PROGNAME << " " << __VERSION << " " << __DATEOFCOMPIL << " " << __NUMBERPERDATE << endl << endl;

        // 2x Hinweiszeile
        OuFi << COMMENT_PF_ENTRY001 << endl;
        OuFi << COMMENT_PF_ENTRY002 << endl << endl;

        // Parameter-Werte
        OuFi << COMMENT_PF_ENTRY003 << " " << ParameterVar.WinchNo << endl;
//        OuFi << COMMENT_PF_ENTRY049 << " " << ParameterVar.WinchCnt << endl;
        OuFi << COMMENT_PF_ENTRY005 << " " << ParameterVar.UnloadCableStretch << endl;
        OuFi << COMMENT_PF_ENTRY006 << " " << ParameterVar.MinDepth << endl;
        OuFi << COMMENT_PF_ENTRY007 << " " << ParameterVar.MaxDepth << endl;
//        OuFi << COMMENT_PF_ENTRY008 << " " << ParameterVar.StartDepth << endl;
        OuFi << COMMENT_PF_ENTRY009 << " " << ParameterVar.Velocity << endl;
		OuFi << COMMENT_PF_ENTRY010 << " " << ParameterVar.Direction << endl;
		OuFi << COMMENT_PF_ENTRY061 << " " << ParameterVar.ReverseDepth << endl;
		OuFi << COMMENT_PF_ENTRY063 << " " << ParameterVar.MMSyncForceOn << endl;
		OuFi << COMMENT_PF_ENTRY064 << " " << ParameterVar.MMSyncForceRangeLim << endl;
        OuFi << COMMENT_PF_ENTRY066 << " " << ParameterVar.WinchBrakeInternAuto << endl;
		OuFi << COMMENT_PF_ENTRY054 << " " << ParameterVar.EngineRevolutionChangeable << endl;
		OuFi << COMMENT_PF_ENTRY060 << " " << ParameterVar.EngineStartStopEnable << endl;
		OuFi << COMMENT_PF_ENTRY065 << " " << ParameterVar.LostMotionEnable << endl;
		OuFi << COMMENT_PF_ENTRY069 << " " << ParameterVar.BrakeOffEnable << endl;
		OuFi << COMMENT_PF_ENTRY068 << " " << ParameterVar.ControllerErrorLactive << endl;
		OuFi << COMMENT_PF_ENTRY070 << " " << ParameterVar.ControllerErrorMonitOnAtSM << endl;
		OuFi << COMMENT_PF_ENTRY027 << " " << ParameterVar.FormatCMImpulse << endl;
//		OuFi << COMMENT_PF_ENTRY024 << " " << ParameterVar.CalcMaxTractionDepthStep << endl;
		OuFi << COMMENT_PF_ENTRY025 << " " << ParameterVar.UnloadCableBeginLenght << endl;
		OuFi << COMMENT_PF_ENTRY026 << " " << ParameterVar.UnloadCableEndLenght << endl;
		OuFi << COMMENT_PF_ENTRY029 << " " << ParameterVar.TractionViewDimensionUnit << endl;
		OuFi << COMMENT_PF_ENTRY067 << " " << ParameterVar.VelocityDimensionUnit << endl;
		OuFi << COMMENT_PF_ENTRY062 << " " << ParameterVar.LOGFileTensionUnit << endl;
		OuFi << COMMENT_PF_ENTRY030 << " " << ParameterVar.LOGFileType << endl;
		OuFi << COMMENT_PF_ENTRY053 << " " << ParameterVar.SpecialSignalMode << endl;
		OuFi << COMMENT_PF_ENTRY071 << " " << ParameterVar.LineLubAvailable << endl;
		OuFi << COMMENT_PF_ENTRY072 << " " << ParameterVar.LineLubPermanentOperation << endl;
		OuFi << COMMENT_PF_ENTRY073 << " " << ParameterVar.LineLubIntensity << endl;

//		OuFi << COMMENT_PF_ENTRY058 << " " << ParameterVar.AutoRunProgramableOn << endl;

//		UC.SetUnicodeFormat("UTF-8");
//		OuFi << COMMENT_PF_ENTRY059 << " ";
//        for (int i = 0; i <= LIM_PAR_AutoRunPrgFileNameLength; ++i) {
//            if (ParameterVar.AutoRunProgramFileName[i] == 0) break;
//            if (UC.EncodeToUTF8(ParameterVar.AutoRunProgramFileName[i], Hstring)) OuFi << Hstring;
//            else break;
//        }
//        OuFi << endl;

//		OuFi << COMMENT_PF_ENTRY037 << " " << ParameterVar.UnloadCable_PID_TD_SAVE << endl;   // gesonderte Werte es gibt hier einen zum Speichern und einen zum Arbeiten
//		OuFi << COMMENT_PF_ENTRY038 << " " << ParameterVar.UnloadCable_PID_TI_SAVE << endl;   // gesonderte Werte es gibt hier einen zum Speichern und einen zum Arbeiten
//		OuFi << COMMENT_PF_ENTRY039 << " " << ParameterVar.UnloadCable_PID_KR_SAVE << endl;   // gesonderte Werte es gibt hier einen zum Speichern und einen zum Arbeiten
//		OuFi << COMMENT_PF_ENTRY040 << " " << ParameterVar.UnloadCable_PID_TD_2_SAVE << endl; // gesonderte Werte es gibt hier einen zum Speichern und einen zum Arbeiten
//		OuFi << COMMENT_PF_ENTRY041 << " " << ParameterVar.UnloadCable_PID_TI_2_SAVE << endl; // gesonderte Werte es gibt hier einen zum Speichern und einen zum Arbeiten
//		OuFi << COMMENT_PF_ENTRY042 << " " << ParameterVar.UnloadCable_PID_KR_2_SAVE << endl; // gesonderte Werte es gibt hier einen zum Speichern und einen zum Arbeiten
		OuFi << COMMENT_PF_ENTRY043 << " " << ParameterVar.CalibTSensorDepthCalc_CaractSlope << endl;
		OuFi << COMMENT_PF_ENTRY044 << " " << ParameterVar.CalibTSensorDepthCalc_CaractZero << endl;
		OuFi << COMMENT_PF_ENTRY045 << " " << ParameterVar.CalibTSensorA_CaractSlope << endl;
		OuFi << COMMENT_PF_ENTRY046 << " " << ParameterVar.CalibTSensorA_CaractZero << endl;
		OuFi << COMMENT_PF_ENTRY047 << " " << ParameterVar.CalibTSensorB_CaractSlope << endl;
		OuFi << COMMENT_PF_ENTRY048 << " " << ParameterVar.CalibTSensorB_CaractZero << endl;
		for (unsigned int i = 0; i < _WinchCnt; ++i)
		{
				OuFi << COMMENT_PF_ENTRY012 << " " << i << CONST__SEPARATOR01 << ParameterVar.MinTraction[i] << endl;
				OuFi << COMMENT_PF_ENTRY013 << " " << i << CONST__SEPARATOR01 << ParameterVar.MaxTraction[i] << endl;
				OuFi << COMMENT_PF_ENTRY014 << " " << i << CONST__SEPARATOR01 << ParameterVar.RelTensionLimitLower[i] << endl;
				OuFi << COMMENT_PF_ENTRY028 << " " << i << CONST__SEPARATOR01 << ParameterVar.RelTensionLimitUpper[i] << endl;
				OuFi << COMMENT_PF_ENTRY015 << " " << i << CONST__SEPARATOR01 << ParameterVar.UnloadCableTraction[i] << endl;
				OuFi << COMMENT_PF_ENTRY016 << " " << i << CONST__SEPARATOR01 << ParameterVar.UnloadCableStartTraction[i] << endl;
				OuFi << COMMENT_PF_ENTRY017 << " " << i << CONST__SEPARATOR01 << ParameterVar.UnloadCableEndTraction[i] << endl;
//				OuFi << COMMENT_PF_ENTRY018 << " " << i << CONST__SEPARATOR01 << ParameterVar.MaxTimeDifference[i] << endl;
				OuFi << COMMENT_PF_ENTRY020 << " " << i << CONST__SEPARATOR01 << ParameterVar.CalFactDepth[i] << endl;
				OuFi << COMMENT_PF_ENTRY021 << " " << i << CONST__SEPARATOR01 << ParameterVar.CalibTractionX[i] << endl;
				OuFi << COMMENT_PF_ENTRY022 << " " << i << CONST__SEPARATOR01 << ParameterVar.CalibTractionY[i] << endl;
				OuFi << COMMENT_PF_ENTRY023 << " " << i << CONST__SEPARATOR01 << ParameterVar.CalibTractionZero[i] << endl;
		};

		if (OuFi.fail())
		{
                CloseFile();
                ApplyTempDataFile(_BAD);                                        // Temporäre Datei wieder löschen
                return _BAD;
        };

        CloseFile();
        if (ApplyTempDataFile(_GOOD) == _BAD) return _BAD;                      // Umbenennen fehlgeschlagen ?

        SavableParamChanged = false;											// Speicherflag zurücksetzen
		return _GOOD;
};

// schreibt die Speziellen Parameter-Daten (für Entwicklung) in eine separate Parameter-Datei
bool WCVarControl::WriteInternalData()
{
        if (OpenInternalFile(false) == _BAD) return _BAD;

        // 1x Schreiben des Dateinamens, Versionsnummer, Compilierungsdatum und Nummer der Compilierung pro Datum
        InternalOuFi << __PROGNAME << " " << __VERSION << " " << __DATEOFCOMPIL << " " << __NUMBERPERDATE << endl << endl;

        // 3x Hinweiszeile
        InternalOuFi << COMMENT_IPF_ENTRY003 << endl << endl;
        InternalOuFi << COMMENT_IPF_ENTRY001 << endl;
        InternalOuFi << COMMENT_IPF_ENTRY002 << endl << endl;

        // Parameter-Werte
        if (InternalOuFi.fail())
        {
                CloseFile();
                return _BAD;
        };

        CloseFile();
        return _GOOD;
};

// Kopiert die Parameterdatei in eine Sicherungsdatei
// Methode ist relativ autark, könnte von anderen Programmteilen aufgerufen werden
bool WCVarControl::FileBackup()
{
        ifstream Source;
        ofstream Destination;
        char HVString[200];

        if (BackupDataFileName == 0) return false;

        // Öffnen Quelle
        Source.open(DataFileName);
        if (!Source.is_open()) return false;

        // Löschen einer evtl. vorhandenen Zieldatei (Dadurch Nutzen des Speichermanagements eines möglichen Flash-Speichers z.B. CF-Card bei neu erstellen einer Datei)
        remove(BackupDataFileName);

        // Öffnen Ziel
        Destination.open(BackupDataFileName);
        if (!Destination.is_open())
        {
                Source.close();                                                 // Aufräumen
                return false;
        };

        // Kopieren
        while (!Source.fail())
        {
                Source.getline(HVString, 200);
                if (Source.eof()) break;
                Destination << HVString << endl;
        };
        return true;
};

// Wendet die temporäre Datendatei bei Erfolg an (Löschen der alten
// Parameterdatei und umbenennen der temporären Datei),
// oder entfernt sie bei Fehler
bool WCVarControl::ApplyTempDataFile(bool OK)
{
        if (OK)
        {
                if (DataFileName != 0) remove(DataFileName);
                if (rename(TempDataFileName, DataFileName) == 0) return _GOOD;
                else return _BAD;
        }else
        {
                if (TempDataFileName != 0) remove(TempDataFileName);
        };
        return _GOOD;
};

// DEPENDENT ON PARAMETER
// Parameter/Variablen bearbeiten
//===========================================================================
// ControlVar
// Entwicklermodus ein/aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_DevelOn()
{
        ControlVar.devel = _ON;
        return ControlVar.devel;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_DevelOff()
{
        ControlVar.devel = _OFF;
        return ControlVar.devel;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::devel()
{
        return ControlVar.devel;
};
//---------------------------------------------------------------------------

// Automatikbetrieb
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_AutoRunOn()
{
//        if (!AutoRunOn_Locked) ControlVar.AutoRunOn = _ON;
        ControlVar.AutoRunOn = _ON;
        return ControlVar.AutoRunOn;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_AutoRunOff()
{
//        if (!AutoRunOn_Locked) ControlVar.AutoRunOn = _OFF;
        ControlVar.AutoRunOn = _OFF;
        return ControlVar.AutoRunOn;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_AutoRunOn()
{
        return ControlVar.AutoRunOn;
};
//---------------------------------------------------------------------------

// Winde
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_WinchOn()
{
//        if (!WinchWork_Locked) ControlVar.WinchWork = _ON;
        ControlVar.WinchWork = _ON;
        return ControlVar.WinchWork;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_WinchOff()
{
//        if (!WinchWork_Locked) ControlVar.WinchWork = _OFF;
        ControlVar.WinchWork = _OFF;
        return ControlVar.WinchWork;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_WinchWork()
{
        return ControlVar.WinchWork;
};

//---------------------------------------------------------------------------
// momentane Zugkraft in Abhängigkeit der Teufe berechnen
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalcCurrTractionsOn()
{
//        if (!CalcCurrTractions_Locked) ParameterVar.CalcCurrTractions = _ON;
        ControlVar.CalcCurrTractions = _ON;
        return ControlVar.CalcCurrTractions;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalcCurrTractionsOff()
{
//        if (!CalcCurrTractions_Locked) ParameterVar.CalcCurrTractions = _OFF;
        ControlVar.CalcCurrTractions = _OFF;
        return ControlVar.CalcCurrTractions;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_CalcCurrTractions()
{
        return ControlVar.CalcCurrTractions;
};

//---------------------------------------------------------------------------
// momentane Zugkraft in Abhängigkeit der Teufe berechnen tatsächlich aktiv
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalcCurrTractionsActiveOn()
{
    ControlVar.CalcCurrTractionsActive = _ON;
    return ControlVar.CalcCurrTractionsActive;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalcCurrTractionsActiveOff()
{
    ControlVar.CalcCurrTractionsActive = _OFF;
    return ControlVar.CalcCurrTractionsActive;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_CalcCurrTractionsActive()
{
    return ControlVar.CalcCurrTractionsActive;
};


//---------------------------------------------------------------------------
// Verzögerung des Ausschaltens vom Steuergerät bei Zugkraft-/ Teufenüberschreitung
// (gleichzeitig Haltedauer der Mittenspannung durch den STR)
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_ThresholdExceedenceStopDelayOn()
{
        ControlVar.ThresholdExceedenceStopDelayOn = _ON;
        return ControlVar.ThresholdExceedenceStopDelayOn;
};

bool WCVarControl::SetControl_ThresholdExceedenceStopDelayOff()
{
        ControlVar.ThresholdExceedenceStopDelayOn = _OFF;
        return ControlVar.ThresholdExceedenceStopDelayOn;
};

bool WCVarControl::GetControl_ThresholdExceedenceStopDelay()
{
        return ControlVar.ThresholdExceedenceStopDelayOn;
};

//---------------------------------------------------------------------------

// Freigabe nach Grenzwertüberschreitung
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_ClearAfterThresholdDepthOn()
{
//        if (!ClearAfterThreshold_Locked) ControlVar.ClearAfterThresholdDepth = _ON;
        ControlVar.ClearAfterThresholdDepth = _ON;
        return ControlVar.ClearAfterThresholdDepth;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_ClearAfterThresholdDepthOff()
{
//        if (!ClearAfterThreshold_Locked) ControlVar.ClearAfterThresholdDepth = _OFF;
        ControlVar.ClearAfterThresholdDepth = _OFF;
        return ControlVar.ClearAfterThresholdDepth;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_ClearAfterThresholdDepth()
{
        return ControlVar.ClearAfterThresholdDepth;
};
//---------------------------------------------------------------------------

// Freigabe nach Grenzwertüberschreitung
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_ClearAfterThresholdTractionOn()
{
//        if (!ClearAfterThreshold_Locked) ControlVar.ClearAfterThresholdTraction = _ON;
        ControlVar.ClearAfterThresholdTraction = _ON;
        return ControlVar.ClearAfterThresholdTraction;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_ClearAfterThresholdTractionOff()
{
//        if (!ClearAfterThreshold_Locked) ControlVar.ClearAfterThresholdTraction = _OFF;
        ControlVar.ClearAfterThresholdTraction = _OFF;
        return ControlVar.ClearAfterThresholdTraction;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_ClearAfterThresholdTraction()
{
        return ControlVar.ClearAfterThresholdTraction;
};
//---------------------------------------------------------------------------
// Bestätigen von ausgewählten durch Sondersignale ausgelösten Fehlern
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_ResetSpecialSignalError1FlagOn()
{
    ControlVar.ResetSpecialSignalError1Flag = _ON;
    return ControlVar.ResetSpecialSignalError1Flag;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_ResetSpecialSignalError1FlagOff()
{
    ControlVar.ResetSpecialSignalError1Flag = _OFF;
    return ControlVar.ResetSpecialSignalError1Flag;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_ResetSpecialSignalError1Flag()
{
    return ControlVar.ResetSpecialSignalError1Flag;
};

//---------------------------------------------------------------------------
// Bestätigen von ausgewählten durch Sondersignale ausgelösten Fehlern
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_ResetSpecialSignalError2FlagOn()
{
    ControlVar.ResetSpecialSignalError2Flag = _ON;
    return ControlVar.ResetSpecialSignalError2Flag;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_ResetSpecialSignalError2FlagOff()
{
    ControlVar.ResetSpecialSignalError2Flag = _OFF;
    return ControlVar.ResetSpecialSignalError2Flag;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_ResetSpecialSignalError2Flag()
{
    return ControlVar.ResetSpecialSignalError2Flag;
};

//---------------------------------------------------------------------------

// Schnelle/Langsame Fahrt
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_FastRunOn()
{
//        if (!FastRun_Locked) ControlVar.FastRun = _ON;
        ControlVar.FastRun = _ON;
        return ControlVar.FastRun;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_FastRunOff()
{
//        if (!FastRun_Locked) ControlVar.FastRun = _OFF;
        ControlVar.FastRun = _OFF;
        return ControlVar.FastRun;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_FastRun()
{
        return ControlVar.FastRun;
};
//---------------------------------------------------------------------------

// "Motor ein" - Signal ein
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_EngineStartOn()
{
//        if (!EngineStart_Locked) ControlVar.EngineStart = _ON;
        ControlVar.EngineStart = _ON;
        return ControlVar.EngineStart;
};

// "Motor ein" - Signal aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_EngineStartOff()
{
//        if (!EngineStart_Locked) ControlVar.EngineStart = _OFF;
        ControlVar.EngineStart = _OFF;
        return ControlVar.EngineStart;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_EngineStart()
{
        return ControlVar.EngineStart;
};
//---------------------------------------------------------------------------

// "Motor aus" - Signal ein
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_EngineStopOn()
{
//        if (!EngineStop_Locked) ControlVar.EngineStop = _ON;
        ControlVar.EngineStop = _ON;
        return ControlVar.EngineStop;
};

// "Motor aus" - Signal aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_EngineStopOff()
{
//        if (!EngineStop_Locked) ControlVar.EngineStop = _OFF;
        ControlVar.EngineStop = _OFF;
        return ControlVar.EngineStop;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_EngineStop()
{
        return ControlVar.EngineStop;
};

//---------------------------------------------------------------------------

// Gibt an, ob die Motordrehzahl reduziert oder erhöht werden soll
int WCVarControl::SetControl_EngineRevolutionUp()
{
        ControlVar.EngineRevolutionUpDown = 1;
        return ControlVar.EngineRevolutionUpDown;
};
int WCVarControl::SetControl_EngineRevolutionDown()
{
        ControlVar.EngineRevolutionUpDown = -1;
        return ControlVar.EngineRevolutionUpDown;
};
int WCVarControl::SetControl_EngineRevolutionHold()
{
        ControlVar.EngineRevolutionUpDown = 0;
        return ControlVar.EngineRevolutionUpDown;
};
int WCVarControl::GetControl_EngineRevolutionUpDown()
{
        return ControlVar.EngineRevolutionUpDown;
};

//---------------------------------------------------------------------------
// Gibt an, ob die Motordrehzahl reduziert oder erhöht werden soll
// Automatik, nur für programminterne Steuerung, nicht Anwendergesteuert
int WCVarControl::SetControl_EngineRevolutionAutoUp()
{
        ControlVar.EngineRevolutionAutoUpDown = 1;
        return ControlVar.EngineRevolutionAutoUpDown;
};

int WCVarControl::SetControl_EngineRevolutionAutoDown()
{
        ControlVar.EngineRevolutionAutoUpDown = -1;
        return ControlVar.EngineRevolutionAutoUpDown;
};

int WCVarControl::SetControl_EngineRevolutionAutoHold()
{
        ControlVar.EngineRevolutionAutoUpDown = 0;
		return ControlVar.EngineRevolutionAutoUpDown;
};

int WCVarControl::GetControl_EngineRevolutionAutoUpDown()
{
		return ControlVar.EngineRevolutionAutoUpDown;
};

//---------------------------------------------------------------------------
// Hallo über das Signal WinchClear zur externen Steuerelektronik
// (forciertes kurzes Auslösen des Signals)
bool WCVarControl::SetControl_Hello_WinchClearOn()
{
	ControlVar.Hello_WinchClear = _ON;
	return ControlVar.Hello_WinchClear;
};

bool WCVarControl::SetControl_Hello_WinchClearOff()
{
	ControlVar.Hello_WinchClear = _OFF;
	return ControlVar.Hello_WinchClear;
};

bool WCVarControl::GetControl_Hello_WinchClear()
{
	return ControlVar.Hello_WinchClear;
};

//---------------------------------------------------------------------------
// Hydraulische Bremse aktiv (true) / inaktiv (false)
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_HydraulicBrakeOn()
{
	ParameterVar.HydraulicBrakeOn = _ON;
	return ParameterVar.HydraulicBrakeOn;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_HydraulicBrakeOff()
{
	ParameterVar.HydraulicBrakeOn = _OFF;
	return ParameterVar.HydraulicBrakeOn;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetParam_HydraulicBrake()
{
	return ParameterVar.HydraulicBrakeOn;
}

//---------------------------------------------------------------------------
// Magnetmarke ein/aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_MagneticMarkOn()
{
        ControlVar.MagneticMarkOn = _ON;
        return ControlVar.MagneticMarkOn;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_MagneticMarkOff()
{
        ControlVar.MagneticMarkOn = _OFF;
        return ControlVar.MagneticMarkOn;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_MagneticMarkOn()
{
		return ControlVar.MagneticMarkOn;
};

//---------------------------------------------------------------------------
// MM-Zwangssynchronisation aktiv (true) oder inaktiv (false)
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_MMSyncForceActiveOn()
{
    ControlVar.MMSyncForceActive = _ON;
	return ControlVar.MMSyncForceActive;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_MMSyncForceActiveOff()
{
	ControlVar.MMSyncForceActive = _OFF;
	return ControlVar.MMSyncForceActive;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_MMSyncForceActiveOn()
{
	return ControlVar.MMSyncForceActive;
}

//---------------------------------------------------------------------------
// MM Teufenzähler für MM-Zwangssynchronisation
// Rückgabewert: der gesetzte Wert
int	WCVarControl::SetControl_MMSyncForceCounter(int Value)
{
    ControlVar.MMSyncForceCounter = Value;
	return ControlVar.MMSyncForceCounter;
}

// Rückgabewert: der gesetzte Wert
int	WCVarControl::GetControl_MMSyncForceCounter()
{
	return ControlVar.MMSyncForceCounter;
}

//---------------------------------------------------------------------------
// Kabel entladen ein/aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_UnloadCableOn()
{
//        if (!UnloadCable_Locked) ControlVar.UnloadCable = _ON;
		ControlVar.UnloadCable = _ON;
		return ControlVar.UnloadCable;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_UnloadCableOff()
{
//        if (!UnloadCable_Locked) ControlVar.UnloadCable = _OFF;
		ControlVar.UnloadCable = _OFF;
        return ControlVar.UnloadCable;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_UnloadCable()
{
        return ControlVar.UnloadCable;
};

//---------------------------------------------------------------------------
// Passiv Kabel entladen ein/aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_UnloadCableActiveOn()
{
//        if (!UnloadCableActive_Locked) ControlVar.UnloadCableActive = _ON;
		ControlVar.UnloadCableActive = _ON;
        return ControlVar.UnloadCableActive;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_UnloadCableActiveOff()
{
//        if (!UnloadCableActive_Locked) ControlVar.UnloadCableActive = _OFF;
		ControlVar.UnloadCableActive = _OFF;
        return ControlVar.UnloadCableActive;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_UnloadCableActive()
{
        return ControlVar.UnloadCableActive;
};

//---------------------------------------------------------------------------
// Aktiv Kabel entladen ein/aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_UnloadCablePassiveOn()
{
//        if (!UnloadCablePassive_Locked) ControlVar.UnloadCablePassive = _ON;
        ControlVar.UnloadCablePassive = _ON;
        return ControlVar.UnloadCablePassive;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_UnloadCablePassiveOff()
{
//        if (!UnloadCablePassive_Locked) ControlVar.UnloadCablePassive = _OFF;
        ControlVar.UnloadCablePassive = _OFF;
        return ControlVar.UnloadCablePassive;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_UnloadCablePassive()
{
        return ControlVar.UnloadCablePassive;
};

//---------------------------------------------------------------------------
// Kalibrieren ein/aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalibrateOn()
{
//        if (!CalibrateOn_Locked) ControlVar.CalibrateOn = _ON;
        ControlVar.CalibrateOn = _ON;
        return ControlVar.CalibrateOn;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalibrateOff()
{
//        if (!CalibrateOn_Locked) ControlVar.CalibrateOn = _OFF;
        ControlVar.CalibrateOn = _OFF;
        return ControlVar.CalibrateOn;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_CalibrateOn()
{
        return ControlVar.CalibrateOn;
};
//---------------------------------------------------------------------------

// Kalibrieren des Teufenrechners
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalibrateDepthOn()
{
//        if (!CalibrateDepth_Locked) ControlVar.CalibrateDepth = _ON;
        ControlVar.CalibrateDepth = _ON;
        return ControlVar.CalibrateDepth;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalibrateDepthOff()
{
//        if (!CalibrateDepth_Locked) ControlVar.CalibrateDepth = _OFF;
        ControlVar.CalibrateDepth = _OFF;
        return ControlVar.CalibrateDepth;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_CalibrateDepth()
{
        return ControlVar.CalibrateDepth;
};
//---------------------------------------------------------------------------

// Kalibrieren des Moduls zur Zugkraftüberwachung
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalibrateTractionOn()
{
//        if (!CalibrateTraction_Locked) ControlVar.CalibrateTraction = _ON;
        ControlVar.CalibrateTraction = _ON;
        return ControlVar.CalibrateTraction;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalibrateTractionOff()
{
//        if (!CalibrateTraction_Locked) ControlVar.CalibrateTraction = _OFF;
        ControlVar.CalibrateTraction = _OFF;
        return ControlVar.CalibrateTraction;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_CalibrateTraction()
{
        return ControlVar.CalibrateTraction;
};

// Gibt an, ob die Software-Version des Steuergeräts ausgelesen werden soll
// Software auslesen ein
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_GetCtlUnitSWVersionOn() {
	ControlVar.GetCtlUnitSWVersion = _ON;
	return ControlVar.GetCtlUnitSWVersion;
}

// Software auslesen aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_GetCtlUnitSWVersionOff() {
	ControlVar.GetCtlUnitSWVersion = _OFF;
	return ControlVar.GetCtlUnitSWVersion;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_GetCtlUnitSWVersion() {
	return ControlVar.GetCtlUnitSWVersion;
}


//---------------------------------------------------------------------------
// Gibt an, ob die Software-Version des Teufenrechners ausgelesen werden soll
// TR-Software auslesen ein
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_GetDepthCalcSWVersionOn()
{
		ControlVar.GetDepthCalcSWVersion = _ON;
		return ControlVar.GetDepthCalcSWVersion;
};

// TR-Software auslesen aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_GetDepthCalcSWVersionOff()
{
		ControlVar.GetDepthCalcSWVersion = _OFF;
		return ControlVar.GetDepthCalcSWVersion;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_GetDepthCalcSWVersion()
{
		return ControlVar.GetDepthCalcSWVersion;
};

//---------------------------------------------------------------------------
// Freilauf-Impuls ein/aus
bool WCVarControl::SetControl_LostMotionImpulseOn()
{

//        if (!LostMotionImpulse_Locked) ControlVar.LostMotionImpulse = _ON;
        ControlVar.LostMotionImpulse = _ON;
        return ControlVar.LostMotionImpulse;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_LostMotionImpulseOff()
{
//        if (!LostMotionImpulse_Locked) ControlVar.LostMotionImpulse = _OFF;
        ControlVar.LostMotionImpulse = _OFF;
        return ControlVar.LostMotionImpulse;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_LostMotionImpulse()
{
        return ControlVar.LostMotionImpulse;
};

//---------------------------------------------------------------------------
// Startteufe im Steuergerät setzen ein/aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SetDepth_On()
{
		ControlVar.SetDepth = _ON;
		return ControlVar.SetDepth;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SetDepth_Off()
{
		ControlVar.SetDepth = _OFF;
		return ControlVar.SetDepth;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_SetDepth()
{
		return ControlVar.SetDepth;
};

//---------------------------------------------------------------------------
// Startteufe im Teufenrechner setzen ein/aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SetDepth_DC_On()
{
//        if (!SetDepth_Locked) ControlVar.SetDepth = _ON;
		ControlVar.SetDepth_DC = _ON;
		return ControlVar.SetDepth_DC;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SetDepth_DC_Off()
{
//        if (!SetDepth_Locked) ControlVar.SetDepth = _OFF;
		ControlVar.SetDepth_DC = _OFF;
		return ControlVar.SetDepth_DC;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_SetDepth_DC()
{
		return ControlVar.SetDepth_DC;
};

//---------------------------------------------------------------------------
// Parameter an Teufenrechner übergeben ein/aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SetDepthCalcParamOn()
{
//        if (!SetDepthCalcParam_Locked) ControlVar.SetDepthCalcParam = _ON;
        ControlVar.SetDepthCalcParam = _ON;
        return ControlVar.SetDepthCalcParam;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SetDepthCalcParamOff()
{
//        if (!SetDepthCalcParam_Locked) ControlVar.SetDepthCalcParam = _OFF;
        ControlVar.SetDepthCalcParam = _OFF;
        return ControlVar.SetDepthCalcParam;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_SetDepthCalcParam()
{
        return ControlVar.SetDepthCalcParam;
};

//---------------------------------------------------------------------------
// Kalibrierfaktor im Steuergerät setzen ein/aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SetDepthCalFactor_On()
{
		ControlVar.SetDepthCalFactor = _ON;
		return ControlVar.SetDepthCalFactor;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SetDepthCalFactor_Off()
{
		ControlVar.SetDepthCalFactor = _OFF;
		return ControlVar.SetDepthCalFactor;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_SetDepthCalFactor()
{
		return ControlVar.SetDepthCalFactor;
};

//---------------------------------------------------------------------------
// Eichfaktor im Teufenrechner setzen ein/aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SetDepthCalFactor_DC_On()
{
//        if (!SetDepthCalFactor_Locked) ControlVar.SetDepthCalFactor = _ON;
		ControlVar.SetDepthCalFactor_DC = _ON;
		return ControlVar.SetDepthCalFactor_DC;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SetDepthCalFactor_DC_Off()
{
//        if (!SetDepthCalFactor_Locked) ControlVar.SetDepthCalFactor = _OFF;
		ControlVar.SetDepthCalFactor_DC = _OFF;
		return ControlVar.SetDepthCalFactor_DC;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_SetDepthCalFactor_DC()
{
		return ControlVar.SetDepthCalFactor_DC;
};

//---------------------------------------------------------------------------
// cm-Impuls-Format im Teufenrechner setzen ein/aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SetFormatCMImpulseOn()
{
    ControlVar.SetFormatCMImpulse = _ON;
    return ControlVar.SetFormatCMImpulse;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SetFormatCMImpulseOff()
{
    ControlVar.SetFormatCMImpulse = _OFF;
    return ControlVar.SetFormatCMImpulse;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_SetFormatCMImpulse()
{
    return ControlVar.SetFormatCMImpulse;
}

//---------------------------------------------------------------------------
// Magnetmarke im Teufenrechner setzen ein/aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SetMagneticMarkOn()
{
    ControlVar.SetMagneticMark = _ON;
    return ControlVar.SetMagneticMark;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SetMagneticMarkOff()
{
    ControlVar.SetMagneticMark = _OFF;
    return ControlVar.SetMagneticMark;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_SetMagneticMark()
{
    return ControlVar.SetMagneticMark;
}

//---------------------------------------------------------------------------
// Bremse der Winde lösen ein/aus
// Rückgabewert: der gesetzte Wert
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_WinchBrakeOff_On()
{
        ControlVar.WinchBrakeOff = _ON;
        return ControlVar.WinchBrakeOff;
};

bool WCVarControl::SetControl_WinchBrakeOff_Off()
{
        ControlVar.WinchBrakeOff = _OFF;
        return ControlVar.WinchBrakeOff;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_WinchBrakeOff()
{
        return ControlVar.WinchBrakeOff;
};

//---------------------------------------------------------------------------
// Impuls zum Steuern "Bremse der Winde lösen"
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_WinchBrakeOffImpulse_On()
{
		ControlVar.WinchBrakeOffImpulse = _ON;
        return ControlVar.WinchBrakeOffImpulse;
};

bool WCVarControl::SetControl_WinchBrakeOffImpulse_Off()
{
        ControlVar.WinchBrakeOffImpulse = _OFF;
        return ControlVar.WinchBrakeOffImpulse;
};

bool WCVarControl::GetControl_WinchBrakeOffImpulse()
{
        return ControlVar.WinchBrakeOffImpulse;
};

//---------------------------------------------------------------------------
// Signal-Horn
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SignalHornOn()
{
        ControlVar.SignalHornOn = _ON;
        return ControlVar.SignalHornOn;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SignalHornOff()
{
        ControlVar.SignalHornOn = _OFF;
        return ControlVar.SignalHornOn;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_SignalHornOn()
{
        return ControlVar.SignalHornOn;
};

//---------------------------------------------------------------------------
// Spannungswert 1 für Kalibrierung Zugkraftmesser
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalibTension_GetVal1On()
{
//        if (!CalibTractionGetValue1_Locked) ControlVar.CalibTractionGetValue1 = _ON;
		ControlVar.CalibTension_GetVal1 = _ON;
		return ControlVar.CalibTension_GetVal1;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalibTension_GetVal1Off()
{
//        if (!CalibTractionGetValue1_Locked) ControlVar.CalibTractionGetValue1 = _OFF;
		ControlVar.CalibTension_GetVal1 = _OFF;
		return ControlVar.CalibTension_GetVal1;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_CalibTension_GetVal1()
{
		return ControlVar.CalibTension_GetVal1;
};

//---------------------------------------------------------------------------
// Spannungswert 2 für Kalibrierung Zugkraftmesser
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalibTension_GetVal2On()
{
		ControlVar.CalibTension_GetVal2 = _ON;
		return ControlVar.CalibTension_GetVal2;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalibTension_GetVal2Off()
{
		ControlVar.CalibTension_GetVal2 = _OFF;
		return ControlVar.CalibTension_GetVal2;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_CalibTension_GetVal2()
{
        return ControlVar.CalibTension_GetVal2;
};

//---------------------------------------------------------------------------
// Parameter für Zugkraftmesser berechnen
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalibTractionCalculateParamOn()
{
//        if (!CalibTractionCalculateParam_Locked) ControlVar.CalibTractionCalculateParam = _ON;
        ControlVar.CalibTractionCalculateParam = _ON;
        return ControlVar.CalibTractionCalculateParam;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalibTractionCalculateParamOff()
{
//        if (!CalibTractionCalculateParam_Locked) ControlVar.CalibTractionCalculateParam = _OFF;
        ControlVar.CalibTractionCalculateParam = _OFF;
        return ControlVar.CalibTractionCalculateParam;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_CalibTractionCalculateParam()
{
        return ControlVar.CalibTractionCalculateParam;
};

//---------------------------------------------------------------------------
// Parameter für Zugkraftmesser übernehmen
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalibTractionSetParamOn()
{
//        if (!CalibTractionSetParam_Locked) ControlVar.CalibTractionSetParam = _ON;
        ControlVar.CalibTractionSetParam = _ON;
        return ControlVar.CalibTractionSetParam;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalibTractionSetParamOff()
{
//        if (!CalibTractionSetParam_Locked) ControlVar.CalibTractionSetParam = _OFF;
        ControlVar.CalibTractionSetParam = _OFF;
        return ControlVar.CalibTractionSetParam;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_CalibTractionSetParam()
{
        return ControlVar.CalibTractionSetParam;
};

//---------------------------------------------------------------------------
// Nullabgleich Zugkraftmesser
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalibTractionSetZeroOn()
{
//        if (!CalibTractionSetZero_Locked) ControlVar.CalibTractionSetZero = _ON;
        ControlVar.CalibTractionSetZero = _ON;
        return ControlVar.CalibTractionSetZero;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_CalibTractionSetZeroOff()
{
//        if (!CalibTractionSetZero_Locked) ControlVar.CalibTractionSetZero = _OFF;
        ControlVar.CalibTractionSetZero = _OFF;
        return ControlVar.CalibTractionSetZero;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_CalibTractionSetZero()
{
        return ControlVar.CalibTractionSetZero;
};

//---------------------------------------------------------------------------
// Sonderfunktion 1
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SpecialFunc1On(){
	ControlVar.SpecialFunc1 = _ON;
	return ControlVar.SpecialFunc1;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_SpecialFunc1Off(){
	ControlVar.SpecialFunc1 = _OFF;
	return ControlVar.SpecialFunc1;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_SpecialFunc1(){
	return ControlVar.SpecialFunc1;
}

//---------------------------------------------------------------------------
// Stromversorgung STR über Schlüsselschalter/Zündung abgeschaltet
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_PowerFailOn()
{
    ControlVar.PowerFail = _ON;
    return ControlVar.PowerFail;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetControl_PowerFailOff()
{
    ControlVar.PowerFail = _OFF;
    return ControlVar.PowerFail;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetControl_PowerFail()
{
    return ControlVar.PowerFail;
};
//---------------------------------------------------------------------------

//===========================================================================
// StatusInfoVar
// Momentane Teufe
// Value: Angabe in mm
// Rückgabewert: der gesetzte Wert
int  WCVarControl::SetInfo_CurrDepth(int Value)
{
        StatusInfoVar.CurrDepth = Value;
        return StatusInfoVar.CurrDepth;
};

// Rückgabewert: der gesetzte Wert
int  WCVarControl::GetInfo_CurrDepth()
{
        return StatusInfoVar.CurrDepth;
};
//---------------------------------------------------------------------------

// Momentane Fahrtrichtung
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_CurrDirectionUp()
{
        StatusInfoVar.CurrDirection = _UPward;
        return StatusInfoVar.CurrDirection;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_CurrDirectionDown()
{
        StatusInfoVar.CurrDirection = _DOWN;
        return StatusInfoVar.CurrDirection;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetInfo_CurrDirection()
{
        return StatusInfoVar.CurrDirection;
};
//---------------------------------------------------------------------------

// Momentane Sondengeschwindigkeit
// Value: Angabe in m/h
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetInfo_CurrVelocity(int Value)
{
        StatusInfoVar.CurrVelocity = Value;
        return StatusInfoVar.CurrVelocity;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetInfo_CurrVelocity()
{
        return StatusInfoVar.CurrVelocity;
};
//---------------------------------------------------------------------------

// Angezeigte Sondengeschwindigkeit
// Value: Angabe in m/h
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetInfo_AnnouncedVelocity(int Value)
{
        StatusInfoVar.AnnouncedVelocity = Value;
        return StatusInfoVar.AnnouncedVelocity;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetInfo_AnnouncedVelocity()
{
        return StatusInfoVar.AnnouncedVelocity;
};
//---------------------------------------------------------------------------

// Momentane Maximalzugkraft
// Value: Angabe in N
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetInfo_CurrTensionLimit_Upper(int Value)
{
		StatusInfoVar.CurrTensionLimit_Upper = Value;
        return StatusInfoVar.CurrTensionLimit_Upper;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetInfo_CurrTensionLimit_Upper()
{
		return StatusInfoVar.CurrTensionLimit_Upper;
};
//---------------------------------------------------------------------------

// Momentane Minimalzugkraft
// Value: Angabe in N
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetInfo_CurrTensionLimit_Lower(int Value)
{
		StatusInfoVar.CurrTensionLimit_Lower = Value;
		return StatusInfoVar.CurrTensionLimit_Lower;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetInfo_CurrTensionLimit_Lower()
{
		return StatusInfoVar.CurrTensionLimit_Lower;
};
//---------------------------------------------------------------------------

// Momentane Zugkraft
// Value: Angabe in N
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetInfo_CurrTraction(int Value)
{
        StatusInfoVar.CurrTraction = Value;
        return StatusInfoVar.CurrTraction;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetInfo_CurrTraction()
{
        return StatusInfoVar.CurrTraction;
};
//---------------------------------------------------------------------------

// Momentane Zugkraft-Differenz
// Value: Angabe in N
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetInfo_CurrTractionDifference(int Value)
{
        StatusInfoVar.CurrTractionDifference = Value;
        return StatusInfoVar.CurrTractionDifference;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetInfo_CurrTractionDifference()
{
        return StatusInfoVar.CurrTractionDifference;
};
//---------------------------------------------------------------------------

// Momentane Zeit-Differenz
// Value: Angabe in s
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetInfo_CurrTimeDifference(int Value)
{
        StatusInfoVar.CurrTimeDifference = Value;
        return StatusInfoVar.CurrTimeDifference;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetInfo_CurrTimeDifference()
{
        return StatusInfoVar.CurrTimeDifference;
};
//---------------------------------------------------------------------------

// Momentaner Spüannungswert vom Zugkraftmesser
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetInfo_CurrTractionVoltage(int Value)
{
        StatusInfoVar.CurrTractionVoltage = Value;
        return StatusInfoVar.CurrTractionVoltage;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetInfo_CurrTractionVoltage()
{
        return StatusInfoVar.CurrTractionVoltage;
};

//---------------------------------------------------------------------------

// Momentane Fahrtrichtung
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_MagneticMarkOn()
{
        StatusInfoVar.MagneticMark = _ON;
        return StatusInfoVar.MagneticMark;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_MagneticMarkOff()
{
        StatusInfoVar.MagneticMark = _OFF;
        return StatusInfoVar.MagneticMark;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetInfo_MagneticMark()
{
        return StatusInfoVar.MagneticMark;
};

//---------------------------------------------------------------------------
// Auftreten einer Magnetmarke statisch für Darstellung und Protokollierung
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_MagneticMarkStaticOn()
{
        StatusInfoVar.MagneticMarkStatic = _ON;
        return StatusInfoVar.MagneticMarkStatic;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_MagneticMarkStaticOff()
{
        StatusInfoVar.MagneticMarkStatic = _OFF;
        return StatusInfoVar.MagneticMarkStatic;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetInfo_MagneticMarkStatic()
{
        return StatusInfoVar.MagneticMarkStatic;
};

//---------------------------------------------------------------------------
// Auftreten einer Magnetmarke statisch für Protokollierung
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_MagneticMarkStaticLOGOn()
{
        StatusInfoVar.MagneticMarkStaticLOG = _ON;
        return StatusInfoVar.MagneticMarkStaticLOG;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_MagneticMarkStaticLOGOff()
{
        StatusInfoVar.MagneticMarkStaticLOG = _OFF;
        return StatusInfoVar.MagneticMarkStaticLOG;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetInfo_MagneticMarkStaticLOG()
{
        return StatusInfoVar.MagneticMarkStaticLOG;
};
//---------------------------------------------------------------------------
// Direktes Auftreten einer Magnetmarke (nicht nur Trigger, wie "MagneticMark")
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_MagneticMarkDirectOn()
{
        StatusInfoVar.MagneticMarkDirect = _ON;
        return StatusInfoVar.MagneticMarkDirect;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_MagneticMarkDirectOff()
{
        StatusInfoVar.MagneticMarkDirect = _OFF;
        return StatusInfoVar.MagneticMarkDirect;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetInfo_MagneticMarkDirect()
{
        return StatusInfoVar.MagneticMarkDirect;
};

//---------------------------------------------------------------------------
// Inkremental-Geber-Fehler
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_IncEncErrorOn()
{
        StatusInfoVar.IncEncError = _ON;
        return StatusInfoVar.IncEncError;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_IncEncErrorOff()
{
        StatusInfoVar.IncEncError = _OFF;
        return StatusInfoVar.IncEncError;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetInfo_IncEncError()
{
        return StatusInfoVar.IncEncError;
};

//---------------------------------------------------------------------------

// Steuergerät Fehler-Signalisierung
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_ControllerErrorOn()
{
		StatusInfoVar.ControllerError = _ON;
		return StatusInfoVar.ControllerError;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_ControllerErrorOff()
{
		StatusInfoVar.ControllerError = _OFF;
        return StatusInfoVar.ControllerError;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetInfo_ControllerError()
{
        return StatusInfoVar.ControllerError;
};
//---------------------------------------------------------------------------

// Freilauf ein/aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_LostMotionOn()
{
        StatusInfoVar.LostMotion = _ON;
        return StatusInfoVar.LostMotion;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_LostMotionOff()
{
        StatusInfoVar.LostMotion = _OFF;
        return StatusInfoVar.LostMotion;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetInfo_LostMotion()
{
        return StatusInfoVar.LostMotion;
};

//---------------------------------------------------------------------------
// Antriebsmotorkühlmitteltemperatur in °C
// Rückgabewert: der gesetzte Wert
int	WCVarControl::SetInfo_EngineCoolantTemp(int Value) {
	StatusInfoVar.EngineCoolantTemp = Value;
	return StatusInfoVar.EngineCoolantTemp;
}

// Rückgabewert: der gesetzte Wert
int	WCVarControl::GetInfo_EngineCoolantTemp() {
	return StatusInfoVar.EngineCoolantTemp;
}

//---------------------------------------------------------------------------
// Motor ein/aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_EngineOn()
{
		StatusInfoVar.EngineOn = _ON;
		return StatusInfoVar.EngineOn;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetInfo_EngineOff()
{
		StatusInfoVar.EngineOn = _OFF;
		return StatusInfoVar.EngineOn;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetInfo_EngineOn()
{
		return StatusInfoVar.EngineOn;
};

//---------------------------------------------------------------------------
// Antriebsmotordrehzahl
// Rückgabewert: der gesetzte Wert
int	WCVarControl::SetInfo_EngineSpeed(int Value) {
	StatusInfoVar.EngineSpeed = Value;
	return StatusInfoVar.EngineSpeed;
}

// Rückgabewert: der gesetzte Wert
int	WCVarControl::GetInfo_EngineSpeed() {
	return StatusInfoVar.EngineSpeed;
}

//---------------------------------------------------------------------------
// Ölfilter wechseln ein/aus
bool WCVarControl::SetInfo_OilStrainerOn()
{
		StatusInfoVar.OilStrainer = _ON;
		return StatusInfoVar.OilStrainer;
};

bool WCVarControl::SetInfo_OilStrainerOff()
{
		StatusInfoVar.OilStrainer = _OFF;
		return StatusInfoVar.OilStrainer;
};

bool WCVarControl::GetInfo_OilStrainer()
{
		return StatusInfoVar.OilStrainer;
};

//---------------------------------------------------------------------------
// Ölfilter 2 wechseln ein/aus
bool WCVarControl::SetInfo_OilStrainer2On()
{
		StatusInfoVar.OilStrainer2 = _ON;
		return StatusInfoVar.OilStrainer2;
};

bool WCVarControl::SetInfo_OilStrainer2Off()
{
		StatusInfoVar.OilStrainer2 = _OFF;
		return StatusInfoVar.OilStrainer2;
};

bool WCVarControl::GetInfo_OilStrainer2()
{
		return StatusInfoVar.OilStrainer2;
};

//---------------------------------------------------------------------------
// Nebenantrieb ist ein/aus
bool WCVarControl::SetInfo_HydraulicOKOn()
{
		StatusInfoVar.HydraulicOK = _ON;
		return StatusInfoVar.HydraulicOK;
};

bool WCVarControl::SetInfo_HydraulicOKOff()
{
		StatusInfoVar.HydraulicOK = _OFF;
		return StatusInfoVar.HydraulicOK;
};

bool WCVarControl::GetInfo_HydraulicOK()
{
		return StatusInfoVar.HydraulicOK;
};

//---------------------------------------------------------------------------
// Aktueller Arbeitsdruck der Hydraulik
int WCVarControl::SetInfo_HydrWrkPressure(int Value) {
    StatusInfoVar.HydrWrkPressure = Value;
	return StatusInfoVar.HydrWrkPressure;
}

int WCVarControl::GetInfo_HydrWrkPressure() {
	return StatusInfoVar.HydrWrkPressure;
}

//---------------------------------------------------------------------------
// Speisedruck der Hydraulik
int WCVarControl::SetInfo_HydrChargePressure(int Value) {
	StatusInfoVar.HydrChargePressure = Value;
	return StatusInfoVar.HydrChargePressure;
}

int WCVarControl::GetInfo_HydrChargePressure() {
	return StatusInfoVar.HydrChargePressure;
}

//---------------------------------------------------------------------------
// Aktueller Hydraulikfüllstand
int	WCVarControl::SetInfo_HydrLvl(int Value) {
	StatusInfoVar.HydrLvl = Value;
	return StatusInfoVar.HydrLvl;
}

int WCVarControl::GetInfo_HydrLvl() {
	return StatusInfoVar.HydrLvl;
}

//---------------------------------------------------------------------------
// Not-Schalter betätigt ein/aus
bool WCVarControl::SetInfo_EmergencySwitchOn()
{
        StatusInfoVar.EmergencySwitch = _ON;
        return StatusInfoVar.EmergencySwitch;
};

bool WCVarControl::SetInfo_EmergencySwitchOff()
{
        StatusInfoVar.EmergencySwitch = _OFF;
        return StatusInfoVar.EmergencySwitch;
};

bool WCVarControl::GetInfo_EmergencySwitch()
{
        return StatusInfoVar.EmergencySwitch;
};

//---------------------------------------------------------------------------
// Momentane Soll-Zugkraft
// Value: Angabe in N
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetInfo_CurrTractionReference(int Value)
{
        StatusInfoVar.CurrTractionReference = Value;
        return StatusInfoVar.CurrTractionReference;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetInfo_CurrTractionReference()
{
        return StatusInfoVar.CurrTractionReference;
};
//---------------------------------------------------------------------------

// Aktueller Programm-Modus
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetInfo_ProgramMode(int Value)
{
        StatusInfoVar.ProgramMode = Value;
        return StatusInfoVar.ProgramMode;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetInfo_ProgramMode()
{
        return StatusInfoVar.ProgramMode;
};
//---------------------------------------------------------------------------
// Teufenbereichsgrenzen überschritten
bool WCVarControl::SetInfo_DepthThresholdExceededOn()
{
        StatusInfoVar.DepthThresholdExceeded = _ON;
        return StatusInfoVar.DepthThresholdExceeded;
};

bool WCVarControl::SetInfo_DepthThresholdExceededOff()
{
        StatusInfoVar.DepthThresholdExceeded = _OFF;
        return StatusInfoVar.DepthThresholdExceeded;
};

bool WCVarControl::GetInfo_DepthThresholdExceeded()
{
        return StatusInfoVar.DepthThresholdExceeded;
};

//---------------------------------------------------------------------------
// Zugkraftgrenze überschritten
bool WCVarControl::SetInfo_TractionThresholdExceededOn()
{
        StatusInfoVar.TractionThresholdExceeded = _ON;
        return StatusInfoVar.TractionThresholdExceeded;
};

bool WCVarControl::SetInfo_TractionThresholdExceededOff()
{
        StatusInfoVar.TractionThresholdExceeded = _OFF;
        return StatusInfoVar.TractionThresholdExceeded;
};

bool WCVarControl::GetInfo_TractionThresholdExceeded()
{
        return StatusInfoVar.TractionThresholdExceeded;
};

//---------------------------------------------------------------------------
// Fahrhebelbetätigung
bool WCVarControl::SetInfo_LeverOn()
{
        StatusInfoVar.LeverOn = _ON;
        return StatusInfoVar.LeverOn;
};

bool WCVarControl::SetInfo_LeverOff()
{
        StatusInfoVar.LeverOn = _OFF;
        return StatusInfoVar.LeverOn;
};

bool WCVarControl::GetInfo_LeverOn()
{
        return StatusInfoVar.LeverOn;
};

//---------------------------------------------------------------------------
// tatsächlicher Hardware-Kontroll-Status
bool WCVarControl::SetInfo_HWCtrlSignalisationOn()
{
        StatusInfoVar.HWCtrlSignalisation = _ON;
        return StatusInfoVar.HWCtrlSignalisation;
};

bool WCVarControl::SetInfo_HWCtrlSignalisationOff()
{
        StatusInfoVar.HWCtrlSignalisation = _OFF;
        return StatusInfoVar.HWCtrlSignalisation;
};

bool WCVarControl::GetInfo_HWCtrlSignalisation()
{
        return StatusInfoVar.HWCtrlSignalisation;
};

//---------------------------------------------------------------------------
// Fahrzeugfehler
bool WCVarControl::SetInfo_AutomotiveErrorOn()
{
        StatusInfoVar.AutomotiveError = _ON;
        return StatusInfoVar.AutomotiveError;
};

bool WCVarControl::SetInfo_AutomotiveErrorOff()
{
        StatusInfoVar.AutomotiveError = _OFF;
        return StatusInfoVar.AutomotiveError;
};

bool WCVarControl::GetInfo_AutomotiveError()
{
        return StatusInfoVar.AutomotiveError;
};

//---------------------------------------------------------------------------
// Konfigurierbares Sondersignal
bool WCVarControl::SetInfo_SpecialSignal01On()
{
		StatusInfoVar.SpecialSignal01 = _ON;
		return StatusInfoVar.SpecialSignal01;
};

bool WCVarControl::SetInfo_SpecialSignal01Off()
{
		StatusInfoVar.SpecialSignal01 = _OFF;
		return StatusInfoVar.SpecialSignal01;
};

bool WCVarControl::GetInfo_SpecialSignal01()
{
        return StatusInfoVar.SpecialSignal01;
};

//---------------------------------------------------------------------------
// Konfigurierbares Sondersignal
bool WCVarControl::SetInfo_SpecialSignal02On()
{
		StatusInfoVar.SpecialSignal02 = _ON;
		return StatusInfoVar.SpecialSignal02;
};

bool WCVarControl::SetInfo_SpecialSignal02Off()
{
		StatusInfoVar.SpecialSignal02 = _OFF;
		return StatusInfoVar.SpecialSignal02;
};

bool WCVarControl::GetInfo_SpecialSignal02()
{
		return StatusInfoVar.SpecialSignal02;
};

//---------------------------------------------------------------------------
// Durch ein Sondersignal überwachte externe Bremse
bool WCVarControl::SetInfo_SpecialSignalExtBrakeOn() {
	StatusInfoVar.SpecialSignalExtBrake = _ON;
	return StatusInfoVar.SpecialSignalExtBrake;
}

bool WCVarControl::SetInfo_SpecialSignalExtBrakeOff() {
	StatusInfoVar.SpecialSignalExtBrake = _OFF;
	return StatusInfoVar.SpecialSignalExtBrake;
}

bool WCVarControl::GetInfo_SpecialSignalExtBrake() {
	return StatusInfoVar.SpecialSignalExtBrake;
}

//---------------------------------------------------------------------------
// 1. Wert für Zugkraftmessungs-Kalibrierung ist ermittelt
bool WCVarControl::SetInfo_CalibTractionValue1OkOn()
{
        StatusInfoVar.CalibTractionValue1Ok = _ON;
        return StatusInfoVar.CalibTractionValue1Ok;
};

bool WCVarControl::SetInfo_CalibTractionValue1OkOff()
{
        StatusInfoVar.CalibTractionValue1Ok = _OFF;
        return StatusInfoVar.CalibTractionValue1Ok;
};

bool WCVarControl::GetInfo_CalibTractionValue1Ok()
{
        return StatusInfoVar.CalibTractionValue1Ok;
};

//---------------------------------------------------------------------------
// 2. Wert für Zugkraftmessung-Kalibrierung ist ermittelt
bool WCVarControl::SetInfo_CalibTractionValue2OkOn()
{
//        if (!CalibTractionValue2Ok_Locked) StatusInfoVar.CalibTractionValue2Ok = _ON;
        StatusInfoVar.CalibTractionValue2Ok = _ON;
        return StatusInfoVar.CalibTractionValue2Ok;
};

bool WCVarControl::SetInfo_CalibTractionValue2OkOff()
{
//        if (!CalibTractionValue2Ok_Locked) StatusInfoVar.CalibTractionValue2Ok = _OFF;
        StatusInfoVar.CalibTractionValue2Ok = _OFF;
        return StatusInfoVar.CalibTractionValue2Ok;
};

bool WCVarControl::GetInfo_CalibTractionValue2Ok()
{
        return StatusInfoVar.CalibTractionValue2Ok;
};

//---------------------------------------------------------------------------
// Gibt an, ob das Kalibrieren der Teufe im Aktuellen Zustand generell möglich ist
// Rückgabe: Der gesetzte Wert
bool WCVarControl::SetInfo_CalibDepthAllowedOn()
{
	return StatusInfoVar.CalibDepthAllowed = _ON;
};

// Rückgabe: Der gesetzte Wert
bool WCVarControl::SetInfo_CalibDepthAllowedOff()
{
	return StatusInfoVar.CalibDepthAllowed = _OFF;
};

// Rückgabe: Der gesetzte Wert
bool WCVarControl::GetInfo_CalibDepthAllowed()
{
	return StatusInfoVar.CalibDepthAllowed;
};

//---------------------------------------------------------------------------
// Für Teufenkalibrierung wurde das Kabel über eine bestimmte Länge gewickelt
// Rückgabe: Der gesetzte Wert
bool WCVarControl::SetInfo_CalibDepthCableOkOn()
{
		StatusInfoVar.CalibDepthCableOk = _ON;
		return StatusInfoVar.CalibDepthCableOk;
};

// Rückgabe: Der gesetzte Wert
bool WCVarControl::SetInfo_CalibDepthCableOkOff()
{
		StatusInfoVar.CalibDepthCableOk = _OFF;
		return StatusInfoVar.CalibDepthCableOk;
};

// Rückgabe: Der gesetzte Wert
bool WCVarControl::GetInfo_CalibDepthCableOk()
{
        return StatusInfoVar.CalibDepthCableOk;
};

//---------------------------------------------------------------------------
// Pufferspeicher für den Versionsstring des Steuergeräts
// Rückgabewert: true, bei Erfolg
//               false, bei Misserfolg/Fehler
bool WCVarControl::SetInfo_CtlUnitSWVersion(char *Version){

		try
		{
				strcpy(StatusInfoVar.CtlUnitSWVersion, Version);
		}catch(...)
		{
				return false;
		};

		return true;
}

// Rückgabewert: Zeiger auf den Puffer
//---------------------------------------------------------------------------
char *WCVarControl::GetInfo_CtlUnitSWVersion(){
	return StatusInfoVar.CtlUnitSWVersion;
}

//---------------------------------------------------------------------------
// Pufferspeicher für den Versionsstring des Teufenrechners
// Rückgabewert: true, bei Erfolg
//               false, bei Misserfolg/Fehler
bool WCVarControl::SetInfo_DepthCalcSWVersion(char *Version)
{
		try
		{
				strcpy(StatusInfoVar.DepthCalcSWVersion, Version);
		}catch(...)
		{
				return false;
		};

		return true;
};

// Rückgabewert: Zeiger auf den Puffer
char *WCVarControl::GetInfo_DepthCalcSWVersion()
{
		return StatusInfoVar.DepthCalcSWVersion;
};

//---------------------------------------------------------------------------
// Enthält die Temperatur des T.-Sensors auf dem Teufenrechner in °C
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetInfo_TemperatureSensorDepthCalc(float Value)
{
        StatusInfoVar.TemperatureSensorDepthCalc = Value;
        return StatusInfoVar.TemperatureSensorDepthCalc;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetInfo_TemperatureSensorDepthCalc()
{
        return StatusInfoVar.TemperatureSensorDepthCalc;
};

//---------------------------------------------------------------------------
// Enthält die Temperatur des T.-Sensors A in °C
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetInfo_TemperatureSensorA(float Value)
{
        StatusInfoVar.TemperatureSensorA = Value;
        return StatusInfoVar.TemperatureSensorA;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetInfo_TemperatureSensorA()
{
        return StatusInfoVar.TemperatureSensorA;
};

//---------------------------------------------------------------------------
// Enthält die Temperatur des T.-Sensors B in °C
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetInfo_TemperatureSensorB(float Value)
{
        StatusInfoVar.TemperatureSensorB = Value;
        return StatusInfoVar.TemperatureSensorB;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetInfo_TemperatureSensorB()
{
        return StatusInfoVar.TemperatureSensorB;
};

//---------------------------------------------------------------------------
// Öltanktemperatur in °C
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetInfo_TemperatureOilTank(int Value) {
    StatusInfoVar.TemperatureOilTank = Value;
	return StatusInfoVar.TemperatureOilTank;
}

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetInfo_TemperatureOilTank() {
	return StatusInfoVar.TemperatureOilTank;
}


//---------------------------------------------------------------------------
// Gemessene momentane Windengeschwindigkeitssteuer-Spannung
int WCVarControl::SetInfo_CurrWinchRevMessVoltage(int Value)
{
        StatusInfoVar.CurrWinchRevMessVoltage = Value;
        return StatusInfoVar.CurrWinchRevMessVoltage;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetInfo_CurrWinchRevMessVoltage()
{
		return StatusInfoVar.CurrWinchRevMessVoltage;
};

//---------------------------------------------------------------------------
// Status der interne Automatischen Windenbremse
//(0-Automatik inaktiv, 1-angebremst, 2-geloest, 3-Bremsen erwartet)
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetInfo_CurrWinchBrakeInternAutoStatus(int Value)
{
	if ((Value < 0) || (Value > 3)) Value = 0;
	StatusInfoVar.CurrWinchBrakeInternAutoStatus = Value;
	return StatusInfoVar.CurrWinchBrakeInternAutoStatus;
}

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetInfo_CurrWinchBrakeInternAutoStatus()
{
	return StatusInfoVar.CurrWinchBrakeInternAutoStatus;
}

//---------------------------------------------------------------------------
//===========================================================================
// ParameterVar
// Nummer der aktuellen Winde
// Rückgabewert: der gesetzte Wert
unsigned int WCVarControl::SetParam_WinchNo(unsigned int Value)
{
//        if (!WinchNo_Locked) ParameterVar.WinchNo = Value;
        ParameterVar.WinchNo = Value;
        return ParameterVar.WinchNo;
};

// Rückgabewert: der gesetzte Wert
unsigned int WCVarControl::GetParam_WinchNo()
{
        return ParameterVar.WinchNo;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_WinchNo(unsigned int Value , unsigned int *Recommendation)
{
        if (Value < LIM_PAR_WinchNo_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_WinchNo_MIN;
                return false;
        };

        if (Value > LIM_PAR_WinchNo_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_WinchNo_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};


/*
//---------------------------------------------------------------------------
// Nummer der aktuellen Winde
// Rückgabewert: der gesetzte Wert
unsigned int WCVarControl::SetParam_WinchCnt(unsigned int Value)
{
        if (Value > _WinchCnt) ParameterVar.WinchCnt = _WinchCnt;               // Test auf < 0 nicht erforderlich, da unsigned
        else ParameterVar.WinchCnt = Value;
        return ParameterVar.WinchCnt;
};

// Rückgabewert: der gesetzte Wert
unsigned int WCVarControl::GetParam_WinchCnt()
{
        return ParameterVar.WinchCnt;
};
*/
//---------------------------------------------------------------------------

// Mindestteufe
// Value: Angabe in mm
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_MinDepth(int Value)
{
//        if (!MinDepth_Locked) ParameterVar.MinDepth = Value;
        ParameterVar.MinDepth = Value;
        return ParameterVar.MinDepth;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_MinDepth()
{
        return ParameterVar.MinDepth;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_MinDepth(int Value, int *Recommendation)
{
        if (Value < LIM_PAR_MinDepth_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_MinDepth_MIN;
                return false;
        };

        if (Value > LIM_PAR_MinDepth_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_MinDepth_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};

//---------------------------------------------------------------------------

// Maximalteufe
// Value: Angabe in mm
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_MaxDepth(int Value)
{
//        if (!MaxDepth_Locked) ParameterVar.MaxDepth = Value;
        ParameterVar.MaxDepth = Value;
        return ParameterVar.MaxDepth;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_MaxDepth()
{
        return ParameterVar.MaxDepth;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_MaxDepth(int Value, int *Recommendation)
{
        if (Value < LIM_PAR_MaxDepth_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_MaxDepth_MIN;
                return false;
        };

        if (Value > LIM_PAR_MaxDepth_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_MaxDepth_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};
//---------------------------------------------------------------------------

// Anfangsteufe
// Value: Angabe in mm
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_StartDepth(int Value)
{
//        if (!StartDepth_Locked) ParameterVar.StartDepth = Value;
        ParameterVar.StartDepth = Value;
        return ParameterVar.StartDepth;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_StartDepth()
{
		return ParameterVar.StartDepth;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_StartDepth(int Value, int *Recommendation)
{
		if (Value < LIM_PAR_StartDepth_MIN)
		{
				if (Recommendation != 0) *Recommendation = LIM_PAR_StartDepth_MIN;
				return false;
		};

		if (Value > LIM_PAR_StartDepth_MAX)
		{
				if (Recommendation != 0) *Recommendation = LIM_PAR_StartDepth_MAX;
				return false;
		};

		if (Recommendation != 0) *Recommendation = Value;
		return true;
};

//---------------------------------------------------------------------------
// Teufe umkehren
// Parameter:	_ON		... Teufenumkehrung aktiv
// Parameter:	_OFF	... Teufenumkehrung inaktiv
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_ReverseDepth(bool Value)
{
	ParameterVar.ReverseDepth = Value;
	return ParameterVar.ReverseDepth;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetParam_ReverseDepth()
{
	return ParameterVar.ReverseDepth;
};
//---------------------------------------------------------------------------

// Soll-Sondengeschwindigkeit
// Value: Angabe in m/h
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_Velocity(int Value)
{
//        if (!Velocity_Locked) ParameterVar.Velocity = Value;
        ParameterVar.Velocity = Value;
        return ParameterVar.Velocity;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_Velocity()
{
        return ParameterVar.Velocity;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_Velocity(int Value, int *Recommendation)
{
        if (Value < LIM_PAR_Velocity_MIN)
        {
				if (Recommendation != 0) *Recommendation = LIM_PAR_Velocity_MIN;
                return false;
        };

        if (Value > LIM_PAR_Velocity_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_Velocity_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};

//---------------------------------------------------------------------------
// Soll-Sondengeschwindigkeit für Programmautomatik
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_Velocity_PrgAutom(int Value) {
    ParameterVar.Velocity_PrgAutom = Value;
    return ParameterVar.Velocity_PrgAutom;
}

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_Velocity_PrgAutom() {
    return ParameterVar.Velocity_PrgAutom;
}

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_Velocity_PrgAutom(int Value, int *Recommendation){
        if (Value < LIM_PAR_Velocity_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_Velocity_MIN;
                return false;
        };

        if (Value > LIM_PAR_Velocity_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_Velocity_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
}

//---------------------------------------------------------------------------
// Soll-Fahrtrichtung
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_DirectionUp()
{
//        if (!Direction_Locked) ParameterVar.Direction = _UPward;
        ParameterVar.Direction = _UPward;
        return ParameterVar.Direction;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_DirectionDown()
{
//        if (!Direction_Locked) ParameterVar.Direction = _DOWN;
        ParameterVar.Direction = _DOWN;
        return ParameterVar.Direction;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetParam_Direction()
{
        return ParameterVar.Direction;
};

//---------------------------------------------------------------------------
// Soll-Fahrtrichtung für Programmautomatik
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_Direction_PrgAutomUp()
{
//        if (!Direction_Locked) ParameterVar.Direction = _UPward;
        ParameterVar.Direction_PrgAutom = _UPward;
        return ParameterVar.Direction_PrgAutom;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_Direction_PrgAutomDown()
{
//        if (!Direction_Locked) ParameterVar.Direction = _DOWN;
        ParameterVar.Direction_PrgAutom = _DOWN;
        return ParameterVar.Direction_PrgAutom;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetParam_Direction_PrgAutom()
{
        return ParameterVar.Direction_PrgAutom;
};

//---------------------------------------------------------------------------
// Drehzahl Fahrzeugmotor veränderbar
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_EngineRevolutionChangeableOn()
{
        ParameterVar.EngineRevolutionChangeable = _ON;
        return ParameterVar.EngineRevolutionChangeable;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_EngineRevolutionChangeableOff()
{
        ParameterVar.EngineRevolutionChangeable = _OFF;
        return ParameterVar.EngineRevolutionChangeable;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetParam_EngineRevolutionChangeable()
{
        return ParameterVar.EngineRevolutionChangeable;
};

//---------------------------------------------------------------------------
// Antriebsmotor Starten / Stoppen möglich
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_EngineStartStopEnable()
{
	ParameterVar.EngineStartStopEnable = _ON;
	return ParameterVar.EngineStartStopEnable;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_EngineStartStopDisable()
{
	ParameterVar.EngineStartStopEnable = _OFF;
	return ParameterVar.EngineStartStopEnable;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetParam_EngineStartStopEnable()
{
	return ParameterVar.EngineStartStopEnable;
};

//---------------------------------------------------------------------------
// Freilaufbedienung möglich
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_LostMotionEnable()
{
	ParameterVar.LostMotionEnable = _ON;
	return ParameterVar.LostMotionEnable;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_LostMotionDisable()
{
	ParameterVar.LostMotionEnable = _OFF;
	return ParameterVar.LostMotionEnable;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetParam_LostMotionEnable()
{
	return ParameterVar.LostMotionEnable;
}

//---------------------------------------------------------------------------
// Bremse abschalten möglich
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_BrakeOffEnable() {
	ParameterVar.BrakeOffEnable = _ON;
	return ParameterVar.BrakeOffEnable;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_BrakeOffDisable() {
	ParameterVar.BrakeOffEnable = _OFF;
	return ParameterVar.BrakeOffEnable;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetParam_BrakeOffEnable() {
	return ParameterVar.BrakeOffEnable;
}

//---------------------------------------------------------------------------
// Externe Bremse verfügbar
bool WCVarControl::SetParam_ExternalBrakeAvailableOn() {
	ParameterVar.ExternalBrakeAvailable = _ON;
	return ParameterVar.ExternalBrakeAvailable;
}

bool WCVarControl::SetParam_ExternalBrakeAvailableOff() {
	ParameterVar.ExternalBrakeAvailable = _OFF;
	return ParameterVar.ExternalBrakeAvailable;
}

bool WCVarControl::GetParam_ExternalBrakeAvailable() {
	return ParameterVar.ExternalBrakeAvailable;
}

//---------------------------------------------------------------------------
// Gibt das Ausgabeformat der cm-Impulse an
// Rückgabewert: der gesetzte Wert
// Value: 1...KARAT
//        2...ONYX
//        3...KEDR
//        x...
int WCVarControl::SetParam_FormatCMImpulse(int Value)
{
        ParameterVar.FormatCMImpulse = Value;
        return ParameterVar.FormatCMImpulse;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_FormatCMImpulse()
{
        return ParameterVar.FormatCMImpulse;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_FormatCMImpulse(int Value, int *Recommendation)
{
        if (Value < LIM_PAR_FormatCMImpulse_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_FormatCMImpulse_MIN;
                return false;
        };

        if (Value > LIM_PAR_FormatCMImpulse_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_FormatCMImpulse_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};

//---------------------------------------------------------------------------

// Maximalzugkraft
// Value: Angabe in N
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_MaxTraction(int Value)
{
//        if (!MaxTraction_Locked) ParameterVar.MaxTraction[ParameterVar.WinchNo] = Value;
        ParameterVar.MaxTraction[ParameterVar.WinchNo] = Value;
        return ParameterVar.MaxTraction[ParameterVar.WinchNo];
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_MaxTraction()
{
        return ParameterVar.MaxTraction[ParameterVar.WinchNo];
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_MaxTraction(int Value, int *Recommendation)
{
        if (Value < LIM_PAR_MaxTraction_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_MaxTraction_MIN;
                return false;
        };

        if (Value > LIM_PAR_MaxTraction_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_MaxTraction_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};

//---------------------------------------------------------------------------

// Minimalzugkraft
// Value: Angabe in N
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_MinTraction(int Value)
{
//        if (!MinTraction_Locked) ParameterVar.MinTraction[ParameterVar.WinchNo] = Value;
        ParameterVar.MinTraction[ParameterVar.WinchNo] = Value;
        return ParameterVar.MinTraction[ParameterVar.WinchNo];
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_MinTraction()
{
        return ParameterVar.MinTraction[ParameterVar.WinchNo];
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_MinTraction(int Value, int *Recommendation)
{
        if (Value < LIM_PAR_MinTraction_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_MinTraction_MIN;
                return false;
        };

        if (Value > LIM_PAR_MinTraction_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_MinTraction_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};

//---------------------------------------------------------------------------
// einstellbare untere Zugkraftgerenze, abhängig von der aktuell höchsten Zugkraft (wichtig bei Abwärtsfahrt)
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_RelTensionLimitLower(int Value)
{
		ParameterVar.RelTensionLimitLower[ParameterVar.WinchNo] = Value;
		return ParameterVar.RelTensionLimitLower[ParameterVar.WinchNo];
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_RelTensionLimitLower()
{
		return ParameterVar.RelTensionLimitLower[ParameterVar.WinchNo];
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_RelTensionLimitLower(int Value, int *Recommendation)
{
		if (Value < LIM_PAR_RelTensionLimitLower_MIN)
		{
				if (Recommendation != 0) *Recommendation = LIM_PAR_RelTensionLimitLower_MIN;
                return false;
        };

		if (Value > LIM_PAR_RelTensionLimitLower_MAX)
        {
				if (Recommendation != 0) *Recommendation = LIM_PAR_RelTensionLimitLower_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};

//---------------------------------------------------------------------------

// Schwellwert für Automatikbetrieb
// Value: Angabe in mm
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_DepthThresholdForAuto(int Value)
{
        ParameterVar.DepthThresholdForAuto = Value;
        return ParameterVar.DepthThresholdForAuto;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_DepthThresholdForAuto()
{
        return ParameterVar.DepthThresholdForAuto;
};
//---------------------------------------------------------------------------

// Teufe vor Endwert, ab wann abgebremst werden soll
// Value: Angabe in mm
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_DepthDistanceForBreak(int Value)
{
		ParameterVar.DepthDistanceForBreak = Value;
		return ParameterVar.DepthDistanceForBreak;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_DepthDistanceForBreak()
{
		return ParameterVar.DepthDistanceForBreak;
};

//---------------------------------------------------------------------------
// MM-Zwangssynchr. Ein/Aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_MMSyncForceOn()
{
	ParameterVar.MMSyncForceOn = _ON;
	return ParameterVar.MMSyncForceOn;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_MMSyncForceOff()
{
	ParameterVar.MMSyncForceOn = _OFF;
	return ParameterVar.MMSyncForceOn;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetParam_MMSyncForceOn()
{
	return ParameterVar.MMSyncForceOn;
}
//---------------------------------------------------------------------------

// MM-Zwangssynchr. Teufenbereichsgrenze in Meter
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_MMSyncForceRangeLim(int Value)
{
	ParameterVar.MMSyncForceRangeLim = Value;
	return ParameterVar.MMSyncForceRangeLim;
}

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_MMSyncForceRangeLim()
{
	return ParameterVar.MMSyncForceRangeLim;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::CheckParam_MMSyncForceRangeLim(int Value, int *Recommendation)
{
	if (Value < LIM_PAR_MMSyncForceRangeLim_MIN) {
		if (Recommendation != 0) *Recommendation = LIM_PAR_MMSyncForceRangeLim_MIN;
		return false;
	}

	if (Value > LIM_PAR_MMSyncForceRangeLim_MAX) {
		if (Recommendation != 0) *Recommendation = LIM_PAR_MMSyncForceRangeLim_MAX;
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------

// Zugkraft beim Entladen des Kabels
// Value: Angabe in N
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_UnloadCableTraction(int Value)
{
//        if (!UnloadCableMinTraction_Locked) ParameterVar.UnloadCableMinTraction[ParameterVar.WinchNo] = Value;
        ParameterVar.UnloadCableTraction[ParameterVar.WinchNo] = Value;
        return ParameterVar.UnloadCableTraction[ParameterVar.WinchNo];
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_UnloadCableTraction()
{
        return ParameterVar.UnloadCableTraction[ParameterVar.WinchNo];
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_UnloadCableTraction(int Value, int *Recommendation)
{
        if (Value < LIM_PAR_UnloadCableTraction_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCableTraction_MIN;
                return false;
        };

        if (Value > LIM_PAR_UnloadCableTraction_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCableTraction_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};

//---------------------------------------------------------------------------

// Mindestzugkraft beim Entladen des Kabels
// Value: Angabe in N
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_UnloadCableMinTraction(int Value)
{
//        if (!UnloadCableMinTraction_Locked) ParameterVar.UnloadCableMinTraction[ParameterVar.WinchNo] = Value;
        ParameterVar.UnloadCableMinTraction = Value;
        return ParameterVar.UnloadCableMinTraction;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_UnloadCableMinTraction()
{
        return ParameterVar.UnloadCableMinTraction;
};

//---------------------------------------------------------------------------

// Kabel entladen mit/ohne stretching
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_UnloadCableStretchOn()
{
//        if (!UnloadCableStrech_Locked) ParameterVar.UnloadCableStretch = _ON;
        ParameterVar.UnloadCableStretch = _ON;
        return ParameterVar.UnloadCableStretch;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_UnloadCableStretchOff()
{
//        if (!UnloadCableStrech_Locked) ParameterVar.UnloadCableStretch = _OFF;
        ParameterVar.UnloadCableStretch = _OFF;
        return ParameterVar.UnloadCableStretch;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetParam_UnloadCableStretch()
{
        return ParameterVar.UnloadCableStretch;
};
//---------------------------------------------------------------------------
// Anfangslänge beim Entladen des Kabels
int WCVarControl::SetParam_UnloadCableBeginLenght(int Value)
{
        ParameterVar.UnloadCableBeginLenght = Value;
        return ParameterVar.UnloadCableBeginLenght;
};

int WCVarControl::GetParam_UnloadCableBeginLenght()
{
        return ParameterVar.UnloadCableBeginLenght;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_UnloadCableBeginLenght(int Value, int *Recommendation)
{
        if (Value < LIM_PAR_UnloadCableBeginLenght_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCableBeginLenght_MIN;
                return false;
        };

        if (Value > LIM_PAR_UnloadCableBeginLenght_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCableBeginLenght_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};

//---------------------------------------------------------------------------
// Anfangslänge beim Entladen des Kabels
int WCVarControl::SetParam_UnloadCableEndLenght(int Value)
{
        ParameterVar.UnloadCableEndLenght = Value;
        return ParameterVar.UnloadCableEndLenght;
};

int WCVarControl::GetParam_UnloadCableEndLenght()
{
        return ParameterVar.UnloadCableEndLenght;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_UnloadCableEndLenght(int Value, int *Recommendation)
{
        if (Value < LIM_PAR_UnloadCableEndLenght_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCableEndLenght_MIN;
                return false;
        };

        if (Value > LIM_PAR_UnloadCableEndLenght_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCableEndLenght_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};


//---------------------------------------------------------------------------
// Anfangszugkraft beim Entladen des Kabels
// Value: Angabe in N
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_UnloadCableStartTraction(int Value)
{
//        if (!UnloadCableStartTraction_Locked) ParameterVar.UnloadCableStartTraction[ParameterVar.WinchNo] = Value;
        ParameterVar.UnloadCableStartTraction[ParameterVar.WinchNo] = Value;
        return ParameterVar.UnloadCableStartTraction[ParameterVar.WinchNo];
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_UnloadCableStartTraction()
{
        return ParameterVar.UnloadCableStartTraction[ParameterVar.WinchNo];
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_UnloadCableStartTraction(int Value, int *Recommendation)
{
        if (Value < LIM_PAR_UnloadCableStartTraction_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCableStartTraction_MIN;
                return false;
        };

        if (Value > LIM_PAR_UnloadCableStartTraction_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCableStartTraction_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};
//---------------------------------------------------------------------------

// Endzugkraft beim Entladen des Kabels
// Value: Angabe in N
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_UnloadCableEndTraction(int Value)
{
//        if (!UnloadCableEndTraction_Locked) ParameterVar.UnloadCableEndTraction[ParameterVar.WinchNo] = Value;
        ParameterVar.UnloadCableEndTraction[ParameterVar.WinchNo] = Value;
        return ParameterVar.UnloadCableEndTraction[ParameterVar.WinchNo];
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_UnloadCableEndTraction()
{
        return ParameterVar.UnloadCableEndTraction[ParameterVar.WinchNo];
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_UnloadCableEndTraction(int Value, int *Recommendation)
{
        if (Value < LIM_PAR_UnloadCableEndTraction_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCableEndTraction_MIN;
                return false;
        };

        if (Value > LIM_PAR_UnloadCableEndTraction_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCableEndTraction_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};

//---------------------------------------------------------------------------
// einstellbare obere Zugkraftgerenze, abhängig von der aktuell niedrigsten Zugkraft (wichtig bei Abwärtsfahrt)
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_RelTensionLimitUpper(int Value)
{
		ParameterVar.RelTensionLimitUpper[ParameterVar.WinchNo] = Value;
		return ParameterVar.RelTensionLimitUpper[ParameterVar.WinchNo];
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_RelTensionLimitUpper()
{
		return ParameterVar.RelTensionLimitUpper[ParameterVar.WinchNo];
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_RelTensionLimitUpper(int Value, int *Recommendation)
{
		if (Value < LIM_PAR_RelTensionLimitUpper_MIN)
		{
				if (Recommendation != 0) *Recommendation = LIM_PAR_RelTensionLimitUpper_MIN;
				return false;
		};

		if (Value > LIM_PAR_RelTensionLimitUpper_MAX)
		{
				if (Recommendation != 0) *Recommendation = LIM_PAR_RelTensionLimitUpper_MAX;
				return false;
		};

		if (Recommendation != 0) *Recommendation = Value;
		return true;
};

/*
//---------------------------------------------------------------------------
// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Vorhaltezeit
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_UnloadCable_PID_TD(float Value)
{
		ParameterVar.UnloadCable_PID_TD = Value;
		return ParameterVar.UnloadCable_PID_TD;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_UnloadCable_PID_TD()
{
        return ParameterVar.UnloadCable_PID_TD;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_UnloadCable_PID_TD(float Value, float *Recommendation)
{
        if (Value < LIM_PAR_UnloadCable_PID_TD_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCable_PID_TD_MIN;
                return false;
        };

        if (Value > LIM_PAR_UnloadCable_PID_TD_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCable_PID_TD_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};

//---------------------------------------------------------------------------
// PID-Regler für Zugkraftkontrolle beim Kabel entladen (zum Speichern): Vorhaltezeit
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_UnloadCable_PID_TD_SAVE(float Value)
{
        ParameterVar.UnloadCable_PID_TD_SAVE = Value;
        return ParameterVar.UnloadCable_PID_TD_SAVE;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_UnloadCable_PID_TD_SAVE()
{
        return ParameterVar.UnloadCable_PID_TD_SAVE;
};

//---------------------------------------------------------------------------
// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Nachstellzeit
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_UnloadCable_PID_TI(float Value)
{
        ParameterVar.UnloadCable_PID_TI = Value;
        return ParameterVar.UnloadCable_PID_TI;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_UnloadCable_PID_TI()
{
        return ParameterVar.UnloadCable_PID_TI;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_UnloadCable_PID_TI(float Value, float *Recommendation)
{
        if (Value < LIM_PAR_UnloadCable_PID_TI_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCable_PID_TI_MIN;
                return false;
        };

        if (Value > LIM_PAR_UnloadCable_PID_TI_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCable_PID_TI_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};
//---------------------------------------------------------------------------
// PID-Regler für Zugkraftkontrolle beim Kabel entladen (zum Speichern): Nachstellzeit
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_UnloadCable_PID_TI_SAVE(float Value)
{
        ParameterVar.UnloadCable_PID_TI_SAVE = Value;
        return ParameterVar.UnloadCable_PID_TI_SAVE;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_UnloadCable_PID_TI_SAVE()
{
        return ParameterVar.UnloadCable_PID_TI_SAVE;
};

//---------------------------------------------------------------------------
// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Verstärkungsfaktor
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_UnloadCable_PID_KR(float Value)
{
        ParameterVar.UnloadCable_PID_KR = Value;
        return ParameterVar.UnloadCable_PID_KR;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_UnloadCable_PID_KR()
{
        return ParameterVar.UnloadCable_PID_KR;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_UnloadCable_PID_KR(float Value, float *Recommendation)
{
        if (Value < LIM_PAR_UnloadCable_PID_KR_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCable_PID_KR_MIN;
                return false;
        };

        if (Value > LIM_PAR_UnloadCable_PID_KR_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCable_PID_KR_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};
//---------------------------------------------------------------------------
// PID-Regler für Zugkraftkontrolle beim Kabel entladen (zum Speichern): Verstärkungsfaktor
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_UnloadCable_PID_KR_SAVE(float Value)
{
        ParameterVar.UnloadCable_PID_KR_SAVE = Value;
        return ParameterVar.UnloadCable_PID_KR_SAVE;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_UnloadCable_PID_KR_SAVE()
{
        return ParameterVar.UnloadCable_PID_KR_SAVE;
};

//---------------------------------------------------------------------------
// schneller PID-Regler für Zugkraftkontrolle beim Kabel entladen: Vorhaltezeit
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_UnloadCable_PID_TD_2(float Value)
{
        ParameterVar.UnloadCable_PID_TD_2 = Value;
        return ParameterVar.UnloadCable_PID_TD_2;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_UnloadCable_PID_TD_2()
{
        return ParameterVar.UnloadCable_PID_TD_2;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_UnloadCable_PID_TD_2(float Value, float *Recommendation)
{
        if (Value < LIM_PAR_UnloadCable_PID_TD_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCable_PID_TD_MIN;
                return false;
        };

        if (Value > LIM_PAR_UnloadCable_PID_TD_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCable_PID_TD_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};

//---------------------------------------------------------------------------
// schneller PID-Regler für Zugkraftkontrolle beim Kabel entladen (zum Speichern): Vorhaltezeit
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_UnloadCable_PID_TD_2_SAVE(float Value)
{
        ParameterVar.UnloadCable_PID_TD_2_SAVE = Value;
        return ParameterVar.UnloadCable_PID_TD_2_SAVE;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_UnloadCable_PID_TD_2_SAVE()
{
        return ParameterVar.UnloadCable_PID_TD_2_SAVE;
};

//---------------------------------------------------------------------------
// schneller PID-Regler für Zugkraftkontrolle beim Kabel entladen: Nachstellzeit
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_UnloadCable_PID_TI_2(float Value)
{
        ParameterVar.UnloadCable_PID_TI_2 = Value;
        return ParameterVar.UnloadCable_PID_TI_2;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_UnloadCable_PID_TI_2()
{
        return ParameterVar.UnloadCable_PID_TI_2;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_UnloadCable_PID_TI_2(float Value, float *Recommendation)
{
        if (Value < LIM_PAR_UnloadCable_PID_TI_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCable_PID_TI_MIN;
                return false;
        };

        if (Value > LIM_PAR_UnloadCable_PID_TI_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCable_PID_TI_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};

//---------------------------------------------------------------------------
// schneller PID-Regler für Zugkraftkontrolle beim Kabel entladen (zum Speichern): Nachstellzeit
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_UnloadCable_PID_TI_2_SAVE(float Value)
{
        ParameterVar.UnloadCable_PID_TI_2_SAVE = Value;
        return ParameterVar.UnloadCable_PID_TI_2_SAVE;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_UnloadCable_PID_TI_2_SAVE()
{
        return ParameterVar.UnloadCable_PID_TI_2_SAVE;
};

//---------------------------------------------------------------------------
// schneller PID-Regler für Zugkraftkontrolle beim Kabel entladen: Verstärkungsfaktor
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_UnloadCable_PID_KR_2(float Value)
{
        ParameterVar.UnloadCable_PID_KR_2 = Value;
        return ParameterVar.UnloadCable_PID_KR_2;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_UnloadCable_PID_KR_2()
{
        return ParameterVar.UnloadCable_PID_KR_2;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_UnloadCable_PID_KR_2(float Value, float *Recommendation)
{
        if (Value < LIM_PAR_UnloadCable_PID_KR_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCable_PID_KR_MIN;
                return false;
        };

        if (Value > LIM_PAR_UnloadCable_PID_KR_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_UnloadCable_PID_KR_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};

//---------------------------------------------------------------------------
// schneller PID-Regler für Zugkraftkontrolle beim Kabel entladen (zum Speichern): Verstärkungsfaktor
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_UnloadCable_PID_KR_2_SAVE(float Value)
{
		ParameterVar.UnloadCable_PID_KR_2_SAVE = Value;
		return ParameterVar.UnloadCable_PID_KR_2_SAVE;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_UnloadCable_PID_KR_2_SAVE()
{
		return ParameterVar.UnloadCable_PID_KR_2_SAVE;
};

//---------------------------------------------------------------------------
// Kabel Entladen mit externem Regler Kennliniendef. Maximalzugkraft in N
// Rückgabewert: der gesetzte Wert
int	WCVarControl::SetParam_UnloadCable_ExtReg_MaxTension(int Value)
{
	ParameterVar.UnloadCable_ExtReg_MaxTension = Value;
    return ParameterVar.UnloadCable_ExtReg_MaxTension;
}

// Rückgabewert: der gesetzte Wert
int	WCVarControl::GetParam_UnloadCable_ExtReg_MaxTension()
{
	return ParameterVar.UnloadCable_ExtReg_MaxTension;
}

//---------------------------------------------------------------------------
// Kabel Entladen mit externem Regler Kennliniendef. Maximalspannung
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_UnloadCable_ExtReg_MaxVoltage(int Value)
{
	ParameterVar.UnloadCable_ExtReg_MaxVoltage = Value;
	return ParameterVar.UnloadCable_ExtReg_MaxVoltage;
}

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_UnloadCable_ExtReg_MaxVoltage()
{
	return ParameterVar.UnloadCable_ExtReg_MaxVoltage;
}

//---------------------------------------------------------------------------
// Kabel Entladen mit externem Regler Kennliniendef. Mindestzugkraft
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_UnloadCable_ExtReg_MinTension(int Value)
{
	ParameterVar.UnloadCable_ExtReg_MinTension = Value;
    return ParameterVar.UnloadCable_ExtReg_MinTension;
}

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_UnloadCable_ExtReg_MinTension()
{
    return ParameterVar.UnloadCable_ExtReg_MinTension;
}

//---------------------------------------------------------------------------
// Kabel Entladen mit externem Regler Kennliniendef. Mindestspannung
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_UnloadCable_ExtReg_MinVoltage(int Value)
{
	ParameterVar.UnloadCable_ExtReg_MinVoltage = Value;
	return ParameterVar.UnloadCable_ExtReg_MinVoltage;
}

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_UnloadCable_ExtReg_MinVoltage()
{
	return ParameterVar.UnloadCable_ExtReg_MinVoltage;
}

//---------------------------------------------------------------------------
// Kabel Entladen mit externem Regler max. positiver Offset der Kennlinie
// Rückgabewert: der gesetzte Wert
//int WCVarControl::SetParam_UnloadCable_ExtReg_MaxPosOffset(int Value)
//{
//	ParameterVar.UnloadCable_ExtReg_MaxPosOffset = Value;
//	return ParameterVar.UnloadCable_ExtReg_MaxPosOffset;
//}

// Rückgabewert: der gesetzte Wert
//int WCVarControl::GetParam_UnloadCable_ExtReg_MaxPosOffset()
//{
//	return ParameterVar.UnloadCable_ExtReg_MaxPosOffset;
//}

//---------------------------------------------------------------------------
// Kabel Entladen mit externem Regler max. negativer Offset der Kennlinie
// Rückgabewert: der gesetzte Wert
//int WCVarControl::SetParam_UnloadCable_ExtReg_MaxNegOffset(int Value)
//{
//	ParameterVar.UnloadCable_ExtReg_MaxNegOffset = Value;
//	return ParameterVar.UnloadCable_ExtReg_MaxNegOffset;
//}

// Rückgabewert: der gesetzte Wert
//int WCVarControl::GetParam_UnloadCable_ExtReg_MaxNegOffset()
//{
//	return ParameterVar.UnloadCable_ExtReg_MaxNegOffset;
//}

//---------------------------------------------------------------------------
// Kabel Entladen mit externem Regler Verzögerung beim Abfahren (Rampe) in ms
// Rückgabewert: der gesetzte Wert
//int WCVarControl::SetParam_UnloadCable_ExtReg_Delay(int Value)
//{
//	ParameterVar.UnloadCable_ExtReg_Delay = Value;
//	return ParameterVar.UnloadCable_ExtReg_Delay;
//}

// Rückgabewert: der gesetzte Wert
//int WCVarControl::GetParam_UnloadCable_ExtReg_Delay()
//{
//	return ParameterVar.UnloadCable_ExtReg_Delay;
//}

//---------------------------------------------------------------------------
// Kabel Entladen mit externem Regler P-Anteil
// Rückgabewert: der gesetzte Wert
//int WCVarControl::SetParam_UnloadCable_ExtReg_P(int Value)
//{
//	ParameterVar.UnloadCable_ExtReg_P = Value;
//	return ParameterVar.UnloadCable_ExtReg_P;
//}

// Rückgabewert: der gesetzte Wert
//int WCVarControl::GetParam_UnloadCable_ExtReg_P()
//{
//	return ParameterVar.UnloadCable_ExtReg_P;
//}

//---------------------------------------------------------------------------
// Kabel Entladen mit externem Regler I-Anteil
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_UnloadCable_ExtReg_I(int Value)
{
	ParameterVar.UnloadCable_ExtReg_I = Value;
	return ParameterVar.UnloadCable_ExtReg_I;
}

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_UnloadCable_ExtReg_I()
{
	return ParameterVar.UnloadCable_ExtReg_I;
}

//---------------------------------------------------------------------------
// Kabel Entladen mit externem Regler Windenansteuerspannung zum Kabel straffen in mV
// Rückgabewert: der gesetzte Wert
int	WCVarControl::SetParam_UnloadCable_ExtReg_ReturnRotVoltage(int Value)
{
	ParameterVar.UnloadCable_ExtReg_ReturnRotVoltage = Value;
	return ParameterVar.UnloadCable_ExtReg_ReturnRotVoltage;
}

// Rückgabewert: der gesetzte Wert
int	WCVarControl::GetParam_UnloadCable_ExtReg_ReturnRotVoltage()
{
	return ParameterVar.UnloadCable_ExtReg_ReturnRotVoltage;
}
*/

//---------------------------------------------------------------------------
// Darstellung der Zugkraft (0=1N|1=1kN|2=1kp)
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_TractionViewDimensionUnit(int Value)
{
        ParameterVar.TractionViewDimensionUnit = Value;
        return ParameterVar.TractionViewDimensionUnit;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_TractionViewDimensionUnit()
{
        return ParameterVar.TractionViewDimensionUnit;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_TractionViewDimensionUnit(int Value, int *Recommendation)
{
        if (Value < LIM_PAR_TractionViewDimensionUnit_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_TractionViewDimensionUnit_MIN;
                return false;
        };

        if (Value > LIM_PAR_TractionViewDimensionUnit_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_TractionViewDimensionUnit_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};
//---------------------------------------------------------------------------
// Eichfaktor für Teufenrechner
// Value: Angabe in mm/Impuls *100000
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_CalFactDepth(int Value)
{
//        if (!CalFactDepth_Locked) ParameterVar.CalFactDepth[ParameterVar.WinchNo] = Value;
		ParameterVar.CalFactDepth[ParameterVar.WinchNo] = Value;
		return ParameterVar.CalFactDepth[ParameterVar.WinchNo];
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_CalFactDepth()
{
        return ParameterVar.CalFactDepth[ParameterVar.WinchNo];
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_CalFactDepth(int Value, int *Recommendation)
{
        if (Value < LIM_PAR_CalFactDepth_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_CalFactDepth_MIN;
                return false;
        };

        if (Value > LIM_PAR_CalFactDepth_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_CalFactDepth_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};

//---------------------------------------------------------------------------
// Teufenkalibrierfaktor, ermittelt bei der Kalibrierung und vorgemerkt für die Übernahme
int WCVarControl::SetParam_CalFactDepth_Earmarked(int Value)
{
	ParameterVar.CalFactDepth_Earmarked = Value;
	return ParameterVar.CalFactDepth_Earmarked;
};

int WCVarControl::GetParam_CalFactDepth_Earmarked()
{
	return ParameterVar.CalFactDepth_Earmarked;
};

// CheckParam_CalFactDepth_Earmarked(int Value, int *Recommendation)
// für diese Funktion kann "WCVarControl::CheckParam_CalFactDepth..." verwendet werden

//---------------------------------------------------------------------------

// Zugkraft-Kalibr. Anstiegsanteil x-Richtung
// Value: Angabe in mV
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_CalibTractionX(int Value)
{
//        if (!CalibTractionX_Locked) ParameterVar.CalibTractionX[ParameterVar.WinchNo] = Value;
        ParameterVar.CalibTractionX[ParameterVar.WinchNo] = Value;
        return ParameterVar.CalibTractionX[ParameterVar.WinchNo];
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_CalibTractionX()
{
		return ParameterVar.CalibTractionX[ParameterVar.WinchNo];
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
int WCVarControl::CheckParam_CalibTractionX(int Value, int *Recomendation)
{
	if (Value == 0) {
		if (Recomendation != 0) *Recomendation = 1;
        return false;
	}

	if (Recomendation != 0) *Recomendation = Value;
	return true;
}

//---------------------------------------------------------------------------

// Zugkraft-Kalibr. Anstiegsanteil y-Richtung
// Value: Angabe in N
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_CalibTractionY(int Value)
{
//        if (!CalibTractionY_Locked) ParameterVar.CalibTractionY[ParameterVar.WinchNo] = Value;
        ParameterVar.CalibTractionY[ParameterVar.WinchNo] = Value;
        return ParameterVar.CalibTractionY[ParameterVar.WinchNo];
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_CalibTractionY()
{
        return ParameterVar.CalibTractionY[ParameterVar.WinchNo];
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
int WCVarControl::CheckParam_CalibTractionY(int Value, int *Recomendation)
{
	if (Value == 0) {
		if (Recomendation != 0) *Recomendation = 1;
        return false;
	}

	if (Recomendation != 0) *Recomendation = Value;
	return true;
}

//---------------------------------------------------------------------------

// Zugkraft-Kalibr. Null-Abgleichswert
// Value: Angabe in N
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_CalibTractionZero(int Value)
{
//        if (!CalibTractionZero_Locked) ParameterVar.CalibTractionZero[ParameterVar.WinchNo] = Value;
        ParameterVar.CalibTractionZero[ParameterVar.WinchNo] = Value;
        return ParameterVar.CalibTractionZero[ParameterVar.WinchNo];
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_CalibTractionZero()
{
        return ParameterVar.CalibTractionZero[ParameterVar.WinchNo];
};

//---------------------------------------------------------------------------

// Inkrementalgeber-Fehler-Überwachung
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_IncEncErrorMonitOn()
{
		ParameterVar.IncEncErrorMonitOn = _ON;
		return ParameterVar.IncEncErrorMonitOn;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_IncEncErrorMonitOff()
{
        ParameterVar.IncEncErrorMonitOn = _OFF;
        return ParameterVar.IncEncErrorMonitOn;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetParam_IncEncErrorMonit()
{
		return ParameterVar.IncEncErrorMonitOn;
};

//---------------------------------------------------------------------------
// Steuergerät - Fehler-Überwachung
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_ControllerErrorMonitOn()
{
		ParameterVar.ControllerErrorMonitOn = _ON;
		return ParameterVar.ControllerErrorMonitOn;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_ControllerErrorMonitOff()
{
		ParameterVar.ControllerErrorMonitOn = _OFF;
		return ParameterVar.ControllerErrorMonitOn;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetParam_ControllerErrorMonit()
{
		return ParameterVar.ControllerErrorMonitOn;
};

//---------------------------------------------------------------------------
// Steuergerät - Fehler-Überwachung in der langsamen Fahrstufe aktiv
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_ControllerErrorMonitAtSMOn() {
	ParameterVar.ControllerErrorMonitOnAtSM = _ON;
	return ParameterVar.ControllerErrorMonitOnAtSM;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_ControllerErrorMonitAtSMOff() {
	ParameterVar.ControllerErrorMonitOnAtSM = _OFF;
	return ParameterVar.ControllerErrorMonitOnAtSM;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetParam_ControllerErrorMonitAtSM() {
	return ParameterVar.ControllerErrorMonitOnAtSM;
}

//---------------------------------------------------------------------------

// Steuergerät - Fehler-Überwachung
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_ControllerErrorLactiveOn() {
	ParameterVar.ControllerErrorLactive = _ON;
	return ParameterVar.ControllerErrorLactive;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_ControllerErrorLactiveOff(){
	ParameterVar.ControllerErrorLactive = _OFF;
	return ParameterVar.ControllerErrorLactive;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetParam_ControllerErrorLactive(){
	return ParameterVar.ControllerErrorLactive;
}

//---------------------------------------------------------------------------
// Zugkraftwert für Kalibrierung, der mit der Spannung aus der ersten Messung korrespondiert
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_CalibTractionValue1(int Value)
{
//        if (!CalibTractionValue1_Locked) ParameterVar.CalibTractionValue1 = Value;
		ParameterVar.CalibTractionValue1 = Value;
		return ParameterVar.CalibTractionValue1;
};

int WCVarControl::GetParam_CalibTractionValue1()
{
		return ParameterVar.CalibTractionValue1;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_CalibTractionValue1(int Value, int *Recommendation)
{
        if (Value < LIM_PAR_CalibTractionValue1_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_CalibTractionValue1_MIN;
                return false;
        };

        if (Value > LIM_PAR_CalibTractionValue1_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_CalibTractionValue1_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;

};


//---------------------------------------------------------------------------
// Zugkraftwert für Kalibrierung, der mit der Spannung aus der zweiten Messung korrespondiert
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_CalibTractionValue2(int Value)
{
//        if (!CalibTractionValue2_Locked) ParameterVar.CalibTractionValue2 = Value;
        ParameterVar.CalibTractionValue2 = Value;
        return ParameterVar.CalibTractionValue2;
};

int WCVarControl::GetParam_CalibTractionValue2()
{
        return ParameterVar.CalibTractionValue2;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_CalibTractionValue2(int Value, int *Recommendation)
{
		if (Value < LIM_PAR_CalibTractionValue2_MIN)
		{
				if (Recommendation != 0) *Recommendation = LIM_PAR_CalibTractionValue2_MIN;
				return false;
		};

		if (Value > LIM_PAR_CalibTractionValue2_MAX)
		{
				if (Recommendation != 0) *Recommendation = LIM_PAR_CalibTractionValue2_MAX;
				return false;
		};

		if (Recommendation != 0) *Recommendation = Value;
		return true;

};
//---------------------------------------------------------------------------
// Der jeweilig mit der Zugkraft korrespondierende Messwert
// Rückgabewert: der gesetzte Wert, bei ungültigem Index 0
int	WCVarControl::SetParam_CalibTension_MeasuringVal(int Idx, int Value)
{
	if ((Idx < 0) || Idx >= CalibTension_NumOfCalMeasuringPoints) return 0;
	ParameterVar.CalibTension_MeasuringVal[Idx] = Value;
	return ParameterVar.CalibTension_MeasuringVal[Idx];
}

// Rückgabewert: der gesetzte Wert, bei ungültigem Index 0
int WCVarControl::GetParam_CalibTension_MeasuringVal(int Idx)
{
	if ((Idx < 0) || Idx >= CalibTension_NumOfCalMeasuringPoints) return 0;
	return ParameterVar.CalibTension_MeasuringVal[Idx];
}

//---------------------------------------------------------------------------
// Die jeweilig mit dem Messwert korrespondierende Zugkraft
// Bei ungültigem Wert Value wird keine Veränderung vorgenommen
// Rückgabewert: der gesetzte Wert, bei ungültigem Index 0,
int	WCVarControl::SetParam_CalibTension_Tension(int Idx, int Value)
{
	if ((Idx < 0) || Idx >= CalibTension_NumOfCalMeasuringPoints) return 0;

	if ((Value >= LIM_PAR_MinTraction_MIN) && (Value <= LIM_PAR_MaxTraction_MAX)) {
		ParameterVar.CalibTension_Tension[Idx] = Value;
	}

	return ParameterVar.CalibTension_Tension[Idx];
}

// Rückgabewert: der gesetzte Wert, bei ungültigem Index 0
int WCVarControl::GetParam_CalibTension_Tension(int Idx)
{
	if ((Idx < 0) || Idx >= CalibTension_NumOfCalMeasuringPoints) return 0;
    return ParameterVar.CalibTension_Tension[Idx];
}

//---------------------------------------------------------------------------
// Anstieg der Kennlinie des T.-Sensors des Teufenrechners in °C/V
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_CalibTSensorDepthCalc_CaractSlope(float Value)
{
		ParameterVar.CalibTSensorDepthCalc_CaractSlope = Value;
		return ParameterVar.CalibTSensorDepthCalc_CaractSlope;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_CalibTSensorDepthCalc_CaractSlope()
{
        return ParameterVar.CalibTSensorDepthCalc_CaractSlope;
};

//---------------------------------------------------------------------------
// Nulldurchgang der Kennlinie des T.-Sensors des Teufenrechners in °C/V
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_CalibTSensorDepthCalc_CaractZero(float Value)
{
        ParameterVar.CalibTSensorDepthCalc_CaractZero = Value;
        return ParameterVar.CalibTSensorDepthCalc_CaractZero;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_CalibTSensorDepthCalc_CaractZero()
{
        return ParameterVar.CalibTSensorDepthCalc_CaractZero;
};

//---------------------------------------------------------------------------
// Anstieg der Kennlinie des T.-Sensors A in °C/V
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_CalibTSensorA_CaractSlope(float Value)
{
        ParameterVar.CalibTSensorA_CaractSlope = Value;
        return ParameterVar.CalibTSensorA_CaractSlope;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_CalibTSensorA_CaractSlope()
{
        return ParameterVar.CalibTSensorA_CaractSlope;
};

//---------------------------------------------------------------------------
// Nulldurchgang der Kennlinie des T.-Sensors A in °C
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_CalibTSensorA_CaractZero(float Value)
{
        ParameterVar.CalibTSensorA_CaractZero = Value;
        return ParameterVar.CalibTSensorA_CaractZero;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_CalibTSensorA_CaractZero()
{
        return ParameterVar.CalibTSensorA_CaractZero;
};

//---------------------------------------------------------------------------
// Anstieg der Kennlinie des T.-Sensors B in °C/V
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_CalibTSensorB_CaractSlope(float Value)
{
        ParameterVar.CalibTSensorB_CaractSlope = Value;
        return ParameterVar.CalibTSensorB_CaractSlope;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_CalibTSensorB_CaractSlope()
{
        return ParameterVar.CalibTSensorB_CaractSlope;
};

//---------------------------------------------------------------------------
// Nulldurchgang der Kennlinie des T.-Sensors B in °C
// Rückgabewert: der gesetzte Wert
float WCVarControl::SetParam_CalibTSensorB_CaractZero(float Value)
{
        ParameterVar.CalibTSensorB_CaractZero = Value;
        return ParameterVar.CalibTSensorB_CaractZero;
};

// Rückgabewert: der gesetzte Wert
float WCVarControl::GetParam_CalibTSensorB_CaractZero()
{
        return ParameterVar.CalibTSensorB_CaractZero;
};

//---------------------------------------------------------------------------
// Programmautomatik Ein/Aus
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_AutoRunProgramableOn()
{
    ParameterVar.AutoRunProgramableOn = _ON;
    return ParameterVar.AutoRunProgramableOn;
}
bool WCVarControl::SetParam_AutoRunProgramableOff()
{
    ParameterVar.AutoRunProgramableOn = _OFF;
    return ParameterVar.AutoRunProgramableOn;
}

bool WCVarControl::GetParam_AutoRunProgramableOn()
{
    return ParameterVar.AutoRunProgramableOn;
}

//---------------------------------------------------------------------------
// Dateiname für eingestelltes Ablaufprogramm für die Programmautomatik
wchar_t *WCVarControl::SetParam_AutoRunProgramFileName(wchar_t *Name)
{
    if (Name != 0) {
        try {

            wcsncpy(ParameterVar.AutoRunProgramFileName, Name, LIM_PAR_AutoRunPrgFileNameLength);
        } catch (...) {
            ParameterVar.AutoRunProgramFileName[0] = 0;  // bei Fehler wird String aus Anwendersicht geleert
        }
    }

    return ParameterVar.AutoRunProgramFileName;
}

wchar_t *WCVarControl::GetParam_AutoRunProgramFileName()
{
	return ParameterVar.AutoRunProgramFileName;
}

//---------------------------------------------------------------------------
// Darstellung der Geschwindigkeit (0=1m/h|1=1m/min)
// Rückgabewert: der gesetzte Wert
int	WCVarControl::SetParam_VelocityDimensionUnit(int Value)
{
	ParameterVar.VelocityDimensionUnit = Value;
	return ParameterVar.VelocityDimensionUnit;
}

int	WCVarControl::GetParam_VelocityDimensionUnit()
{
	return ParameterVar.VelocityDimensionUnit;
}

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_VelocityDimensionUnit(int Value, int *Recommendation)
{
	if (Value < LIM_PAR_VelocityDimensionUnit_MIN){
		if (Recommendation != 0) *Recommendation = LIM_PAR_VelocityDimensionUnit_MIN;
		return false;
	};

	if (Value > LIM_PAR_VelocityDimensionUnit_MAX){
		if (Recommendation != 0) *Recommendation = LIM_PAR_VelocityDimensionUnit_MAX;
		return false;
	};

	if (Recommendation != 0) *Recommendation = Value;
	return true;
}

//---------------------------------------------------------------------------
// Windenbremse intern automatisch
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_WinchBrakeInternAutoOn()
{
	ParameterVar.WinchBrakeInternAuto = _ON;
	return ParameterVar.WinchBrakeInternAuto;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_WinchBrakeInternAutoOff()
{
	ParameterVar.WinchBrakeInternAuto = _OFF;
	return ParameterVar.WinchBrakeInternAuto;
}

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetParam_WinchBrakeInternAuto()
{
	return ParameterVar.WinchBrakeInternAuto;
}

//---------------------------------------------------------------------------
// Lösen der Windenbremse bei interner Automatikbremse in Prozent
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_WinchBrakeInternAutoReleasePerc(int Value)
{
	ParameterVar.WinchBrakeInternAutoReleasePerc = Value;
	return ParameterVar.WinchBrakeInternAutoReleasePerc;
}

// Rückgabewert: der gesetzte Wert
int	WCVarControl::GetParam_WinchBrakeInternAutoReleasePerc()
{
	return ParameterVar.WinchBrakeInternAutoReleasePerc;
}

//---------------------------------------------------------------------------
// Hysterese Lösen/Feststellen der Windenbremse bei interner Automatikbremse in Prozent
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_WinchBrakeInternAutoReleaseHysteresisPerc(int Value)
{
	ParameterVar.WinchBrakeInternAutoReleaseHysteresisPerc = Value;
	return ParameterVar.WinchBrakeInternAutoReleaseHysteresisPerc;
}

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_WinchBrakeInternAutoReleaseHysteresisPerc()
{
	return ParameterVar.WinchBrakeInternAutoReleaseHysteresisPerc;
}

//---------------------------------------------------------------------------
// Zugkrafteinheit für die Protokollierung
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_LOGFileTensionUnit(int Value)
{
		 ParameterVar.LOGFileTensionUnit = Value;
		 return ParameterVar.LOGFileTensionUnit;
}

// Rückgabewert: der gesetzte Wert
int	WCVarControl::GetParam_LOGFileTensionUnit()
{
		 return ParameterVar.LOGFileTensionUnit;
}

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_LOGFileTensionUnit(int Value, int *Recommendation)
{
		if (Value < LIM_PAR_LOGFileTensionUnit_MIN)
        {
				if (Recommendation != 0) *Recommendation = LIM_PAR_LOGFileTensionUnit_MIN;
                return false;
        };

		if (Value > LIM_PAR_LOGFileTensionUnit_MAX)
		{
				if (Recommendation != 0) *Recommendation = LIM_PAR_LOGFileTensionUnit_MAX;
                return false;
        };

}

//---------------------------------------------------------------------------
// Datei-Typ der LOG-Datei
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_LOGFileType(int Value)
{
		ParameterVar.LOGFileType = Value;
		return ParameterVar.LOGFileType;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_LOGFileType()
{
		return ParameterVar.LOGFileType;
};

// Rückgabewert: true ... "Value" valid; false ... "Value" invalid
// "Recommendation" ... Rückgabe der Empfehlung; ist "Value" valid,dann "*Recommendation = Value"
// "Recommendation" wird ignoriert, wenn Zeiger auf 0
bool WCVarControl::CheckParam_LOGFileType(int Value, int *Recommendation)
{
        if (Value < LIM_PAR_LOGFileType_MIN)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_LOGFileType_MIN;
                return false;
        };

        if (Value > LIM_PAR_LOGFileType_MAX)
        {
                if (Recommendation != 0) *Recommendation = LIM_PAR_LOGFileType_MAX;
                return false;
        };

        if (Recommendation != 0) *Recommendation = Value;
        return true;
};

//---------------------------------------------------------------------------
// Modus, wie die Sondersignale ausgewertet werden sollen (welche Version der Abarbeitung)
// Rückgabewert: der gesetzte Wert
int WCVarControl::SetParam_SpecialSignalMode(int Value)
{
		ParameterVar.SpecialSignalMode = Value;
		return ParameterVar.SpecialSignalMode;
};

// Rückgabewert: der gesetzte Wert
int WCVarControl::GetParam_SpecialSignalMode()
{
		return ParameterVar.SpecialSignalMode;
};

// Rückgabewert:
// false ... Value ungültig ( Wert in Recommendation verwenden empfohlen),
// true ... Value im erlaubten Bereich
bool WCVarControl::CheckParam_SpecialSignalMode(int Value, int *Recommendation)
{
		if (Value < LIM_PAR_SpecialSignalMode_MIN)
		{
				if (Recommendation != 0) *Recommendation = LIM_PAR_SpecialSignalMode_MIN;
				return false;
		};

		if (Value > LIM_PAR_SpecialSignalMode_MAX)
		{
				if (Recommendation != 0) *Recommendation = LIM_PAR_SpecialSignalMode_MAX;
				return false;
		};

		if (Recommendation != 0) *Recommendation = Value;
		return true;
};

//---------------------------------------------------------------------------
// Kabel-/Seilöler verfügbar
// Parameter:	_ON		... Seilöler verfügbar
// 				_OFF	... Seilöler nicht verfügbar
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_LineLubAvailable(bool Value) {
	ParameterVar.LineLubAvailable = Value;
	return ParameterVar.LineLubAvailable;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetParam_LineLubAvailable() {
	return ParameterVar.LineLubAvailable;
};

//---------------------------------------------------------------------------
// Kabel-Seilöler Dauerbetrieb
// Parameter:	_ON		... Seilöler Dauerbetrieb
// 				_OFF	... Seilöler Betrieb nur bei Tastendruck
// Rückgabewert: der gesetzte Wert
bool WCVarControl::SetParam_LineLubPermanentOperation(bool Value) {
	ParameterVar.LineLubPermanentOperation = Value;
	return ParameterVar.LineLubPermanentOperation;
};

// Rückgabewert: der gesetzte Wert
bool WCVarControl::GetParam_LineLubPermanentOperation(){
	return ParameterVar.LineLubPermanentOperation;
};

//---------------------------------------------------------------------------
// Kabel-Seilöler Intensität
// Rückgabewert: der gesetzte Wert
int	WCVarControl::SetParam_LineLubIntensity(int Value) {
	ParameterVar.LineLubIntensity = Value;
	return ParameterVar.LineLubIntensity;
}

// Rückgabewert: der gesetzte Wert
int	WCVarControl::GetParam_LineLubIntensity() {
	return ParameterVar.LineLubIntensity;
}

// Rückgabewert:
// false ... Value ungültig ( Wert in Recommendation verwenden empfohlen),
// true ... Value im erlaubten Bereich
bool WCVarControl::CheckParam_LineLubIntensity(int Value, int *Recommendation) {

	if (Value < LIM_PAR_LineLubIntensity_MIN) {
		if (Recommendation != 0) *Recommendation = LIM_PAR_LineLubIntensity_MIN;
		return false;
	};

	if (Value > LIM_PAR_LineLubIntensity_MAX){
		if (Recommendation != 0) *Recommendation = LIM_PAR_LineLubIntensity_MAX;
		return false;
	};

	if (Recommendation != 0) *Recommendation = Value;
	return true;
}
