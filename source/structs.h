/*
WinchControl    Jörg Müller    structs.h

Enthält alle Datenstrukturen des Programms

Portierbarkeit: vollständig
*/

#ifndef __STRUCTS_H
#define __STRUCTS_H

#include "constants.h"
#include "PrgAutomatic.h"
#include "EvntMgr.h"

// Enthält Kontrollvariablen zur Programmsteuerung
struct WCControlVarS
{
        // Fahrsteuerung
		bool AutoRunOn;                                 // Automatische Teufenfahrt (true) oder manuelle Teufenfahrt (false)
		bool WinchWork;                                 // Winde ein (true), angehalten (false)
        bool CalcCurrTractions;                         // momentane maximale Zugkraft in Abhängigkeit der kleinsten aufgetretenen Zugkraft berechnen
        bool CalcCurrTractionsActive;                   // momentane maximale Zugkraft in Abhängigkeit der kleinsten aufgetretenen Zugkraft berechnen tatsächlich aktiv
        bool ThresholdExceedenceStopDelayOn;            // Verzögerung des Ausschaltens vom Steuergerät bei Zugkraft-/ Teufenüberschreitung
        bool ClearAfterThresholdDepth;                  // Freigabe nach Grenzwertüberschreitung aktiv (true) oder nicht (false)
        bool ClearAfterThresholdTraction;               // Freigabe nach Grenzwertüberschreitung aktiv (true) oder nicht (false)
        bool ResetSpecialSignalError1Flag;              // Bestätigen von ausgewählten durch Sondersignale ausgelösten Fehlern
        bool ResetSpecialSignalError2Flag;              // Bestätigen von ausgewählten durch Sondersignale ausgelösten Fehlern
        bool FastRun;                                   // Schnelle (true) oder langsame (false) Fahrt
        int  EngineRevolutionUpDown;                    // Motordrehzahl erhöhen/verringern
		int  EngineRevolutionAutoUpDown;                // Motordrehzahl erhöhen/verringern; Variable für interne automatische De-/Aktivierung
		bool Hello_WinchClear;							// Hallo über das Signal WinchClear zur externen Steuerelektronik (forciertes kurzes Auslösen des Signals)

        // Magnetmarken
        bool MagneticMarkOn;                            // Magnetmarken ein (true) oder aus (false)
        bool MMSyncForceActive;							// MM-Zwangssynchronisation aktiv (true) oder inaktiv (false)
        int MMSyncForceCounter;							// MM Teufenzähler für MM-Zwangssynchronisation

        // Programmmodi und sonstiges
        bool devel;                                     // Entwicklermodus ein (true), aus (false)

        bool UnloadCable;                            	// Kabel entladen ein (true) oder aus (false)
        bool UnloadCableActive;                      	// Kabel entladen mit Zugkraftüberwachung
        bool UnloadCablePassive;                     	// Kabel entladen ohne Zugkraftüberwachung
        bool CalibrateOn;                               // Kalibrieren ein (true) oder aus (false)
        bool CalibrateDepth;                            // Kalibrieren des Teufenrechners (true) - Eichwertbestimmung
        bool CalibrateTraction;                         // Kalibrieren des Zugraftmesser-Moduls (true) - Anstieg- und Nullwertbestimmung
		bool GetCtlUnitSWVersion;						// Gibt an, ob die Software-Version des Steuergeräts ausgelesen werden soll
		bool GetDepthCalcSWVersion;                     // Gibt an, ob die Software-Version des Teufenrechners ausgelesen werden soll
		bool LostMotionImpulse;                         // Gibt an, Freilauf ob der Freilaufimpuls erfolgen soll (true) oder nicht (false)
		bool SetDepth_DC;                               // Gibt an, ob der Wert in StartDepth an den Teufenrechner übergeben werden soll (true) oder nicht (false)
		bool SetDepth;                               	// Gibt an, ob der Wert in StartDepth an das Steuergerät übergeben werden soll (true) oder nicht (false)
		bool SetDepthCalcParam;                         // Gibt an, ob dem Teufenrechner die Konfigurationswerte übergeben werden sollen (true) oder nicht (false)
		bool SetDepthCalFactor;                      	// Gibt an, ob der Wert in CalFactDepth an das Steuergerät übergeben werden soll (true) oder nicht (false)
		bool SetDepthCalFactor_DC;                      // Gibt an, ob der Wert in CalFactDepth an den Teufenrechner übergeben werden soll (true) oder nicht (false)
		bool SetFormatCMImpulse;                        // Gibt an, ob der Wert in FormatCMImpulse an den Teufenrechner übergeben werden soll (true) oder nicht (false)
		bool SetMagneticMark;                           // Gibt an, ob der Wert in MagneticMarkOn an den Teufenrechner übergeben werden soll (true) oder nicht (false)
		bool WinchBrakeOff;                             // Gibt an, ob die Bremse der Winde gelöst werden soll (NICHT GLEICH FREILAUF)
		bool WinchBrakeOffImpulse;                      // Gibt an, ob ein Impuls zur Steuerung der Bremslösung bzw. Anzug kommen soll

		// Kalibrieren
		bool CalibTension_GetVal1;                    	// Gibt an, ob der erste Spannungswert für die Zugkraftkalibrierung ermittelt werden soll
		bool CalibTension_GetVal2;                    	// Gibt an, ob der zweite Spannungswert für die Zugkraftkalibrierung ermittelt werden soll
		bool CalibTractionCalculateParam;               // Gibt an, ob die Parameter zur Zugkraftermittlung berechnet werden sollen
		bool CalibTractionSetParam;                     // Gibt an, ob die Parameter zur Zugkraftermittlung übernommen werden sollen
		bool CalibTractionSetZero;                      // Gibt an, ob ein Nullableich für den Zugkraftmesser durchegführt werden soll

		// Signale
		bool SignalHornOn;                              // Gibt an, ob das Signal-Horn(Hupe) betätigt wird (true)
		bool EngineStart;                               // Motor einschalten
		bool EngineStop;                                // Motor ausschalten

		// Sonderfunktion
		bool SpecialFunc1;								// Sonderfunktion 1

		// System gesteuerte Ereignisse
		bool PowerFail;                                 // Gibt an, ob ein ein "PowerFail" Ereignis signalisiert wird
};

// Enthält zur Laufzeit ermittelte Statusinformationen
struct WCStatusInfoVarS
{
        // Fahrwerte
        int CurrDepth;                                  // momentane Teufe
        bool CurrDirection;                             // momentane Fahrtrichtung; abwärts (false) / aufwärts (true)
		int CurrVelocity;                               // momentane Sondengeschwindigkeit
        int AnnouncedVelocity;                          // angezeigte Sondengeschwindigkeit
		int CurrWinchRevMessVoltage;                    // Gemessene momentane Windengeschwindigkeitssteuer-Spannung in mV
		int CurrWinchBrakeInternAutoStatus;				// Status der interne Automatischen Windenbremse (0-Automatik inaktiv, 1-angebremst, 2-geloest, 3-Bremsen erwartet)

        // Zugkraft
		int CurrTensionLimit_Lower;                     // momentane Mindestzugkraft
		int CurrTensionLimit_Upper;                     // momentane Maximalzugkraft
        int CurrTraction;                               // momentane Zugkraft
        int CurrTractionDifference;                     // momentane Zugkraftdifferenz
        int CurrTractionReference;                      // momentane Soll-Zugkraft
        int CurrTractionVoltage;                        // momentaner Spannungswert vom Zugkraftmesser

		// Hydraulikzustand
		bool HydraulicOK;                            	// Hydraulik OK
		int  HydrWrkPressure;							// Aktueller Arbeitsdruck der Hydraulik
		int  HydrChargePressure; 						// Speisedruck der Hydraulik
        int  HydrLvl;									// Aktueller Hydraulikfüllstand

		// Zeitwerte
		int CurrTimeDifference;                         // momentane Zeitdifferenz

		// sonst. Signalisierungen
		bool MagneticMark;                              // Magnetmarke tritt auf (true)
		bool MagneticMarkStatic;                        // Magnetmarke tritt auf (true), statisch (für Anzeige und Protokollierung)
		bool MagneticMarkStaticLOG;                     // Magnetmarke tritt auf (true), statisch (für Protokollierung)
		bool MagneticMarkDirect;                        // Gibt direkt den Magnetmarkenstatus wieder (aktiv, solange eine Magnetmarke auftritt)
		bool IncEncError;                               // Inkrementalgeber scheint fehlerhaft (true)
		bool ControllerError;                           // Steuergerät Fehler-Signalisierung
		bool LostMotion;                                // Leerlauf aktiv (true), inaktiv (false)
		bool OilStrainer;                               // Ölfilter wechseln ein/aus
		bool OilStrainer2;                              // Ölfilter wechseln ein/aus
		bool EmergencySwitch;                           // Notschalter betätigt ein/aus
		bool DepthThresholdExceeded;                    // Bereich zwischen Mindest- und Maximal-Teufe überschritten
		bool TractionThresholdExceeded;                 // Maximal-Zugkraft überschritten
		bool LeverOn;                                   // Fahrhebel aktiv (Manuelle Fahrtrichtungs-/Geschwindigkeitseinstellung)
		bool HWCtrlSignalisation;                       // Signalisiert den tatsächlichen Hardware-Kontroll-Status
		bool SpecialSignal01;                           // Konfigurierbares Sondersignal zur Signalisierung verschiedener Ereignisse
		bool SpecialSignal02;                           // Konfigurierbares Sondersignal zur Signalisierung verschiedener Ereignisse
		bool SpecialSignalExtBrake;                     // Durch ein Sondersignal überwachte externe Bremse

		// Antriebsmotor
		int EngineSpeed;								// Antriebsmotordrehzahl in rpm
		bool EngineOn;                                  // Motor an/aus
		int EngineCoolantTemp;							// Temperatur der Antriebsmotorkühlflüssigkeit
		bool AutomotiveError;                           // Signalisiert einen Fahrzeugfehler (Warnung für z.B. Motor-Öldruck, Motor-Temperatur)

        // Programmmodus
        int ProgramMode;                                // Der momentan aktive Programmmodus

        // Kalibrieren
        bool CalibTractionValue1Ok;                     // Gibt an, ob der erste Spannungswert erfolgreich ermittelt wurde
        bool CalibTractionValue2Ok;                     // Gibt an, ob der zweite Spannungswert erfolgreich ermittelt wurde
		bool CalibDepthAllowed;                         // Gibt an, ob das Kalibrieren der Teufe im Aktuellen Zustand generell möglich ist
		bool CalibDepthCableOk;                         // Gibt an, ob das Kabel eine bestimmte Länge herausgezogen wurde und nun die Teufe angegeben werden kann

		// TR-Spezifikationen
		char DepthCalcSWVersion[DEPTHCALC_SW_VERSION_NAMELENGTH]; // Pufferspeicher für den Versionsstring des Teufenrechners

		// Steuergerät-Spezifikation
		char CtlUnitSWVersion[CTLUNIT_SW_VERSION_NAMELENGTH]; // Pufferspeicher für den Versionsstring des Steuergeräts

		// Temperaturwerte
		float TemperatureSensorDepthCalc;               // Enthält die Temperatur des T.-Sensors auf dem Teufenrechner in °C
        float TemperatureSensorA;                       // Enthält die Temperatur des T.-Sensors A in °C
		float TemperatureSensorB;                       // Enthält die Temperatur des T.-Sensors B in °C
		int TemperatureOilTank;							// Öltanktemperatur in °C
};

// Enthält sämtliche Programmeinstellungen bezüglich des Programmzwecks
struct WCParameterVarS
{
		// allgem.
		unsigned int WinchNo;                           // Nummer der aktuellen Winde
//		unsigned int WinchCnt;                          // Anzahl der durch WinchControl zu verwaltenden Winden (Maximal _WinchCnt [constants.h])
		bool UnloadCableStretch;                     	// Stretching beim entladen des Kabels ein (true) oder aus (false)
		bool IncEncErrorMonitOn;                        // Inkrementalgeber-Fehler-Überwachung
		bool ControllerErrorMonitOn;                    // Steuergerät - Fehler-Überwachung
		bool ControllerErrorMonitOnAtSM;				// Steuergerät - Fehler-Überwachung in der langsamen Fahrstufe aktiv
		bool ControllerErrorLactive;					// Steuergerät - Fehler L-aktiv
		int SpecialSignalMode;                          // Modus der Sondersignalverarbeitung festlegen (Version der Signalverarbeitung)

		// Teufen
		int MinDepth;                                   // einstellbare Mindestteufe
		int MaxDepth;                                   // einstellbare Maximalteufe
		int StartDepth;                                 // einstellbare Startteufe
		bool ReverseDepth;								// Einstellung, ob die Teufe umgekehrt angezeigt werden soll (aus - wird + und umgekehrt)
		int DepthThresholdForAuto;                      // Teufe, ab wann Automatikbetrieb möglich ist
		int DepthDistanceForBreak;                      // Abstand vorm Endewert, ab welchem die Winde abgebremst werden soll

		// Magnetmarke
		bool MMSyncForceOn;								// MM-Zwangssynchr. Ein/Aus
		int MMSyncForceRangeLim;   						// MM-Zwangssynchr. Teufenbereichsgrenze in Meter

		// Fahrtparameter
		int Velocity;                                   // einstellbare Geschwindigkeit
		int Velocity_PrgAutom;                          // einstellbare Geschwindigkeit für Programmautomatik (wird je nach Programmpunkt gesetzt)
		bool Direction;                                 // einstellbare Fahrtrichtung (false = abwärts, true = aufwärts)
		bool Direction_PrgAutom;                        // einstellbare Fahrtrichtung (false = abwärts, true = aufwärts) für Programmautomatik (wird je nach Programmpunkt gesetzt)
		bool EngineRevolutionChangeable;                // Drehzahl Fahrzeugmotor veränderbar
		bool EngineStartStopEnable;                     // Antriebsmotor Starten / Stoppen möglich
		bool LostMotionEnable;                     		// Freilaufbedienung möglich
		bool BrakeOffEnable;                            // Bremse abschalten möglich
		bool ExternalBrakeAvailable;					// Externe Bremse verfügbar
		bool AutoRunProgramableOn;                      // Programmautomatik Ein/Aus
		wchar_t AutoRunProgramFileName[LIM_PAR_AutoRunPrgFileNameLength + 1]; // Dateiname für eingestelltes Ablaufprogramm für die Programmautomatik
		bool WinchBrakeInternAuto;						// Windenbremse intern automatisch
		int WinchBrakeInternAutoReleasePerc;			// Lösen der Windenbremse bei interner Automatikbremse in Prozent
		int WinchBrakeInternAutoReleaseHysteresisPerc;  // Hysterese Lösen/Feststellen der Windenbremse bei interner Automatikbremse in Prozent
		bool HydraulicBrakeOn;							// Hydraulische Bremse aktiv (true) / inaktiv (false)

		// Zugkraft
		int MinTraction[_WinchCnt];                     // einstellbare Mindestzugkraft
		int MaxTraction[_WinchCnt];                     // einstellbare Maximalzugkraft
		int RelTensionLimitLower[_WinchCnt];           	// einstellbare untere Zugkraftgerenze, abhängig von der aktuell höchsten Zugkraft (wichtig bei Abwärtsfahrt)
		int RelTensionLimitUpper[_WinchCnt];            // einstellbare obere Zugkraftgerenze, abhängig von der aktuell niedrigsten Zugkraft (wichtig bei Abwärtsfahrt)
		int UnloadCableBeginLenght;                  	// Anfangslänge des Kabels für Stretching
		int UnloadCableEndLenght;                    	// Endlänge des Kabels für Stretching
		int UnloadCableTraction[_WinchCnt];          	// einstellbare Zugkraft zum Entladen des Kabels für aktiven Zustand (Regime 1)
		int UnloadCableMinTraction;                  	// Mindestzugkraft zum Entladen des Kabels für aktiven Zustand (Regime 1)
		int UnloadCableStartTraction[_WinchCnt];     	// einstellbare Anfangszugkraft beim Entladen des Kabels
		int UnloadCableEndTraction[_WinchCnt];       	// einstellbare Endzugkraft beim Entladen des Kabels
		bool CalcCurrTractions;                         // momentane Zugkraft in Abhängigkeit der Teufe berechnen
//		float UnloadCable_PID_TD;                    	// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Vorhaltezeit (langsamer Regler [straffen])
//		float UnloadCable_PID_TD_SAVE;                  // PID-Regler für Zugkraftkontrolle beim Kabel entladen: Vorhaltezeit (langsamer Regler [straffen]) (zum Abspeichern)
//		float UnloadCable_PID_TI;                    	// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Nachstellzeit (langsamer Regler [straffen])
//		float UnloadCable_PID_TI_SAVE;               	// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Nachstellzeit (langsamer Regler [straffen]) (zum Abspeichern)
//		float UnloadCable_PID_KR;                    	// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Verstärkungsfaktor (langsamer Regler [straffen])
//		float UnloadCable_PID_KR_SAVE;               	// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Verstärkungsfaktor (langsamer Regler [straffen]) (zum Abspeichern)
//		float UnloadCable_PID_TD_2;                  	// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Vorhaltezeit (schneller Regler [lockern])
//		float UnloadCable_PID_TD_2_SAVE;             	// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Vorhaltezeit (schneller Regler [lockern]) (zum Abspeichern)
//		float UnloadCable_PID_TI_2;                  	// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Nachstellzeit (schneller Regler [lockern])
//		float UnloadCable_PID_TI_2_SAVE;             	// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Nachstellzeit (schneller Regler [lockern]) (zum Abspeichern)
//		float UnloadCable_PID_KR_2;                  	// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Verstärkungsfaktor (schneller Regler [lockern])
//		float UnloadCable_PID_KR_2_SAVE;             	// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Verstärkungsfaktor (schneller Regler [lockern]) (zum Abspeichern)
//		int UnloadCable_ExtReg_MaxTension;	  			// Kabel Entladen mit externem Regler Kennliniendef. Maximalzugkraft
//		int UnloadCable_ExtReg_MaxVoltage;	  			// Kabel Entladen mit externem Regler Kennliniendef. Maximalspannung
//		int UnloadCable_ExtReg_MinTension;	  			// Kabel Entladen mit externem Regler Kennliniendef. Mindestzugkraft
//		int UnloadCable_ExtReg_MinVoltage;	  			// Kabel Entladen mit externem Regler Kennliniendef. Mindestspannung
//		int UnloadCable_ExtReg_MaxPosOffset;            // Kabel Entladen mit externem Regler max. positiver Offset der Kennlinie
//		int UnloadCable_ExtReg_MaxNegOffset;            // Kabel Entladen mit externem Regler max. negativer Offset der Kennlinie
//		int UnloadCable_ExtReg_Delay;                   // Kabel Entladen mit externem Regler Verzögerung beim Abfahren (Rampe) in ms
//		int UnloadCable_ExtReg_P;                      	// Kabel Entladen mit externem Regler P-Anteil
//		int UnloadCable_ExtReg_I;                      	// Kabel Entladen mit externem Regler I-Anteil (Nachstellzeit in ms)
//		int UnloadCable_ExtReg_ReturnRotVoltage;		// Kabel Entladen mit externem Regler Windenansteuerspannung zum Kabel straffen in mV

		int TractionViewDimensionUnit;                  // Darstellung der Zugkraft (0=1N|1=1kN|2=1kp)
		int VelocityDimensionUnit;						// Darstellung der Geschwindigkeit (0=1m/h|1=1m/min)

		// Kalibrierfaktoren
		int CalFactDepth[_WinchCnt];                    // Kalibrierfaktor für den Teufenrechner
		int CalFactDepth_Earmarked;                     // Teufenkalibrierfaktor, ermittelt bei der Kalibrierung und vorgemerkt für die Übernahme
		int CalibTractionX[_WinchCnt];                  // Zugkraftkalibrierung X-Achse für Anstiegsberechnung
		int CalibTractionY[_WinchCnt];                  // Zugkraftkalibrierung Y-Achse für Anstiegsberechnung
		int CalibTractionZero[_WinchCnt];               // Nullwert für Zugkraftkalibrierung

		// Kalibrieren
		int CalibTractionValue1;                        // Die mit dem, bei der Zugkraftkalibrierung ermittelte, ersten Spannungswert korespondierende Zugkraft
		int CalibTractionValue2;                        // Die mit dem, bei der Zugkraftkalibrierung ermittelte, zweiten Spannungswert korespondierende Zugkraft
		int CalibTension_MeasuringVal[CalibTension_NumOfCalMeasuringPoints]; // Der jeweilig mit der Zugkraft korrespondierende Messwert
		int CalibTension_Tension[CalibTension_NumOfCalMeasuringPoints];
//		int CalibDepthCalcCalFactor;                    // Puffer für berechneten oder eingegebenen Eichfaktor für den Teufenrechner (wird nach Bestätigung in CalFactDepth übernommen und damit aktiv)

		// Temperatursensor-Kalibrierung
		float CalibTSensorDepthCalc_CaractSlope;        // Anstieg der Kennlinie des T.-Sensors des Teufenrechners in °C/V
		float CalibTSensorDepthCalc_CaractZero;         // Nulldurchgang der Kennlinie des T.-Sensors des Teufenrechners in °C
		float CalibTSensorA_CaractSlope;                // Anstieg der Kennlinie des T.-Sensors A in °C/V
		float CalibTSensorA_CaractZero;                 // Nulldurchgang der Kennlinie des T.-Sensors A in °C
		float CalibTSensorB_CaractSlope;                // Anstieg der Kennlinie des T.-Sensors B in °C/V
		float CalibTSensorB_CaractZero;                 // Nulldurchgang der Kennlinie des T.-Sensors B in °C

		// cm-Impuls Ausgabe
		int FormatCMImpulse;                            // eingestelltes Format für den cm-Impuls (0=nichts | 1=KARAT | 2=ONYX | 3=KEDR)

		// LOG-Datei
		int LOGFileTensionUnit;							// Zugkrafteinheit für die Protokollierung (0...Standard kN, 1...kg)
		int LOGFileType;                                // Typ der LOG-Datei (0...Standard; 1...LAS 2.0)

		// Kabel-/Seilöler
		bool LineLubAvailable;							// Kabel-/Seilöler verfügbar
		bool LineLubPermanentOperation;					// Kabel-/Seilöler Dauerbetrieb
		int LineLubIntensity;							// Kabel-/Seilöler Intensität
};

// Enthält Programm-organisatorische Variablen
struct WCGenOrganisationS
{
		char CurrFunctionPanel;                         // hier wird direkt eingetragen, welches Unter-Panel von FunctionsPanel gerade angezeigt werden soll
		char FavoriteFunctionPanel;						// das bevorzugt anzuzeigende Unter-Panel von FunctionsPanel

        char *DepthCalc_Port;                           // Port für den Teufenrechner (COMx)
        char DepthCalc_DataBit;                         // Konfigurations-Werte für die Kommunikation über DepthCalc_Port
        char DepthCalc_StopBit;
        char DepthCalc_Parity;
        int DepthCalc_Baud;

        char *InfoOut_Port;                             // Port für Ausgabe der Teufe auf Anfrage (COMx)
        int InfoOut_DataBit;                            // Konfigurations-Werte für die Kommunikation über InfoOut_Port
        int InfoOut_StopBit;
        int InfoOut_Parity;
        int InfoOut_Baud;

        unsigned short AO_WinchRevChannel;              // Windendrehzahl: Analoger Ausgangs-Kanal
        unsigned short AO_WinchRevReference;            // Windendrehzahl: 0...interene Referenzquelle, 1...externe Referenzquelle
        float AO_WinchRevMinVoltage;                    // Windendrehzahl: Mindestausgangsspannung
        float AO_WinchRevMaxVoltage;                    // Windendrehzahl: Maximalausgangsspannung
        float AO_WinchRevZeroBy;                        // Windendrehzahl: Ruhestellung bei 2,5V
		float AO_WinchRevCfgMinVoltage;					// Mindestausgangsspannung für Schnittstellenkonfig. in mV (Achtung, Hardware-Fähigkeiten beachten!)
		float AO_WinchRevCfgMaxVoltage;                 // Maximalausgangsspannung für Schnittstellenkonfig. in mV (Achtung, Hardware-Fähigkeiten beachten!)
		int	AO_DAQNavi_WinchRevCfgRange;  		        // DAQNavi: Maximalausgangsspannung für Schnittstellenkofig. 0..V_0To5, 1..V_0To10, 2..V_Neg5To5, 3..V_Neg10To10

		unsigned short AO_TensionOutChannel;            // Zugkraft-Ausgabe an externe Apparatur: Analoger Ausgangs-Kanal
		unsigned short AO_TensionOutReference;          // Zugkraft-Ausgabe an externe Apparatur: 0...interene Referenzquelle, 1...externe Referenzquelle
		float          AO_TensionOutMinVoltage;         // Zugkraft-Ausgabe an externe Apparatur: Mindestausgangsspannung
		float          AO_TensionOutMaxVoltage;         // Zugkraft-Ausgabe an externe Apparatur: Maximalausgangsspannung
		float		   AO_TensionOutCfgMinVoltage;		// Mindestausgangsspannung für Schnittstellenkofig. in mV (Achtaung, Hardware-Fähigkeiten beachten!)
		float          AO_TensionOutCfgMaxVoltage;		// Maximalausgangsspannung für Schnittstellenkofig. in mV (Achtung, Hardware-Fähigkeiten beachten!)
		int            AO_DAQNavi_TensionOutRange;		// DAQNavi: Maximalausgangsspannung für Schnittstellenkofig. 0..V_0To5, 1..V_0To10, 2..V_Neg5To5, 3..V_Neg10To10

		unsigned short AO_TensionCtrl_ExtReg_Channel;	// Zugkraft-Steuerung bei Kabel entladen: Analoger Ausgangs-Kanal
		unsigned short AO_TensionCtrl_ExtReg_Reference; // Zugkraft-Steuerung bei Kabel entladen: 0...interene Referenzquelle, 1...externe Referenzquelle
		float		   AO_TensionCtrl_ExtReg_MinVoltage;// Zugkraft-Steuerung bei Kabel entladen: Mindestausgangsspannung
		float		   AO_TensionCtrl_ExtReg_MaxVoltage;// Zugkraft-Steuerung bei Kabel entladen: Maximalausgangsspannung
		float          AO_TensionCtrl_ExtReg_CfgMinVolt;// Mindestausgangsspannung für Schnittstellenkonfig. in mV (Achtung, Hardware-Fähigkeiten beachten!)
		float          AO_TensionCtrl_ExtReg_CfgMaxVolt;// Maximalausgangsspannung für Schnittstellenkonfig. in mV (Achtung, Hardware-Fähigkeiten beachten!)
		int			   AO_DAQNavi_TensionCtrl_ExtReg_Range;// DAQNavi: Maximalausgangsspannung für Schnittstellenkonfig. 0..V_0To5, 1..V_0To10, 2..V_Neg5To5, 3..V_Neg10To10

        unsigned short AI_TractionChannel;              // Zugkraftmesser: Kanal       
        unsigned short AI_TractionGain;                 // Zugkraftmesser: Verstärkung für Eingangsspannungen
		int			   AI_DAQNavi_TensionRange;    		// DAQNavi: Eingangsspannunggsbereich, 0..V_0To5, 1..V_0To10, 2..V_Neg5To5, 3..V_Neg10To10, 4..V_0To2pt5, 5..V_Neg2pt5To2pt5

		unsigned short AI_WinchRevMessChannel;          // Windensteuerungsspannungsmessung: Kanal
		unsigned short AI_WinchRevMessGain;
		int			   AI_DAQNavi_WinchRevMessRange;    // DAQNavi: Eingangsspannunggsbereich, 0..V_0To5, 1..V_0To10, 2..V_Neg5To5, 3..V_Neg10To10, 4..V_0To2pt5, 5..V_Neg2pt5To2pt5

		unsigned short AI_TemperatureDepthCalcChannel;  // Temperatursensor Teufenrechner: Kanal
		unsigned short AI_TemperatureDepthCalcGain;     // Temperatursensor Teufenrechner: Verstärkung für Eingangsspannungen
		int            AI_DAQNavi_TemperatureDepthCalcRange;// DAQNavi: Eingangsspannunggsbereich, 0..V_0To5, 1..V_0To10, 2..V_Neg5To5, 3..V_Neg10To10, 4..V_0To2pt5, 5..V_Neg2pt5To2pt5

		unsigned short AI_TemperatureAChannel;          // Temperatursensor A: Kanal
		unsigned short AI_TemperatureAGain;             // Temperatursensor A: Verstärkung für Eingangsspannungen
		int            AI_DAQNavi_TemperatureARange;    // DAQNavi: Eingangsspannunggsbereich, 0..V_0To5, 1..V_0To10, 2..V_Neg5To5, 3..V_Neg10To10, 4..V_0To2pt5, 5..V_Neg2pt5To2pt5

		unsigned short AI_TemperatureBChannel;          // Temperatursensor B: Kanal
		unsigned short AI_TemperatureBGain;             // Temperatursensor B: Verstärkung für Eingangsspannungen
		int            AI_DAQNavi_TemperatureBRange;    // DAQNavi: Eingangsspannunggsbereich, 0..V_0To5, 1..V_0To10, 2..V_Neg5To5, 3..V_Neg10To10, 4..V_0To2pt5, 5..V_Neg2pt5To2pt5

		bool Clock_100ms;                               // Takt-Signal entsprechend Bezeichnung

		JMGS_EvntMgr Evnt;								// Ereignismeldungsverwaltung

        // LOG
        char LOGPath[sizeof(LOG_Path) / sizeof(LOG_Path[0])]; // Speicherplatz des im Programm zu verwendenden Pfads für die Protokolldateien
        bool NewLOG;                                    // Gibt an, ob eine neue LOG-Datei angelegt werden soll (true)
        int LOG_Lng;                                    // Gibt die Sprache an, in der die LOG-Datei geschrieben werden soll
        char LOG_LOCATION[LOG_LOCATION_LENGHT];         // Ortsbezeichnung
        char LOG_WELL[LOG_WELL_LENGHT];         		// Bohrungsbezeichnung
        char LOG_NAME[LOG_NAME_LENGHT];                 // Name
		char LOG_COMMENT[LOG_COMMENT_LENGHT];           // Kommentar
		char LOG_TEAM[LOG_TEAM_LENGTH];					// Trupp
		char LOG_WINCHMACHINERY[LOG_WINCHMACHINERY_LENGTH];	// Winde

        // LOG intern
        char LOGPathinternal[sizeof(LOG_Path_internal) / sizeof(LOG_Path_internal[0])]; // Speicherplatz des im Programm zu verwendenden Pfads für die interne Protokolldatei

        // Interne LOG-Parameter
        int internal_LOG_AbsoluteMaxTraction;           // momentane absolute Maximal-Zugkraft

        // Speicherplatz
        bool LOGDiskFree;                               // Gibt an, ob noch genügend Speicherplatz auf dem Datenträger für die offiziellen LOG-Dateien ist
        bool LOGInternalDiskFree;                       // Gibt an, ob noch genügend Speicherplatz auf dem Datenträger für die internen LOG-Dateien ist
        char CurrDrive[StdDriveLength];                 // Laufwerksbuchstabe, unter dem WinchCtrl gestartet wurde

        // Grafikinformationen
        int MainForm_ClientWidth;                       // Gibt die Breite der verfügbaren Programmfensterfläche an
        int MainForm_ClientHeight;                      // Gibt die Höhe der verfügbaren Programmfensterfläche an

        // Devel-Info
        float ControlThreadLoopsPerSecound;             // Schleifendurchläufe pro Sekunde
        float DisplayThreadLoopsPerSecound;             // Schleifendurchläufe pro Sekunde
		float SoundThreadLoopsPerSecound;               // Schleifendurchläufe pro Sekunde
		float DevelExtraInfoFloat1;                     // Zusätzliche Informationspuffer zur beliebigen Belegung während der Entwurfszeit
        float DevelExtraInfoFloat2;                     // Zusätzliche Informationspuffer zur beliebigen Belegung während der Entwurfszeit
        float DevelExtraInfoFloat3;                     // Zusätzliche Informationspuffer zur beliebigen Belegung während der Entwurfszeit
        float DevelExtraInfoFloat4;                     // Zusätzliche Informationspuffer zur beliebigen Belegung während der Entwurfszeit

        int Lng;                                        // Sprache der Anwendung

// ToDo: Nach bewähren der neuen Methode "AcousticSignalOn"-Flag vollständig entfernen
		// Akustische Signalisierung
//        bool AcousticSignalOn;                          // Akustisches Signal (Warnung/Alarm)

        // Debugging
        bool DebugFlag0001;

        // Programmautomatik
        WCPrgAutomatic  PrgAutomatic;                   // Klasse für die Verwaltung der Programmablaufpunkte der Programmautomatik
        int PrgAutoIndex;                               // Index für den aktuellen Programmpunkt

};

#endif  // __STRUCTS_H