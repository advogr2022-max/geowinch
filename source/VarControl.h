/*
WinchControl    Jörg Müller    VarControl.cpp/VarControl.h

Klassendeklaration für die Organisation der Kontroll-
und Informationsvariablen

Portierbarkeit: vollständig
*/
//---------------------------------------------------------------------------
#ifndef VarControlH
#define VarControlH

#include <fstream.h>

#include "constants.h"
#include "structs.h"
//---------------------------------------------------------------------------

// Organisation/Verwaltung der Kontroll- und Informationsvariablen
// Methoden-Nameneinleitung Get->abfragen, Set->setzen
class WCVarControl
{
	private:
		WCControlVarS ControlVar;                               // Kontrollvariablen
		WCStatusInfoVarS StatusInfoVar;                         // Statusinformationen zur Teufenfahrt
		WCParameterVarS ParameterVar;                           // Einstellvariablen

		// Status-Flags
		bool InitOK;                                            // Gibt an, ob mit der Klasseninstanz gearbeitet werden kann (true)

		char *DataFileName;                                     // Dateiname der Datendatei
		char *InternalDataFileName;                             // Dateiname der internen Datendatei
		char *TempDataFileName;                                 // temporärer Dateiname der Datendatei
		char *BackupDataFileName;                               // Name der Backup-Datendatei

		// Handles
		ifstream InFi;                                          // Lese - Handle des Parameterdatei-Streams
		ofstream OuFi;                                          // Schreib - Handle des Parameterdatei-Streams
		ifstream InternalInFi;                                  // Lese - Handle des speziellen separaten Parameterdatei-Streams (für Entwicklung)
		ofstream InternalOuFi;                                  // Schreib - Handle des speziellen separaten Parameterdatei-Streams (für Entwicklung)

	protected:
		// Dateiarbeit
		bool OpenFile(bool read);                               // Öffnet eine bestehende Parameter-Datei und erstellt bei Bedarf eine neue
		bool OpenInternalFile(bool read);                       // Öffnet eine bestehende spezielle separate spezielle Parameter-Datei und erstellt bei Bedarf eine neue (für Entwicklung)
		void CloseFile();                                       // Schließt die momentan geöffnete Datei
		bool new_ReadData();                                    // Einlesen der Parameter aus der Parameterdatei

		bool FileBackup();                                      // Kopiert die Parameterdatei in eine Sicherungsdatei
		bool ApplyTempDataFile(bool OK);                        // Wendet die temporäre Datendatei bei Erfolg an, oder entfernt sie bei Fehler

		// Verwaltung
		void SetStandards();                                    // Standardwerte setzen

	public:
		WCVarControl();                                         // Konstruktor
		~WCVarControl();                                        // Destruktor

		// Initialisierung
		bool Init(char *filename, char *tmpfilename,  char *backupfile, char *internalfilename);  // Initialisieren der Klasse und setzen von Standardwerten
		void CleanUp();                                         // Aufräumen

		// Schreibaufforderung
		bool WriteData();                                       // schreibt Daten in die Parameter-Datei
		bool WriteInternalData();                               // schreibt die Speziellen Parameter-Daten (für Entwicklung) in eine separate Parameter-Datei

		// Status-Abfragen
		bool Is_Initialized();                                  // Gibt den Status der Initialisierung zurück
		bool Is_File_Writable();                                // Gibt an, ob die Parameter-Datei beschreibbar ist
		bool Is_File_Readable();                                // Gibt an, ob die Parameter-Datei auslesbar ist

		// Verwalten der Werte

		// ControlVar
		// ----------
		// Entwicklermodus ein/aus
		bool	SetControl_DevelOn();
		bool    SetControl_DevelOff();
		bool    devel();

        // Automatikbetrieb
		bool    SetControl_AutoRunOn();
		bool    SetControl_AutoRunOff();
		bool    GetControl_AutoRunOn();
		bool    AutoRunOn_Locked;                       // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

		// Winde
		bool    SetControl_WinchOn();
		bool    SetControl_WinchOff();
		bool    GetControl_WinchWork();
		bool    WinchWork_Locked;                       // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

		// momentane Zugkraft in Abhängigkeit der Teufe berechnen
		bool    SetControl_CalcCurrTractionsOn();
		bool    SetControl_CalcCurrTractionsOff();
		bool    GetControl_CalcCurrTractions();
		bool    CalcCurrTractions_Locked;               // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

		// momentane Zugkraft in Abhängigkeit der Teufe berechnen tatsächlich aktiv
		bool    SetControl_CalcCurrTractionsActiveOn();
		bool    SetControl_CalcCurrTractionsActiveOff();
		bool    GetControl_CalcCurrTractionsActive();

		// Verzögerung des Ausschaltens vom Steuergerät bei Zugkraft-/ Teufenüberschreitung (gleichzeitig Haltedauer der Mittenspannung durch den STR)
		bool    SetControl_ThresholdExceedenceStopDelayOn();
		bool    SetControl_ThresholdExceedenceStopDelayOff();
		bool    GetControl_ThresholdExceedenceStopDelay();

		// Freigabe nach Grenzwertüberschreitung
		bool    SetControl_ClearAfterThresholdDepthOn();
		bool    SetControl_ClearAfterThresholdDepthOff();
		bool    GetControl_ClearAfterThresholdDepth();

		// Freigabe nach Grenzwertüberschreitung
		bool    SetControl_ClearAfterThresholdTractionOn();
		bool    SetControl_ClearAfterThresholdTractionOff();
		bool    GetControl_ClearAfterThresholdTraction();
		bool    Reset_Locked;             				// Gibt an, ob die Einstellung möglich ist (true) oder nicht (false) für Zugkraft und Teufe

		// Bestätigen von ausgewählten durch Sondersignale ausgelösten Fehlern
		bool    SetControl_ResetSpecialSignalError1FlagOn();
		bool    SetControl_ResetSpecialSignalError1FlagOff();
		bool    GetControl_ResetSpecialSignalError1Flag();

		// Bestätigen von ausgewählten durch Sondersignale ausgelösten Fehlern
		bool    SetControl_ResetSpecialSignalError2FlagOn();
		bool    SetControl_ResetSpecialSignalError2FlagOff();
		bool    GetControl_ResetSpecialSignalError2Flag();

		// Schnelle/Langsame Fahrt
		bool    SetControl_FastRunOn();
		bool    SetControl_FastRunOff();
		bool    GetControl_FastRun();
		bool    FastRun_Locked;                         // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

    	// "Motor ein"
		bool    SetControl_EngineStartOn();             // "Motor ein" - Signal ein
		bool    SetControl_EngineStartOff();            // "Motor ein" - Signal aus
		bool    GetControl_EngineStart();
		bool    EngineStart_Locked;                     // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

    	// "Motor aus"
		bool    SetControl_EngineStopOn();              // "Motor aus" - Signal ein
		bool    SetControl_EngineStopOff();             // "Motor aus" - Signal aus
		bool    GetControl_EngineStop();
		bool    EngineStop_Locked;                      // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

		// Gibt an, ob die Motordrehzahl reduziert oder erhöht werden soll
		int     SetControl_EngineRevolutionUp();
		int     SetControl_EngineRevolutionDown();
		int     SetControl_EngineRevolutionHold();
		int     GetControl_EngineRevolutionUpDown();

    	// Gibt an, ob die Motordrehzahl reduziert oder erhöht werden soll
		int     SetControl_EngineRevolutionAutoUp();
		int     SetControl_EngineRevolutionAutoDown();  // Automatik, nur für programminterne Steuerung, nicht Anwendergesteuert
		int     SetControl_EngineRevolutionAutoHold();
		int     GetControl_EngineRevolutionAutoUpDown();

		// Hallo über das Signal WinchClear zur externen Steuerelektronik (forciertes kurzes Auslösen des Signals)
		bool	SetControl_Hello_WinchClearOn();
		bool	SetControl_Hello_WinchClearOff();
		bool	GetControl_Hello_WinchClear();

		// Magnetmarke ein/aus
		bool    SetControl_MagneticMarkOn();
		bool    SetControl_MagneticMarkOff();
		bool    GetControl_MagneticMarkOn();
		bool    MagneticMarkOn_Locked;                  // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

		// MM-Zwangssynchronisation aktiv (true) oder inaktiv (false)
		bool	SetControl_MMSyncForceActiveOn();
		bool	SetControl_MMSyncForceActiveOff();
		bool	GetControl_MMSyncForceActiveOn();

		// MM Teufenzähler für MM-Zwangssynchronisation
		int		SetControl_MMSyncForceCounter(int Value);
		int		GetControl_MMSyncForceCounter();

		// Kabel entladen ein/aus
		bool    SetControl_UnloadCableOn();
		bool    SetControl_UnloadCableOff();
		bool    GetControl_UnloadCable();
		bool    UnloadCable_Locked;                  	// Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

		// Passiv Kabel entladen ein/aus
		bool    SetControl_UnloadCableActiveOn();
		bool    SetControl_UnloadCableActiveOff();
		bool    GetControl_UnloadCableActive();
		bool    UnloadCableActive_Locked;            	// Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

		// Aktiv Kabel entladen ein/aus
		bool    SetControl_UnloadCablePassiveOn();
		bool    SetControl_UnloadCablePassiveOff();
		bool    GetControl_UnloadCablePassive();
		bool    UnloadCablePassive_Locked;            	// Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

		// Kalibrieren ein/aus
		bool    SetControl_CalibrateOn();
		bool    SetControl_CalibrateOff();
		bool    GetControl_CalibrateOn();
		bool    CalibrateOn_Locked;                     // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

		// Kalibrieren des Teufenrechners
		bool    SetControl_CalibrateDepthOn();
		bool    SetControl_CalibrateDepthOff();
		bool    GetControl_CalibrateDepth();
		bool    CalibrateDepth_Locked;                  // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

		// Kalibrieren des Moduls zur Zugkraftüberwachung
		bool	SetControl_CalibrateTractionOn();
		bool    SetControl_CalibrateTractionOff();
		bool    GetControl_CalibrateTraction();
		bool    CalibrateTraction_Locked;               // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

		// Gibt an, ob die Software-Version des Steuergeräts ausgelesen werden soll
		bool    SetControl_GetCtlUnitSWVersionOn();
		bool    SetControl_GetCtlUnitSWVersionOff();
		bool    GetControl_GetCtlUnitSWVersion();

		// Gibt an, ob die Software-Version des Teufenrechners ausgelesen werden soll
		bool    SetControl_GetDepthCalcSWVersionOn();
		bool    SetControl_GetDepthCalcSWVersionOff();
		bool    GetControl_GetDepthCalcSWVersion();
		bool    GetDepthCalcSWVersion_Locked;           // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

		// Freilauf-Impuls ein/aus
		bool    SetControl_LostMotionImpulseOn();
		bool    SetControl_LostMotionImpulseOff();
		bool    GetControl_LostMotionImpulse();
		bool    LostMotionImpulse_Locked;               // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

		// Startteufe im Steuergerät setzen ein/aus
		bool    SetControl_SetDepth_On();
		bool    SetControl_SetDepth_Off();
		bool    GetControl_SetDepth();
		bool    SetControl_SetDepth_DC_On();            // Startteufe im Teufenrechner setzen ein/aus
		bool    SetControl_SetDepth_DC_Off();
		bool    GetControl_SetDepth_DC();
		bool    SetDepth_DC_Locked;

    	// Parameter an Teufenrechner übergeben ein/aus
		bool    SetControl_SetDepthCalcParamOn();
		bool    SetControl_SetDepthCalcParamOff();
		bool    GetControl_SetDepthCalcParam();
		bool    SetDepthCalcParam_Locked;

		// Kalibrierfaktor im Steuergerät setzen ein/aus
		bool    SetControl_SetDepthCalFactor_On();
		bool    SetControl_SetDepthCalFactor_Off();
		bool    GetControl_SetDepthCalFactor();
		bool    SetControl_SetDepthCalFactor_DC_On();   // Eichfaktor im Teufenrechner setzen ein/aus
		bool    SetControl_SetDepthCalFactor_DC_Off();
		bool    GetControl_SetDepthCalFactor_DC();
		bool    SetDepthCalFactor_DC_Locked;

		// cm-Impuls-Format im Teufenrechner setzen ein/aus
		bool    SetControl_SetFormatCMImpulseOn();
		bool    SetControl_SetFormatCMImpulseOff();
		bool    GetControl_SetFormatCMImpulse();

		// Magnetmarke im Teufenrechner setzen ein/aus
		bool    SetControl_SetMagneticMarkOn();
		bool    SetControl_SetMagneticMarkOff();
		bool    GetControl_SetMagneticMark();

		// Bremse der Winde lösen
		bool    SetControl_WinchBrakeOff_On();
		bool    SetControl_WinchBrakeOff_Off();
		bool    GetControl_WinchBrakeOff();
		bool    WinchBrakeOff_Locked;

		// Impuls zum Steuern "Bremse der Winde lösen"
		bool    SetControl_WinchBrakeOffImpulse_On();
		bool    SetControl_WinchBrakeOffImpulse_Off();
		bool    GetControl_WinchBrakeOffImpulse();

		// Signal-Horn
		bool    SetControl_SignalHornOn();
		bool    SetControl_SignalHornOff();
		bool    GetControl_SignalHornOn();

    	// Spannungswert 1 für Kalibrierung Zugkraftmesser
		bool	SetControl_CalibTension_GetVal1On();
		bool    SetControl_CalibTension_GetVal1Off();
		bool    GetControl_CalibTension_GetVal1();
		bool    CalibTension_GetVal1_Locked;

		// Spannungswert 2 für Kalibrierung Zugkraftmesser
		bool    SetControl_CalibTension_GetVal2On();
		bool    SetControl_CalibTension_GetVal2Off();
		bool    GetControl_CalibTension_GetVal2();
		bool    CalibTension_GetVal2_Locked;

		// Parameter für Zugkraftmesser berechnen
		bool    SetControl_CalibTractionCalculateParamOn();
		bool    SetControl_CalibTractionCalculateParamOff();
		bool    GetControl_CalibTractionCalculateParam();
		bool    CalibTractionCalculateParam_Locked;

		// Parameter für Zugkraftmesser übernehmen
		bool    SetControl_CalibTractionSetParamOn();
		bool    SetControl_CalibTractionSetParamOff();
		bool    GetControl_CalibTractionSetParam();
		bool    CalibTractionSetParam_Locked;

		// Nullabgleich Zugkraftmesser
		bool    SetControl_CalibTractionSetZeroOn();
		bool    SetControl_CalibTractionSetZeroOff();
		bool    GetControl_CalibTractionSetZero();
		bool    CalibTractionSetZero_Locked;

		// Sonderfunktion 1
		bool 	SetControl_SpecialFunc1On();
		bool 	SetControl_SpecialFunc1Off();
		bool 	GetControl_SpecialFunc1();

		// Stromversorgung STR über Schlüsselschalter/Zündung abgeschaltet
		bool    SetControl_PowerFailOn();
		bool    SetControl_PowerFailOff();
		bool    GetControl_PowerFail();

		// StatusInfoVar
		// -------------
		// Momentane Teufe
		int     SetInfo_CurrDepth(int Value);
		int     GetInfo_CurrDepth();

		// Momentane Fahrtrichtung
		bool    SetInfo_CurrDirectionUp();
		bool    SetInfo_CurrDirectionDown();
		bool    GetInfo_CurrDirection();

		// Momentane Sondengeschwindigkeit
		int     SetInfo_CurrVelocity(int Value);
		int     GetInfo_CurrVelocity();

						int             SetInfo_AnnouncedVelocity(int Value);   // Angezeigte Sondengeschwindigkeit
						int             GetInfo_AnnouncedVelocity();

						int             SetInfo_CurrWinchRevMessVoltage(int Value); // Gemessene momentane Windengeschwindigkeitssteuer-Spannung in mV
						int             GetInfo_CurrWinchRevMessVoltage();

						int 			SetInfo_CurrWinchBrakeInternAutoStatus(int Value);// Status der interne Automatischen Windenbremse (0-Automatik inaktiv, 1-angebremst, 2-geloest, 3-Bremsen erwartet)
						int 			GetInfo_CurrWinchBrakeInternAutoStatus();

						int             SetInfo_CurrTensionLimit_Upper(int Value);// Momentane Maximalzugkraft
						int             GetInfo_CurrTensionLimit_Upper();

						int             SetInfo_CurrTensionLimit_Lower(int Value);// Momentane Minimalzugkraft
						int             GetInfo_CurrTensionLimit_Lower();

						int             SetInfo_CurrTraction(int Value);        // Momentane Zugkraft
						int             GetInfo_CurrTraction();                 // Momentane Zugkraft

						int             SetInfo_CurrTractionDifference(int Value); // Momentane Zugkraft-Differenz
						int             GetInfo_CurrTractionDifference();       // Momentane Zugkraft-Differenz

						int             SetInfo_CurrTimeDifference(int Value);  // Momentane Zeit-Differenz
						int             GetInfo_CurrTimeDifference();           // Momentane Zeit-Differenz

						int             SetInfo_CurrTractionVoltage(int Value); // Momentaner Spannungswert vom Zugkraftmesser
						int             GetInfo_CurrTractionVoltage();

						bool            SetInfo_MagneticMarkOn();               // Auftreten einer Magnetmarke
						bool            SetInfo_MagneticMarkOff();
						bool            GetInfo_MagneticMark();

						bool            SetInfo_MagneticMarkStaticOn();         // Auftreten einer Magnetmarke statisch für Darstellung und Protokollierung
						bool            SetInfo_MagneticMarkStaticOff();
						bool            GetInfo_MagneticMarkStatic();

						bool            SetInfo_MagneticMarkStaticLOGOn();      // Auftreten einer Magnetmarke statisch für Protokollierung
						bool            SetInfo_MagneticMarkStaticLOGOff();
						bool            GetInfo_MagneticMarkStaticLOG();

						bool            SetInfo_MagneticMarkDirectOn();         // Auftreten einer Magnetmarke, direkter Zustand(nicht nur Trigger, wie "MagneticMark")
						bool            SetInfo_MagneticMarkDirectOff();
						bool            GetInfo_MagneticMarkDirect();

						bool            SetInfo_IncEncErrorOn();                // Inkremental-Geber-Fehler
						bool            SetInfo_IncEncErrorOff();
						bool            GetInfo_IncEncError();

						bool            SetInfo_ControllerErrorOn();            // Steuergerät Fehler-Signalisierung
						bool            SetInfo_ControllerErrorOff();
						bool            GetInfo_ControllerError();

						bool            SetInfo_LostMotionOn();                 // Freilauf ein/aus
						bool            SetInfo_LostMotionOff();
						bool            GetInfo_LostMotion();

						int				SetInfo_EngineCoolantTemp(int Value);	// Antriebsmotorkühlmitteltemperatur
						int				GetInfo_EngineCoolantTemp();

						bool            SetInfo_EngineOn();                     // Motor ein
						bool            SetInfo_EngineOff();                    // Motor aus
						bool            GetInfo_EngineOn();

						int				SetInfo_EngineSpeed(int Value);			// Antriebsmotordrehzahl
						int				GetInfo_EngineSpeed();

						bool            SetInfo_OilStrainerOn();                // Ölfilter wechseln ein/aus
						bool            SetInfo_OilStrainerOff();
						bool            GetInfo_OilStrainer();

						bool            SetInfo_OilStrainer2On();               // Ölfilter 2 wechseln ein/aus
						bool            SetInfo_OilStrainer2Off();
						bool            GetInfo_OilStrainer2();

						bool            SetInfo_HydraulicOKOn();             	// Hydraulik OK ist ein/aus
						bool            SetInfo_HydraulicOKOff();
						bool            GetInfo_HydraulicOK();

						int             SetInfo_HydrWrkPressure(int Value);     // Aktueller Arbeitsdruck der Hydraulik
						int             GetInfo_HydrWrkPressure();

						int             SetInfo_HydrChargePressure(int Value); 	// Speisedruck der Hydraulik
						int             GetInfo_HydrChargePressure();

		// Aktueller Hydraulikfüllstand
		int		SetInfo_HydrLvl(int Value);
		int     GetInfo_HydrLvl();

						bool            SetInfo_EmergencySwitchOn();            // Not-Schalter betätigt ein/aus
						bool            SetInfo_EmergencySwitchOff();
						bool            GetInfo_EmergencySwitch();

						int             SetInfo_CurrTractionReference(int Value);// Momentane Soll-Zugkraft
						int             GetInfo_CurrTractionReference();

                        int             SetInfo_ProgramMode(int Value);         // Aktueller Programm-Modus
                        int             GetInfo_ProgramMode();

                        bool            SetInfo_DepthThresholdExceededOn();     // Teufenbereichsgrenzen überschritten
                        bool            SetInfo_DepthThresholdExceededOff();
                        bool            GetInfo_DepthThresholdExceeded();

                        bool            SetInfo_TractionThresholdExceededOn();  // Zugkraftgrenze überschritten
                        bool            SetInfo_TractionThresholdExceededOff();
                        bool            GetInfo_TractionThresholdExceeded();

                        bool            SetInfo_LeverOn();                      // Fahrhebelbetätigung
                        bool            SetInfo_LeverOff();
                        bool            GetInfo_LeverOn();

                        bool            SetInfo_HWCtrlSignalisationOn();        // tatsächlicher Hardware-Kontroll-Status
                        bool            SetInfo_HWCtrlSignalisationOff();
                        bool            GetInfo_HWCtrlSignalisation();

                        bool            SetInfo_AutomotiveErrorOn();            // Fahrzeugfehler
                        bool            SetInfo_AutomotiveErrorOff();
                        bool            GetInfo_AutomotiveError();

                        bool            SetInfo_SpecialSignal01On();            // Konfigurierbares Sondersignal
                        bool            SetInfo_SpecialSignal01Off();
                        bool            GetInfo_SpecialSignal01();

                        bool            SetInfo_SpecialSignal02On();            // Konfigurierbares Sondersignal
                        bool            SetInfo_SpecialSignal02Off();
                        bool            GetInfo_SpecialSignal02();

						bool            SetInfo_SpecialSignalExtBrakeOn();      // Durch ein Sondersignal überwachte externe Bremse
						bool            SetInfo_SpecialSignalExtBrakeOff();
						bool            GetInfo_SpecialSignalExtBrake();

						bool            SetInfo_CalibTractionValue1OkOn();      // 1. Wert für Zugkraftmessungs-Kalibrierung ist ermittelt
                        bool            SetInfo_CalibTractionValue1OkOff();
                        bool            GetInfo_CalibTractionValue1Ok();
                        bool            CalibTractionValue1Ok_Locked;

                        bool            SetInfo_CalibTractionValue2OkOn();      // 2. Wert für Zugkraftmessung-Kalibrierung ist ermittelt
                        bool            SetInfo_CalibTractionValue2OkOff();
                        bool            GetInfo_CalibTractionValue2Ok();
                        bool            CalibTractionValue2Ok_Locked;

						bool            SetInfo_CalibDepthAllowedOn();          // Gibt an, ob das Kalibrieren der Teufe im Aktuellen Zustand generell möglich ist
						bool            SetInfo_CalibDepthAllowedOff();
						bool            GetInfo_CalibDepthAllowed();

						bool            SetInfo_CalibDepthCableOkOn();          // Für Teufenkalibrierung wurde das Kabel über eine bestimmte Länge gewickelt
						bool            SetInfo_CalibDepthCableOkOff();
						bool            GetInfo_CalibDepthCableOk();

						bool            SetInfo_CtlUnitSWVersion(char *Version); // Pufferspeicher für den Versionsstring des Steuergeräts
						char            *GetInfo_CtlUnitSWVersion();

						bool            SetInfo_DepthCalcSWVersion(char *Version); // Pufferspeicher für den Versionsstring des Teufenrechners
						char            *GetInfo_DepthCalcSWVersion();

						float           SetInfo_TemperatureSensorDepthCalc(float Value); // Enthält die Temperatur des T.-Sensors auf dem Teufenrechner in °C
						float           GetInfo_TemperatureSensorDepthCalc();

						float           SetInfo_TemperatureSensorA(float Value); // Enthält die Temperatur des T.-Sensors A in °C
						float           GetInfo_TemperatureSensorA();

						float           SetInfo_TemperatureSensorB(float Value); // Enthält die Temperatur des T.-Sensors B in °C
						float           GetInfo_TemperatureSensorB();

						int           	SetInfo_TemperatureOilTank(int Value); 	// Öltanktemperatur in °C
						int           	GetInfo_TemperatureOilTank();

						// ParameterVar
						bool			SavableParamChanged; 					// Dieses Flag signalisiert, dass zu speichernde Parameter verändert wurden

						unsigned int    SetParam_WinchNo(unsigned int Value);   // Nummer der aktuellen Winde
						unsigned int    GetParam_WinchNo();
						bool            CheckParam_WinchNo(unsigned int Value, unsigned int *Recommendation);
						bool            WinchNo_Locked;                         // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

		// Hydraulische Bremse aktiv (true) / inaktiv (false)
		bool	SetParam_HydraulicBrakeOn();
		bool	SetParam_HydraulicBrakeOff();
		bool	GetParam_HydraulicBrake();


//                        unsigned int    SetParam_WinchCnt(unsigned int Value);  // Anzahl der zu verwaltenden Winden
//                        unsigned int    GetParam_WinchCnt();
//                        bool            WinchCnt_Locked;                        // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

						int             SetParam_MinDepth(int Value);           // Mindestteufe
						int             GetParam_MinDepth();
						bool            CheckParam_MinDepth(int Value, int *Recommendation);
						bool            MinDepth_Locked;                        // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

						int             SetParam_MaxDepth(int Value);           // Maximalteufe
						int             GetParam_MaxDepth();
						bool            CheckParam_MaxDepth(int Value, int *Recommendation);
						bool            MaxDepth_Locked;                        // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

						int             SetParam_StartDepth(int Value);         // Anfangsteufe
						int             GetParam_StartDepth();
						bool            CheckParam_StartDepth(int Value, int *Recommendation);
						bool            StartDepth_Locked;                      // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

						bool            SetParam_ReverseDepth(bool Value);      // Teufe umkehren
						bool            GetParam_ReverseDepth();
						bool            ReverseDepth_Locked;                    // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

						int             SetParam_Velocity(int Value);           // Soll-Sondengeschwindigkeit
						int             GetParam_Velocity();
						bool            CheckParam_Velocity(int Value, int *Recommendation);
						bool            Velocity_Locked;                        // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

						int             SetParam_Velocity_PrgAutom(int Value);  // Soll-Sondengeschwindigkeit für Programmautomatik
						int             GetParam_Velocity_PrgAutom();
						bool            CheckParam_Velocity_PrgAutom(int Value, int *Recommendation);

						bool            SetParam_DirectionUp();                 // Soll-Fahrtrichtung
						bool            SetParam_DirectionDown();
						bool            GetParam_Direction();
						bool            Direction_Locked;                       // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

                        bool            SetParam_Direction_PrgAutomUp();        // Soll-Fahrtrichtung für Programmautomatik
                        bool            SetParam_Direction_PrgAutomDown();
                        bool            GetParam_Direction_PrgAutom();

                        bool            SetParam_EngineRevolutionChangeableOn();// Drehzahl Fahrzeugmotor veränderbar
                        bool            SetParam_EngineRevolutionChangeableOff();
                        bool            GetParam_EngineRevolutionChangeable();
                        bool            EngineRevolutionChangeable_Locked;

						bool            SetParam_EngineStartStopEnable();       // Antriebsmotor Starten / Stoppen möglich
						bool            SetParam_EngineStartStopDisable();
						bool            GetParam_EngineStartStopEnable();
						bool            EngineStartStopEnable_Locked;

						bool            SetParam_LostMotionEnable();       		// Freilaufbedienung möglich
						bool            SetParam_LostMotionDisable();
						bool            GetParam_LostMotionEnable();
						bool            LostMotionEnable_Locked;

						bool            SetParam_BrakeOffEnable();       		// Bremse abschalten möglich
						bool            SetParam_BrakeOffDisable();
						bool            GetParam_BrakeOffEnable();
						bool            BrakeOffEnable_Locked;

						bool            SetParam_ExternalBrakeAvailableOn();  	// Externe Bremse verfügbar
						bool            SetParam_ExternalBrakeAvailableOff();
						bool            GetParam_ExternalBrakeAvailable();

						int             SetParam_FormatCMImpulse(int Value);    // Gibt das Ausgabeformat der cm-Impulse an
                        int             GetParam_FormatCMImpulse();
                        bool            CheckParam_FormatCMImpulse(int Value, int *Recommendation);
                        bool            FormatCMImpulse_Locked;                 // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

                        int             SetParam_MaxTraction(int Value);        // Maximalzugkraft
                        int             GetParam_MaxTraction();
                        bool            CheckParam_MaxTraction(int Value, int *Recommendation);
                        bool            MaxTraction_Locked;                     // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

                        int             SetParam_MinTraction(int Value);        // Minimalzugkraft
                        int             GetParam_MinTraction();
                        bool            CheckParam_MinTraction(int Value, int *Recommendation);
                        bool            MinTraction_Locked;                     // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

						int             SetParam_RelTensionLimitLower(int Value);// einstellbare untere Zugkraftgerenze, abhängig von der aktuell höchsten Zugkraft (wichtig bei Abwärtsfahrt)
						int             GetParam_RelTensionLimitLower();
						bool            CheckParam_RelTensionLimitLower(int Value, int *Recommendation);

                        int             SetParam_DepthThresholdForAuto(int Value);// Schwellwert für Automatikbetrieb
                        int             GetParam_DepthThresholdForAuto();

                        int             SetParam_DepthDistanceForBreak(int Value);// Teufe vor Endwert, ab wann abgebremst werden soll
                        int             GetParam_DepthDistanceForBreak();

						bool			SetParam_MMSyncForceOn();				// MM-Zwangssynchr. Ein/Aus
						bool			SetParam_MMSyncForceOff();
						bool			GetParam_MMSyncForceOn();
                        bool			MMSyncForceOn_Locked;

						int				SetParam_MMSyncForceRangeLim(int Value);	// MM-Zwangssynchr. Teufenbereichsgrenze in Meter
						int				GetParam_MMSyncForceRangeLim();
						bool			CheckParam_MMSyncForceRangeLim(int Value, int *Recommendation);
						bool            MMSyncForceRangeLim_Locked;

						int             SetParam_UnloadCableTraction(int Value);// Zugkraft beim Entladen des Kabels
						int             GetParam_UnloadCableTraction();
						bool            CheckParam_UnloadCableTraction(int Value, int *Recommendation);
						bool            UnloadCableTraction_Locked;          	// Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

						int             SetParam_UnloadCableMinTraction(int Value);// Mindestzugkraft beim Entladen des Kabels
						int             GetParam_UnloadCableMinTraction();
						bool            UnloadCableMinTraction_Locked;       	// Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

                        bool            SetParam_UnloadCableStretchOn();     	// Kabel entladen mit/ohne stretching
						bool            SetParam_UnloadCableStretchOff();
						bool            GetParam_UnloadCableStretch();
                        bool            UnloadCableStrech_Locked;            	// Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

						int             SetParam_UnloadCableBeginLenght(int Value); // Anfangslänge beim Entladen des Kabels
                        int             GetParam_UnloadCableBeginLenght();
                        bool            CheckParam_UnloadCableBeginLenght(int Value, int *Recommendation);
                        bool            UnloadCableBeginLenght_Locked;

						int             SetParam_UnloadCableEndLenght(int Value); // Anfangslänge beim Entladen des Kabels
						int             GetParam_UnloadCableEndLenght();
                        bool            CheckParam_UnloadCableEndLenght(int Value, int *Recommendation);
                        bool            UnloadCableEndLenght_Locked;

						int             SetParam_UnloadCableStartTraction(int Value);// Anfangszugkraft beim Entladen des Kabels
						int             GetParam_UnloadCableStartTraction();
						bool            CheckParam_UnloadCableStartTraction(int Value, int *Recommendation);
						bool            UnloadCableStartTraction_Locked;     	// Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

						int             SetParam_UnloadCableEndTraction(int Value);// Endzugkraft beim Entladen des Kabels
						int             GetParam_UnloadCableEndTraction();
						bool            CheckParam_UnloadCableEndTraction(int Value, int *Recommendation);
                        bool            UnloadCableEndTraction_Locked;       	// Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

						int             SetParam_RelTensionLimitUpper(int Value); // einstellbare obere Zugkraftgerenze, abhängig von der aktuell niedrigsten Zugkraft (wichtig bei Abwärtsfahrt)
						int             GetParam_RelTensionLimitUpper();
						bool            CheckParam_RelTensionLimitUpper(int Value, int *Recommendation);

/*						float           SetParam_UnloadCable_PID_TD(float Value); // PID-Regler für Zugkraftkontrolle beim Kabel entladen: Vorhaltezeit
						float           GetParam_UnloadCable_PID_TD();
                        bool            CheckParam_UnloadCable_PID_TD(float Value, float *Recommendation);

						float           SetParam_UnloadCable_PID_TD_SAVE(float Value); // PID-Regler für Zugkraftkontrolle beim Kabel entladen: Vorhaltezeit (zum abspeichern)
                        float           GetParam_UnloadCable_PID_TD_SAVE();

						float           SetParam_UnloadCable_PID_TI(float Value); // PID-Regler für Zugkraftkontrolle beim Kabel entladen: Nachstellzeit
						float           GetParam_UnloadCable_PID_TI();
                        bool            CheckParam_UnloadCable_PID_TI(float Value, float *Recommendation);

						float           SetParam_UnloadCable_PID_TI_SAVE(float Value); // PID-Regler für Zugkraftkontrolle beim Kabel entladen: Nachstellzeit (zum abspeichern)
                        float           GetParam_UnloadCable_PID_TI_SAVE();

						float           SetParam_UnloadCable_PID_KR(float Value); // PID-Regler für Zugkraftkontrolle beim Kabel entladen: Verstärkungsfaktor
						float           GetParam_UnloadCable_PID_KR();
                        bool            CheckParam_UnloadCable_PID_KR(float Value, float *Recommendation);

                        float           SetParam_UnloadCable_PID_KR_SAVE(float Value); // PID-Regler für Zugkraftkontrolle beim Kabel entladen: Verstärkungsfaktor (zum abspeichern)
                        float           GetParam_UnloadCable_PID_KR_SAVE();

						float           SetParam_UnloadCable_PID_TD_2(float Value); // schneller PID-Regler für Zugkraftkontrolle beim Kabel entladen: Vorhaltezeit
						float           GetParam_UnloadCable_PID_TD_2();
						bool            CheckParam_UnloadCable_PID_TD_2(float Value, float *Recommendation);

						float           SetParam_UnloadCable_PID_TD_2_SAVE(float Value); // schneller PID-Regler für Zugkraftkontrolle beim Kabel entladen: Vorhaltezeit (zum abspeichern)
						float           GetParam_UnloadCable_PID_TD_2_SAVE();

						float           SetParam_UnloadCable_PID_TI_2(float Value); // schneller PID-Regler für Zugkraftkontrolle beim Kabel entladen: Nachstellzeit
						float           GetParam_UnloadCable_PID_TI_2();
						bool            CheckParam_UnloadCable_PID_TI_2(float Value, float *Recommendation);

						float           SetParam_UnloadCable_PID_TI_2_SAVE(float Value); // schneller PID-Regler für Zugkraftkontrolle beim Kabel entladen: Nachstellzeit (zum abspeichern)
						float           GetParam_UnloadCable_PID_TI_2_SAVE();

						float           SetParam_UnloadCable_PID_KR_2(float Value); // schneller PID-Regler für Zugkraftkontrolle beim Kabel entladen: Verstärkungsfaktor
						float           GetParam_UnloadCable_PID_KR_2();
                        bool            CheckParam_UnloadCable_PID_KR_2(float Value, float *Recommendation);

						float           SetParam_UnloadCable_PID_KR_2_SAVE(float Value); // schneller PID-Regler für Zugkraftkontrolle beim Kabel entladen: Verstärkungsfaktor (zum abspeichern)
						float           GetParam_UnloadCable_PID_KR_2_SAVE();

						int				SetParam_UnloadCable_ExtReg_MaxTension(int Value); // Kabel Entladen mit externem Regler Kennliniendef. Maximalzugkraft in N
						int				GetParam_UnloadCable_ExtReg_MaxTension();
						bool			UnloadCable_ExtReg_MaxTension_Locked;

						int 			SetParam_UnloadCable_ExtReg_MaxVoltage(int Value); // Kabel Entladen mit externem Regler Kennliniendef. Maximalspannung
						int 			GetParam_UnloadCable_ExtReg_MaxVoltage();
						bool			UnloadCable_ExtReg_MaxVoltage_Locked;

						int 			SetParam_UnloadCable_ExtReg_MinTension(int Value); // Kabel Entladen mit externem Regler Kennliniendef. Mindestzugkraft
						int 			GetParam_UnloadCable_ExtReg_MinTension();
						bool			UnloadCable_ExtReg_MinTension_Locked;

						int 			SetParam_UnloadCable_ExtReg_MinVoltage(int Value); // Kabel Entladen mit externem Regler Kennliniendef. Mindestspannung
						int 			GetParam_UnloadCable_ExtReg_MinVoltage();
						bool			UnloadCable_ExtReg_MinVoltage_Locked;

//						int 			SetParam_UnloadCable_ExtReg_MaxPosOffset(int Value); // Kabel Entladen mit externem Regler max. positiver Offset der Kennlinie
//						int 			GetParam_UnloadCable_ExtReg_MaxPosOffset();
//						bool			UnloadCable_ExtReg_MaxPosOffset_Locked;

//						int 			SetParam_UnloadCable_ExtReg_MaxNegOffset(int Value); // Kabel Entladen mit externem Regler max. negativer Offset der Kennlinie
//						int 			GetParam_UnloadCable_ExtReg_MaxNegOffset();
//						bool			UnloadCable_ExtReg_MaxNegOffset_Locked;

//						int 			SetParam_UnloadCable_ExtReg_Delay(int Value); // Kabel Entladen mit externem Regler Verzögerung beim Abfahren (Rampe) in ms
//						int 			GetParam_UnloadCable_ExtReg_Delay();
//						bool			UnloadCable_ExtReg_Delay_Locked;

//						int 			SetParam_UnloadCable_ExtReg_P(int Value); // Kabel Entladen mit externem Regler P-Anteil
//						int 			GetParam_UnloadCable_ExtReg_P();
//						bool			UnloadCable_ExtReg_P_Locked;

						int 			SetParam_UnloadCable_ExtReg_I(int Value); // Kabel Entladen mit externem Regler I-Anteil
						int 			GetParam_UnloadCable_ExtReg_I();
						bool			UnloadCable_ExtReg_I_Locked;

						int				SetParam_UnloadCable_ExtReg_ReturnRotVoltage(int Value); // Kabel Entladen mit externem Regler Windenansteuerspannung zum Kabel straffen in mV
						int				GetParam_UnloadCable_ExtReg_ReturnRotVoltage();
						bool			UnloadCable_ExtReg_ReturnRotVoltage_Locked;
*/
						int             SetParam_TractionViewDimensionUnit(int Value); // Darstellung der Zugkraft (0=1N|1=1kN|2=1kp)
                        int             GetParam_TractionViewDimensionUnit();
                        bool            CheckParam_TractionViewDimensionUnit(int Value, int *Recommendation);
                        bool            TractionViewDimensionUnit_Locked;

                        int             SetParam_CalFactDepth(int Value);       // Eichfaktor für Teufenrechner
                        int             GetParam_CalFactDepth();
						bool            CheckParam_CalFactDepth(int Value, int *Recommendation);
						bool            CalFactDepth_Locked;                    // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

						int             SetParam_CalFactDepth_Earmarked(int Value);// Teufenkalibrierfaktor, ermittelt bei der Kalibrierung und vorgemerkt für die Übernahme
						int             GetParam_CalFactDepth_Earmarked();
						bool            CalFactDepth_Earmarked_Locked;          // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

                        int             SetParam_CalibTractionX(int Value);     // Zugkraft-Kalibr. Anstiegsanteil x-Richtung
						int             GetParam_CalibTractionX();
						int             CheckParam_CalibTractionX(int Value, int *Recomendation);
						bool            CalibTractionX_Locked;                  // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

                        int             SetParam_CalibTractionY(int Value);     // Zugkraft-Kalibr. Anstiegsanteil y-Richtung
                        int             GetParam_CalibTractionY();
						int             CheckParam_CalibTractionY(int Value, int *Recomendation);
						bool            CalibTractionY_Locked;                  // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

						int             SetParam_CalibTractionZero(int Value);  // Zugkraft-Kalibr. Null-Abgleichswert
						int             GetParam_CalibTractionZero();
						bool            CalibTractionZero_Locked;               // Gibt an, ob die Einstellung möglich ist (true) oder nicht (false)

						int             SetParam_StopVelocityStepSAVE(int Value); // Geschwindigkeitsabstufung beim Anhalten der Winde im Automatikbetrieb (zum abspeichern)
                        int             GetParam_StopVelocityStepSAVE();
                        bool            StopVelocityStepSAVE_Locked;

                        bool            SetParam_IncEncErrorMonitOn();          // Inkrementalgeber-Fehler-Überwachung
                        bool            SetParam_IncEncErrorMonitOff();
                        bool            GetParam_IncEncErrorMonit();

						bool            SetParam_ControllerErrorMonitOn();      // Steuergerät - Fehler-Überwachung
						bool            SetParam_ControllerErrorMonitOff();
						bool            GetParam_ControllerErrorMonit();

						bool            SetParam_ControllerErrorMonitAtSMOn();  // Steuergerät - Fehler-Überwachung in der langsamen Fahrstufe aktiv
						bool            SetParam_ControllerErrorMonitAtSMOff();
						bool            GetParam_ControllerErrorMonitAtSM();
						bool			ControllerErrorMonitAtSM_Locked;

						bool            SetParam_ControllerErrorLactiveOn();      // Steuergerät - Fehler-Überwachung
						bool            SetParam_ControllerErrorLactiveOff();
						bool            GetParam_ControllerErrorLactive();
						bool            ControllerErrorLactive_Locked;

						int             SetParam_CalibTractionValue1(int Value);// Zugkraftwert für Kalibrierung, der mit der Spannung aus der ersten Messung korrespondiert
						int             GetParam_CalibTractionValue1();
						bool            CheckParam_CalibTractionValue1(int Value, int *Recommendation);
						bool            CalibTractionValue1_Locked;

    	// Der jeweilig mit der Zugkraft korrespondierende Messwert
		int		SetParam_CalibTension_MeasuringVal(int Idx, int Value);
		int     GetParam_CalibTension_MeasuringVal(int Idx);
		bool    CalibTension_MeasuringVal_Locked;

		// Die jeweilig mit dem Messwert korrespondierende Zugkraft
		int		SetParam_CalibTension_Tension(int Idx, int Value);
		int     GetParam_CalibTension_Tension(int Idx);
		bool    CalibTension_Tension_Locked;

						int             SetParam_CalibTractionValue2(int Value);// Zugkraftwert für Kalibrierung, der mit der Spannung aus der zweiten Messung korrespondiert
                        int             GetParam_CalibTractionValue2();
                        bool            CheckParam_CalibTractionValue2(int Value, int *Recommendation);
                        bool            CalibTractionValue2_Locked;

                        float           SetParam_CalibTSensorDepthCalc_CaractSlope(float Value); // Anstieg der Kennlinie des T.-Sensors des Teufenrechners in °C/V
                        float           GetParam_CalibTSensorDepthCalc_CaractSlope();
                        bool            CalibTSensorDepthCalc_CaractSlope_Locked;

                        float           SetParam_CalibTSensorDepthCalc_CaractZero(float Value); // Nulldurchgang der Kennlinie des T.-Sensors des Teufenrechners in °C/V
                        float           GetParam_CalibTSensorDepthCalc_CaractZero();
                        bool            CalibTSensorDepthCalc_CaractZero_Locked;

                        float           SetParam_CalibTSensorA_CaractSlope(float Value); // Anstieg der Kennlinie des T.-Sensors A in °C/V
                        float           GetParam_CalibTSensorA_CaractSlope();
                        bool            CalibTSensorA_CaractSlope_Locked;

                        float           SetParam_CalibTSensorA_CaractZero(float Value); // Nulldurchgang der Kennlinie des T.-Sensors A in °C
                        float           GetParam_CalibTSensorA_CaractZero();
                        bool            CalibTSensorA_CaractZero_Locked;

                        float           SetParam_CalibTSensorB_CaractSlope(float Value); // Anstieg der Kennlinie des T.-Sensors B in °C/V
                        float           GetParam_CalibTSensorB_CaractSlope();
                        bool            CalibTSensorB_CaractSlope_Locked;

                        float           SetParam_CalibTSensorB_CaractZero(float Value); // Nulldurchgang der Kennlinie des T.-Sensors B in °C
                        float           GetParam_CalibTSensorB_CaractZero();
                        bool            CalibTSensorB_CaractZero_Locked;

                        bool            SetParam_AutoRunProgramableOn();        // Programmautomatik Ein/Aus
                        bool            SetParam_AutoRunProgramableOff();
                        bool            GetParam_AutoRunProgramableOn();
                        bool            AutoRunProgramableOn_Locked;

                        wchar_t*        SetParam_AutoRunProgramFileName(wchar_t *Name); // Dateiname für eingestelltes Ablaufprogramm für die Programmautomatik
                        wchar_t*        GetParam_AutoRunProgramFileName();
                        bool            AutoRunProgramFileName_Locked;

						int				SetParam_VelocityDimensionUnit(int Value);	// Darstellung der Geschwindigkeit (0=1m/h|1=1m/min)
						int				GetParam_VelocityDimensionUnit();
						bool            CheckParam_VelocityDimensionUnit(int Value, int *Recommendation);
						bool            VelocityDimensionUnit_Locked;

						bool			SetParam_WinchBrakeInternAutoOn();		// Windenbremse intern automatisch
						bool		    SetParam_WinchBrakeInternAutoOff();
						bool			GetParam_WinchBrakeInternAuto();
						bool			WinchBrakeInternAuto_Locked;

						int 			SetParam_WinchBrakeInternAutoReleasePerc(int Value);// Lösen der Windenbremse bei interner Automatikbremse in Prozent
						int				GetParam_WinchBrakeInternAutoReleasePerc();
						bool			WinchBrakeInternAutoReleasePerc_Locked;

						int 			SetParam_WinchBrakeInternAutoReleaseHysteresisPerc(int Value);// Hysterese Lösen/Feststellen der Windenbremse bei interner Automatikbremse in Prozent
						int 			GetParam_WinchBrakeInternAutoReleaseHysteresisPerc();
						bool            WinchBrakeInternAutoReleaseHysteresisPerc_Locked;

						int				SetParam_LOGFileTensionUnit(int Value); // Zugkrafteinheit für die Protokollierung
						int				GetParam_LOGFileTensionUnit();
						bool			CheckParam_LOGFileTensionUnit(int Value, int *Recommendation);
						bool 			LOGFileTensionUnit_Locked;

						int             SetParam_LOGFileType(int Value);        // Datei-Typ der LOG-Datei
						int             GetParam_LOGFileType();
						bool            CheckParam_LOGFileType(int Value, int *Recommendation);
						bool            LOGFileType_Locked;

						int             SetParam_SpecialSignalMode(int Value);  // Modus, wie die Sondersignale ausgewertet werden sollen (welche Version der Abarbeitung)
						int             GetParam_SpecialSignalMode();
                        bool            CheckParam_SpecialSignalMode(int Value, int *Recomendation);
                        bool            SpecialSignalMode_Locked;

						bool			SetParam_LineLubAvailable(bool Value);	// Kabel-/Seilöler verfügbar
						bool			GetParam_LineLubAvailable();

						bool			SetParam_LineLubPermanentOperation(bool Value);	// Kabel-Seilöler Dauerbetrieb
						bool			GetParam_LineLubPermanentOperation();
						bool			LineLubPermanentOperation_Locked;

						int				SetParam_LineLubIntensity(int Value);	// Kabel-Seilöler Intensität
						int				GetParam_LineLubIntensity();
						bool            CheckParam_LineLubIntensity(int Value, int *Recomendation);
						bool			LineLubIntensity_Locked;


						// SONDERFUNKTIONEN
                        // Sperren von Funktionen, die sich auf keine, programmzweckbezogene Variable, beziehen
//                        bool            ParameterOKButton_Locked;               // Die OK-Tasten im Parametermenü werden blockiert
						bool            CalibDepthSetParm_Locked;               // Bei der Teufenkalibrierung erfassten Wert übernehmen (CalibDepthAcceptButton)
						bool            CancelButton_Locked;                    // Abbrechen/Verwerfen - Taste im Funktionspanel
                        bool            FileAdministration_Locked;              // Dateiverwaltung kann nicht ausgeführt werden
                        bool            ParameterMenu_Locked;                   // Parametermenü kann nicht ausgewählt werden
                        bool            SystemMenu_Locked;                      // Systemmenü ist blockiert
                        bool            SystemShutdownButton_Locked;            // Systemmenü: Die Taste zum System herunterfahren ist blockiert
                        bool            SystemRestartButton_Locked;             // Systemmenü: Die Taste zum System-Neustart ist blockiert
                        bool            TerminationButton_Locked;               // Systemmenü: Die Taste zum Beenden des Programms ist blockiert
                        bool            SystemDateTimeAcceptButton_Locked;      // Systemmenü: Die Taste zum Setzen von Datum und Uhrzeit ist blockiert
                        bool            SetAutoVelocityXButton_Locked;          // Geschwindigkeitsanpassungstasten für Automatikfahrt blockiert
                        bool            SetAutoRunProgramableIndex_Locked;      // Programmablaufpunkt-Auswahl für Programmautomatik blockiert

                        bool            VelocityLock_Active;                    // Geschwndigkeitsabhängige Sperre aktiv (ab bestimmter Geschwindigkeit wird Sperre aktiviert, ab einer anderen Geschwindigkeit wieder deaktiviert)
};

#endif
