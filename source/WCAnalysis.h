//---------------------------------------------------------------------------
#ifndef WCAnalysisH
#define WCAnalysisH

#include "VarControl.h"
#include "WCIO.h"
#include "tools.h"
#include "AverageManager_float.h"
#include "AverageManager_int.h"
#include "BasicCAN.h"
//---------------------------------------------------------------------------

class WCAnalysis
{
        private:WCVarControl *V;                                                // Zeiger auf die Daten-Klasse
                WCGenOrganisationS *GenOrg;                                     // Zeiger auf die gennerelle Organisationsstruktur
				bool FirstRun;                                                	// Beim ersten Durchlauf müssen noch nicht alle Routinen abgearbeitet werden

                // alte Werte, für Kalkulationen (Hilfsvariable)
                long    GetDepth_Time_old;
                long    GetDirection_Depth_old;
//                long    GetVelocity_Depth_old;
                long    GetVelocity_Time_old;
                long    GetVelocity_Real;

                clock_t GetTemperature_Time_old;                                // Damit Temperatur nicht allzu oft abgefragt wird
				clock_t GetCurrWinchRevMessVoltage_Time_old;                    // Taktung der Abfrage der Steuerspannung für das Steuergerät
				clock_t GetHydrWrkPressure_Time_old;							// Takt der Anfrage des aktuellen Hydraulikarbeitsdrucks
				clock_t ActSignalHorn_101_Time_old;							// TEST5: Takt der 0x101-Ident-Sendung (500ms)
				clock_t ThresholdExceedenceStopDelay_StartTime;                 // Beginn der Zeitmessung für Verzögerung des Ausschaltens des Steuergeräts bei Schwellwertüberschreitung Teufe/Zugkraft
				clock_t OilStrainerOnTriggerTime;                               // Triggerzeit für Ölfilter an-Signal (zum Entprellen des Signals)
				clock_t PowerFailOnTriggerTime;                                 // Triggerzeit bis zum Auslösen des Power-Fail-Signals (zum Entprellen des Signals)
				clock_t CtlUnitCAN_ReInit_Time;									// Triggerzeit bis zum Reinitialisieren der Steuergeräte-CAN-Schnittstelle

				bool PowerFailFunctionActive;                                   // PowerFail Signal ist aktiv, wenn Variable true

                unsigned int GetVelocity_DampNoise_Time_old;                    // für Dämpfung der Geschwindigkeitsschwankungen der internen Geschwindigkeit

                long    ActSpecialSignals_Time_old;                             // Signalisierungsverzögerung bei den Sondersignalen
                bool    ActSpecialSignals_V01_EventON;                          // Sondersignal Variante 1 Ereignis dedektiert (Hilfsvariable)

                struct VelocityCalcBufferS                                      // Hilfspuffer für Geschwindigkeitsberechnung
                {
                        int TimeDiff;                                           // Zeitdifferenz zur vorherigen Teufenabfrage in ms
                        int Depth;                                              // Teufe zum Zeitpunkt der Abfrage in ms
                } VelocityCalcBuffer[VELOCITY_CALC_BUF_SIZE];
                int CurrVelocityCalcBufIndex;                                   // Index des Puffers (an welcher Position befindet sich de aktuelle Wert)

				int tmpMagneticMark_cntdwn;                                     // Hilfsvar. für Geschwindigkeitsfehlerkompensation bei Auftreten einer Magnetmarke

                bool MMSyncForceAwait;											// Erwartungsflag der Teufenerhöhung bei MM-Zwangssynchronisation

//				int TensionDecreaseBuffer[TENSION_DECREASE_CALC_BUFF_SIZE];     // Hilfspuffer für Berechnung der Zugkraftschwelle für Zugkraftabfall bei Abwärtsfahrt (Schleifenfänger)
//				int CurrTensionDecreaseBufIndex;                                // Index des Puffers (an welcher Position befindet sich de aktuelle Wert)

#ifdef __TRACTION_NOISE_DAMP
				clock_t GetTraction_Dump_Time_old;                              // für Dämpfung der Zugkraftschwankungen
				JMGS_float_AverageManager GetTraction_AverageManager;           // Verwaltet den Mittelwert für die Zugkrafterfassung
#endif

				long	ActController_Hello_WinchClear_StartTime;				// Startzeitpunkt des aktuellen Hellos über das Signal WinchClear
				bool	ActController_Hello_WinchClear_Active;					// Flag, welches angibt, ob das Hello über das Signal WinchClear aktiv ist oder nicht

				bool	WinchBrakeInternAuto_Released;							// Hilfsflag, Windenbremse durch interne Automatik gelöst
				bool	WinchBrakeInternAuto_ReleaseAllowed;					// Hilfsflag, Windenbremse durch interne Automatik lösen erlaubt
				bool	WinchBrakeInternAuto_StoppExpected;						// Hilfsflag, Windenbremse durch interne Automatik - Anhalten erwartet
				int		WinchBrakeInternAuto_StartingAidRefPos;					// Hilfvariable, Windenbremse durch interne Automatik - Referenzposition für Anfahrhilfe in mV
				clock_t	WinchBrakeInternAuto_StoppExpect_elapsed;				// Hilfsvariable, interne Automatik - abgelaufene Zeit bis zum akzeptieren der Geschwindigkeit für den Stopp

				// temporäre Hilfsvariablen Zugkraftkalibrierung
				int		CalibTension_MeasuringValDiff, CalibTension_TensionDiff;
				clock_t CalibTension_AverageBuffer_FillTrigger;             	// Hilfsvariable zur Mittelwertbilung des Zukraftsignals für die Zugkraftkalibrierung
				JMGS_int_AverageManager CalibTension_AverageManager;

				JMGS_int_AverageManager CurrWinchRevMessVoltage_AverageManager;
				JMGS_int_AverageManager HydrWrkPressure_AverageManager;

				bool    PrgAutom_ReadStep;                                      // Flag, welches festlegt, ob der aktuelle Programmpunkt für die Automatik eingelesen werden muss

				// Steuergeräte-CAN
				// -----------------
			    JMGS_BasicCAN CtlUnitCAN;										// Steuergeräte-CAN-Klasse
				void CtlUnitCAN_VarInit();										// Variableninitialisierung für Steuergeräte-CAN-Kommunikation
				void CtlUnitCAN_StatusLevelCtl();			                    // Zustandsebenensteuerung für Steuergeräte-CAN
				void CtlUnitCAN_TxQueueAdmin();									// Steuergeräte CAN-Sendewarteschlangenverwaltung

				int CtlUnitCAN_StatusLvl;										// Angabe der Zustandsebene für die Steuergeräte-CAN-Schnittstelle

				struct  CtlUnitCAN_MsgS {                                       // Inhalt einer CAN-Botschaft mit Modifizierungsflag
					TPCANMsg CANMsg;											// Die CAN-Botschaft selbst
					bool Modified;                                              // Wird gesetzt, sobald Inhalt geschrieben ist, gelöscht, sobald Inhalt verarbeitet wurde
					bool Expected;												// Wird gesetzt, wenn eine Botschaft erwartet wird
					bool Delayed;												// Botschaft erwartet, aber noch nicht eingetroffen
					clock_t ModTS;												// Zeitstempel der letzten Modifikation
					unsigned char tx_queue_ctdwn;								// Sendewarteschlangencountdown solange 0xFF, dann nicht aktiviert. Paket mit niedrigstem Wert wird als erstes gesendet.
				};

				struct CtlUnitCAN_MsgBufS {										// Puffer für alle zu behandelnden CAN-Botschaften mit ID kodiert.
					CtlUnitCAN_MsgS Msg_0x00000101;
					CtlUnitCAN_MsgS Msg_0x00000102;
					CtlUnitCAN_MsgS Msg_0x00000103;
					CtlUnitCAN_MsgS Msg_0x00000104;
					CtlUnitCAN_MsgS Msg_0x00000201;
					CtlUnitCAN_MsgS Msg_0x00000204;
					CtlUnitCAN_MsgS Msg_0x00000301;
					CtlUnitCAN_MsgS Msg_0x00000302;
					CtlUnitCAN_MsgS Msg_0x00000303;
					CtlUnitCAN_MsgS Msg_0x00000304;
					CtlUnitCAN_MsgS Msg_0x00000305;
				} CtlUnitCAN_MsgBuf;

				CtlUnitCAN_MsgS *CtlUnitCAN_TxNominee;							// Zeiger auf den aktuellen Sendekandidaten
				clock_t CtlUnitCAN_TxTrigger;									// Hilfsvariable zum Triggern der Steuergeräte-CAN-Botschaften

				// HILFSVARIABLE
				// --------------

				// Timer-Hilfsvariablen
				// Impulslänge für Freilaufimpuls
				short   LostMotionImpulsLenght;                                 // Gibt an, wie lange die Impulsdauer für den Freilauf bereits anhält
				bool    LostMotionImpulseOn;                                    // Freilaufimpuls ist aktiviert
				bool    LostMotionImpulseTrigger;                               // zur Taktflankendetektierung

				// Taktstufen für Maximal-Zugkraftermittlung
				int     DynMaxTractionStepLenght;                               // Die Vergangene Zeit seit der letzten Maximal-Zugkraftermittlung
				bool    DynMaxTractionTrigger;                                  // zur Taktflankendetektierung

				// Taktzähler für Verzögerung von Motor Start/Stopp
				int     EngineStartDelayCounter;                                // Zähler Verzögerung Motor ein
				bool    EngineStartDelayTrigger;                                // zur Taktflankendetektierung
				int     EngineStopDelayCounter;                                 // Zähler Verzögerung Motor aus
				bool    EngineStopDelayTrigger;                                 // zur Taktflankendetektierung
				int     EngineStopSignalOffDelay;                               // Mindestdauer des direkten Motor aus Signals
				int     EngineStopSignalOffDelayTrigger;                        // zur Taktflankendetektierung
				bool    EngineStopSignal;                                       // das direkte Signal zum Motor ausschalten

				// Timer für Automatische Motordrehzahlsteuerung
				bool    EngineRevolutionAutoIsOn;                               // Timerindikator als Hilfe zur automatischen Änderung der Motordrehzahl (zeigt Status "Ist aktiv")
				long    EngineRevolutionAutoStartTime;                          // Timervariable (intern zu setzende Startzeit) als Hilfe zur automatischen Änderung der Motordrehzahl

				// interner Takt
				short Clock100ms_cnt;                                           // Timer-Zähler für interne Taktung
				clock_t LastClock;

protected:

				// ANALYSE UND REAKTION
				void GetHydraulicStatus();                                      // Hydraulikstatus-Überwachung
				void GetHydrChargePressure();                                   // Speisedruck der Hydraulik erfassen
				void GetHydrLvl();                                   			// Aktuellen Hydraulikfüllstand erfassen
				void GetHydrWrkPressure();										// Arbeitsdruck der Hydraulik erfassen
				void GetCtlUnitCAN_I();  										// Steuergeräte CAN-Botschaft einlesen
				void GetCtlUnitErrorMsg();										// Erfassen der Fehlermeldungen vom Steuergerät
				void GetCtlUnitSWVersion();										// Ermittelt die SW-Version des Steuergeräts
				void GetCurrWinchRevMessVoltage();                              // Ermitteln der gemessenen momentanen Windendrehzahlsteuerspannung (zum Steuergerät)
				void GetDepth();                                                // Teufe ermitteln und entsprechende Daten aktualisieren
				void GetDepthCalcSWVersion();                                   // Ermittelt die SW-Version des Teufenrechners
				void GetDepthParam();											// Teufenparameter auslesen (vom Steuergerät)
				void GetDI();                                                   // Einlesen der Stati der Digitalen Eingänge
				void GetDirection();                                            // Fahrtrichtung ermitteln
				void GetUnloadCableTractionReverence();                      	// Ermittelt die momentane Sollzugkraft für das aktive Kabel entladen
                void GetEmergencySwitch();                                      // Not-Schalter-Überwachung
				void GetEngineInfo();                                           // Motor-Zustandsinformationen
                void GetIncEncError();                                          // IGR-Fehler-Überwachung
                void GetControllerError();                                      // Steuergerät - Fehler-Überwachung
                void GetInternalClock();                                        // Internen Takt ermitteln
                void GetLeverState();                                           // Fahrhebelbetätigtungs-Überwachung
                void GetLostMotion();                                           // Freilauf-Überwachung
                void GetOilStrainer();                                          // Ölfilter wechseln Signal überwachen
                void GetPowerFail();                                            // Power-Fail-Signalauswertung
                void GetSpecialSignals();                                       // Einlesen der Sondersignale
				void GetTemperature();                                          // Ermitteln der Temperaturwerte der verschiedenen Sensoren
				void GetThresholdExceedance();                                  // Aktualisieren der Schwellwertzustände
				void GetTension();                                             	// Zugkraft-Ermittlung
				void GetDynTensionLimits();										// Berechnet die dynamischen Zugkraftgrenzen
				void GetVelocity();                                             // Geschwindigkeit ermitteln
                void GetVoltages();                                             // Liest alle Spannungen vom ADU-Ein

				void SetDepthCalcParamm();                                      // Parameter an den Teufenrechner übergeben
				void SetDepthParam();											// Teufenparameter setzen (für TR siehe "SetDepthCalcParamm()")
				void SetLockStates();                                           // Aktualisieren der Sperr-Anweisungen
                void SetProgramMode();                                          // Setzen des Programm-Modus

                void ActAutomatic();                                            // Automatik-Reaktion
				void ActCalibrationDepth();                                     // Aktionen zum Kalibrieren der Teufe
				void ActCalibrationTension();                                   // Aktionen zum Kalibrieren der Zugkraft
				void ActCtlUnitCANIdent();										// Identifizieren des Steuergeräts anfordern
				void ActCtlUnitCAN_O();  										// Steuergeräte CAN-Botschaft ausgeben (senden)
				void ActUnloadCable();                                       	// Kabel-entladen-Reaktion
				void ActDO();                                                   // Zustände an digitalen Ausgang übertragen
				void ActEngineRevolution();                                     // Motor-Drehzahl-Reglung
                void ActEngineStart();                                          // Motor-an-Reaktion
                void ActEngineStop();                                           // Motor-aus-Ausgang-Reaktion
                void ActErrorReactions();                                       // Reaktionen auf diverse Fehler (Resets usw.)
                void ActExtInfoOut();                                           // Externe Informationsausgabe
                void ActFastRun();                                              // Schnell/Langsam-Umschaltungs-Reaktion
            	void ActHydraulicBrake();										// Hydraulische Bremse
				void ActLostMotion();                                           // Freilauf-Impuls-Reaktion
				void ActMMSyncForce();											// MM-Zwangssynchronisation ausführen
				void ActPowerLock();                                            // Selbsthalteschaltung aktivieren
                void ActPrgAutomatic();                                         // Steuert die Einstellungen der Programmautomatik
                void ActController();                                           // Ansteuerung für Ventile ein
                void ActSignalHorn();                                           // SignalHorn-Ausgang-Reaktion
            	void ActSpecialFunc();											// Sonderfunktion ausführen
				void ActSpecialSignals();                                       // Auswertung der Sondersignale
				void ActTensionOut_KEDR();                                      // Ausgabe der Zugkraft als Spannung für KEDR
                void ActVoltages();                                             // Ausgabe der Spannungen an den DAU
				void ActWinchBrakeInternAuto();                                 // Steuerung der Windenbremse durch eine interne Automatik
				void ActWinchBrakeOff();                                        // Steuerung der Windenbremse
        public:
				WCAnalysis();                                                   // Konstruktor
                ~WCAnalysis();                                                  // Destruktor

                // Globale Variable
                WCIO IOControl;

                // VERWALTUNG
                void CleanUp();                                                 // Aufräumen
				void Init(WCVarControl *VarControl, WCGenOrganisationS *GenOrgan); // Initialisieren der Klasse

                void Analyze();                                                 // Liest Hardware und Werte aus, analysiert Daten, setzt Werte steuert Hardware
};

#endif
